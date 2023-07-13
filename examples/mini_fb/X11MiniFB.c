#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#include "MiniFB.h"

#define KEY_FUNCTION 0xFF
#define KEY_ESC 0x1B

static int s_fbfd;
static struct fb_var_screeninfo s_vinfo;
static struct fb_fix_screeninfo s_finfo;
static int s_width;
static int s_height;
static char* s_framebuffer;

int mfb_open(const char* title, int width, int height)
{
	s_fbfd = open("/dev/fb0", O_RDWR);
	if (s_fbfd == -1)
		return -1;

	if (ioctl(s_fbfd, FBIOGET_FSCREENINFO, &s_finfo) == -1)
	{
		close(s_fbfd);
		return -1;
	}

	if (ioctl(s_fbfd, FBIOGET_VSCREENINFO, &s_vinfo) == -1)
	{
		close(s_fbfd);
		return -1;
	}

	s_width = width;
	s_height = height;

	if (s_width > s_vinfo.xres || s_height > s_vinfo.yres)
	{
		close(s_fbfd);
		return -1;
	}

	size_t size = s_width * s_height * (s_vinfo.bits_per_pixel / 8);
	s_framebuffer = (char*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, s_fbfd, 0);
	if (s_framebuffer == MAP_FAILED)
	{
		close(s_fbfd);
		return -1;
	}

	return 1;
}

static int processEvents()
{
	// Implement your event handling code here if needed
	return 0;
}

int mfb_update(void* buffer)
{
	memcpy(s_framebuffer, buffer, s_width * s_height * (s_vinfo.bits_per_pixel / 8));
	if (processEvents() < 0)
		return -1;

	return 0;
}

void mfb_close(void)
{
	munmap(s_framebuffer, s_width * s_height * (s_vinfo.bits_per_pixel / 8));
	close(s_fbfd);
}
