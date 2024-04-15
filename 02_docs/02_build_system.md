<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-03-31 10:33:54
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-15 11:21:04
 * @Description: file content
-->

# 1、构建整个系统

cd Buildroot_2020.02.x
make clean
make 100ask_imx6ull_pro_ddr512m_systemV_qt5_defconfig
make all -j4


#  2、编译内核
make mrproper
make 100ask_imx6ull_defconfig
make zImage -j16
make dtbs
cp arch/arm/boot/zImage ~/nfs_rootfs
cp arch/arm/boot/dts/100ask_imx6ull-14x14.dtb ~/nfs_rootfs


## 编译安装内核模块
make modules
make ARCH=arm INSTALL_MOD_PATH=/home/book/nfs_rootfs modules_install
mount -t nfs -o nolock,vers=3 192.168.100.123:/home/book/nfs_rootfs /mnt
cp /mnt/zImage /boot
cp /mnt/100ask_imx6ull-14x14.dtb /boot
cp /mnt/lib/modules /lib -rfd
sync

最后重启开发板，它就使用新的 zImage、dtb、模块了。
