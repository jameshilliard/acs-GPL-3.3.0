#
# $Id: Makefile,v 1.3 2005/05/16 11:03:03 kukuk Exp $
#
# This Makefile controls a build process of $(TITLE) module for
# Linux-PAM. You should not modify this Makefile (unless you know
# what you are doing!).
#
# Created by Andrew Morgan <morgan@linux.kernel.org> 2000/08/27
#

include ../../Make.Rules

TITLE=pam_rootok

include ../Simple.Rules

ifeq ($(HAVE_LIBSELINUX),yes)
CFLAGS += -DWITH_SELINUX
LINK_PAMMODUTILS += -lselinux
endif

