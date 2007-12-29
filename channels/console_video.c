/*
 * Experimental support for video sessions. We use SDL for rendering, ffmpeg
 * as the codec library for encoding and decoding, and Video4Linux and X11
 * to generate the local video stream.
 *
 * If one of these pieces is not available, either at compile time or at
 * runtime, we do our best to run without it. Of course, no codec library
 * means we can only deal with raw data, no SDL means we cannot do rendering,
 * no V4L or X11 means we cannot generate data (but in principle we could
 * stream from or record to a file).
 *
 * We need a recent (2007.07.12 or newer) version of ffmpeg to avoid warnings.
 * Older versions might give 'deprecated' messages during compilation,
 * thus not compiling in AST_DEVMODE, or don't have swscale, in which case
 * you can try to compile #defining OLD_FFMPEG here.
 *
 * $Revision$
 */

//#define DROP_PACKETS 5       /* if set, drop this % of video packets */
//#define OLD_FFMPEG	1	/* set for old ffmpeg with no swscale */

#include "asterisk.h"
#include <sys/ioctl.h>
#include "asterisk/cli.h"
#include "asterisk/file.h"
#include "asterisk/channel.h"

#include "console_video.h"

/*
The code is structured as follows.

When a new console channel is created, we call console_video_start()
to initialize SDL, the source, and the encoder/ decoder for the
formats in use (XXX the latter two should be done later, once the
codec negotiation is complete).  Also, a thread is created to handle
the video source and generate frames.

While communication is on, the local source is generated by the
video thread, which wakes up periodically, generates frames and
enqueues them in chan->readq.  Incoming rtp frames are passed to
console_write_video(), decoded and passed to SDL for display.

For as unfortunate and confusing as it can be, we need to deal with a
number of different video representations (size, codec/pixel format,
codec parameters), as follows:

 loc_src	is the data coming from the camera/X11/etc.
	The format is typically constrained by the video source.

 enc_in		is the input required by the encoder.
	Typically constrained in size by the encoder type.

 enc_out	is the bitstream transmitted over RTP.
	Typically negotiated while the call is established.

 loc_dpy	is the format used to display the local video source.
	Depending on user preferences this can have the same size as
	loc_src_fmt, or enc_in_fmt, or thumbnail size (e.g. PiP output)

 dec_in		is the incoming RTP bitstream. Negotiated
	during call establishment, it is not necessarily the same as
	enc_in_fmt

 dec_out	the output of the decoder.
	The format is whatever the other side sends, and the
	buffer is allocated by avcodec_decode_... so we only
	copy the data here.

 rem_dpy	the format used to display the remote stream

We store the format info together with the buffer storing the data.
As a future optimization, a format/buffer may reference another one
if the formats are equivalent. This will save some unnecessary format
conversion.


In order to handle video you need to add to sip.conf (and presumably
iax.conf too) the following:

	[general](+)
		videosupport=yes
		allow=h263	; this or other video formats
		allow=h263p	; this or other video formats

 */

/*
 * Codecs are absolutely necessary or we cannot do anything.
 * SDL is optional (used for rendering only), so that we can still
 * stream video withouth displaying it.
 */
#if !defined(HAVE_VIDEO_CONSOLE) || !defined(HAVE_FFMPEG)
/* stubs if required pieces are missing */
int console_write_video(struct ast_channel *chan, struct ast_frame *f)
{
	return 0;	/* writing video not supported */
}

int console_video_cli(struct video_desc *env, const char *var, int fd)
{
	return 1;	/* nothing matched */
}

int console_video_config(struct video_desc **penv, const char *var, const char *val)
{
	return 1;	/* no configuration */
}

void console_video_start(struct video_desc *env, struct ast_channel *owner)
{
	ast_log(LOG_NOTICE, "voice only, console video support not present\n");
}

void console_video_uninit(struct video_desc *env)
{
}

int console_video_formats = 0;

#else /* defined(HAVE_FFMPEG) && defined(HAVE_SDL) */

/*! The list of video formats we support. */
int console_video_formats = 
	AST_FORMAT_H263_PLUS | AST_FORMAT_H263 |
	AST_FORMAT_MP4_VIDEO | AST_FORMAT_H264 | AST_FORMAT_H261 ;



static void my_scale(struct fbuf_t *in, AVPicture *p_in,
	struct fbuf_t *out, AVPicture *p_out);

struct grab_desc;		/* grabber description */
struct video_codec_desc;	/* forward declaration */
/*
 * Descriptor of the local source, made of the following pieces:
 *  + configuration info (geometry, device name, fps...). These are read
 *    from the config file and copied here before calling video_out_init();
 *  + the frame buffer (buf) and source pixel format, allocated at init time;
 *  + the encoding and RTP info, including timestamps to generate
 *    frames at the correct rate;
 *  + source-specific info, i.e. fd for /dev/video, dpy-image for x11, etc,
 *    filled in by grabber_open
 * NOTE: loc_src.data == NULL means the rest of the struct is invalid, and
 *	the video source is not available.
 */
struct video_out_desc {
	/* video device support.
	 * videodevice and geometry are read from the config file.
	 * At the right time we try to open it and allocate a buffer.
	 * If we are successful, webcam_bufsize > 0 and we can read.
	 */
	/* all the following is config file info copied from the parent */
	char		videodevice[64];
	int		fps;
	int		bitrate;
	int		qmin;

	int sendvideo;

	struct fbuf_t	loc_src_geometry;	/* local source geometry only (from config file) */
	struct fbuf_t	enc_out;	/* encoder output buffer, allocated in video_out_init() */

	struct video_codec_desc *enc;	/* encoder */
	void		*enc_ctx;	/* encoding context */
	AVCodec		*codec;
	AVFrame		*enc_in_frame;	/* enc_in mapped into avcodec format. */
					/* The initial part of AVFrame is an AVPicture */
	int		mtu;
	struct timeval	last_frame;	/* when we read the last frame ? */

	struct grab_desc *grabber;
	void		*grabber_data;
};

/*
 * The overall descriptor, with room for config info, video source and
 * received data descriptors, SDL info, etc.
 * This should be globally visible to all modules (grabber, vcodecs, gui)
 * and contain all configurtion info.
 */
struct video_desc {
	char			codec_name[64];	/* the codec we use */

	pthread_t		vthread;	/* video thread */
	ast_mutex_t		dec_lock;	/* sync decoder and video thread */
	int			shutdown;	/* set to shutdown vthread */
	struct ast_channel	*owner;		/* owner channel */


	struct fbuf_t	enc_in;		/* encoder input buffer, allocated in video_out_init() */

	char			keypad_file[256];	/* image for the keypad */
	char                    keypad_font[256];       /* font for the keypad */

	char			sdl_videodriver[256];

	struct fbuf_t		rem_dpy;	/* display remote video, no buffer (it is in win[WIN_REMOTE].bmp) */
	struct fbuf_t		loc_dpy;	/* display local source, no buffer (managed by SDL in bmp[1]) */


	/* local information for grabbers, codecs, gui */
	struct gui_info		*gui;
	struct video_dec_desc	*in;		/* remote video descriptor */
	struct video_out_desc	out;		/* local video descriptor */
};

static AVPicture *fill_pict(struct fbuf_t *b, AVPicture *p);

static void fbuf_free(struct fbuf_t *b)
{
	struct fbuf_t x = *b;

	if (b->data && b->size)
		ast_free(b->data);
	bzero(b, sizeof(*b));
	/* restore some fields */
	b->w = x.w;
	b->h = x.h;
	b->pix_fmt = x.pix_fmt;
}

#include "vcodecs.c"
#include "console_gui.c"

/*------ end codec specific code -----*/

/* descriptor for a grabber */
struct grab_desc {
	const char *name;
	void *(*open)(const char *name, struct fbuf_t *geom, int fps);
	struct fbuf_t *(*read)(void *d);
	void (*move)(void *d, int dx, int dy);
	void *(*close)(void *d);
};


#ifdef HAVE_X11
struct grab_x11_desc {
	Display		*dpy;			/* x11 grabber info */
	XImage		*image;
	int		screen_width;	/* width of X screen */
	int		screen_height;	/* height of X screen */
	struct fbuf_t	b;
};

static void *grab_x11_open(const char *name, struct fbuf_t *geom, int fps)
{
	XImage *im;
	int screen_num;
	struct grab_x11_desc *v;
	struct fbuf_t *b;

	if (strcasecmp(name, "X11"))
		return NULL;	/* not us */
	v = ast_calloc(1, sizeof(*v));
	if (v == NULL)
		return NULL;	/* no memory */

	/* init the connection with the X server */
	v->dpy = XOpenDisplay(NULL);
	if (v->dpy == NULL) {
		ast_log(LOG_WARNING, "error opening display\n");
		goto error;
	}

	v->b = *geom;	/* copy geometry */
	b = &v->b;	/* shorthand */
	/* find width and height of the screen */
	screen_num = DefaultScreen(v->dpy);
	v->screen_width = DisplayWidth(v->dpy, screen_num);
	v->screen_height = DisplayHeight(v->dpy, screen_num);

	v->image = im = XGetImage(v->dpy,
		RootWindow(v->dpy, DefaultScreen(v->dpy)),
		b->x, b->y, b->w, b->h, AllPlanes, ZPixmap);
	if (v->image == NULL) {
		ast_log(LOG_WARNING, "error creating Ximage\n");
		goto error;
	}
	switch (im->bits_per_pixel) {
	case 32:
		b->pix_fmt = PIX_FMT_RGBA32;
		break;
	case 16:
		b->pix_fmt = (im->green_mask == 0x7e0) ? PIX_FMT_RGB565 : PIX_FMT_RGB555;
		break;
	}

	ast_log(LOG_NOTICE, "image: data %p %d bpp fmt %d, mask 0x%lx 0x%lx 0x%lx\n",
		im->data,
		im->bits_per_pixel,
		b->pix_fmt,
		im->red_mask, im->green_mask, im->blue_mask);

	/* set the pointer but not the size as this is not malloc'ed */
	b->data = (uint8_t *)im->data;
	return v;

error:
	/* XXX maybe XDestroy (v->image) ? */
	if (v->dpy)
		XCloseDisplay(v->dpy);
	v->dpy = NULL;
	ast_free(v);
	return NULL;
}

static struct fbuf_t *grab_x11_read(void *desc)
{
	/* read frame from X11 */
	struct grab_x11_desc *v = desc;
	struct fbuf_t *b = &v->b;

	XGetSubImage(v->dpy,
		RootWindow(v->dpy, DefaultScreen(v->dpy)),
			b->x, b->y, b->w, b->h, AllPlanes, ZPixmap, v->image, 0, 0);

		b->data = (uint8_t *)v->image->data;
	return b;
}

static int boundary_checks(int x, int limit)
{
        return (x <= 0) ? 0 : (x > limit ? limit : x);
}

/* move the origin for the grabbed area */
static void grab_x11_move(void *desc, int dx, int dy)
{
	struct grab_x11_desc *v = desc;

        v->b.x = boundary_checks(v->b.x + dx, v->screen_width - v->b.w);
        v->b.y = boundary_checks(v->b.y + dy, v->screen_height - v->b.h);
}

static void *grab_x11_close(void *desc)
{
	struct grab_x11_desc *v = desc;

	XCloseDisplay(v->dpy);
	v->dpy = NULL;
	v->image = NULL;
	ast_free(v);
	return NULL;
}

static struct grab_desc grab_x11_desc = {
	.name = "X11",
	.open = grab_x11_open,
	.read = grab_x11_read,
	.move = grab_x11_move,
	.close = grab_x11_close,
};

#endif	/* HAVE_X11 */

/* Video4Linux stuff is only used in grabber_open() */
#ifdef HAVE_VIDEODEV_H
#include <linux/videodev.h>

struct grab_v4l1_desc {
	int fd;
	int fps;
	struct fbuf_t	b;
};

/*!
 * Open the local video source and allocate a buffer
 * for storing the image. Return 0 on success, -1 on error
 */
static void *grab_v4l1_open(const char *dev, struct fbuf_t *geom, int fps)
{
	struct video_window vw = { 0 };	/* camera attributes */
	struct video_picture vp;
	int fd, i;
	struct grab_v4l1_desc *v;
	struct fbuf_t *b;

	fd = open(dev, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		ast_log(LOG_WARNING, "error opening camera %s\n", dev);
		return NULL;
	}

	v = ast_calloc(1, sizeof(*v));
	if (v == NULL) {
		ast_log(LOG_WARNING, "no memory for camera %s\n", dev);
		close(fd);
		return NULL;	/* no memory */
	}
	v->fd = fd;
	v->b = *geom;
	b = &v->b;	/* shorthand */

	i = fcntl(fd, F_GETFL);
	if (-1 == fcntl(fd, F_SETFL, i | O_NONBLOCK)) {
		/* non fatal, just emit a warning */
		ast_log(LOG_WARNING, "error F_SETFL for %s [%s]\n",
			dev, strerror(errno));
	}
	/* set format for the camera.
	 * In principle we could retry with a different format if the
	 * one we are asking for is not supported.
	 */
	vw.width = b->w;
	vw.height = b->h;
	vw.flags = fps << 16;
	if (ioctl(fd, VIDIOCSWIN, &vw) == -1) {
		ast_log(LOG_WARNING, "error setting format for %s [%s]\n",
			dev, strerror(errno));
		goto error;
	}
	if (ioctl(fd, VIDIOCGPICT, &vp) == -1) {
		ast_log(LOG_WARNING, "error reading picture info\n");
		goto error;
	}
	ast_log(LOG_WARNING,
		"contrast %d bright %d colour %d hue %d white %d palette %d\n",
		vp.contrast, vp.brightness,
		vp.colour, vp.hue,
		vp.whiteness, vp.palette);
	/* set the video format. Here again, we don't necessary have to
	 * fail if the required format is not supported, but try to use
	 * what the camera gives us.
	 */
	b->pix_fmt = vp.palette;
	vp.palette = VIDEO_PALETTE_YUV420P;
	if (ioctl(v->fd, VIDIOCSPICT, &vp) == -1) {
		ast_log(LOG_WARNING, "error setting palette, using %d\n",
			b->pix_fmt);
	} else
		b->pix_fmt = vp.palette;
	/* allocate the source buffer.
	 * XXX, the code here only handles yuv411, for other formats
	 * we need to look at pix_fmt and set size accordingly
	 */
	b->size = (b->w * b->h * 3)/2;	/* yuv411 */
	ast_log(LOG_WARNING, "videodev %s opened, size %dx%d %d\n",
		dev, b->w, b->h, b->size);
	b->data = ast_calloc(1, b->size);
	if (!b->data) {
		ast_log(LOG_WARNING, "error allocating buffer %d bytes\n",
			b->size);
		goto error;
	}
	ast_log(LOG_WARNING, "success opening camera\n");
	return v;

error:
	close(v->fd);
	fbuf_free(b);
	ast_free(v);
	return NULL;
}

static struct fbuf_t *grab_v4l1_read(void *desc)
{
	struct grab_v4l1_desc *v = desc;
	struct fbuf_t *b = &v->b;
	for (;;) {
		int r, l = b->size - b->used;
		r = read(v->fd, b->data + b->used, l);
		// ast_log(LOG_WARNING, "read %d of %d bytes from webcam\n", r, l);
		if (r < 0)	/* read error */
			break;
		if (r == 0)	/* no data */
			break;
		b->used += r;
		if (r == l) {
			b->used = 0; /* prepare for next frame */
			return b;
		}
	}
	return NULL;
}

static void *grab_v4l1_close(void *desc)
{
	struct grab_v4l1_desc *v = desc;

	close(v->fd);
	v->fd = -1;
	fbuf_free(&v->b);
	ast_free(v);
	return NULL;
}

static struct grab_desc grab_v4l1_desc = {
	.name = "v4l1",
	.open = grab_v4l1_open,
	.read = grab_v4l1_read,
	.close = grab_v4l1_close,
};
#endif /* HAVE_VIDEODEV_H */

static struct grab_desc *my_grabbers[] = {
	&grab_x11_desc,
	&grab_v4l1_desc,
	NULL
};

/* try to open a video source, return 0 on success, 1 on error
 */
static int grabber_open(struct video_out_desc *v)
{
	struct grab_desc *g;
	void *g_data;
	int i;

	for (i = 0; (g = my_grabbers[i]); i++) {
		g_data = g->open(v->videodevice, &v->loc_src_geometry, v->fps);
		if (g_data) {
			v->grabber = g;
			v->grabber_data = g_data;
			return 0;
		}
	}
	return 1; /* no source found */
}

/*! \brief complete a buffer from the local video source.
 * Called by get_video_frames(), in turn called by the video thread.
 */
static struct fbuf_t *grabber_read(struct video_out_desc *v)
{
	struct timeval now = ast_tvnow();

	if (v->grabber == NULL) /* not initialized */
		return 0;

	/* check if it is time to read */
	if (ast_tvzero(v->last_frame))
		v->last_frame = now;
	if (ast_tvdiff_ms(now, v->last_frame) < 1000/v->fps)
		return 0;	/* too early */
	v->last_frame = now; /* XXX actually, should correct for drift */
	return v->grabber->read(v->grabber_data);
}

static void grabber_move(struct video_out_desc *v, int dx, int dy)
{
	if (v->grabber && v->grabber->move)
                v->grabber->move(v->grabber_data, dx, dy);
}

/* Helper function to process incoming video.
 * For each incoming video call invoke ffmpeg_init() to intialize
 * the decoding structure then incoming video frames are processed
 * by write_video() which in turn calls pre_process_data(), to extract
 * the bitstream; accumulates data into a buffer within video_desc. When
 * a frame is complete (determined by the marker bit in the RTP header)
 * call decode_video() to decoding and if it successful call show_frame()
 * to display the frame.
 */

/*
 * Map the codec name to the library. If not recognised, use a default.
 * This is useful in the output path where we decide by name, presumably.
 */
static struct video_codec_desc *map_config_video_format(char *name)
{
	int i;

	for (i = 0; supported_codecs[i]; i++)
		if (!strcasecmp(name, supported_codecs[i]->name))
			break;
	if (supported_codecs[i] == NULL) {
		ast_log(LOG_WARNING, "Cannot find codec for '%s'\n", name);
		i = 0;
		strcpy(name, supported_codecs[i]->name);
	}
	ast_log(LOG_WARNING, "Using codec '%s'\n", name);
	return supported_codecs[i];
}


/*! \brief uninitialize the descriptor for local video stream */
static int video_out_uninit(struct video_desc *env)
{
	struct video_out_desc *v = &env->out;

	/* XXX this should be a codec callback */
	if (v->enc_ctx) {
		AVCodecContext *enc_ctx = (AVCodecContext *)v->enc_ctx;
		avcodec_close(enc_ctx);
		av_free(enc_ctx);
		v->enc_ctx = NULL;
	}
	if (v->enc_in_frame) {
		av_free(v->enc_in_frame);
		v->enc_in_frame = NULL;
	}
	v->codec = NULL;	/* nothing to free, this is only a reference */
	/* release the buffers */
	fbuf_free(&env->enc_in);
	fbuf_free(&v->enc_out);
	/* close the grabber */
	sleep(1);
	if (v->grabber) {
		v->grabber_data = v->grabber->close(v->grabber_data);
		v->grabber = NULL;
	}
	return -1;
}

/*
 * Initialize the encoder for the local source:
 * - AVCodecContext, AVCodec, AVFrame are used by ffmpeg for encoding;
 * - encbuf is used to store the encoded frame (to be sent)
 * - mtu is used to determine the max size of video fragment
 * NOTE: we enter here with the video source already open.
 */
static int video_out_init(struct video_desc *env)
{
	int codec;
	int size;
	struct fbuf_t *enc_in;
	struct video_out_desc *v = &env->out;

	v->enc_ctx		= NULL;
	v->codec		= NULL;
	v->enc_in_frame		= NULL;
	v->enc_out.data		= NULL;

	codec = map_video_format(v->enc->format, CM_WR);
	v->codec = avcodec_find_encoder(codec);
	if (!v->codec) {
		ast_log(LOG_WARNING, "Cannot find the encoder for format %d\n",
			codec);
		return -1;	/* error, but nothing to undo yet */
	}

	v->mtu = 1400;	/* set it early so the encoder can use it */

	/* allocate the input buffer for encoding.
	 * ffmpeg only supports PIX_FMT_YUV420P for the encoding.
	 */
	enc_in = &env->enc_in;
	enc_in->pix_fmt = PIX_FMT_YUV420P;
	enc_in->size = (enc_in->w * enc_in->h * 3)/2;
	enc_in->data = ast_calloc(1, enc_in->size);
	if (!enc_in->data) {
		ast_log(LOG_WARNING, "Cannot allocate encoder input buffer\n");
		return video_out_uninit(env);
	}
	/* construct an AVFrame that points into buf_in */
	v->enc_in_frame = avcodec_alloc_frame();
	if (!v->enc_in_frame) {
		ast_log(LOG_WARNING, "Unable to allocate the encoding video frame\n");
		return video_out_uninit(env);
	}

	/* parameters for PIX_FMT_YUV420P */
	size = enc_in->w * enc_in->h;
	v->enc_in_frame->data[0] = enc_in->data;
	v->enc_in_frame->data[1] = v->enc_in_frame->data[0] + size;
	v->enc_in_frame->data[2] = v->enc_in_frame->data[1] + size/4;
	v->enc_in_frame->linesize[0] = enc_in->w;
	v->enc_in_frame->linesize[1] = enc_in->w/2;
	v->enc_in_frame->linesize[2] = enc_in->w/2;

	/* now setup the parameters for the encoder.
	 * XXX should be codec-specific
	 */
    {
	AVCodecContext *enc_ctx = avcodec_alloc_context();
	v->enc_ctx = enc_ctx;
	enc_ctx->pix_fmt = enc_in->pix_fmt;
	enc_ctx->width = enc_in->w;
	enc_ctx->height = enc_in->h;
	/* XXX rtp_callback ?
	 * rtp_mode so ffmpeg inserts as many start codes as possible.
	 */
	enc_ctx->rtp_mode = 1;
	enc_ctx->rtp_payload_size = v->mtu / 2; // mtu/2
	enc_ctx->bit_rate = v->bitrate;
	enc_ctx->bit_rate_tolerance = enc_ctx->bit_rate/2;
	enc_ctx->qmin = v->qmin;	/* should be configured */
	enc_ctx->time_base = (AVRational){1, v->fps};
	enc_ctx->gop_size = v->fps*5; // emit I frame every 5 seconds

	v->enc->enc_init(v->enc_ctx);
 
	if (avcodec_open(enc_ctx, v->codec) < 0) {
		ast_log(LOG_WARNING, "Unable to initialize the encoder %d\n",
			codec);
		av_free(enc_ctx);
		v->enc_ctx = NULL;
		return video_out_uninit(env);
	}
    }
	/*
	 * Allocate enough for the encoded bitstream. As we are compressing,
	 * we hope that the output is never larger than the input size.
	 */
	v->enc_out.data = ast_calloc(1, enc_in->size);
	v->enc_out.size = enc_in->size;
	v->enc_out.used = 0;

	return 0;
}

/*! \brief uninitialize the entire environment.
 * In practice, signal the thread and give it a bit of time to
 * complete, giving up if it gets stuck. Because uninit
 * is called from hangup with the channel locked, and the thread
 * uses the chan lock, we need to unlock here. This is unsafe,
 * and we should really use refcounts for the channels.
 */
void console_video_uninit(struct video_desc *env)
{
	int i, t = 100;	/* initial wait is shorter, than make it longer */
	env->shutdown = 1;
	for (i=0; env->shutdown && i < 10; i++) {
                ast_channel_unlock(env->owner);
                usleep(t);
		t = 1000000;
                ast_channel_lock(env->owner);
        }
	env->owner = NULL;
}

/*! fill an AVPicture from our fbuf info, as it is required by
 * the image conversion routines in ffmpeg.
 * XXX This depends on the format.
 */
static AVPicture *fill_pict(struct fbuf_t *b, AVPicture *p)
{
	/* provide defaults for commonly used formats */
	int l4 = b->w * b->h/4; /* size of U or V frame */
	int len = b->w;		/* Y linesize, bytes */
	int luv = b->w/2;	/* U/V linesize, bytes */

	bzero(p, sizeof(*p));
	switch (b->pix_fmt) {
	case PIX_FMT_RGB555:
	case PIX_FMT_RGB565:
		len *= 2;
		luv = 0;
		break;
	case PIX_FMT_RGBA32:
		len *= 4;
		luv = 0;
		break;
	case PIX_FMT_YUYV422:	/* Packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr */
		len *= 2;	/* all data in first plane, probably */
		luv = 0;
		break;
	}
	p->data[0] = b->data;
	p->linesize[0] = len;
	/* these are only valid for component images */
	p->data[1] = luv ? b->data + 4*l4 : b->data+len;
	p->data[2] = luv ? b->data + 5*l4 : b->data+len;
	p->linesize[1] = luv;
	p->linesize[2] = luv;
	return p;
}

/*! convert/scale between an input and an output format.
 * Old version of ffmpeg only have img_convert, which does not rescale.
 * New versions use sws_scale which does both.
 */
static void my_scale(struct fbuf_t *in, AVPicture *p_in,
	struct fbuf_t *out, AVPicture *p_out)
{
	AVPicture my_p_in, my_p_out;

	if (p_in == NULL)
		p_in = fill_pict(in, &my_p_in);
	if (p_out == NULL)
		p_out = fill_pict(out, &my_p_out);

#ifdef OLD_FFMPEG
	/* XXX img_convert is deprecated, and does not do rescaling */
	img_convert(p_out, out->pix_fmt,
		p_in, in->pix_fmt, in->w, in->h);
#else /* XXX replacement */
    {
	struct SwsContext *convert_ctx;

	convert_ctx = sws_getContext(in->w, in->h, in->pix_fmt,
		out->w, out->h, out->pix_fmt,
		SWS_BICUBIC, NULL, NULL, NULL);
	if (convert_ctx == NULL) {
		ast_log(LOG_ERROR, "FFMPEG::convert_cmodel : swscale context initialization failed");
		return;
	}
	if (0)
		ast_log(LOG_WARNING, "in %d %dx%d out %d %dx%d\n",
			in->pix_fmt, in->w, in->h, out->pix_fmt, out->w, out->h);
	sws_scale(convert_ctx,
		p_in->data, p_in->linesize,
		in->w, in->h, /* src slice */
		p_out->data, p_out->linesize);

	sws_freeContext(convert_ctx);
    }
#endif /* XXX replacement */
}

struct video_desc *get_video_desc(struct ast_channel *c);

/*
 * This function is called (by asterisk) for each video packet
 * coming from the network (the 'in' path) that needs to be processed.
 * We need to reconstruct the entire video frame before we can decode it.
 * After a video packet is received we have to:
 * - extract the bitstream with pre_process_data()
 * - append the bitstream to a buffer
 * - if the fragment is the last (RTP Marker) we decode it with decode_video()
 * - after the decoding is completed we display the decoded frame with show_frame()
 */
int console_write_video(struct ast_channel *chan, struct ast_frame *f);
int console_write_video(struct ast_channel *chan, struct ast_frame *f)
{
	struct video_desc *env = get_video_desc(chan);
	struct video_dec_desc *v = env->in;

	if (!env->gui)	/* no gui, no rendering */
		return 0;
	if (v == NULL)
		env->in = v = dec_init(f->subclass & ~1);
	if (v == NULL) {
		/* This is not fatal, but we won't have incoming video */
		ast_log(LOG_WARNING, "Cannot initialize input decoder\n");
		return 0;
	}

	if (v->dec_in_cur == NULL)	/* no buffer for incoming frames, drop */
		return 0;
#if defined(DROP_PACKETS) && DROP_PACKETS > 0
	/* Simulate lost packets */
	if ((random() % 10000) <= 100*DROP_PACKETS) {
		ast_log(LOG_NOTICE, "Packet lost [%d]\n", f->seqno);
		return 0;
	}
#endif
	if (v->discard) {
		/*
		 * In discard mode, drop packets until we find one with
		 * the RTP marker set (which is the end of frame).
		 * Note that the RTP marker flag is sent as the LSB of the
		 * subclass, which is a  bitmask of formats. The low bit is
		 * normally used for audio so there is no interference.
		 */
		if (f->subclass & 0x01) {
			v->dec_in_cur->used = 0;
			v->dec_in_cur->ebit = 0;
			v->next_seq = f->seqno + 1;	/* wrap at 16 bit */
			v->discard = 0;
			ast_log(LOG_WARNING, "out of discard mode, frame %d\n", f->seqno);
		}
		return 0;
	}

	/*
	 * Only in-order fragments will be accepted. Remember seqno
	 * has 16 bit so there is wraparound. Also, ideally we could
	 * accept a bit of reordering, but at the moment we don't.
	 */
	if (v->next_seq != f->seqno) {
		ast_log(LOG_WARNING, "discarding frame out of order, %d %d\n",
			v->next_seq, f->seqno);
		v->discard = 1;
		return 0;
	}
	v->next_seq++;

	if (f->data == NULL || f->datalen < 2) {
		ast_log(LOG_WARNING, "empty video frame, discard\n");
		return 0;
	}
	if (v->d_callbacks->dec_decap(v->dec_in_cur, f->data, f->datalen)) {
		ast_log(LOG_WARNING, "error in dec_decap, enter discard\n");
		v->discard = 1;
	}
	if (f->subclass & 0x01) {	// RTP Marker
		/* prepare to decode: advance the buffer so the video thread knows. */
		struct fbuf_t *tmp = v->dec_in_cur;	/* store current pointer */
		ast_mutex_lock(&env->dec_lock);
		if (++v->dec_in_cur == &v->dec_in[N_DEC_IN])	/* advance to next, circular */
			v->dec_in_cur = &v->dec_in[0];
		if (v->dec_in_dpy == NULL) {	/* were not displaying anything, so set it */
			v->dec_in_dpy = tmp;
		} else if (v->dec_in_dpy == v->dec_in_cur) { /* current slot is busy */
			v->dec_in_cur = NULL;
		}
		ast_mutex_unlock(&env->dec_lock);
	}
	return 0;
}


/*! \brief read a frame from webcam or X11 through grabber_read(),
 * display it,  then encode and split it.
 * Return a list of ast_frame representing the video fragments.
 * The head pointer is returned by the function, the tail pointer
 * is returned as an argument.
 */
static struct ast_frame *get_video_frames(struct video_desc *env, struct ast_frame **tail)
{
	struct video_out_desc *v = &env->out;
	struct ast_frame *dummy;
	struct fbuf_t *loc_src = grabber_read(v);

	if (!loc_src)
		return NULL;	/* can happen, e.g. we are reading too early */

	if (tail == NULL)
		tail = &dummy;
	*tail = NULL;
	/* Scale the video for the encoder, then use it for local rendering
	 * so we will see the same as the remote party.
	 */
	my_scale(loc_src, NULL, &env->enc_in, NULL);
	show_frame(env, WIN_LOCAL);
	if (!v->sendvideo)
		return NULL;
	if (v->enc_out.data == NULL) {
		static volatile int a = 0;
		if (a++ < 2)
			ast_log(LOG_WARNING, "fail, no encbuf\n");
		return NULL;
	}
	v->enc->enc_run(v);
	return v->enc->enc_encap(&v->enc_out, v->mtu, tail);
}

/*
 * Helper thread to periodically poll the video source and enqueue the
 * generated frames to the channel's queue.
 * Using a separate thread also helps because the encoding can be
 * computationally expensive so we don't want to starve the main thread.
 */
static void *video_thread(void *arg)
{
	struct video_desc *env = arg;
	int count = 0;
	char save_display[128] = "";

	/* if sdl_videodriver is set, override the environment. Also,
	 * if it contains 'console' override DISPLAY around the call to SDL_Init
	 * so we use the console as opposed to the x11 version of aalib
	 */
	if (!ast_strlen_zero(env->sdl_videodriver)) { /* override */
		const char *s = getenv("DISPLAY");
		setenv("SDL_VIDEODRIVER", env->sdl_videodriver, 1);
		if (s && !strcasecmp(env->sdl_videodriver, "aalib-console")) {
			ast_copy_string(save_display, s, sizeof(save_display));
			unsetenv("DISPLAY");
		}
	}
	sdl_setup(env);
	if (!ast_strlen_zero(save_display))
		setenv("DISPLAY", save_display, 1);

        /* initialize grab coordinates */
        env->out.loc_src_geometry.x = 0;
        env->out.loc_src_geometry.y = 0;

	ast_mutex_init(&env->dec_lock);	/* used to sync decoder and renderer */

	if (grabber_open(&env->out)) {
		ast_log(LOG_WARNING, "cannot open local video source\n");
	} else {
#if 0
		/* In principle, try to register the fd.
		 * In practice, many webcam drivers do not support select/poll,
		 * so don't bother and instead read periodically from the
		 * video thread.
		 */
		if (env->out.fd >= 0)
			ast_channel_set_fd(env->owner, 1, env->out.fd);
#endif
		video_out_init(env);
	}

	for (;;) {
		struct timeval t = { 0, 50000 };	/* XXX 20 times/sec */
		struct ast_frame *p, *f;
		struct ast_channel *chan = env->owner;
		int fd = chan->alertpipe[1];
		char *caption = NULL, buf[160];

		/* determine if video format changed */
		if (count++ % 10 == 0) {
			if (env->out.sendvideo)
			    sprintf(buf, "%s %s %dx%d @@ %dfps %dkbps",
				env->out.videodevice, env->codec_name,
				env->enc_in.w, env->enc_in.h,
				env->out.fps, env->out.bitrate/1000);
			else
			    sprintf(buf, "hold");
			caption = buf;
		}

		/* manage keypad events */
		/* XXX here we should always check for events,
		* otherwise the drag will not work */ 
		if (env->gui)
			eventhandler(env, caption);
 
		/* sleep for a while */
		ast_select(0, NULL, NULL, NULL, &t);

	    if (env->in) {
		struct video_dec_desc *v = env->in;
		
		/*
		 * While there is something to display, call the decoder and free
		 * the buffer, possibly enabling the receiver to store new data.
		 */
		while (v->dec_in_dpy) {
			struct fbuf_t *tmp = v->dec_in_dpy;	/* store current pointer */

			if (v->d_callbacks->dec_run(v, tmp))
				show_frame(env, WIN_REMOTE);
			tmp->used = 0;	/* mark buffer as free */
			tmp->ebit = 0;
			ast_mutex_lock(&env->dec_lock);
			if (++v->dec_in_dpy == &v->dec_in[N_DEC_IN])	/* advance to next, circular */
				v->dec_in_dpy = &v->dec_in[0];

			if (v->dec_in_cur == NULL)	/* receiver was idle, enable it... */
				v->dec_in_cur = tmp;	/* using the slot just freed */
			else if (v->dec_in_dpy == v->dec_in_cur) /* this was the last slot */
				v->dec_in_dpy = NULL;	/* nothing more to display */
			ast_mutex_unlock(&env->dec_lock);
		}
	    }

		if (env->shutdown)
			break;
		f = get_video_frames(env, &p);	/* read and display */
		if (!f)
			continue;
		chan = env->owner;
		ast_channel_lock(chan);

		/* AST_LIST_INSERT_TAIL is only good for one frame, cannot use here */
		if (chan->readq.first == NULL) {
			chan->readq.first = f;
		} else {
			chan->readq.last->frame_list.next = f;
		}
		chan->readq.last = p;
		/*
		 * more or less same as ast_queue_frame, but extra
		 * write on the alertpipe to signal frames.
		 */
		if (fd > -1) {
			int blah = 1, l = sizeof(blah);
			for (p = f; p; p = AST_LIST_NEXT(p, frame_list)) {
				if (write(fd, &blah, l) != l)
					ast_log(LOG_WARNING, "Unable to write to alert pipe on %s, frametype/subclass %d/%d: %s!\n",
					    chan->name, f->frametype, f->subclass, strerror(errno));
			}
		}
		ast_channel_unlock(chan);
	}
	/* thread terminating, here could call the uninit */
	/* uninitialize the local and remote video environments */
	env->in = dec_uninit(env->in);
	video_out_uninit(env);

	if (env->gui)
		env->gui = cleanup_sdl(env->gui);
	ast_mutex_destroy(&env->dec_lock);
	env->shutdown = 0;
	return NULL;
}

static void copy_geometry(struct fbuf_t *src, struct fbuf_t *dst)
{
	if (dst->w == 0)
		dst->w = src->w;
	if (dst->h == 0)
		dst->h = src->h;
}

/*! initialize the video environment.
 * Apart from the formats (constant) used by sdl and the codec,
 * we use enc_in as the basic geometry.
 */
static void init_env(struct video_desc *env)
{
	struct fbuf_t *c = &(env->out.loc_src_geometry);		/* local source */
	struct fbuf_t *ei = &(env->enc_in);		/* encoder input */
	struct fbuf_t *ld = &(env->loc_dpy);	/* local display */
	struct fbuf_t *rd = &(env->rem_dpy);		/* remote display */

	c->pix_fmt = PIX_FMT_YUV420P;	/* default - camera format */
	ei->pix_fmt = PIX_FMT_YUV420P;	/* encoder input */
	if (ei->w == 0 || ei->h == 0) {
		ei->w = 352;
		ei->h = 288;
	}
	ld->pix_fmt = rd->pix_fmt = PIX_FMT_YUV420P; /* sdl format */
	/* inherit defaults */
	copy_geometry(ei, c);	/* camera inherits from encoder input */
	copy_geometry(ei, rd);	/* remote display inherits from encoder input */
	copy_geometry(rd, ld);	/* local display inherits from remote display */
}

/*!
 * The first call to the video code, called by oss_new() or similar.
 * Here we initialize the various components we use, namely SDL for display,
 * ffmpeg for encoding/decoding, and a local video source.
 * We do our best to progress even if some of the components are not
 * available.
 */
void console_video_start(struct video_desc *env, struct ast_channel *owner)
{
	if (env == NULL)	/* video not initialized */
		return;
	if (owner == NULL)	/* nothing to do if we don't have a channel */
		return;
	env->owner = owner;
	init_env(env);
	env->out.enc = map_config_video_format(env->codec_name);

	ast_log(LOG_WARNING, "start video out %s %dx%d\n",
		env->codec_name, env->enc_in.w,  env->enc_in.h);
	/*
	 * Register all codecs supported by the ffmpeg library.
	 * We only need to do it once, but probably doesn't
	 * harm to do it multiple times.
	 */
	avcodec_init();
	avcodec_register_all();
	av_log_set_level(AV_LOG_ERROR);	/* only report errors */

	if (env->out.fps == 0) {
		env->out.fps = 15;
		ast_log(LOG_WARNING, "fps unset, forcing to %d\n", env->out.fps);
	}
	if (env->out.bitrate == 0) {
		env->out.bitrate = 65000;
		ast_log(LOG_WARNING, "bitrate unset, forcing to %d\n", env->out.bitrate);
	}

	ast_pthread_create_background(&env->vthread, NULL, video_thread, env);
}

/*
 * Parse a geometry string, accepting also common names for the formats.
 * Trick: if we have a leading > or < and a numeric geometry,
 * return the larger or smaller one.
 * E.g. <352x288 gives the smaller one, 320x240
 */
static int video_geom(struct fbuf_t *b, const char *s)
{
	int w = 0, h = 0;

	static struct {
		const char *s; int w; int h;
	} *fp, formats[] = {
		{"16cif",	1408, 1152 },
		{"xga",		1024, 768 },
		{"4cif",	704, 576 },
		{"vga",		640, 480 },
		{"cif",		352, 288 },
		{"qvga",	320, 240 },
		{"qcif",	176, 144 },
		{"sqcif",	128, 96 },
		{NULL,		0, 0 },
	};
	if (*s == '<' || *s == '>')
		sscanf(s+1,"%dx%d", &w, &h);
	for (fp = formats; fp->s; fp++) {
		if (*s == '>') {	/* look for a larger one */
			if (fp->w <= w) {
				if (fp > formats)
					fp--; /* back one step if possible */
				break;
			}
		} else if (*s == '<') {	/* look for a smaller one */
			if (fp->w < w)
				break;
		} else if (!strcasecmp(s, fp->s)) { /* look for a string */
			break;
		}
	}
	if (*s == '<' && fp->s == NULL)	/* smallest */
		fp--;
	if (fp->s) {
		b->w = fp->w;
		b->h = fp->h;
	} else if (sscanf(s, "%dx%d", &b->w, &b->h) != 2) {
		ast_log(LOG_WARNING, "Invalid video_size %s, using 352x288\n", s);
		b->w = 352;
		b->h = 288;
	}
	return 0;
}

/* extend ast_cli with video commands. Called by console_video_config */
int console_video_cli(struct video_desc *env, const char *var, int fd)
{
	if (env == NULL)
		return 1;	/* unrecognised */

        if (!strcasecmp(var, "videodevice")) {
		ast_cli(fd, "videodevice is [%s]\n", env->out.videodevice);
        } else if (!strcasecmp(var, "videocodec")) {
		ast_cli(fd, "videocodec is [%s]\n", env->codec_name);
        } else if (!strcasecmp(var, "sendvideo")) {
		ast_cli(fd, "sendvideo is [%s]\n", env->out.sendvideo ? "on" : "off");
        } else if (!strcasecmp(var, "video_size")) {
		int in_w = 0, in_h = 0;
		if (env->in) {
			in_w = env->in->dec_out.w;
			in_h = env->in->dec_out.h;
		}
		ast_cli(fd, "sizes: video %dx%d camera %dx%d local %dx%d remote %dx%d in %dx%d\n",
			env->enc_in.w, env->enc_in.h,
			env->out.loc_src_geometry.w, env->out.loc_src_geometry.h,
			env->loc_dpy.w, env->loc_dpy.h,
			env->rem_dpy.w, env->rem_dpy.h,
			in_w, in_h);
        } else if (!strcasecmp(var, "bitrate")) {
		ast_cli(fd, "bitrate is [%d]\n", env->out.bitrate);
        } else if (!strcasecmp(var, "qmin")) {
		ast_cli(fd, "qmin is [%d]\n", env->out.qmin);
        } else if (!strcasecmp(var, "fps")) {
		ast_cli(fd, "fps is [%d]\n", env->out.fps);
        } else {
		return 1;	/* unrecognised */
	}
	return 0;	/* recognised */
}

/*! parse config command for video support. */
int console_video_config(struct video_desc **penv,
	const char *var, const char *val)
{
	struct video_desc *env;

	if (penv == NULL) {
		ast_log(LOG_WARNING, "bad argument penv=NULL\n");
		return 1;	/* error */
	}
	/* allocate the video descriptor first time we get here */
	env = *penv;
	if (env == NULL) {
		env = *penv = ast_calloc(1, sizeof(struct video_desc));
		if (env == NULL) {
			ast_log(LOG_WARNING, "fail to allocate video_desc\n");
			return 1;	/* error */
		
		}
		/* set default values */
		ast_copy_string(env->out.videodevice, "X11", sizeof(env->out.videodevice));
		env->out.fps = 5;
		env->out.bitrate = 65000;
		env->out.sendvideo = 1;
		env->out.qmin = 3;
	}
	CV_START(var, val);
	CV_STR("videodevice", env->out.videodevice);
	CV_BOOL("sendvideo", env->out.sendvideo);
	CV_F("video_size", video_geom(&env->enc_in, val));
	CV_F("camera_size", video_geom(&env->out.loc_src_geometry, val));
	CV_F("local_size", video_geom(&env->loc_dpy, val));
	CV_F("remote_size", video_geom(&env->rem_dpy, val));
	CV_STR("keypad", env->keypad_file);
	CV_F("region", keypad_cfg_read(env->gui, val));
	CV_STR("keypad_font", env->keypad_font);
	CV_STR("sdl_videodriver", env->sdl_videodriver);
	CV_UINT("fps", env->out.fps);
	CV_UINT("bitrate", env->out.bitrate);
	CV_UINT("qmin", env->out.qmin);
	CV_STR("videocodec", env->codec_name);
	return 1;	/* nothing found */

	CV_END;		/* the 'nothing found' case */
	return 0;		/* found something */
}

#endif	/* video support */
