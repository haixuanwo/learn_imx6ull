
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/mman.h>

/* ./video_test </dev/video0> */

int main(int argc, char **argv)
{
    int fd;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum fsenum;
    int fmt_index = 0;
    int frame_index = 0;
    int i;
    void *bufs[32];
    int buf_cnt;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct pollfd fds[1];
    char filename[32];
    int file_cnt = 0;

    if (argc != 2)
    {
        printf("Usage: %s </dev/videoX>, print format detail for video device\n", argv[0]);
        return -1;
    }

    /* open */
    fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
        printf("can not open %s\n", argv[1]);
        return -1;
    }

    /* 查询能力 */
    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(struct v4l2_capability));
    
    if (0 == ioctl(fd, VIDIOC_QUERYCAP, &cap))
    {        
        if((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) {
            fprintf(stderr, "Error opening device %s: video capture not supported.\n",
                    argv[1]);
            return -1;
        }
        
        if(!(cap.capabilities & V4L2_CAP_STREAMING)) {
            fprintf(stderr, "%s does not support streaming i/o\n", argv[1]);
            return -1;
        }
    }
    else
    {
        printf("can not get capability\n");
        return -1;
    }

    while (1)
    {
        /* 枚举格式 */
        fmtdesc.index = fmt_index;  // 比如从0开始
        fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;  // 指定type为"捕获"
        if (0 != ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
            break;

        frame_index = 0;
        while (1)
        {
            /* 枚举这种格式所支持的帧大小 */
            memset(&fsenum, 0, sizeof(struct v4l2_frmsizeenum));
            fsenum.pixel_format = fmtdesc.pixelformat;
            fsenum.index = frame_index;

            if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsenum) == 0)
            {
                printf("format %s,%d, framesize %d: %d x %d\n", fmtdesc.description, fmtdesc.pixelformat, frame_index, fsenum.discrete.width, fsenum.discrete.height);
            }
            else
            {
                break;
            }

            frame_index++;
        }

        fmt_index++;
    }


    /* 设置格式 */
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 1024;
    fmt.fmt.pix.height = 768;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (0 == ioctl(fd, VIDIOC_S_FMT, &fmt))
    {
        printf("set format ok: %d x %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
    }
    else
    {
        printf("can not set format\n");
        return -1;
    }

    /*
     * 申请buffer
     */
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.count = 32;
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory = V4L2_MEMORY_MMAP;

    if (0 == ioctl(fd, VIDIOC_REQBUFS, &rb))
    {
        /* 申请成功后, mmap这些buffer */
        buf_cnt = rb.count;
        for(i = 0; i < rb.count; i++) {
            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(struct v4l2_buffer));
            buf.index = i;
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            if (0 == ioctl(fd, VIDIOC_QUERYBUF, &buf))
            {
                /* mmap */
                bufs[i] = mmap(0 /* start anywhere */ ,
                                  buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                  buf.m.offset);
                if(bufs[i] == MAP_FAILED) {
                    perror("Unable to map buffer");
                    return -1;
                }
            }
            else
            {
                printf("can not query buffer\n");
                return -1;
            }            
        }

        printf("map %d buffers ok\n", buf_cnt);
        
    }
    else
    {
        printf("can not request buffers\n");
        return -1;
    }

    /* 把所有buffer放入"空闲链表" */
    for(i = 0; i < buf_cnt; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(struct v4l2_buffer));
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (0 != ioctl(fd, VIDIOC_QBUF, &buf))
        {
            perror("Unable to queue buffer");
            return -1;
        }
    }
    printf("queue buffers ok\n");

    /* 启动摄像头 */
    if (0 != ioctl(fd, VIDIOC_STREAMON, &type))
    {
        perror("Unable to start capture");
        return -1;
    }
    printf("start capture ok\n");

    while (1)
    {
        /* poll */
        memset(fds, 0, sizeof(fds));
        fds[0].fd = fd;
        fds[0].events = POLLIN;
        if (1 == poll(fds, 1, -1))
        {
            /* 把buffer取出队列 */
            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(struct v4l2_buffer));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            
            if (0 != ioctl(fd, VIDIOC_DQBUF, &buf))
            {
                perror("Unable to dequeue buffer");
                return -1;
            }
            
            /* 把buffer的数据存为文件 */
            sprintf(filename, "video_raw_data_%04d.jpg", file_cnt++);
            int fd_file = open(filename, O_RDWR | O_CREAT, 0666);
            if (fd_file < 0)
            {
                printf("can not create file : %s\n", filename);
            }
            printf("capture to %s\n", filename);
            write(fd_file, bufs[buf.index], buf.bytesused);
            close(fd_file);

            /* 把buffer放入队列 */
            if (0 != ioctl(fd, VIDIOC_QBUF, &buf))
            {
                perror("Unable to queue buffer");
                return -1;
            }
        }
    }

    if (0 != ioctl(fd, VIDIOC_STREAMOFF, &type))
    {
        perror("Unable to stop capture");
        return -1;
    }
    printf("stop capture ok\n");
    close(fd);

    return 0;
}

