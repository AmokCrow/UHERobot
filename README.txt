This repo is for the Under House Explorer robot project.

Contents:
RobotControlBoard - Code for the MCU on the robot's PCB.
RobotControlDaemon - an FCGI server process. Mediates between the web UI and the control board serial port.
StaticWebContent - The web UI.

Update:
7.4.2014 Thinking of deleting the python cgi part. Development's moving towards fcgi anyways.


Library dependencies (for lack of a better place to list them):
libfcgi-dev (basic FastCGI interface, get c++ ver if separate)
libpthreads-dev (needed for fcgi's threading)
libcgicc-dev (does the cgi inputs parsing)
spawn-fcgi (a lighttdp spin-off, a magic tool for spawning and un-parenting an fgci process with the right pipe. Don't ask me how it does it.)
