#!/bin/sh

# Turns out that crontab does not quite work like this. TODO: Redo using -l, modifying and streaming back in.
#crontab -e @reboot /home/pi/RCD/run_camera.sh

# TODO: Add bcm2835-v4l2 to /etc/modules   (or /etc/modules.conf if on Red Hat)

