#!/bin/bash

cd ../RobotControlDaemon
/home/crow/RasPiCC/qt5pi/bin/qmake RobotControlDaemon.pro
make -j 4
make clean

cd ../daemon_build_scripts

