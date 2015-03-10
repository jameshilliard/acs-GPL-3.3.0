/* Framegrabber device structure */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/types.h>

#include <linux/videodev.h>

#include <pthread.h>

#define SETTING_BRIGHTNESS	0
#define SETTING_HUE		1
#define SETTING_COLOUR		2
#define SETTING_CONTRAST	3

#define FORMAT_GREY		VIDEO_PALETTE_GREY
#define FORMAT_RGB565		VIDEO_PALETTE_RGB565
#define FORMAT_RGB32		VIDEO_PALETTE_RGB32

#define IMAGE_BUFFER_EMPTY	0
#define IMAGE_BUFFER_FULL	1
#define IMAGE_BUFFER_INUSE	2

static const int error_exit_status = -1; 

struct fgdevice {
			int video_dev;

			int width;
			int height;
			int input;
			int format;

			struct video_mmap vid_mmap[2];
			int current_grab_number;
			struct video_mbuf vid_mbuf;
			char *video_map;
			int grabbing_active;
			int have_new_frame;
			void *current_image;
			pthread_mutex_t buffer_mutex;
			pthread_t grab_thread;
			pthread_cond_t buffer_cond;
			int totalframecount;
			int image_size;
			int image_pixels;
			int framecount;
			int fps_update_interval;
			double fps;
			double lasttime;
		};

/* Prototypes */

int fg_open_device (struct fgdevice *fg, char *devicename);
int fg_close_device (struct fgdevice *fg);

int fg_print_info(struct fgdevice *fg); 

void fg_set_fps_interval(struct fgdevice *fg, int interval);
double fg_get_fps(struct fgdevice *fg);

int fg_get_setting(struct fgdevice *fg, int which_setting);
int fg_set_setting(struct fgdevice *fg, int which_setting, int value);
int fg_set_channel(struct fgdevice *fg, int channel);

int fg_start_grab_image (struct fgdevice *fg, int width, int height, int format);
int fg_stop_grab_image (struct fgdevice *fg);
void * fg_get_next_image(struct fgdevice *fg);

double timeGet(void);
 