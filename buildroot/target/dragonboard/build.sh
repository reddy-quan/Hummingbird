#!/bin/sh

# sysroot exist?
if [ ! -d "./sysroot" ]; then
    echo "extract dragonboard sysroot.tar.gz"
    tar zxf sysroot.tar.gz
fi

if [ ! -d "./output/bin" ]; then
    mkdir -p ./output/bin
fi

cd src
make
if [ $? -ne 0 ]; then
    exit 1
fi
cd ..

if [ ! -d "rootfs/etc/mdev" ]; then
    mkdir -p rootfs/etc/mdev
fi

if [ ! -d "rootfs/dragonboard" ]; then
    mkdir -p rootfs/dragonboard
fi

if [ ! -d "rootfs/usr/share/video" ]; then
    mkdir -p rootfs/usr/share/video
fi
#merrii

cp -rf extra/mdev.conf rootfs/etc
cp -rf extra/mdev/*  	rootfs/etc/mdev

cp -rf extra/autorun.sh rootfs/
cp -rf extra/boardset.sh rootfs/
cp -rf extra/wpa_supplicant.conf rootfs/etc
cp -rf extra/Qmerrii rootfs/
cp -rf extra/samegame rootfs/root
cp -rf extra/script_parser.sh rootfs/root

cp -rf extra/CedarX/libdemux_rmvb.so rootfs/lib
cp -rf extra/CedarX/librm.so rootfs/lib
cp -rf extra/CedarX/CedarXPlayerTest rootfs/usr/bin
cp -rf extra/CedarX/test.mkv rootfs/usr/share/video/
###############ip address####################
cp -rf extra/auto_config_network rootfs/etc/init.d/
#################jdk#################
#cp -rf extra/jdk1.7.0_45 rootfs/opt/
##########  OPEN SSH###################
mkdir -p 	rootfs/usr/local/etc/
mkdir -p 	rootfs/usr/local/sbin/
mkdir -p 	rootfs/usr/local/bin/
mkdir -p 	rootfs/usr/local/libexec/
mkdir -p 	rootfs/var/run/
mkdir -p 	rootfs/var/empty/

rm -rf extra/extra/ssh-5.6p1/ssh_host*
extra/openssh-5.6p1/ssh-keygen -t rsa -f ssh_host_rsa_key -N ""
extra/openssh-5.6p1/ssh-keygen -t dsa -f ssh_host_dsa_key -N ""
extra/openssh-5.6p1/ssh-keygen -t ecdsa -f ssh_host_ecdsa_key -N ""

cp -rf extra/openssh-5.6p1/sshd  			rootfs/usr/local/sbin/

cp -rf extra/openssh-5.6p1/scp  			rootfs/usr/local/bin/
cp -rf extra/openssh-5.6p1/sftp  			rootfs/usr/local/bin/
cp -rf extra/openssh-5.6p1/ssh				rootfs/usr/local/bin/
cp -rf extra/openssh-5.6p1/ssh-add			rootfs/usr/local/bin/
cp -rf extra/openssh-5.6p1/ssh-agent		rootfs/usr/local/bin/
cp -rf extra/openssh-5.6p1/ssh-keygen  	rootfs/usr/local/bin/
cp -rf extra/openssh-5.6p1/ssh-keyscan  	rootfs/usr/local/bin/

cp -rf extra/openssh-5.6p1/sftp-server  	rootfs/usr/local/libexec/
cp -rf extra/openssh-5.6p1/ssh-keysign   	rootfs/usr/local/libexec/

cp   extra/openssh-5.6p1/sshd_config 		rootfs/usr/local/etc/
cp   extra/openssh-5.6p1/ssh_config  		rootfs/usr/local/etc/
cp   extra/openssh-5.6p1/moduli   			rootfs/usr/local/etc/

cp   extra/openssh-5.6p1/ssh_host_*_key	rootfs/usr/local/etc/
chmod 		755 rootfs/var/empty 
cp extra/shadow rootfs/etc/

############################################
rm -rf rootfs/dragonboard/*
cp -rf output/* rootfs/dragonboard/

cp -rf Trolltech rootfs/opt

echo "dragonboard generating rootfs..."
BR_ROOT=`(cd ../..; pwd)`
export PATH=$PATH:$BR_ROOT/target/tools/host/usr/bin

NR_SIZE=`du -sm rootfs | awk '{print $1}'`
NEW_NR_SIZE=$(((($NR_SIZE+64)/32)*32))
TARGET_IMAGE=rootfs.ext4

echo "blocks: $NR_SIZE"M" -> $NEW_NR_SIZE"M""
make_ext4fs -l $NEW_NR_SIZE"M" $TARGET_IMAGE rootfs/
fsck.ext4 -y $TARGET_IMAGE > /dev/null
echo "success in generating rootfs"
echo "Build at: `date`"
