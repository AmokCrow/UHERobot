This repo is for the Under House Explorer robot project.

Contents:
RobotControlBoard - Code for the MCU on the robot's PCB.
RobotControlDaemon - an FCGI server process. Mediates between the web UI and the control board serial port.
StaticWebContent - The web UI.


Library dependencies:
libpthreads-dev (may be called libevent-pthreads in your distro now)
libfcgi-dev (basic FastCGI interface, get c++ ver if separate)
libpthreads-dev (needed for fcgi's threading)
libcgicc-dev (does the cgi inputs parsing)
spawn-fcgi (a lighttdp spin-off, a magic tool for spawning and un-parenting an fgci process with the right pipe. Don't ask me how it does it.)


Usage:
1. Install the cross-compilation toolchain for your Linux board. (You don't want to compile on a low-power single-board computer, trust me.)
  - For the Arietta board, see acmesystems.it . They made it and provide fairly good instructions for the toolchain.
  - For Raspberry Pi... google "cross-compile Qt for Raspberry Pi".
2. Boot your Linux board. The one that you're going to use as the robot's brains.
3. On your Linux board, install the above libraries, plus Nginx server.
4. Uninstall Apache, unless you know better and/or actually use Apache for something. If it's installed in the first place. Arietta came with Apache installed.
5. Shut down your Linux board. Take out the memory card. Use the "dd" command to copy the card contents into an image on your hard drive.

For Arietta:
6. Mount the larger partition in the image to /mnt/ariettaroot
  - You can get the partitions in the image with the command "fdisk -l <image-file-name>"
  - You then mount with "mount -o loop,offset=<filesystem-offset-in-bytes> <image-file-name> /mnt/ariettaroot
  - You can mount elsewhere, but then you'll have to fix the path to the root filesystem in arietta.cmake
7. Make a directory named "build-daemon". Navigate in it.
8. Copy the arietta.cmake file into this directory.
9. Call cmake with "cmake -DCMAKE_TOOLCHAIN_FILE=arietta.cmake ../RobotControlDaemon"
  - If it complains about not finding referenced items, it means that the Linux board image is missing some of the dependency libraries. Or is not mounted correctly.
10. Call "make"

For Raspberry Pi:
6. Cross-compile Qt. (I had this done. I write what I know.)
7. Install Qt on the Raspberry Pi image.
8. Notice that the qmake executable is actually still built for your PC even though the rest of it is cross-compiled.
9. Navigate to the folder RobotControlDaemon
10. Call the cross-compile qmake
11. Call "make"

Common for both:
12. Copy the image back to the memory card, insert to your Linux board and boot it up.
13. Copy the cross-compiled executable to your board.

For Arietta:
14. Using the online tools at acmesystems.it, make a pin configuration file that will give you a UART.
  - Get the UART on pins that match the custom PCB if mounting it directly on top.
15. Download the configuration file to the Arietta board's boot partition.
16. Replace the previous configuration file there.
17. Reboot the board for the new configuration to take effect.

For Raspberry Pi:
14. Disable the shell in the serial port.
  - On newer images, this is one of the first-bot configuration options.
  - For older images, follow Adafruit's instructions on the subject.

Common for both:
18. Copy the scripts in daemon_build_scripts/target_run_scripts to the Linux board, to folder /home/pi/RCD
19. Copy the folder StaticWebContent to the Linux board to /home/pi/RCD/StaticWebContent
20. Copy the cross-compiled executable to /home/pi/RDC/bin
21. Make /home/pi/RCD/install_config.sh executable and execute it. This should copy the nginx config in the right place.
22. Make similarly executable and execute the install_daemon.sh script. This should get the cross-compiled executable to be handled as a daemon.
23. Check that the UART port name in start_daemon.sh is correct.
24. Make start_daemon.sh and kill_daemon.sh executable if they are not already.
25. Reboot the Linux board.
26. Done. Finished. ...Profit?
  - The Linux board should have the nginx server running and serving the robot's UI.
  - For an easier life, consider setting a static address for the robot. See /etc/networking/interfaces.conf and google "Linux interfaces static address".


