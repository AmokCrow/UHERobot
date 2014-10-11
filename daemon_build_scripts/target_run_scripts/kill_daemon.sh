#!/bin/bash

PROGNAME="RobotControlDaemon"

if [ -f /var/run/fcgi.pid ]
then
  kill -9 `cat /var/run/fcgi.pid`
fi

