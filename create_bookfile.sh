#!/bin/bash

cd /home/fedor/Project/SOCR

find * -name '*.bin.png' | sort -t '/' -k2 -k3 -n > ruphy-all
#find * -name '*.bin.png' > ruphy-all
sed 1,50d ruphy-all > ruphy-train
sed 50q ruphy-all > ruphy-test
