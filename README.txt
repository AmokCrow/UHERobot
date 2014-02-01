This repo is for the Building And Construction Narrow Spaces Exploration Robot project.

Contents:
RobotControlBoard - an Arduino sketch for the Arduino-compatible servo- and motor-control MCU.
RobotControlDaemon - a GCC project for a linux daemon that listens for input from a named pipe and outputs instructions to a serial interface. This is the brains of the robot, containing most of the logic. May be upgraded to a Qt project if it grows too big for a Makefile. Written in C++ (C++11).
RobotCgiUI - the UI component. Perl, HTML and JavaScript.




