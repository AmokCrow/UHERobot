#!/bin/sh

TARGETMACHINE="pi@192.168.0.13"

ssh $TARGETMACHINE "./RCD/kill_daemon.sh ; rm -rf RCD/RobotControlDaemon"

scp ../RobotControlDaemon/RobotControlDaemon "${TARGETMACHINE}:RCD/RobotControlDaemon"

ssh $TARGETMACHINE "chmod u+rwx RCD/RobotControlDaemon ; ./RCD/start_daemon.sh"

