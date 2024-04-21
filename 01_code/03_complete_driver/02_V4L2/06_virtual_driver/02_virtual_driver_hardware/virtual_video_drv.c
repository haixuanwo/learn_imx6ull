#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>

static struct list_head g_queued_bufs;

/* intermediate buffers with raw data from the USB device */
struct virtual_frame_buf {
	/* common v4l buffer stuff -- must be first */
	struct vb2_v4l2_buffer vb;
	struct list_head list;
};

static int virtual_querycap(struct file *file, void *fh,
		struct v4l2_capability *cap)
{
	strlcpy(cap->driver, "100ask_virtual_video", sizeof(cap->driver));
	strlcpy(cap->card, "no-card", sizeof(cap->card));
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_READWRITE ;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;

	return 0;
}

static int virtual_enum_fmt_cap(struct file *file, void *priv,
        struct v4l2_fmtdesc *f)
{
    if (f->index > 0)
        return -EINVAL;

    strlcpy(f->description, "100ask motion jpeg", sizeof(f->description));
    f->pixelformat = V4L2_PIX_FMT_MJPEG;

    return 0;
}

        
static int virtual_s_fmt_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    /* 分辨用户传入的参数是否可用
     * 如果不可用, 给APP提供最接近的、硬件支持的参数
     */

    if (f.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
        return -EINVAL;
    if (f.fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG)
        return -EINVAL;
    
    f.fmt.pix.width = 800;
    f.fmt.pix.height = 600;

    return 0;
}

static int virtual_enum_framesizes(struct file *file, void *fh,
                     struct v4l2_frmsizeenum *fsize)
{
    if (fsize->index > 0)
        return -EINVAL;

    fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
    fsize->discrete.width = 800;
    fsize->discrete.height = 600;
    return 0;
}


static const struct v4l2_file_operations virtual_fops = {
	.owner                    = THIS_MODULE,
	.open                     = v4l2_fh_open,
	.release                  = vb2_fop_release,
	.read                     = vb2_fop_read,
	.poll                     = vb2_fop_poll,
	.mmap                     = vb2_fop_mmap,
	.unlocked_ioctl           = video_ioctl2,
};

static const struct v4l2_ioctl_ops virtual_ioctl_ops = {
	.vidioc_querycap          = virtual_querycap,

	.vidioc_enum_fmt_vid_cap  = virtual_enum_fmt_cap,
	.vidioc_s_fmt_vid_cap     = virtual_s_fmt_cap,
	.vidioc_enum_framesizes   = virtual_enum_framesizes,

	.vidioc_reqbufs           = vb2_ioctl_reqbufs,
	.vidioc_create_bufs       = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf       = vb2_ioctl_prepare_buf,
	.vidioc_querybuf          = vb2_ioctl_querybuf,
	.vidioc_qbuf              = vb2_ioctl_qbuf,
	.vidioc_dqbuf             = vb2_ioctl_dqbuf,

	.vidioc_streamon          = vb2_ioctl_streamon,
	.vidioc_streamoff         = vb2_ioctl_streamoff,
};

static struct video_device g_vdev = {
	.name                     = "100ask_virtual_video",
	.release                  = video_device_release_empty,
	.fops                     = &virtual_fops,
	.ioctl_ops                = &virtual_ioctl_ops,
};
static struct v4l2_device g_v4l2_dev;

static struct vb2_queue g_vb_queue;

/* Videobuf2 operations */
static int virtual_queue_setup(struct vb2_queue *vq,
		unsigned int *nbuffers,
		unsigned int *nplanes, unsigned int sizes[], struct device *alloc_devs[])
{
    /* 假装:至少需要8个buffer, 每个buffer只有1个plane */

	/* Need at least 8 buffers */
	if (vq->num_buffers + *nbuffers < 8)
		*nbuffers = 8 - vq->num_buffers;
	*nplanes = 1;
	sizes[0] = PAGE_ALIGN(800*600*2);

	return 0;
}

        
static void virtual_buf_queue(struct vb2_buffer *vb)
{

    /* 把这个buffer告诉硬件相关的驱动程序 */

    struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
    struct virtual_frame_buf *buf =
            container_of(vbuf, struct virtual_frame_buf, vb);
    unsigned long flags;

    //spin_lock_irqsave(&s->queued_bufs_lock, flags);
    list_add_tail(&buf->list, &g_queued_bufs);
    //spin_unlock_irqrestore(&s->queued_bufs_lock, flags);
}

static int virtual_start_streaming(struct vb2_queue *vq, unsigned int count)
{
    /* 启动硬件传输 */
    return 0;
}

static void virtual_stop_streaming(struct vb2_queue *vq)
{
    /* 停止硬件传输 */
}


static const struct vb2_ops virtul_vb2_ops = {
	.queue_setup            = virtual_queue_setup,
	.buf_queue              = virtual_buf_queue,
	.start_streaming        = virtual_start_streaming,
	.stop_streaming         = virtual_stop_streaming,
	.wait_prepare           = vb2_ops_wait_prepare,
	.wait_finish            = vb2_ops_wait_finish,
};


static int virtual_video_drv_init(void)
{
    int ret;
    
    /* 分配/设置/注册video_device */

    /* 设置
     * 1. 函数调用(比如ioctl)
     * 2. 队列/buffer的管理
     */

    g_vb_queue.type = V4L2_BUF_TYPE_SDR_CAPTURE;
    g_vb_queue.io_modes = VB2_MMAP | VB2_USERPTR | VB2_READ;
    g_vb_queue.drv_priv = NULL;
    g_vb_queue.buf_struct_size = sizeof(struct virtual_frame_buf);  /* 分配vb时, 分配的空间大小为buf_struct_size */
    g_vb_queue.ops = &virtul_vb2_ops;
    g_vb_queue.mem_ops = &vb2_vmalloc_memops;
    g_vb_queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	ret = vb2_queue_init(&s->vb_queue);
	if (ret) {
		printk("Could not initialize vb2 queue\n");
		return -1;;
	}

    g_vdev.queue = &g_vb_queue;     
    g_vdev.queue->lock = &s->vb_queue_lock;

	/* Register the v4l2_device structure(辅助作用) */
	g_v4l2_dev.release = airspy_video_release;
	ret = v4l2_device_register(NULL, &g_v4l2_dev);
	if (ret) {
		printk("Failed to register v4l2-device (%d)\n", ret);
		return -1;
	}
	g_vdev.v4l2_dev = &g_v4l2_dev;
	g_vdev.lock = &s->v4l2_lock;

	ret = video_register_device(&g_vdev, VFL_TYPE_SDR, -1);
	if (ret) {
		printk("Failed to register as video device (%d)\n", ret);
		return -1;
	}

	INIT_LIST_HEAD(&g_queued_bufs);
    
    return 0;
}

static void virtual_video_drv_exit(void)
{
    /* 反注册/释放video_device */
    v4l2_device_unregister(&g_v4l2_dev);
    video_unregister_device(&g_vdev);
}

module_init(virtual_video_drv_init);
module_exit(virtual_video_drv_exit);

MODULE_LICENSE("GPL");


