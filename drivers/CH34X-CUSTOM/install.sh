#!/bin/sh
# CH34X-custom

MODULEDIR=$1

[ -d $MODULEDIR ] || mkdir $MODULEDIR

cp -p ch34x-custom.ko $MODULEDIR
depmod -a

# Blacklist builtin
echo "blacklist ch341-uart" > /etc/modprobe.d/blacklist-ch341.conf

# Autoload module when usb inserted
cat <<EOF > /etc/udev/rules.d/99-ch34x-custom.rules
ATTRS{idVendor}=="4348", ATTRS{idProduct}=="5523", MODE:="0666", ENV{ID_MM_DEVICE_IGNORE}="1", RUN+="/sbin/modprobe ch34x-custom"
ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", MODE:="0666", ENV{ID_MM_DEVICE_IGNORE}="1", RUN+="/sbin/modprobe ch34x-custom"
ATTRS{idVendor}=="9986", ATTRS{idProduct}=="7523", MODE:="0666", ENV{ID_MM_DEVICE_IGNORE}="1", RUN+="/sbin/modprobe ch34x-custom"
ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="5523", MODE:="0666", ENV{ID_MM_DEVICE_IGNORE}="1", RUN+="/sbin/modprobe ch34x-custom"
EOF

udevadm control --reload-rules
udevadm trigger