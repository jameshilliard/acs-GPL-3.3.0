# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# just set the variable
APPSDIR = $(kde_appsdir)/Utilities
# set the include path for X, qt and KDE
INCLUDES= @all_includes@
# claim, which subdirectories you want to install
# SUBDIRS = doc

bin_PROGRAMS = 	kisdnlog

kisdnlog_SOURCES	= functions.cpp kcalls.cpp kcurcalls.cpp koldcalls.cpp kchan.cpp kconnect.cpp khost.cpp kisdnlog.h klogo.cpp klog.cpp klogwin.cpp kmenu.cpp kmessage.cpp ksplit.cpp kthruput.cpp main.cpp message.cpp message.h messagenr.h mkisdnlog.cpp

# the library search path
kisdnlog_LDFLAGS = @all_libraries@

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kisdnlog_LDADD   = -lkdecore -lkdeui -lqt -lX11 -lXext @LIBSOCKET@

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = 

# just to make sure, automake makes them 
BUILTSOURCES =	mkisdnlog.cpp

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(BUILTSOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ktranslate $(kisdnlog_SOURCES)

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(DESTDIR)$(APPSDIR)
	$(INSTALL_DATA) Kisdnlog.kdelnk $(DESTDIR)$(APPSDIR)

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(DESTDIR)$(APPSDIR)/Kisdnlog.kdelnk

# add a dependency for every moc file to be full portable
# I've added a key binding to emacs for this. 
mkisdnlog.cpp: kisdnlog.h
	$(MOC) kisdnlog.h -o mkisdnlog.cpp

