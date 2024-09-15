/*
 *  tslib/tests/ts_print_mt.c
 *
 *  Copyright (C) 2017 Martin Kepplinger
 *
 * This file is part of tslib.
 *
 * ts_print_mt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * ts_print_mt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ts_print_mt.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 *
 * Just prints touchscreen events -- does not paint them on framebuffer
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>

#include <linux/input.h>
#define TS_HAVE_EVDEV


#include <sys/ioctl.h>

#include "tslib.h"

#ifndef ABS_MT_SLOT /* < 2.6.36 kernel headers */
# define ABS_MT_SLOT             0x2f    /* MT slot being modified */
#endif


/* ./ts_test /dev/input/event1 
 * 
 */
int main(int argc, char **argv)
{
	int ret, i, j;
	int read_samples = 1;
	struct ts_sample_mt **samp_mt = NULL;
	struct tsdev *ts;
	
	struct input_absinfo slot;
	int max_slots = 1;
	
	if (argc != 2)
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	
	ts = ts_setup(argv[1], 0);
	if (!ts) {
		printf("ts_setup err\n");
		return errno;
	}

	/* 确定驱动程序支持多少个触点 */
	if (ioctl(ts_fd(ts), EVIOCGABS(ABS_MT_SLOT), &slot) < 0) {
		printf("ioctl EVIOGABS err\n");
		ts_close(ts);
		return errno;
	}

	max_slots = slot.maximum + 1 - slot.minimum;

	/* 分配存储空间 */
	samp_mt = malloc(read_samples * sizeof(struct ts_sample_mt *));
	if (!samp_mt) {
		ts_close(ts);
		return -ENOMEM;
	}
	for (i = 0; i < read_samples; i++) {
		samp_mt[i] = calloc(max_slots, sizeof(struct ts_sample_mt));
		if (!samp_mt[i]) {
			free(samp_mt);
			ts_close(ts);
			return -ENOMEM;
		}
	}

	/* 读数据 */
	while (1) {
		ret = ts_read_mt(ts, samp_mt, max_slots, read_samples);

		if (ret < 0) {
			printf("ts_print_mt: read returns %d\n", ret);
			ts_close(ts);
			return -1;
		}

		/* ret : 每个触点读了多少次数据 */
		for (j = 0; j < ret; j++) {
			for (i = 0; i < max_slots; i++) {
				if (!(samp_mt[j][i].valid & TSLIB_MT_VALID))
					continue;

				printf("sample %d - %ld.%06ld - (slot %d) %6d %6d %6d\n",
				       j,
				       samp_mt[j][i].tv.tv_sec,
				       samp_mt[j][i].tv.tv_usec,
				       samp_mt[j][i].slot,
				       samp_mt[j][i].x,
				       samp_mt[j][i].y,
				       samp_mt[j][i].pressure);
			}
		}
	}

	ts_close(ts);
	
	return 0;
}
