#!/bin/bash

USER="pi"

cp -f "/home/${pi}/RCD/nginx_config" /etc/nginx/nginx.conf

nginx -s reload


