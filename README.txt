This repo is for the Building And Construction Narrow Spaces Exploration Robot project.

Contents:
RobotControlBoard - an Arduino sketch for the Arduino-compatible servo- and motor-control MCU.
RobotControlDaemon - a GCC project for a linux daemon that listens for input from a named pipe and outputs instructions to a serial interface. This is the brains of the robot, containing most of the logic. May be upgraded to a Qt project if it grows too big for a Makefile. Written in C++ (C++11).
RobotCgiUI - the UI component. Perl, HTML and JavaScript.

Update:
7.4.2014 Thinking of deleting the python cgi part. Development's moving towards fcgi anyways.


Library dependencies (for lack of a better place to list them):
libfcgi-dev (basic FastCGI interface, get c++ ver if separate)
libpthreads-dev (needed for fcgi's threading)
libcgicc-dev (does the cgi inputs parsing)
spawn-fcgi (a lighttdp spin-off, a magic tool for spawning and un-parenting an fgci process with the right pipe. Don't ask me how it does it.)
