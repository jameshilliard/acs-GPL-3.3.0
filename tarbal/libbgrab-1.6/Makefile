# libbgrab Makefile, A. Schiffler, schiffler@zkm.de

CC = cc
AR = ar
RANLIB = ranlib 

# Comment out -DUSEAUDIO to disable volume control in zoomgrab

CFLAGS = -O2 -Wall -D_REENTRANT -I/usr/include/glide -DUSEAUDIO
LIBS = -L/usr/X11R6/lib -lXext -lX11 -lm -lpthread

OBJS = bgrab.o xutil.o

# Remove zoomgrab and artcam if you don not have glide (Voodoo) drivers installed

default: libbgrab.a testgrab asciigrab zoomgrab artcam

libbgrab.a: Makefile bgrab.o xutil.o
	$(AR) r libbgrab.a $(OBJS)
	$(RANLIB) libbgrab.a 

testgrab: Makefile testgrab.o
	$(CC) testgrab.o -o testgrab -L. -lbgrab $(LIBS)

asciigrab: Makefile asciigrab.o
	$(CC) asciigrab.o -o asciigrab -L. -lbgrab $(LIBS)

zoomgrab: Makefile zoomgrab.o spline.o
	$(CC) zoomgrab.o spline.o -o zoomgrab -L. -lbgrab $(LIBS) -lglide2x

artcam: Makefile artcam.o
	$(CC) artcam.o -o artcam -L. -lbgrab $(LIBS) -lglide2x
	
clean:
	rm -f *.o libbgrab.a testgrab asciigrab zoomgrab artcam core *~

bgrab.o: bgrab.c bgrab.h
xutil.o: xutil.c xutil.h
testgrab.o: testgrab.c
asciigrab.o: asciigrab.c
zoomgrab.o: zoomgrab.c
