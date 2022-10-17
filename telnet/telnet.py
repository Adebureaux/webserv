import telnetlib
import sys

if len(sys.argv) < 4:
    print("Missing argument")
    sys.exit(1)

host = sys.argv[1]
port = sys.argv[2]

request = []

i = 3
while i < len(sys.argv):
	with open(sys.argv[i], 'r') as f:
		for l in f:
			request.append(l.rstrip("\n"))
		print(request[0])
		with telnetlib.Telnet(host, port, 1) as session:
			for l in request:
				session.write(l.encode('ascii') + b"")
				session.write(bytes("\r\n", 'ascii'))
			session.write(bytes("\r\n", 'ascii'))
			output = session.read_some()
			session.close()
			start_line = output.decode().partition('\n')[0]
			status = start_line.split()
			print(status[1])
		request.clear()
		f.close()
	i += 1
