#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/platform_device.h>
#include <linux/videodev2.h>
// #include <linux/stacktrace.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/freezer.h>
#include <media/v4l2-device.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("river");

#define MAX_WIDTH 1920
#define MAX_HEIGHT 1200

struct pic_format {
	__u32	width;
	__u32	height;
	__u32	pixelsize;
	__u32	field;
	__u32	fourcc;
	__u32	depth;
};

/* buffer for one video frame */
struct vivi_buffer {
	/* common v4l buffer stuff -- must be first */
	struct vb2_buffer	vb;
	struct list_head	list;
};

/**/
struct vivi_dmaqueue {
	/* active list */
	struct list_head active;

	/* thread for generating video stream */
	struct task_struct *kthread;
	struct wait_queue_head wq;
};

/**
 * struct vivi_dev - All internal data for one instance of device
 * @v4l2_dev: top-level v4l2 device struct
 * @vdev: video node structure
 * @ctrl_handler: control handler structure
 * @lock: ioctl serialization mutex
 * @std: current SDTV standard
 * @timings: current HDTV timings
 * @format: current pix format
 * @input: current video input (0 = SDTV, 1 = HDTV)
 * @queue: vb2 video capture queue
 * @qlock: spinlock controlling access to buf_list and sequence
 * @buf_list: list of buffers queued for DMA
 * @sequence: frame sequence counter
 */
struct vivi_dev {
	struct v4l2_device	v4l2_dev;
	struct video_device	vdev;
	struct pic_format	fmt;

	struct vb2_queue	queue;

	spinlock_t	slock;
	struct mutex		lock;

	struct vivi_dmaqueue	dmaq;
};

DECLARE_WAIT_QUEUE_HEAD(wait_queue_head);

static void vivi_pdev_release(struct device *dev)
{
	printk(KERN_ALERT "vivi platform device release\n");
}

static struct platform_device vivi_pdev = {
	.name		= "vivi",
	.dev.release	= vivi_pdev_release,
};

/* called from VIDIOC_REQBUFS() and VIDIOC_CREATE_BUFS()
 * prepare buffers,
 * @num_buffers - buffer count
 * @num_planes - plane number
 * @sizes - plane image size
 * @alloc_devs[] -
 */
static int queue_setup(struct vb2_queue *vq,
			unsigned int *num_buffers, unsigned int *num_planes,
			unsigned int sizes[], struct device *alloc_devs[])
{
	unsigned long size;
	struct vivi_dev *dev = vb2_get_drv_priv(vq);

	/* prepare 32 buffers for video queue */
	size = dev->fmt.width * dev->fmt.height * dev->fmt.pixelsize;

	if (*num_buffers == 0)
		*num_buffers = 32;

	*num_planes = 1;
	sizes[0] = size;

	printk(KERN_ALERT "queue_setup, num_planes = %d, sizes[0] = %d\n",
				*num_planes, sizes[0]);

	return 0;
}

/*
 * Prepare the buffer for queueing to the DMA engine: check and set the
 * payload size.
 */
static int buffer_prepare(struct vb2_buffer *vb)
{
	unsigned long size;
	unsigned long plane_size;

	struct vivi_dev *dev = vb2_get_drv_priv(vb->vb2_queue);

	size = dev->fmt.width * dev->fmt.height * dev->fmt.pixelsize;

	plane_size = vb2_plane_size(vb, 0);

	printk(KERN_ALERT "plane_size = %lu, size = %lu\n", plane_size, size);

	if (plane_size < size) {
		printk(KERN_ERR "buffer size too small (%lu < %lu)\n",
		plane_size, size);
		return -EINVAL;
	}

	vb2_set_plane_payload(vb, 0, size);

	return 0;
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vivi_dev *dev = vb2_get_drv_priv(vb->vb2_queue);
	struct vivi_buffer *buf = container_of(vb, struct vivi_buffer, vb);
	struct vivi_dmaqueue *dma_q = &dev->dmaq;
	unsigned long flags = 0;

	/* directly move data info to dma_q */
	spin_lock_irqsave(&dev->slock, flags);
	list_add_tail(&buf->list, &dma_q->active);
	spin_unlock_irqrestore(&dev->slock, flags);

	printk(KERN_ALERT "enqueue buffer to vivi dma queue\n");

	return;
}

/* fill frame buffer, just color bwtween [0,0,0] ~ [ff,ff,ff]*/
static void vivi_fillbuf(struct vivi_dev *dev, struct vivi_buffer *buf)
{
	void *vbuf = NULL;
	int width = dev->fmt.width;
	int height = dev->fmt.height;
	int stride = width * dev->fmt.pixelsize;
	unsigned int j;
	unsigned char *p;
	int percent = 0;
	unsigned char color = 0x00;
	static unsigned char base = 0x00;

	vbuf = vb2_plane_vaddr(&buf->vb, 0);
	p = vbuf;

	printk("width = %d, height = %d, stride = %d\n",
			width, height, stride);

	if (base >= 0xff)
		base = 0x00;

	/* V4L2_PIX_FMT_YUYV - packed format with 1/2 horizontal chroma resolution,
	 * also known as YUV 4:2:2...
	 */
	for (j = 0; j < height; j++) {
		percent = (j*0xff)/height;
		color = base + percent;

		if (color >= 0xff)
			color -= 0xff;

		// printk(KERN_ALERT "color: 0x%x\n", color);

		/* fill buffer for 1 line */
		memset(p, color, stride);
		p += stride;
	}

	base += 0x2;
}

/* kernel thread wake up and invoke it */
static void vivi_thread_tick(struct vivi_dev *dev)
{
	struct vivi_buffer *buf = NULL;
	unsigned long flags = 0;

	printk(KERN_ALERT "vivi_thread_tick...\n");

	spin_lock_irqsave(&dev->slock, flags);
	if (list_empty(&dev->dmaq.active)) {
		printk(KERN_ALERT "No active queue to serve\n");
		spin_unlock_irqrestore(&dev->slock, flags);
		return;
	}

	/* get the dmaq's first entry */
	buf = list_first_entry(&dev->dmaq.active, struct vivi_buffer, list);
	list_del(&buf->list);
	spin_unlock_irqrestore(&dev->slock, flags);

	printk(KERN_ALERT "fill vivi_buffer...\n");

	/* fill data */
	vivi_fillbuf(dev, buf);

	/* data process done, copy data to done list */
	vb2_buffer_done(&buf->vb, VB2_BUF_STATE_DONE);
	printk(KERN_ALERT "filled buffer %p, and move it to done_list \n", buf->vb.planes[0].mem_priv);
}

#define WAKE_NUMERATOR 30
#define WAKE_DENOMINATOR 1001
#define BUFFER_TIMEOUT		msecs_to_jiffis(500) /* 0.5 seconds */
#define frames_to_ms(frames)			\
		((frames * WAKE_NUMERATOR * 1000) / WAKE_DENOMINATOR)
static void vivi_routine(struct vivi_dev *dev)
{
	struct vivi_dmaqueue *dma_q = &dev->dmaq;
	int timeout;
	DECLARE_WAITQUEUE(wait, current);

	add_wait_queue(&dma_q->wq, &wait);
	if (kthread_should_stop())
		goto stop_task;

	printk(KERN_ALERT "vivi_routine\n");

	/* Calculate time to wake up */
	timeout = msecs_to_jiffies(frames_to_ms(1));
	printk(KERN_ALERT "timeout = %d\n", timeout);

	vivi_thread_tick(dev);

	schedule_timeout_interruptible(timeout);

stop_task:
	remove_wait_queue(&wait_queue_head, &wait);
	try_to_freeze();
}

static int vivi_thread(void *data)
{
	struct vivi_dev *dev = (struct vivi_dev *)data;

	set_freezable();
	for (;;) {
		vivi_routine(dev);

		if (kthread_should_stop())
			break;
	}

	printk(KERN_ALERT "thread: exit\n");

	return 0;
}

static int vivi_start_generating(struct vivi_dev *dev)
{
	struct vivi_dmaqueue *dma_q = &dev->dmaq;

	dma_q->kthread = kthread_run(vivi_thread, dev, "vivi");

	if (IS_ERR(dma_q->kthread)) {
		printk(KERN_ERR "kthread_run error\n");
		return PTR_ERR(dma_q->kthread);
	}

	/* wakes thread */
	wake_up_interruptible(&dma_q->wq);

	return 0;
}

/* start vivi streaming,
 * launch a kthread and fill buffer in queued_list
 * and move it to done_list.
 */
static int start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct vivi_dev *dev = vb2_get_drv_priv(vq);
	int err;

	err = vivi_start_generating(dev);

	/* if start generating failed, flush dmaq data to done_list */
	if (err) {
		struct vivi_buffer *buf, *tmp;

		list_for_each_entry_safe(buf, tmp, &dev->dmaq.active, list) {
			list_del(&buf->list);
			vb2_buffer_done(&buf->vb,
					VB2_BUF_STATE_QUEUED);
		}
	}

	printk(KERN_ALERT "start_streaming\n");

	return 0;
}

/* stop vivi streaming,
 * stop kthread.
 */
static void stop_streaming(struct vb2_queue *vq)
{
	struct vivi_dev *dev = vb2_get_drv_priv(vq);
	struct vivi_dmaqueue *dma_q = &dev->dmaq;

	/* shutdown control thread */
	if (dma_q->kthread) {
		kthread_stop(dma_q->kthread);
		dma_q->kthread = NULL;
	}

	/* release all active buffers */
	while (!list_empty(&dma_q->active)) {
		struct vivi_buffer *buf;
		buf = list_first_entry(&dma_q->active, struct vivi_buffer, list);
		list_del(&buf->list);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	printk(KERN_ALERT "stop_streaming\n");
}

/*
 * The vb2 queue ops. Note that since q->lock is set we can use the standard
 * vb2_ops_wait_prepare/finish helper functions. If q->lock would be NULL,
 * then this driver would have to provide these ops.
 */
static const struct vb2_ops vivi_qops = {
	.queue_setup		= queue_setup,
	.buf_prepare		= buffer_prepare,
	.buf_queue		= buffer_queue,
	.start_streaming	= start_streaming,
	.stop_streaming		= stop_streaming,
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
};

static const struct vb2_queue vivi_queue = {
	.type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
	.io_modes = VB2_MMAP,
	.buf_struct_size = sizeof(struct vivi_buffer),
	.ops = &vivi_qops,
	.mem_ops = &vb2_vmalloc_memops,
	.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC,
	.min_buffers_needed = 1,
};

/* query device capability */
static int vidioc_querycap(struct file *file, void *fh,
			struct v4l2_capability *cap)
{
	strlcpy(cap->driver, "vivi", sizeof(cap->driver));
	strlcpy(cap->card, "vivi", sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info), "vivi");
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS | V4L2_CAP_STREAMING;

	printk(KERN_ALERT "vivi_vidioc_querycap\n");

	// dump_stack();

	return 0;
}

/* enum supported format for the video device */
static int vidioc_enum_fmt_vid_cap(struct file *file, void *fh,
			struct v4l2_fmtdesc *f)
{
	struct vivi_dev *dev = video_drvdata(file);

	if (f->index >= 1)
		return -EINVAL;

	strlcpy(f->description, "vivi", sizeof(f->description));
	f->pixelformat = dev->fmt.fourcc;

	printk(KERN_ALERT "vivi_vidioc_enum_fmt\n");

	return 0;
}

/* re-size format according to vivi device capability */
static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
			struct v4l2_format *f)
{
	struct vivi_dev *dev = video_drvdata(file);

	if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
		return -EINVAL;

	f->fmt.pix.field = V4L2_FIELD_INTERLACED;
	v4l_bound_align_image(&f->fmt.pix.width, 48, MAX_WIDTH, 2,
			&f->fmt.pix.height, 32, MAX_HEIGHT, 0, 0);
	f->fmt.pix.bytesperline = (f->fmt.pix.width * dev->fmt.depth) / 8;
	f->fmt.pix.sizeimage = f->fmt.pix.height *f->fmt.pix.bytesperline;

	if (dev->fmt.fourcc == V4L2_PIX_FMT_YUYV)
		f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	else
		f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

	return 0;
}

/* set video format */
static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
			struct v4l2_format *f)
{
	int ret = 0;
	struct vivi_dev *dev = video_drvdata(file);

	ret = vidioc_try_fmt_vid_cap(file, priv, f);
	if (ret < 0)
		return ret;

	/* save the format setting from user space */
	dev->fmt.fourcc		= V4L2_PIX_FMT_YUYV;
	dev->fmt.pixelsize	= dev->fmt.depth / 8;
	dev->fmt.width		= f->fmt.pix.width;
	dev->fmt.height		= f->fmt.pix.height;
	dev->fmt.field		= f->fmt.pix.field;

	printk(KERN_ALERT "vidioc_s_fmt_vid_cap\n");

	return 0;
}

/* get supported video format */
static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
			struct v4l2_format *f) {
	struct vivi_dev *dev = video_drvdata(file);

	/* return format to user space */
	f->fmt.pix.width	= dev->fmt.width;
	f->fmt.pix.height	= dev->fmt.height;
	f->fmt.pix.field	= dev->fmt.field;
	f->fmt.pix.pixelformat	= dev->fmt.fourcc;
	f->fmt.pix.bytesperline = (dev->fmt.width * dev->fmt.depth) / 8;
	f->fmt.pix.sizeimage	= (dev->fmt.height * f->fmt.pix.bytesperline);
	if (dev->fmt.fourcc == V4L2_PIX_FMT_YUYV)
		f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	else
		f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

	printk(KERN_ALERT "vidioc_g_fmt_vid_cap\n");

	return 0;
}

static struct v4l2_ioctl_ops vivi_ioctl_ops = {
	.vidioc_querycap	= vidioc_querycap,

	.vidioc_enum_fmt_vid_cap = vidioc_enum_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap	= vidioc_s_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap	= vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap	= vidioc_try_fmt_vid_cap,

	.vidioc_reqbufs		= vb2_ioctl_reqbufs,
	.vidioc_create_bufs	= vb2_ioctl_create_bufs,
	.vidioc_querybuf	= vb2_ioctl_querybuf,
	.vidioc_qbuf		= vb2_ioctl_qbuf,
	.vidioc_dqbuf		= vb2_ioctl_dqbuf,
	.vidioc_expbuf		= vb2_ioctl_expbuf,

	.vidioc_streamon	= vb2_ioctl_streamon,
	.vidioc_streamoff	= vb2_ioctl_streamoff,
};

static const struct v4l2_file_operations vivi_fops = {
	.owner = THIS_MODULE,
	.open = v4l2_fh_open,
	.release = vb2_fop_release,
	.unlocked_ioctl = video_ioctl2,
	.read = vb2_fop_read,
	.mmap = vb2_fop_mmap,
	.poll = vb2_fop_poll,
};

static const struct video_device vivi_template = {
	.name = "vivi",
	.fops = &vivi_fops,
	.ioctl_ops = &vivi_ioctl_ops,
	.release = video_device_release_empty,
	.device_caps = V4L2_CAP_VIDEO_CAPTURE |
		       V4L2_CAP_READWRITE | V4L2_CAP_STREAMING,
};

static int vivi_probe(struct platform_device *pdev)
{
	struct vivi_dev *vivi;
	int ret = 0;
	printk(KERN_ALERT "vivi_probe\n");

	vivi = (struct vivi_dev *)devm_kzalloc(&pdev->dev,
			sizeof(struct vivi_dev), GFP_KERNEL);

	if (!vivi) {
		printk(KERN_ERR "allocate memory for vivi failed\n");
		ret = -ENOMEM;
		goto err;
	}

	/* Initialize the top-level structure */
	ret = v4l2_device_register(&pdev->dev, &vivi->v4l2_dev);
	if (ret < 0) {
		printk(KERN_ERR "v4l2_device regsiter fail, ret(%d)\n", ret);
		goto free_mem;
	}

	/* initialize locks */
	spin_lock_init(&vivi->slock);

	/* initialize the vb2 queue */
	mutex_init(&vivi->lock);
	vivi->queue = vivi_queue;
	vivi->queue.drv_priv = vivi;
	vivi->queue.lock = &vivi->lock;
	vivi->queue.dev = &pdev->dev;

	ret = vb2_queue_init(&vivi->queue);
	if (ret)
		goto remove_v4l2;

	/* init video dma queues */
	INIT_LIST_HEAD(&vivi->dmaq.active);
	init_waitqueue_head(&vivi->dmaq.wq);

	/* initialize the video_device structure */
	vivi->vdev = vivi_template;
	vivi->vdev.lock = &vivi->lock;
	vivi->vdev.queue = &vivi->queue;
	vivi->vdev.v4l2_dev = &vivi->v4l2_dev;
	video_set_drvdata(&vivi->vdev, vivi);

	/* default format */
	vivi->fmt.depth = 16;
	vivi->fmt.fourcc = V4L2_PIX_FMT_YUYV;

	/* register video device */
	ret = video_register_device(&vivi->vdev, VFL_TYPE_GRABBER, -1);
	if (ret < 0) {
		printk(KERN_ERR "video_device register failed, ret(%d)\n", ret);
		goto remove_v4l2;
	}

	return 0;

remove_v4l2:
	v4l2_device_unregister(&vivi->v4l2_dev);

free_mem:
	kfree(vivi);

err:
	return ret;
}

static int vivi_remove(struct platform_device *pdev)
{
	struct vivi_dev *dev = dev_get_drvdata(&pdev->dev);

	video_unregister_device(&dev->vdev);
	v4l2_device_unregister(&dev->v4l2_dev);

	printk(KERN_ALERT "vivi_remove\n");

	return 0;
}

static struct platform_driver vivi_pdrv = {
	.driver		= {
		.name	= "vivi",
	},
	.probe		= vivi_probe,
	.remove		= vivi_remove,
};

static int __init vivi_init(void)
{
	int ret = 0;

	printk (KERN_ALERT "vivi init succeed\n");

	ret = platform_device_register(&vivi_pdev);
	if (ret < 0) {
		printk(KERN_ERR "vivi platform_device regsiter failed\n");
		goto reg_pdev_fail;
	}

	ret = platform_driver_register(&vivi_pdrv);
	if (ret < 0) {
		printk(KERN_ERR "vivi platform_driver register failed\n");
		goto reg_pdrv_fail;
	}

	return 0;

reg_pdrv_fail:
	platform_device_unregister(&vivi_pdev);
reg_pdev_fail:

	return ret;
}

static void __exit vivi_exit(void)
{
	printk (KERN_ALERT "vivi exit succeed\n");

	platform_driver_unregister(&vivi_pdrv);

	platform_device_unregister(&vivi_pdev);

	return;
}

module_init(vivi_init);
module_exit(vivi_exit);
