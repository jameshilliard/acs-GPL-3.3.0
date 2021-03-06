/******************************************************************************
 * A simple user-attribute based module for PAM.
 *
 * Copyright (c) 2003 Red Hat, Inc.
 * Written by Nalin Dahyabhai <nalin@redhat.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU Public License, in which case the provisions of the GPL are
 * required INSTEAD OF the above restrictions.  (This clause is
 * necessary due to a potential bad interaction between the GPL and
 * the restrictions contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <security/pam_modules.h>
#include <security/_pam_modutil.h>

#define MODULE "pam_succeed_if"

static void
log_error(int priority, const char *fmt, ...)
{
	va_list va;
	char *fmt2;
	fmt2 = malloc(strlen(fmt) + strlen(MODULE) + 3);
	va_start(va, fmt);
	if (fmt2 == NULL) {
		vsyslog(LOG_AUTHPRIV | priority, fmt, va);
	} else {
		snprintf(fmt2, strlen(fmt) + strlen(MODULE) + 3,
			 "%s: %s", MODULE, fmt);
		vsyslog(LOG_AUTHPRIV | priority, fmt2, va);
		free(fmt2);
	}
	va_end(va);
}

/* Basically, run cmp(atol(left), atol(right)), returning PAM_SUCCESS if
 * the function returns non-zero, PAM_AUTH_ERR if it returns zero, and
 * PAM_SYSTEM_ERR if the arguments can't be parsed as numbers. */
static int
evaluate_num(const char *left, const char *right, int (*cmp)(int, int))
{
	long l, r;
	char *p;
	int ret = PAM_SUCCESS;

	errno = 0;
	l = strtol(left, &p, 0);
	if ((p == NULL) || (*p != '\0') || errno) {
		log_error(LOG_INFO, "\"%s\" is not a number", left);
		ret = PAM_SERVICE_ERR;
	}

	r = strtol(right, &p, 0);
	if ((p == NULL) || (*p != '\0') || errno) {
		log_error(LOG_INFO, "\"%s\" is not a number", right);
		ret = PAM_SERVICE_ERR;
	}

	if (ret != PAM_SUCCESS) {
		return ret;
	}

	return cmp(l, r) ? PAM_SUCCESS : PAM_AUTH_ERR;
}

/* Simple numeric comparison callbacks. */
static int
eq(int i, int j)
{
	return i == j;
}
static int
ne(int i, int j)
{
	return i != j;
}
static int
lt(int i, int j)
{
	return i < j;
}
static int
le(int i, int j)
{
	return lt(i, j) || eq(i, j);
}
static int
gt(int i, int j)
{
	return i > j;
}
static int
ge(int i, int j)
{
	return gt(i, j) || eq(i, j);
}

/* Test for numeric equality. */
static int
evaluate_eqn(const char *left, const char *right)
{
	return evaluate_num(left, right, eq);
}
/* Test for string equality. */
static int
evaluate_eqs(const char *left, const char *right)
{
	return (strcmp(left, right) == 0) ? PAM_SUCCESS : PAM_AUTH_ERR;
}
/* Test for numeric inequality. */
static int
evaluate_nen(const char *left, const char *right)
{
	return evaluate_num(left, right, ne);
}
/* Test for string inequality. */
static int
evaluate_nes(const char *left, const char *right)
{
	return (strcmp(left, right) != 0) ? PAM_SUCCESS : PAM_AUTH_ERR;
}
/* Test for numeric less-than-ness(?) */
static int
evaluate_lt(const char *left, const char *right)
{
	return evaluate_num(left, right, lt);
}
/* Test for numeric less-than-or-equal-ness(?) */
static int
evaluate_le(const char *left, const char *right)
{
	return evaluate_num(left, right, le);
}
/* Test for numeric greater-than-ness(?) */
static int
evaluate_gt(const char *left, const char *right)
{
	return evaluate_num(left, right, gt);
}
/* Test for numeric greater-than-or-equal-ness(?) */
static int
evaluate_ge(const char *left, const char *right)
{
	return evaluate_num(left, right, ge);
}
/* Check for file glob match. */
static int
evaluate_glob(const char *left, const char *right)
{
	return (fnmatch(right, left, 0) == 0) ? PAM_SUCCESS : PAM_AUTH_ERR;
}
/* Check for file glob mismatch. */
static int
evaluate_noglob(const char *left, const char *right)
{
	return (fnmatch(right, left, 0) != 0) ? PAM_SUCCESS : PAM_AUTH_ERR;
}
/* Return PAM_SUCCESS if the user is in the group. */
static int
evaluate_ingroup(pam_handle_t *pamh, const char *user, const char *group)
{
	int ret;
	ret = _pammodutil_user_in_group_nam_nam(pamh, user, group);
	switch (ret) {
	case 1:
		return PAM_SUCCESS;
		break;
	default:
		break;
	}
	return PAM_AUTH_ERR;
}
/* Return PAM_SUCCESS if the user is NOT in the group. */
static int
evaluate_notingroup(pam_handle_t *pamh, const char *user, const char *group)
{
	int ret;
	ret = _pammodutil_user_in_group_nam_nam(pamh, user, group);
	switch (ret) {
	case 0:
		return PAM_SUCCESS;
		break;
	default:
		break;
	}
	return PAM_AUTH_ERR;
}

/* Match a triple. */
static int
evaluate(pam_handle_t *pamh, int debug,
	 const char *left, const char *qual, const char *right,
	 struct passwd *pwd)
{
	char buf[LINE_MAX] = "";
	const char *attribute = left;
	/* Figure out what we're evaluating here, and convert it to a string.*/
	if ((strcasecmp(left, "login") == 0) ||
	    (strcasecmp(left, "name") == 0) ||
	    (strcasecmp(left, "user") == 0)) {
		snprintf(buf, sizeof(buf), "%s", pwd->pw_name);
		left = buf;
	}
	if (strcasecmp(left, "uid") == 0) {
		snprintf(buf, sizeof(buf), "%lu", (unsigned long) pwd->pw_uid);
		left = buf;
	}
	if (strcasecmp(left, "gid") == 0) {
		snprintf(buf, sizeof(buf), "%lu", (unsigned long) pwd->pw_gid);
		left = buf;
	}
	if (strcasecmp(left, "shell") == 0) {
		snprintf(buf, sizeof(buf), "%s", pwd->pw_shell);
		left = buf;
	}
	if ((strcasecmp(left, "home") == 0) ||
	    (strcasecmp(left, "dir") == 0) ||
	    (strcasecmp(left, "homedir") == 0)) {
		snprintf(buf, sizeof(buf), "%s", pwd->pw_dir);
		left = buf;
	}
	/* If we have no idea what's going on, return an error. */
	if (left != buf) {
		log_error(LOG_CRIT, "unknown attribute \"%s\"", left);
		return PAM_SERVICE_ERR;
	}
	if (debug) {
		log_error(LOG_DEBUG, "'%s' resolves to '%s'", attribute, left);
	}

	/* Attribute value < some threshold. */
	if ((strcasecmp(qual, "<") == 0) ||
	    (strcasecmp(qual, "lt") == 0)) {
		return evaluate_lt(left, right);
	}
	/* Attribute value <= some threshold. */
	if ((strcasecmp(qual, "<=") == 0) ||
	    (strcasecmp(qual, "le") == 0)) {
		return evaluate_le(left, right);
	}
	/* Attribute value > some threshold. */
	if ((strcasecmp(qual, ">") == 0) ||
	    (strcasecmp(qual, "gt") == 0)) {
		return evaluate_gt(left, right);
	}
	/* Attribute value >= some threshold. */
	if ((strcasecmp(qual, ">=") == 0) ||
	    (strcasecmp(qual, "ge") == 0)) {
		return evaluate_ge(left, right);
	}
	/* Attribute value == some threshold. */
	if (strcasecmp(qual, "eq") == 0) {
		return evaluate_eqn(left, right);
	}
	/* Attribute value = some string. */
	if (strcasecmp(qual, "=") == 0) {
		return evaluate_eqs(left, right);
	}
	/* Attribute value != some threshold. */
	if (strcasecmp(qual, "ne") == 0) {
		return evaluate_nen(left, right);
	}
	/* Attribute value != some string. */
	if (strcasecmp(qual, "!=") == 0) {
		return evaluate_nes(left, right);
	}
	/* Attribute value matches some pattern. */
	if ((strcasecmp(qual, "=~") == 0) ||
	    (strcasecmp(qual, "glob") == 0)) {
		return evaluate_glob(left, right);
	}
	if ((strcasecmp(qual, "!~") == 0) ||
	    (strcasecmp(qual, "noglob") == 0)) {
		return evaluate_noglob(left, right);
	}
	/* User is in this group. */
	if (strcasecmp(qual, "ingroup") == 0) {
		return evaluate_ingroup(pamh, pwd->pw_name, right);
	}
	/* User is not in this group. */
	if (strcasecmp(qual, "notingroup") == 0) {
		return evaluate_notingroup(pamh, pwd->pw_name, right);
	}
	/* Fail closed. */
	return PAM_SERVICE_ERR;
}

int
pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	const void *prompt;
	const char *user;
	struct passwd *pwd;
	int ret, i, count, use_uid, debug;
	const char *left, *right, *qual;
	int quiet_fail, quiet_succ;

	/* Get the user prompt. */
	ret = pam_get_item(pamh, PAM_USER_PROMPT, &prompt);
	if ((ret != PAM_SUCCESS) || (prompt == NULL) || (strlen(prompt) == 0)) {
		prompt = "login: ";
	}

	quiet_fail = 0;
	quiet_succ = 0;
	for (use_uid = 0, debug = 0, i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0) {
			debug++;
		}
		if (strcmp(argv[i], "use_uid") == 0) {
			use_uid++;
		}
		if (strcmp(argv[i], "quiet") == 0) {
			quiet_fail++;
			quiet_succ++;
		}
		if (strcmp(argv[i], "quiet_fail") == 0) {
			quiet_fail++;
		}
		if (strcmp(argv[i], "quiet_success") == 0) {
			quiet_succ++;
		}
	}

	if (use_uid) {
		/* Get information about the user. */
		pwd = _pammodutil_getpwuid(pamh, getuid());
		if (pwd == NULL) {
			log_error(LOG_CRIT,
				  "error retrieving information about user %ld",
				  (long)getuid());
			return PAM_SERVICE_ERR;
		}
		user = pwd->pw_name;
	} else {
		/* Get the user's name. */
		ret = pam_get_user(pamh, &user, prompt);
		if ((ret != PAM_SUCCESS) || (user == NULL)) {
			log_error(LOG_CRIT, "error retrieving user name: %s",
				  pam_strerror(pamh, ret));
			return ret;
		}

		/* Get information about the user. */
		pwd = _pammodutil_getpwnam(pamh, user);
		if (pwd == NULL) {
			log_error(LOG_CRIT,
				  "error retrieving information about user %s",
				  user);
			return PAM_SERVICE_ERR;
		}
	}

	/* Walk the argument list. */
	i = count = 0;
	left = qual = right = NULL;
	while (i <= argc) {
		if ((left != NULL) && (qual != NULL) && (right != NULL)) {
			ret = evaluate(pamh, debug,
				       left, qual, right,
				       pwd);
			if (ret != PAM_SUCCESS) {
				if(!quiet_fail)
					log_error(LOG_INFO,
						  "requirement \"%s %s %s\" "
						  "not met by user \"%s\"",
						  left, qual, right, user);
				break;
			}
			else
				if(!quiet_succ)
					log_error(LOG_INFO,
						  "requirement \"%s %s %s\" "
						  "was met by user \"%s\"",
						  left, qual, right, user);
			left = qual = right = NULL;
		}
		if ((i < argc) && (strcmp(argv[i], "debug") == 0)) {
			i++;
			continue;
		}
		if ((i < argc) && (strcmp(argv[i], "use_uid") == 0)) {
			i++;
			continue;
		}
		if ((i < argc) && (strcmp(argv[i], "quiet") == 0)) {
			i++;
			continue;
		}
		if ((i < argc) && (strcmp(argv[i], "quiet_fail") == 0)) {
			i++;
			continue;
		}
		if ((i < argc) && (strcmp(argv[i], "quiet_success") == 0)) {
			i++;
			continue;
		}
		if ((i < argc) && (left == NULL)) {
			left = argv[i++];
			count++;
			continue;
		}
		if ((i < argc) && (qual == NULL)) {
			qual = argv[i++];
			count++;
			continue;
		}
		if ((i < argc) && (right == NULL)) {
			right = argv[i++];
			count++;
			continue;
		}
		i++;
	}

	return ret;
}

int
pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	return PAM_SUCCESS;
}

int
pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	return pam_sm_authenticate(pamh, flags, argc, argv);
}
