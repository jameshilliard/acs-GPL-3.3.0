#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/extensions/XShm.h>
#include <X11/keysym.h>

#include <sys/shm.h>
#include <sys/ipc.h>

struct xwinbuffer {
	int width;
	int height;
	int depth;
	Display* display_;
	GC gc_;
	Window wtwin_;
	XImage *shmimage;
	XShmSegmentInfo* shminfo;
      };

int setup_xwinbuffer(struct xwinbuffer *xwin, char *prgname, int width, int height, int depth);
int sync_xwinbuffer(struct xwinbuffer *xwin);
int keypoll_xwinbuffer (struct xwinbuffer *xwin, KeySym *key, int *button, int *mousex, int *mousey);
int close_xwinbuffer (struct xwinbuffer *xwin);
