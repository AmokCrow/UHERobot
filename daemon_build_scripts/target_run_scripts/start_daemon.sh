#!/bin/sh

PROGNAME="/home/pi/RCD/bin/RobotControlDaemon"

/usr/local/bin/spawn-fcgi -p 8000 -P /var/run/uherobotd.pid -- "$PROGNAME" -p /dev/ttyAMA0

