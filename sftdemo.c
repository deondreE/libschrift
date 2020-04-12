/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <schrift.h>

#include "arg.h"

#define APP_NAME "sftdemo"

char *argv0;

static XRenderColor fgcolor, bgcolor;
static Display *dpy;
static int screen;
static Window win;
static Pixmap fgpix;
static Picture pic, fgpic;
static SFT_Font *font;
static SFT *sft;

static void
die(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

static void
usage(void)
{
	fprintf(stderr,
		"usage: %s [-f font file] [-s size in px]\n", argv0);
}

static void
draw(int width, int height)
{
	int extents[4], w, h;
	const char *c;

	XRenderFillRectangle(dpy, PictOpSrc, pic, &bgcolor, 0, 0, width, height);

	sft_setflag(sft, SFT_CHAR_RENDER, 0);
	sft_move(sft, 0, -50);
	for (c = "Hello, World!"; *c; ++c) {
		if (sft_char(sft, *c, extents, NULL) < 0)
			die("Can't render character.");
		w = extents[2] - extents[0];
		h = extents[3] - extents[1];

		XRenderComposite(dpy, PictOpOver, fgpic, None, pic, 0, 0, 0, 0,
			extents[0], extents[1], w, h);
	}
}

static void
handleevent(XEvent *ev)
{
	switch (ev->type) {
	case Expose:
		draw(ev->xexpose.width, ev->xexpose.height);
		break;
	}
}

static void
setupx(void)
{
	XRenderPictFormat *format;
	XRenderPictureAttributes attr;

	if (!(dpy = XOpenDisplay(NULL)))
		die("Can't open X display\n");
	screen = DefaultScreen(dpy);

	win = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, 200, 100, 0,
	                    DefaultDepth(dpy, screen), InputOutput,
	                    CopyFromParent, 0, NULL);
	XStoreName(dpy, win, APP_NAME);
	XSelectInput(dpy, win, ExposureMask);
	XMapRaised(dpy, win);

	format = XRenderFindVisualFormat(dpy, DefaultVisual(dpy, screen));
	pic = XRenderCreatePicture(dpy, win, format, 0, NULL);

	fgpix = XCreatePixmap(dpy, win, 1, 1, 24);
	format = XRenderFindStandardFormat(dpy, PictStandardRGB24);
	attr.repeat = True;
	fgpic = XRenderCreatePicture(dpy, fgpix, format, CPRepeat, &attr);
	XRenderFillRectangle(dpy, PictOpSrc, fgpic, &fgcolor, 0, 0, 1, 1);
}

static void
runx(void)
{
	XEvent ev;

	while (!XNextEvent(dpy, &ev))
		handleevent(&ev);
}

int
main(int argc, char *argv[])
{
	const char *filename;
	double size;

	filename = "resources/Ubuntu-R.ttf";
	size = 16.0;
	bgcolor = (XRenderColor) { 0xFFFF, 0x0000, 0x0000, 0xFFFF };
	fgcolor = (XRenderColor) { 0x0000, 0xFFFF, 0x0000, 0xFFFF };

	ARGBEGIN {
	case 'f':
		filename = EARGF(usage());
		break;
	case 's':
		size = atof(EARGF(usage()));
		break;
	default:
		usage();
		exit(1);
	} ARGEND
	if (argc) {
		usage();
		exit(1);
	}

	if ((font = sft_loadfile(filename)) == NULL)
		die("Can't load font file.");
	if ((sft = sft_create()) == NULL)
		die("Can't create schrift context.");
	sft_setfont(sft, font);
	sft_setscale(sft, size, size);

	setupx();
	runx();
	return 0;
}

