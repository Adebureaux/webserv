typedef enum e_state {
	ERROR = -1 , INCOMPLETE, READY, DONE
} t_state;

class Message
{
public:
	t_state			state;
	std::string		raw_data;
	std::string		header;
	std::string		body;
	Client			*client;
	Message			*ptr; // response || request
	Request			info;

	Message(Client c) :
		client(c),
		state(INCOMPLETE),
		raw_data(NULL),
		header(NULL),
		body(NULL),
		ptr(NULL),
		info(NULL)
	{};
	virtual ~Message();

};

class Client {

private:

public:
	int				fd;
	sockaddr_in		address;
	Message			request;
	Message			response;
	Client(int file_descriptor, sockaddr_in addr) : fd(fd), address(addr) {};
	virtual ~Client();
	int receive(void) {

	}
};
