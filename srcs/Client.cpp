class client {

private:

public:
	int fd;
	sockaddr_in address;

	client (int file_descriptor, sockaddr_in addr) : fd(fd), address(addr) {};
	virtual ~client ();
};
