#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>

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
	.vidioc_querycap          = airspy_querycap,

	.vidioc_enum_fmt_sdr_cap  = airspy_enum_fmt_sdr_cap,
	.vidioc_g_fmt_sdr_cap     = airspy_g_fmt_sdr_cap,
	.vidioc_s_fmt_sdr_cap     = airspy_s_fmt_sdr_cap,
	.vidioc_try_fmt_sdr_cap   = airspy_try_fmt_sdr_cap,

	.vidioc_reqbufs           = vb2_ioctl_reqbufs,
	.vidioc_create_bufs       = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf       = vb2_ioctl_prepare_buf,
	.vidioc_querybuf          = vb2_ioctl_querybuf,
	.vidioc_qbuf              = vb2_ioctl_qbuf,
	.vidioc_dqbuf             = vb2_ioctl_dqbuf,

	.vidioc_streamon          = vb2_ioctl_streamon,
	.vidioc_streamoff         = vb2_ioctl_streamoff,

	.vidioc_g_tuner           = airspy_g_tuner,
	.vidioc_s_tuner           = airspy_s_tuner,

	.vidioc_g_frequency       = airspy_g_frequency,
	.vidioc_s_frequency       = airspy_s_frequency,
	.vidioc_enum_freq_bands   = airspy_enum_freq_bands,

	.vidioc_subscribe_event   = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
	.vidioc_log_status        = v4l2_ctrl_log_status,
};

static struct video_device g_vdev = {
	.name                     = "100ask_virtual_video",
	.release                  = video_device_release_empty,
	.fops                     = &virtual_fops,
	.ioctl_ops                = &virtual_ioctl_ops,
};
static struct v4l2_device g_v4l2_dev;

static struct vb2_queue g_vb_queue;

static const struct vb2_ops virtul_vb2_ops = {
	.queue_setup            = airspy_queue_setup,
	.buf_queue              = airspy_buf_queue,
	.start_streaming        = airspy_start_streaming,
	.stop_streaming         = airspy_stop_streaming,
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
    g_vb_queue.buf_struct_size = sizeof(struct airspy_frame_buf);
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


