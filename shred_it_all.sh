#!/bin/bash

cd /home/fedor/Project/SOCR
bmp=$(ls ./img | grep bmp)

for i in $bmp
do
echo "./img/${i%.*}.bmp"
./SOCR -L "./img/${i%.*}.bmp" -T "./txt/${i%.*}.txt" 
echo " "
done