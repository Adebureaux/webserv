import telnetlib
import sys

if len(sys.argv) != 4:
    print("Missing argument")
    sys.exit(1)

host = sys.argv[1]
port = sys.argv[2]

# with open(sys.argv[3], 'r') as f:
# 	print(f.read())

request = []

with open(sys.argv[3], 'r') as f:
	for l in f:
		request.append(l.rstrip("\n"))
	f.close()

with telnetlib.Telnet(host, port, 1) as session:
	for l in request:
		session.write(l.encode('ascii') + b"")
		session.write(bytes("\r\n", 'ascii'))
	session.write(bytes("\r\n\r\n", 'ascii'))
	output = session.read_some()
	session.close()
	start_line = output.decode().partition('\n')[0]
	status = start_line.split()
	print(status[1])
