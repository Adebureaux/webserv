#!/bin/bash

host="127.0.0.1"
port="8080"

nginx_host="127.0.0.1"
nginx_port="8081"

python3 telnet.py $host $port request/* > webserv.result
# python3 telnet.py $nginx_host $nginx_port request/* > nginx.result

diff webserv.result nginx.result
# rm -rf webserv.result
# rm -rf nginx.result
