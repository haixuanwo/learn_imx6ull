#!/bin/bash -e
### BEGIN INIT INFO
# Provides:          adbd
# Required-Start:
# Required-Stop:
# Default-Start:
# Default-Stop:
# Short-Description:
# Description:       Linux ADB
### END INIT INFO

VENDOR_ID="0x1d6b"
PRODUCT_ID="0x0104"
UDC=`ls /sys/class/udc/ | awk '{print $1}'`

start() {
        mkdir -p /dev/usb-ffs -m 0770
        mkdir -p /dev/usb-ffs/adb -m 0770

        mkdir -p /sys/kernel/config/usb_gadget/g1  -m 0770

        echo ${VENDOR_ID} > /sys/kernel/config/usb_gadget/g1//idVendor
        echo ${PRODUCT_ID} > /sys/kernel/config/usb_gadget/g1//idProduct

        mkdir  -p /sys/kernel/config/usb_gadget/g1/strings/0x409   -m 0770

        echo "0123456789ABCDEF" > /sys/kernel/config/usb_gadget/g1/strings/0x409/serialnumber
        echo "STMicroelectronics"  > /sys/kernel/config/usb_gadget/g1/strings/0x409/manufacturer
        echo "STM32MP1"  > /sys/kernel/config/usb_gadget/g1/strings/0x409/product

        mkdir  -p /sys/kernel/config/usb_gadget/g1/functions/ffs.adb
        mkdir  -p /sys/kernel/config/usb_gadget/g1/configs/b.1  -m 0770
        mkdir  -p /sys/kernel/config/usb_gadget/g1/configs/b.1/strings/0x409  -m 0770

        ln -s  /sys/kernel/config/usb_gadget/g1/functions/ffs.adb /sys/kernel/config/usb_gadget/g1/configs/b.1
        echo "adb" > /sys/kernel/config/usb_gadget/g1/configs/b.1/strings/0x409/configuration
        mount -t functionfs adb /dev/usb-ffs/adb

        start-stop-daemon --start --oknodo --pidfile /var/run/adbd.pid --startas /bin/adbd --background

        sleep 1

        echo $UDC > /sys/kernel/config/usb_gadget/g1/UDC
}

stop() {
        start-stop-daemon --stop --oknodo --pidfile /var/run/adbd.pid --retry 5
        umount /dev/usb-ffs/adb
}

restart() {
        echo $UDC > /sys/kernel/config/usb_gadget/g1/UDC
}

if [  "$UDC" != "" ]; then
        case $1 in
                start|stop|restart) "$1" ;;
        esac
fi

exit $?