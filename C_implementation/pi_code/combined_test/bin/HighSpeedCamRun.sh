#!/bin/bash

#filesystem locations

echo "running high speed camera capture"

SHMDIR=/run/shm/HSCamMEMWrite
RUNDIR=$PWD
CURDT=`date +%T+%Y+%m+%d`

#core script

mkdir -p $SHMDIR
mkdir -p $RUNDIR/temp

echo start: `date +%s.%N`

libcamera-vid -n --codec mjpeg -t 500 --segment 1 -o $SHMDIR/capture_%02d.jpg --width 1296 --height 972 --brightness 0.0 --contrast 0.7 --exposure normal --framerate 60 --gain 0 --awb auto --metering centre --saturation 1.0 --sharpness 1.5 --denoise off

echo end: `date +%s.%N`

mv -v $SHMDIR/* $RUNDIR/temp
cd $RUNDIR/temp
touch DT:$CURDT
