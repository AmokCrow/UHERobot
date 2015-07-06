#!/usr/bin/python

# Import the modules to send commands to the system and access GPIO pins
import RPi.GPIO as gpio
import os
from time import sleep

#Set pin numbering to board numbering
gpio.setmode(gpio.BCM)

#Set up pin 7 as an input
gpio.setup(4, gpio.IN, pull_up_down=gpio.PUD_UP)

# A delay to give pull-up time to settle
sleep(2)

# Set up an interrupt to look for pressed button
gpio.wait_for_edge(4, gpio.FALLING) 

# Shutdown
os.system('shutdown now -h')

