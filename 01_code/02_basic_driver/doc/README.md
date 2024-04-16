<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-15 17:05:30
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-16 14:31:22
 * @Description: file content
-->

# Basic Driver

## 驱动程序编写有3种方法
差别在于：如何指定硬件资源

- 1、传统方法
驱动程序代码中写死硬件资源, 代码简单/不易扩展
file_operations
class_create
device_create

- 2、总线设备驱动模型
把驱动程序分为两部分
platform_device：指定硬件资源
platform_driver：分配/设置/注册file_operations结构体

特点：
易于扩展，但是有很多冗余代码(每种配置都对应一个platform_device结构体),
硬件有变动时需要重新编译内核或驱动程序。

- 3、设备树
两部分(platform_driver, 设备树*.dts)
设备树*.dts中指定硬件资源, dts被编译为dtb文件, 在启动单板时会将dtb文件传给内核,
内核根据dtb文件分配/设置/注册多个platform_device

特点：
易于扩展，没有冗余代码
硬件有变动时不需要重新编译内核或驱动程序，只需要提供不一样的dtb文件


## LED 驱动程序
详细步骤如下：
① 看原理图确定引脚，确定引脚输出什么电平才能点亮/熄灭 LED
② 看主芯片手册，确定寄存器操作方法：哪些寄存器？哪些位？地址是？
③ 编写驱动：先写框架，再写硬件操作的代码

# IMX6ULL LED 驱动程序编写
- 使能power/clock control
- 复用pin mode
- 输入或输出
- 值


关闭出厂的驱动
echo none > /sys/class/leds/sys-led/trigger

## APP读按键
- 查询方式
- 休眠-唤醒方式
- poll 方式
- 异步通知方式

## 驱动开发基本技能
- 中断
- 休眠
- 唤醒
- poll 等机制

## APP开发基本技能
- 阻塞
- 非阻塞
- 休眠
- poll
- 异步通知

