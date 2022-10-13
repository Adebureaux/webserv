import telnetlib
import sys

if len(sys.argv) != 4:
    print("Missing argument")
    sys.exit(-1)


host = sys.argv[1]
port = sys.argv[2]


with telnetlib.Telnet(host, port, 1) as session:
	with open(sys.argv[3], 'r') as f:
		session.write(f.readline().encode('ascii') + b"")
	session.write(bytes("\r\n\r\n", 'ascii'))
	output = session.read_all()
	session.close()
	f.close()
	print(output.decode().partition('\n')[0])
