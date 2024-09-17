<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-13 21:43:49
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-16 17:38:23
 * @Description: file content
-->

# 1 driver 驱动

Linux 驱动 = 软件框架 + 硬件操作

GPIO 功能：
- 输入输出
- 防抖动
- 中断
- 唤醒


static struct file_operations hello_drv = {
	.owner	 = THIS_MODULE,
	.open    = hello_drv_open,
	.read    = hello_drv_read,
	.write   = hello_drv_write,
	.release = hello_drv_close,
};



copy_to_user
copy_from_user

register_chrdev
class_create
device_create

device_destroy
class_destroy
unregister_chrdev

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

# 2
alloc_chrdev_region
cdev_init
cdev_add
cdev_del

# 3

GPIO5_GDIR = ioremap(0x020AC004, 4);

struct inode *inode = file_inode(file);
int minor = iminor(inode);

--------------------------------------------------------------------------------

# Linux驱动

## 1、类型
- 字符设备驱动
- 块设备驱动
- 网络设备驱动
- 其他设备驱动

## 2. 三种驱动创建方式
- 传统class_create/device_create/file_operations
- 总线resource/platform_device/platform_driver: probe/remove
- 设备树dts

## 3、驱动编写
- 确定主设备号，也可以让内核分配
- 定义file_operations结构体
- 实现对应的drv_open/drv_read/drv_write等函数，填入file_operations结构体
- 把file_operations结构体告诉内核：
- 注册驱动程序入口函数register_chrdev
- 卸载驱动程序出口函数unregister_chrdev
- 其他完善：提供设备信息，自动创建设备节点：class_create, device_create


## 4、pinctrl子系统
管理引脚的复用和配置
BSP工程师在GPIO子系统、Pinctrl子系统中把自家芯片的支持加进去
后，我们就可以非常方便地使用这些引脚了：点灯简直太简单了

kernel\Documentation\devicetree\bindings\pinctrl\pinctrl-bindings.txt

- pin controller
- client device:
  - pin state, groups 和 function
  - Generic pin multiplexing node 和 Generic pin configuration node


## 5、GPIO 子系统
BSP工程师实现了GPIO子系统
- 在设备树里指定 GPIO 引脚
- 在驱动代码中：使用GPIO子系统的标准函数获得GPIO、设置GPIO方向、读取/设置GPIO值
这样的驱动代码，将是单板无关的。

sysfs 中访问 GPIO
- /sys/class/gpio/gpiochip0/


gpio
- gpio_request
- gpio_free
- gpio_direction_input
- gpio_direction_output
- gpio_get_value
- gpio_set_value
- gpio_cansleep
- gpio_to_irq
- irq_to_gpio
- gpio_set_debounce
- gpio_get_debounce
- gpio_set_pull
- gpio_get_pull
- gpio_set_flags
- gpio_get_flags
- gpio_lock
- gpio_unlock
- gpio_to_desc
- gpio_desc_get_label
- gpio_desc_get_name
- gpio_desc_get_pin
- gpio_desc_get_gpio
- gpio_desc_get_flags
- gpio_desc_get_drvdata
- gpio_desc_set_drvdata
- gpio_desc_get_custom
- gpio_desc_set_custom
- gpio_desc_get_custom_length
- gpio_desc_set_custom_length


gpio irq
- request_irq
- free_irq
- wait_event_interruptible
- wake_up_interruptible
- kzalloc
- gpio_to_desc
- gpio_to_irq
- of_gpio_count

poll机制
- poll_wait
- poll_initwait
- poll_free
- poll_wait_event
- poll_schedule_timeout
- poll_unregister
- poll_wait_timeout
- poll_init
- poll_add_wait_queue
- poll_remove_wait_queue
- poll_wait_fd
- poll_fd_unregister
- poll_fd_add
- poll_fd_remove
- poll_fd_requeue
- poll_event
- poll_events_poll
- poll_events_wait
- poll_events_fd
- poll_events_tqueue
- poll_events_ptqueue
- poll_events_process
- poll_events_port
- poll_events_timerfd
- poll_events_signalfd
- poll_events_eventfd
- poll_events_aio
- poll_events_uring

- file_operations.pool
- file_operations.fasync

同步
- struct fasync_struct
- kill_fasync
- fasync_helper
- fasync_helper_install
- fasync_helper_remove
- fasync_helper_kill
- fasync_helper_enable
- fasync_helper_disable
- fasync_helper_queue
- fasync_helper_try_to_wake
- fasync_helper_signal
- fasync_helper_exit
- fasync_helper_exit_if_done
- fasync_helper_exit_if_inactive
- fasync_helper_exit_if_pending
- fasync_helper_exit_if_signaled
- fasync_helper_exit_if_disabled
- fasync_helper_exit_if_not_queued
- fasync_helper_exit_if_not_running
- fasync_helper_exit_if_not_async
- fasync_helper_exit_if_not_async_and_not_queued
- fasync_helper_exit_if_not_async_and_not_running
- fasync_helper_exit_if_not_async_and_not_signaled
- fasync_helper_exit_if_not_async_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_queued_and_not_running
- fasync_helper_exit_if_not_async_and_not_queued_and_not_signaled
- fasync_helper_exit_if_not_async_and_not_queued_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_running_and_not_signaled
- fasync_helper_exit_if_not_async_and_not_running_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_signaled_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_queued_and_not_running_and_not_signaled
- fasync_helper_exit_if_not_async_and_not_queued_and_not_running_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_queued_and_not_signaled_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_running_and_not_signaled_and_not_disabled
- fasync_helper_exit_if_not_async_and_not_queued_and_not_running_and_not_signaled_and_not_disabled

- struct file->f_flags & O_NONBLOCK

定时器
- struct timer_list
- from_timer
- mod_timer
- setup_timer
- add_timer
- del_timer

任务队列
- tasklet_init
- tasklet_schedule
- tasklet_kill
- tasklet_init
- tasklet_handler
- tasklet_kill_pending
- tasklet_trylock
- tasklet_unlock
- tasklet_disable
- tasklet_enable
- tasklet_run
- tasklet_desc_init
- tasklet_desc_kill
- tasklet_desc_lock
- tasklet_desc_unlock
- tasklet_desc_disable
- tasklet_desc_enable
- tasklet_desc_run
- tasklet_desc_kill_pending
- tasklet_desc_trylock
- tasklet_desc_unlock
- tasklet_desc_disable
- tasklet_desc_enable
- tasklet_desc_run

- struct workqueue_struct
- create_workqueue
- destroy_workqueue
- queue_work
- struct work_struct
- schedule_work
- INIT_WORK

- IRQ_WAKE_THREAD
- IRQ_HANDLED
- request_threaded_irq

# 6 mmap
- struct file_operations.mmap
- kmalloc
- kfree
- virt_to_phys
- remap_pfn_range
-