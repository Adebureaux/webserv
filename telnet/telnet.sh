#!/bin/bash

host="127.0.0.1"
port="8080"

python3 telnet.py $host $port request/* > webserv.result
