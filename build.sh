#!/bin/bash

set -e

buildroot/scripts/mkcommon.sh $@

if [ -f tools/pack/sun7i_dragonboard_wing-k70.img ]
then
    mv tools/pack/sun7i_dragonboard_wing-k70.img .
fi
