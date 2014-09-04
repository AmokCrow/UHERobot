#!/bin/bash

PROGNAME="RobotControlDaemon"

/usr/local/bin/spawn-fcgi -p 8000 -P /var/run/fcgi.pid -- "$PROGNAME" -p /dev/ttyAMA0

