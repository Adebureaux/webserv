import telnetlib
import base64

import cv2

request = """
POST /test/ HTTP/1.0 From: frog@jmarshall.com
User-Agent: HTTPTool/1.0
Content-Type: application/x-www-form-urlencoded
Content-Length: 32
"""

host = "127.0.0.1"
port = 8083

with telnetlib.Telnet(host, port, 100) as session:
    session.write(str.encode(request))
    output = session.read_until(b"done", 100)
    session.close()
    print(output)
    print("Done")
