#!/bin/sh


if [ -f /var/run/uherobotd.pid ]
then
  kill -9 `cat /var/run/uherobotd.pid`
fi

