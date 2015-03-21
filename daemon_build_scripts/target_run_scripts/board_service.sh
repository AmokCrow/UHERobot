#!/bin/bash
# /etc/init.d/uhedaemon

### BEGIN INIT INFO
# Provides:          uhedaemon
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: UHERobot web-service daemon
# Description:       This service is used for controlling the (hopefully) attached UHERobot control board
### END INIT INFO


start() {
        echo "Starting uhedaemon"

        /home/pi/RCD/start_daemon.sh
        return
}

stop() {
        echo "Stopping uhedaemon"

        /home/pi/RCD/kill_daemon.sh
        return
}


case "$1" in 
    start)
        start
        ;;
    stop)
        stop
        ;;
    status)

        ;;
    restart)
        stop
        start
        ;;
    reload)
        stop
        start
        ;;
    *)
        echo "Usage: /etc/init.d/uhedaemon start|stop|restart"
        exit 1
        ;;
esac

exit $?

