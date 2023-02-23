#!/bin/bash

TMPDIR=$PWD/temp
mkdir -p $TMPDIR

libcamera-still -o $TMPDIR/motion.jpg -n -t 1 --width 800 --height 600 --brightness 0.0 --exposure normal --gain 0 --awb auto --metering centre --saturation 1.0 --sharpness 1.5 --denoise off