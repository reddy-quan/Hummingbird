#!/bin/bash

path=$(pwd)
export PATH=$path/boot/config/gcc-linaro/bin:$PATH:$path/u-boot/tools

./build.sh -p sun7i_dragonboard

