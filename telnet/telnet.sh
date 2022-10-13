#!/bin/bash

host="127.0.0.1"
port="8080"

python3 telnet.py $host $port request/g_host_ok
python3 telnet.py $host $port request/g_simple_ok
python3 telnet.py $host $port request/g_target_ko

python3 telnet.py $host $port request/p_version_ko

# This one send 505 HTTP Version not supported /!\
python3 telnet.py $host $port request/u_request_ko
