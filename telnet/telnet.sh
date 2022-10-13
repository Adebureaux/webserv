#!/bin/bash

host="127.0.0.1"
port="8080"

if [ $(python3 telnet.py $host $port request/g_host_200) == 200 ]
then echo "g_host_200 success !"
else echo "g_host_200 failed returned "$(python3 telnet.py $host $port request/g_host_200)
fi

if [ $(python3 telnet.py $host $port request/g_simple_200) == 200 ]
then echo "g_simple_200 success !"
else echo "g_simple_200 failed returned "$(python3 telnet.py $host $port request/g_simple_200)
fi

if [ $(python3 telnet.py $host $port request/g_target_404) == 404 ]
then echo "g_target_404 success !"
else echo "g_target_404 failed returned "$(python3 telnet.py $host $port request/g_target_404)
fi

# Not sur for this one
if [ $(python3 telnet.py $host $port request/p_json_404) == 404 ]
then echo "p_json_404 success !"
else echo "p_json_404 failed : returned "$(python3 telnet.py $host $port request/p_json_404)
fi

if [ $(python3 telnet.py $host $port request/p_version_505) == 505 ]
then echo "p_version_505 success !"
else echo "p_version_505 failed : returned "$(python3 telnet.py $host $port request/p_version_505)
fi

# This one send 505 HTTP Version not supported /!\
if [ $(python3 telnet.py $host $port request/u_request_400) == 400 ]
then echo "u_request_400 success !"
else echo "u_request_400 failed : returned "$(python3 telnet.py $host $port request/u_request_400)
fi
