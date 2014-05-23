#!/bin/sh

source script_parser.sh

if [ ! -d /system/vendor/ ]; then
    mkdir -p /system/vendor/
    ln -s /lib/modules/`uname -r`/ /system/vendor/modules
    ln -s /boot/*.hcd /system/vendor/modules/
    ln -s /boot/*.bin /system/vendor/modules/
    ln -s /boot/*.txt /system/vendor/modules/
fi

ROOT_DEVICE="/dev/mmcblk0p7"
for parm in $(cat /proc/cmdline); do
    case $parm in
        root=*)
            ROOT_DEVICE=`echo $parm | awk -F\= '{print $2}'`
            ;;  
    esac
done

# install nand driver if we boot from sdmmc
nand_activated=`script_fetch "nand" "activated"`
if [ $nand_activated -eq 1 ]; then
    case $ROOT_DEVICE in
        /dev/mmc*)
  	    
        nand_module_path=`script_fetch "nand" "module_path"`
        if [ -n "$nand_module_path" ]; then
     	  	 insmod "$nand_module_path"
  	    fi
            ;;
    esac

fi

#insmod /system/vendor/modules/disp.ko
#insmod /system/vendor/modules/lcd.ko
#insmod /system/vendor/modules/hdmi.ko
# insmod ir driver
ir_activated=`script_fetch "ir" "activated"`
if [ $ir_activated -eq 1 ]; then
    ir_module_path=`script_fetch "ir" "module_path"`
    if [ -n "$ir_module_path" ]; then
        insmod "$ir_module_path"
    fi
fi

# start camera test firstly
#while true; do
while false; do
    camera_activated=`script_fetch "camera" "activated"`
    echo "camera activated #$camera_activated"
    if [ $camera_activated -eq 1 ]; then
        echo "camera activated"
        module_count=`script_fetch "camera" "module_count"`
        if [ $module_count -gt 0 ]; then
            for i in $(seq $module_count); do
                key_name="module"$i"_path"
                module_path=`script_fetch "camera" "$key_name"`
                if [ -n "$module_path" ]; then
                    insmod "$module_path"
                    if [ $? -ne 0 ]; then
                        echo "insmod $module_path failed"
                        break 2
                    fi
                fi
            done
        fi
    else
        echo "camera not activated"
        break
    fi

    echo "camera module insmod done"
    touch /tmp/camera_insmod_done
done

# insmod touchscreen driver
tp_module_path=`script_fetch "tp" "module_path"`
if [ -n "$tp_module_path" ]; then
    insmod "$tp_module_path"

    # calibrate touchscreen if need
    tp_type=`script_fetch "tp" "type"`
    if [ $tp_type -eq 0 ]; then
        while true; do
            ts_calibrate
            if [ $? -eq 0 ]; then
                break
            fi
        done
    fi

    #ts_test
else
    echo "NO!!! touchscreen driver to be insmod"
fi
#############ssh service###########
/usr/local/sbin/sshd
###################################
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s

wpa_supplicant -i wlan0 -c /etc/wpa_supplicant.conf &
insmod /system/vendor/modules/sunxi_gmac.ko
#sleep 4
#udhcpc -i wlan0 -n -q -t 6
# sh /etc/init.d/auto_config_network
#for QT . /boardset.sh
/dragonboard/bin/bttester.sh &
/dragonboard/bin/hdmitester 0 0 0 0 &
#sleep 4
#qj /Qmerrii -qws &
mkdir /mnt/vfs
export LD_LIBRARY_PATH=/mnt/vfs/lib
export PATH=$PATH:/mnt/vfs/bin:/mnt/vfs/sbin:/mnt/vfs/usr/bin

ifconfig eth0 hw ether 06:94:A4:F1:64:D4
ifconfig eth0 up
killall udhcpc
udhcpc -i eth0 &

