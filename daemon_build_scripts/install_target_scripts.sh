#!/bin/bash

TARGETADDR="$1"

# Note!: All the scripts depend on the username pi on the target. Because.

ssh pi@"${TARGETADDR}" "rm -rf RCD"

scp -r target_run_scripts pi@"${TARGETADDR}":RCD

scp -r ../StaticWebContent pi@"${TARGETADDR}":RCD/StaticWebContent

ssh pi@"${TARGETADDR}" "chmod u+rx RCD/install_daemon.sh ; cd RCD ; ./install_daemon.sh "

