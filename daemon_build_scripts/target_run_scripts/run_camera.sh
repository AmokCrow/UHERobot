#!/bin/sh

cvlc v4l2:///dev/video0 --v4l2-width 800 --v4l2-height 600 --v4l2-chroma h264 --sout '#standard{access=http,mux=ts,dst=0.0.0.0:8554}' :demux=264 &
#cvlc v4l2:///dev/video0 --live-caching=10 --no-audio --v4l2-width 800 --v4l2-height 600 --v4l2-chroma h264 --sout '#standard{access=http,mux=ts,dst=0.0.0.0:8554}' :demux=264
#--network-caching 100
#--sout-mux-caching 100
#--http-caching=0
#--no-audio


