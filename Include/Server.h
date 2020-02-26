#ifndef _Server
#define _Server

// C++
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <exception>

// LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

//BACKLOG FOR int listen(int,int);
#define BACK_LOG 5

class DBDriver;


class Server
{
public:
	Server( const std::string&& ip_addr_,
		    const std::string&& port_) : ip_addr(ip_addr_), port(port_),
										BeginString("SND:8=FIX.4.2")
	{
	}
	~Server();

    int create_socket();
    int sockaddr_init(size_t port);
    int bind_();
    int listen_();
    int recv_();
    int send_();
    int close_();
    int select_( std::map < std::string, std::pair < int, double > > ticket_quantity, 
                 DBDriver* db
               );

	//int recvdata(Parser&, DBDriver&,int&);	
	int parse_rcvdata( const std::string& );

	void CheckParse( std::map <std::string,std::pair<int,double>> ticket_quantity,
                     DBDriver* db, 
                     std::string& quote);

	template <std::size_t SIZE>
		std::string quote_string( std::array<int, SIZE>& arr );

	std::string send_reject( const std::string& error_string );

	int process_rcvdata( std::map <std::string,std::pair<int,double>> ticket_quantity, 
                         DBDriver* db, 
                         int ClientSocket);

private:
	const std::string ip_addr;
	const std::string port;

	std::map<size_t, std::string> recv_param;
	const std::string BeginString;


	int sockfd = 0; // server fd for socket
    int clifd = 0; // client fd after accept()
    struct sockaddr_in serv_addr, cli;
    std::array<char,512> arr;
    std::array<int,FD_SETSIZE> client;
    fd_set rset,allset;


	//NOW WITHOUT 43 FROM ST.HEADER !!!!!!!!!!!!!!!!
	std::array<int, 10> quote_arr = { 9,10,34,35,49,52,55,56,131,133 };
	std::array<int, 15> single_arr = { 9,10,11,17,32,34,35,37,38,49,50,52,55,56,59 };
	std::array<int, 17> exec_arr = { 9,10,11,17,31,32,34,35,37,38,49,50,52,55,56,59,150 };
	std::array<int, 8> header_tail = { 9,10,34,35,43,49,52,56 };
	std::array<int, 10> logon = { 9,10,34,35,43,49,52,56,108,141 };
	std::array<int, 11> reject_arr = { 9,10,34,35,43,45,49,52,56,57,58 };

	int snd_count = 0;
};

#endif