#!/bin/bash

USER="pi"

cp -f "/home/${USER}/RCD/nginx_config" /etc/nginx/nginx.conf

nginx -s reload


