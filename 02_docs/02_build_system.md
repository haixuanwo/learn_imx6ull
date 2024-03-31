<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-03-31 10:33:54
 * @LastEditors: Clark
 * @LastEditTime: 2024-03-31 10:33:55
 * @Description: file content
-->

# 1、构建整个系统

cd Buildroot_2020.02.x
make clean
make 100ask_imx6ull_pro_ddr512m_systemV_qt5_defconfig
make all -j4


