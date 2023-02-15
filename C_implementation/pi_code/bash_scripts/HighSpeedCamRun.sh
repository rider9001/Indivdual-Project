#!/bin/bash

#filesystem locations

OUTDIR=$PWD/temp
CURDT=`date +%T+%Y+%m+%d`

#core script

mkdir -p $OUTDIR

libcamera-vid -n --codec mjpeg -t 1000 --segment 1 -o $OUTDIR/capture_%02d.jpg --width 1296 --height 972 --brightness 0.0 --exposure normal --framerate 25 --gain 0 --awb auto --metering centre --saturation 1.0 --sharpness 1.5 --denoise off

cd $OUTDIR
touch DT:$CURDT