#!/bin/sh


# Put Nginx configuration file in place
chmod a+rx install_nginx_config.sh
./install_nginx_config.sh
chmod 0 install_nginx_config.sh

# Make the start and stop scripts executable (the permissions are wide, but I can't be bothered to think what is actually needed)
chmod a+rx start_daemon.sh
chmod a+rx kill_daemon.sh
chmod a+rx board_service.sh

# Link from init.d to the control script and add it to the startup list. The daemon should now start with the RasPi.
ln -s /home/pi/RCD/board_service.sh /etc/init.d/uherobotd
update-rc.d uherobotd defaults




