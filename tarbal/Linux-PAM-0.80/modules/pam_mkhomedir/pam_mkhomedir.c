/* PAM Make Home Dir module

   This module will create a users home directory if it does not exist
   when the session begins. This allows users to be present in central
   database (such as nis, kerb or ldap) without using a distributed
   file system or pre-creating a large number of directories.

   Here is a sample /etc/pam.d/login file for Debian GNU/Linux
   2.1:

   auth       requisite  pam_securetty.so
   auth       sufficient pam_ldap.so
   auth       required   pam_pwdb.so
   auth       optional   pam_group.so
   auth       optional   pam_mail.so
   account    requisite  pam_time.so
   account    sufficient pam_ldap.so
   account    required   pam_pwdb.so
   session    required   pam_mkhomedir.so skel=/etc/skel/ umask=0022
   session    required   pam_pwdb.so
   session    optional   pam_lastlog.so
   password   required   pam_pwdb.so

   Released under the GNU LGPL version 2 or later
   Originally written by Jason Gunthorpe <jgg@debian.org> Feb 1999
   Structure taken from pam_lastlogin by Andrew Morgan
     <morgan@parc.power.net> 1996
 */

/* I want snprintf dammit */
#define _GNU_SOURCE 1
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

/*
 * here, we make a definition for the externally accessible function
 * in this file (this definition is required for static a module
 * but strongly encouraged generally) it is used to instruct the
 * modules include file to define the function prototypes.
 */

#define PAM_SM_SESSION

#include <security/pam_modules.h>
#include <security/_pam_macros.h>
#include <security/_pam_modutil.h>


/* argument parsing */
#define MKHOMEDIR_DEBUG      020	/* keep quiet about things */
#define MKHOMEDIR_QUIET      040	/* keep quiet about things */

static unsigned int UMask = 0022;
static char SkelDir[BUFSIZ] = "/etc/skel"; /* THIS MODULE IS NOT THREAD SAFE */

/* some syslogging */
static void
_log_err (int err, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    openlog("PAM-mkhomedir", LOG_CONS|LOG_PID, LOG_AUTH);
    vsyslog(err, format, args);
    va_end(args);
    closelog();
}

static int
_pam_parse (int flags, int argc, const char **argv)
{
   int ctrl = 0;

   /* does the appliction require quiet? */
   if ((flags & PAM_SILENT) == PAM_SILENT)
      ctrl |= MKHOMEDIR_QUIET;

   /* step through arguments */
   for (; argc-- > 0; ++argv)
   {
      if (!strcmp(*argv, "silent")) {
	 ctrl |= MKHOMEDIR_QUIET;
      } else if (!strncmp(*argv,"umask=",6)) {
	 UMask = strtol(*argv+6,0,0);
      } else if (!strncmp(*argv,"skel=",5)) {
	 strncpy(SkelDir,*argv+5,sizeof(SkelDir));
	 SkelDir[sizeof(SkelDir)-1] = '\0';
      } else {
	 _log_err(LOG_ERR, "unknown option; %s", *argv);
      }
   }

   D(("ctrl = %o", ctrl));
   return ctrl;
}

/* This common function is used to send a message to the applications
   conversion function. Our only use is to ask the application to print
   an informative message that we are creating a home directory */
static int converse(pam_handle_t * pamh, int ctrl, int nargs
		    ,struct pam_message **message
		    ,struct pam_response **response)
{
   int retval;
   const void *void_conv;
   const struct pam_conv *conv;

   D(("begin to converse"));

   retval = pam_get_item(pamh, PAM_CONV, &void_conv);
   conv = (const struct pam_conv *)void_conv;
   if (retval == PAM_SUCCESS && conv)
   {

      retval = conv->conv(nargs, (const struct pam_message **) message
			  ,response, conv->appdata_ptr);

      D(("returned from application's conversation function"));

      if (retval != PAM_SUCCESS && (ctrl & MKHOMEDIR_DEBUG))
      {
	 _log_err(LOG_DEBUG, "conversation failure [%s]"
		  ,pam_strerror(pamh, retval));
      }

   }
   else
   {
      _log_err(LOG_ERR, "couldn't obtain coversation function [%s]"
	       ,pam_strerror(pamh, retval));
     if (retval == PAM_SUCCESS)
         retval = PAM_BAD_ITEM; /* conv was NULL */
   }

   D(("ready to return from module conversation"));

   return retval;		/* propagate error status */
}

/* Ask the application to display a short text string for us. */
static int
make_remark (pam_handle_t *pamh, int ctrl, const char *remark)
{
   int retval;

   if ((ctrl & MKHOMEDIR_QUIET) != MKHOMEDIR_QUIET)
   {
      struct pam_message msg[1], *mesg[1];
      struct pam_response *resp = NULL;

      mesg[0] = &msg[0];
      msg[0].msg_style = PAM_TEXT_INFO;
      msg[0].msg = remark;

      retval = converse(pamh, ctrl, 1, mesg, &resp);

      msg[0].msg = NULL;
      if (resp)
      {
	 _pam_drop_reply(resp, 1);
      }
   }
   else
   {
      D(("keeping quiet"));
      retval = PAM_SUCCESS;
   }

   D(("returning %s", pam_strerror(pamh, retval)));
   return retval;
}

static int
rec_mkdir (const char *dir, mode_t mode)
{
  char *cp;
  char *parent = strdup (dir);

  if (parent == NULL)
    return 1;

  cp = strrchr (parent, '/');

  if (cp != NULL)
    {
      struct stat st;

      *cp++ = '\0';
      if (stat (parent, &st) == -1 && errno == ENOENT)
        if (rec_mkdir (parent, mode) != 0)
	  {
	    free (parent);
	    return 1;
	  }
    }

  free (parent);

  if (mkdir (dir, mode) != 0 && errno != EEXIST)
    return 1;

  return 0;
}

/* Do the actual work of creating a home dir */
static int
create_homedir (pam_handle_t * pamh, int ctrl,
		const struct passwd *pwd,
		const char *source, const char *dest)
{
   char remark[BUFSIZ];
   DIR *D;
   struct dirent *Dir;
   int retval = PAM_AUTH_ERR;

   /* Mention what is happening, if the notification fails that is OK */
   if (snprintf(remark,sizeof(remark),"Creating directory '%s'.", dest) == -1)
      return PAM_PERM_DENIED;

   make_remark(pamh, ctrl, remark);

   /* Create the new directory */
   if (rec_mkdir (dest,0755) != 0)
   {
      _log_err(LOG_DEBUG, "unable to create directory %s",dest);
      return PAM_PERM_DENIED;
   }

   /* See if we need to copy the skel dir over. */
   if ((source == NULL) || (strlen(source) == 0))
   {
     retval = PAM_SUCCESS;
     goto go_out;
   }

   /* Scan the directory */
   D = opendir (source);
   if (D == 0)
   {
      _log_err(LOG_DEBUG, "unable to read directory %s",source);
      retval = PAM_PERM_DENIED;
      goto go_out;
   }

   for (Dir = readdir(D); Dir != 0; Dir = readdir(D))
   {
      int SrcFd;
      int DestFd;
      int Res;
      struct stat St;
#ifndef PATH_MAX
      char *newsource = NULL, *newdest = NULL;
      /* track length of buffers */
      int nslen = 0, ndlen = 0;
      int slen = strlen(source), dlen = strlen(dest);
#else
      char newsource[PATH_MAX], newdest[PATH_MAX];
#endif

      /* Skip some files.. */
      if (strcmp(Dir->d_name,".") == 0 ||
	  strcmp(Dir->d_name,"..") == 0)
	 continue;

      /* Determine what kind of file it is. */
#ifndef PATH_MAX
      nslen = slen + strlen(Dir->d_name) + 2;

      if (nslen <= 0)
	{
	  retval = PAM_BUF_ERR;
	  goto go_out;
	}

      if ((newsource = malloc (nslen)) == NULL)
	{
	  retval = PAM_BUF_ERR;
	  goto go_out;
	}

      sprintf(newsource, "%s/%s", source, Dir->d_name);
#else
      snprintf(newsource,sizeof(newsource),"%s/%s",source,Dir->d_name);
#endif

      if (lstat(newsource,&St) != 0)
#ifndef PATH_MAX
      {
	      free(newsource);
	      newsource = NULL;
         continue;
      }
#else
      continue;
#endif


      /* We'll need the new file's name. */
#ifndef PATH_MAX
      ndlen = dlen + strlen(Dir->d_name)+2;

      if (ndlen <= 0)
	{
	  retval = PAM_BUF_ERR;
	  goto go_out;
	}

      if ((newdest = malloc(ndlen)) == NULL)
	{
	  free (newsource);
	  retval = PAM_BUF_ERR;
	  goto go_out;
	}

      sprintf (newdest, "%s/%s", dest, Dir->d_name);
#else
      snprintf (newdest,sizeof (newdest),"%s/%s",dest,Dir->d_name);
#endif

      /* If it's a directory, recurse. */
      if (S_ISDIR(St.st_mode))
      {
        retval = create_homedir (pamh, ctrl, pwd, newsource, newdest);

#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif

         if (retval != PAM_SUCCESS)
	   {
	     closedir(D);
	     goto go_out;
	   }
         continue;
      }

      /* If it's a symlink, create a new link. */
      if (S_ISLNK(St.st_mode))
      {
	 int pointedlen = 0;
#ifndef PATH_MAX
	 char *pointed = NULL;
           {
		   int size = 100;

		   while (1) {
			   pointed = (char *) malloc(size);
			   if ( ! pointed ) {
				   free(newsource);
				   free(newdest);
				   return PAM_BUF_ERR;
			   }
			   pointedlen = readlink (newsource, pointed, size);
			   if ( pointedlen < 0 ) break;
			   if ( pointedlen < size ) break;
			   free (pointed);
			   size *= 2;
		   }
	   }
	   if ( pointedlen < 0 )
		   free(pointed);
	   else
		   pointed[pointedlen] = 0;
#else
         char pointed[PATH_MAX];
         memset(pointed, 0, sizeof(pointed));

         pointedlen = readlink(newsource, pointed, sizeof(pointed) - 1);
#endif

	 if ( pointedlen >= 0 ) {
            if(symlink(pointed, newdest) == 0)
            {
               if (lchown(newdest,pwd->pw_uid,pwd->pw_gid) != 0)
               {
                   closedir(D);
                   _log_err(LOG_DEBUG, "unable to change perms on link %s",
                            newdest);
#ifndef PATH_MAX
		   free(pointed);
		   free(newsource);
		   free(newdest);
#endif
                   return PAM_PERM_DENIED;
               }
            }
#ifndef PATH_MAX
	    free(pointed);
#endif
         }
#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif
         continue;
      }

      /* If it's not a regular file, it's probably not a good idea to create
       * the new device node, FIFO, or whatever it is. */
      if (!S_ISREG(St.st_mode))
      {
#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif
         continue;
      }

      /* Open the source file */
      if ((SrcFd = open(newsource,O_RDONLY)) < 0 || fstat(SrcFd,&St) != 0)
      {
         closedir(D);
         _log_err(LOG_DEBUG, "unable to open src file %s",newsource);

#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif

	 return PAM_PERM_DENIED;
      }
      stat(newsource,&St);

      /* Open the dest file */
      if ((DestFd = open(newdest,O_WRONLY | O_TRUNC | O_CREAT,0600)) < 0)
      {
	 close(SrcFd);
	 closedir(D);
         _log_err(LOG_DEBUG, "unable to open dest file %s",newdest);

#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif
	 return PAM_PERM_DENIED;
      }

      /* Set the proper ownership and permissions for the module. We make
       	 the file a+w and then mask it with the set mask. This preseves
       	 execute bits */
      if (fchmod(DestFd,(St.st_mode | 0222) & (~UMask)) != 0 ||
	  fchown(DestFd,pwd->pw_uid,pwd->pw_gid) != 0)
      {
         close(SrcFd);
         close(DestFd);
         closedir(D);
         _log_err(LOG_DEBUG, "unable to chang perms on copy %s",newdest);

#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif

	 return PAM_PERM_DENIED;
      }

      /* Copy the file */
      do
      {
	 Res = _pammodutil_read(SrcFd,remark,sizeof(remark));

	 if (Res == 0)
	     continue;

	 if (Res > 0) {
	     if (_pammodutil_write(DestFd,remark,Res) == Res)
		continue;
	 }

	 /* If we get here, pammodutil_read returned a -1 or
	    _pammodutil_write returned something unexpected. */
	 close(SrcFd);
	 close(DestFd);
	 closedir(D);
	 _log_err(LOG_DEBUG, "unable to perform IO");

#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif

	 return PAM_PERM_DENIED;
      }
      while (Res != 0);
      close(SrcFd);
      close(DestFd);

#ifndef PATH_MAX
	 free(newsource); newsource = NULL;
	 free(newdest); newdest = NULL;
#endif

   }
   closedir(D);

   retval = PAM_SUCCESS;

 go_out:

   if (chmod(dest,0777 & (~UMask)) != 0 ||
       chown(dest,pwd->pw_uid,pwd->pw_gid) != 0)
   {
      _log_err(LOG_DEBUG, "unable to change perms on directory %s",dest);
      return PAM_PERM_DENIED;
   }

   return retval;
}

/* --- authentication management functions (only) --- */

PAM_EXTERN
int pam_sm_open_session(pam_handle_t * pamh, int flags, int argc
			,const char **argv)
{
   int retval, ctrl;
   const void *user;
   const struct passwd *pwd;
   struct stat St;

   /* Parse the flag values */
   ctrl = _pam_parse(flags, argc, argv);

   /* Determine the user name so we can get the home directory */
   retval = pam_get_item(pamh, PAM_USER, &user);
   if (retval != PAM_SUCCESS || user == NULL || *(const char *)user == '\0')
   {
      _log_err(LOG_NOTICE, "user unknown");
      return PAM_USER_UNKNOWN;
   }

   /* Get the password entry */
   pwd = _pammodutil_getpwnam (pamh, user);
   if (pwd == NULL)
   {
      D(("couldn't identify user %s", user));
      return PAM_CRED_INSUFFICIENT;
   }

   /* Stat the home directory, if something exists then we assume it is
      correct and return a success*/
   if (stat(pwd->pw_dir,&St) == 0)
      return PAM_SUCCESS;

   return create_homedir(pamh,ctrl,pwd,SkelDir,pwd->pw_dir);
}

/* Ignore */
PAM_EXTERN
int pam_sm_close_session(pam_handle_t * pamh, int flags, int argc
			 ,const char **argv)
{
   return PAM_SUCCESS;
}

#ifdef PAM_STATIC

/* static module data */
struct pam_module _pam_mkhomedir_modstruct =
{
   "pam_mkhomedir",
   NULL,
   NULL,
   NULL,
   pam_sm_open_session,
   pam_sm_close_session,
   NULL,
};

#endif
