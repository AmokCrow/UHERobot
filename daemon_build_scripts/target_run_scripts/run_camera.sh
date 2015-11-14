#!/bin/sh

# -- VLC --
#cvlc v4l2:///dev/video0 --v4l2-width 800 --v4l2-height 600 --v4l2-chroma h264 --sout '#standard{access=http,mux=ts,dst=0.0.0.0:8554}' :demux=264 &
#cvlc v4l2:///dev/video0 --live-caching=10 --no-audio --v4l2-width 800 --v4l2-height 600 --v4l2-chroma h264 --sout '#standard{access=http,mux=ts,dst=0.0.0.0:8554}' :demux=264
#--network-caching 100
#--sout-mux-caching 100
#--http-caching=0
#--no-audio

# -- MJPG-Streamer


cd /home/pi/mjpg-streamer/mjpg-streamer-experimental
#export LD_LIBRARY_PATH=/home/pi/mjpg-streamer/mjpg-streamer-experimental/plugins
./mjpg_streamer -i "./input_uvc.so -f 5 -r 320x240 -n" -o "./output_http.so -w ./www"


