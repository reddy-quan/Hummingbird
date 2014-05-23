#!/bin/sh

#export PATH=~/bird/MerriiLinux/boot/config/gcc-linaro/bin:$PATH:~/bird/MerriiLinux/u-boot/tools
path=$(pwd)
export PATH=$path/boot/config/gcc-linaro/bin:$PATH:$path/u-boot/tools
echo "AAAAA: $PATH"

#mv -f build.log build.log.old | tee -a build.log
rm -rf build.log build.log.old | tee -a build.log
sh ./build.sh -p sun7i_dragonboard | tee -a build.log

#./build.sh pack

#mkdir -p images
#cp -rf tools/pack/sun7i_dragonboard_wing-k70.img images/sun7i_dragonboard_wing-k70.img

echo "rootfs is at: ./buildroot/target/dragonboard/rootfs"

#make menuconfig sun7ismp_android_defconfig ARCH=arm
