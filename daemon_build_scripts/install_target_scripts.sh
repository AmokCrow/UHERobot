#!/bin/bash

TARGETADDR="192.168.0.13"

ssh pi@"${TARGETADDR}" "rm -rf RCD"

scp -r target_run_scripts pi@"${TARGETADDR}":RCD

ssh pi@"${TARGETADDR}" "chmod u+rwx RCD/install_nginx_config.sh ; chmod u+rwx RCD/start_daemon.sh ; chmod u+rwx RCD/kill_daemon.sh "

scp -r ../StaticWebContent pi@"${TARGETADDR}":RCD/StaticWebContent


