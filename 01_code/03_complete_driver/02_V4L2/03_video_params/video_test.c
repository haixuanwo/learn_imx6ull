/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-19 20:26:51
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-19 20:36:30
 * @Description: file content
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

/* ./video_test </dev/video0> */

int main(int argc, char **argv)
{
    int fd;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum fsenum;
    int fmt_index = 0;
    int frame_index = 0;

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
    printf("open %s success\n", argv[1]);

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

    return 0;
}

