#!/bin/sh

USER="pi"

cp -f nginx_config /etc/nginx/nginx.conf

nginx -s reload


