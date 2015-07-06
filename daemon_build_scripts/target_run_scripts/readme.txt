Add the following to sudo crontab (sudo crontab -e):
@reboot sh /home/pi/RCD/shutdown.sh >/home/pi/RCD/log/cronlog 2>&1

Create directory for cronlog:
mkdir /home/pi/RCD/log

Add the following to pi crontab (crontab -u pi -e):
@reboot /home/pi/RCD/run_camera.sh

All scripts need to be made runnable.
The scripts install_daemon.sh and install_nginx_config.sh automate some of the installation of the Control Daemon.
 
