#!/bin/bash
rm -rf out
rm tools/pack/*.img
rm tools/pack/out
cd buildroot/target/dragonboard
./clean.sh
cd ../../../linux-3.4
make distclean
