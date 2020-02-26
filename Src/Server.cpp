#include "../Include/Server.h"

#include "../Include/DBDriver/DBDriver.h"

Server::~Server()
{
    //dtor
    FD_ZERO(&allset);
    FD_ZERO(&rset);
    close(clifd);
    close(sockfd);

    std::cout << "TERMINATED~~\n";
}

int Server::create_socket()
{
    try
    {
        if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) // IPv4 - TCP socket
            throw std::exception();
    }
    catch(const std::exception& r)
    {
        std::cout << "create_socket:" << r.what() << std::endl;
        close(sockfd);
        return 0;
    }

}

int Server::sockaddr_init(size_t port)
{
    bzero(&serv_addr,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

int Server::bind_()
{
    try
    {
        if( (bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0 ))
        {
            throw std::exception();
        }
    }
    catch(const std::exception& r)
    {
        std::cout << "bind " << r.what() << std::endl;
        close(sockfd);
        return 0;
    }
}

int Server::listen_()
{
    try
    {
        listen(sockfd,BACK_LOG); // define BACK_LOG 5
    }
    catch(const std::exception& e)
    {
        //close(sockfd);
        std::cout << "listen" << e.what() << std::endl;
        close(sockfd);
        return 0;
    }

}

int Server::recv_()
{
    try
    {
        int v = read(clifd, arr.data(),
                      sizeof(arr));
                      

        if(v < 0)
            throw std::runtime_error("Send Failed!\n");
        
        std::cout << "Read!" << v << std::endl;
    }
    catch(const std::exception& r)
    {
        //close(sockfd);
        std::cerr << r.what() << errno << '\n';
        close(sockfd);
        return 0;
    }
}

int Server::send_()
{
    try
    {
        int v  = write(clifd, arr.data(),
                      strlen(arr.data()));

        if(v < 0)
            throw std::runtime_error("Send Failed!\n");
        
        std::cout << "Send!" << v << std::endl;
        std::fill(std::begin(arr),std::end(arr),0);
    }
    catch(const std::runtime_error& r)
    {
        std::cerr << r.what() << errno << '\n';
        return 0;
    }

}

int Server::close_()
{
    close(sockfd);
}

int Server::select_( std::map < std::string, std::pair < int, double > > ticket_quantity, 
                     DBDriver* db
                   )
{
    int maxi, maxfd, tempsock;
    int nready, nbytes;

    maxfd = sockfd; // now maxfd = 3;
    maxi = -1; // max index in client[]

    // INIT client array
    std::fill(std::begin(client),std::end(client),-1);
    FD_ZERO(&allset);

    // input in allset struct
    FD_SET(sockfd,&allset);

    try
    {
        for(;;)
        {
            rset = allset;

            // SET rset TO READ && RETURN COUNT OF CLIENTS
            nready = select(maxfd+1,&rset,NULL,NULL,NULL);

            if(FD_ISSET(sockfd,&rset)) // for new connection
            {
                clifd = accept(sockfd,NULL,NULL);

                // FIND FIRST -1 for INPUT IN CLIENT NEW CONNECTION - clifd
                auto it = std::find(client.cbegin(),client.cend(),-1);

                if( it != client.cend() )
                    client[std::distance(client.cbegin(),it)] = clifd;

                // FD_SETSIZE IS 1024
                if( std::distance(client.cbegin(),it) == FD_SETSIZE )
                    throw std::exception();
                    //throw std::exception("Too many clients\n");
            
                FD_SET(clifd,&allset);

                // UPDATE MAX DESCRIPTOR VALUE
                if(clifd > maxfd)
                    maxfd = clifd;

                // UPDATE MAX INDEX IN client
                if( std::distance(client.cbegin(),it) > maxi )
                    maxi = std::distance(client.cbegin(),it); // now maxi=0
                
                if( --nready <= 0 )
                    continue; // no more descriptors ready for reading
            }

            // CHECK EACH CLIENT IF READY TO READ
            for(int i = 0;i<= maxi;i++)
            {
                // IF CLIENT IS EXIST IN ARRAY
                if( (tempsock = client[i]) < 0 )
                    continue;


                // IF DESCRIPTOR IS READY
                if(FD_ISSET(tempsock,&rset))
                {
                    if( (nbytes = read(tempsock,arr.data(),arr.size()) == 0) )
                    {
                        close(tempsock);
                        FD_CLR(tempsock,&allset);
                        client[i] = -1;
                    }
                    else
                    {

                        std::cout << "ARR.DATA():" << arr.data() << std::endl;
                        process_rcvdata( ticket_quantity, db, tempsock);
                        //int verify = write(tempsock,quote.data(),
                                       //strlen(quote.data()) );
                        //std::cout << "verifyR:" << verify << std::endl;
                        arr.fill(0);
                        //std::fill(std::begin(arr),std::end(arr),0); 
                    }

                    if( --nready <= 0 )
                        break; // no more descriptors ready for reading
                }
            }

         }
        FD_ZERO(&allset);
        FD_ZERO(&rset);
        close(clifd);
        close(sockfd);
        

    }
    catch(const std::exception& r)
    {
        close(tempsock);
        close(clifd);
        std::cerr << "Select: " << r.what() << '\n';
        return 0;
    }
    
}

inline std::pair<size_t,std::string> tag(std::string& substr)
{
    std::array<char, 1> separator = { '=' };
    substr.erase(std::remove(substr.begin(), substr.end(), ' '), substr.end());

    for (auto& n : separator)
    {
        if (substr.find(n) != std::string::npos && substr.find_last_of(n) != std::string::npos)
        {
            return std::make_pair
             ( std::stoi(substr.substr(0, substr.find(n))), substr.substr(substr.find_last_of(n)+1) );
        }
    }
    return std::make_pair(0,"");
}


int Server::parse_rcvdata(const std::string& sample)
{
    // EXAMPLE RECVMSG: ~54=Buy ~49=SendComp ~55=Ticker ~56=TargComp ~38=QuantityReceived
    // INPUT DATA: std::array arr;

    size_t pos = sample.find('~');
    std::string substr = "";
    while (pos != sample.npos)
    {
        size_t temp = pos;
        pos = sample.find('~', temp + 1);

        // substr.append(source_str, start_pos, count_of_symb_to_append);
        substr.append(sample, temp + 1, (pos - temp - 1));
        recv_param.insert(tag(substr));
        //std::cout << "parse_rcvdata" << substr << std::endl;
        substr.clear();
    }
    return 0;
}

template <std::size_t SIZE>
inline std::string Server::quote_string(std::array<int, SIZE>& arr)
{
    std::string quote = BeginString;
    for (size_t i = 0; i < arr.size(); i++)
    {
        quote += '~' + std::to_string(arr[i]) + '=' + recv_param[arr[i]];
    }

    //std::cout << std::endl << quote << std::endl << std::endl;
    return (quote);
}

std::string Server::send_reject(const std::string& error_string)
{
    // TargetSubID - 57
    recv_param[57] = recv_param[50];
    
    // Text - 58
    recv_param[58] = error_string;

    //RefSeqNum - 45
    recv_param[45] = recv_param[34];

    return (quote_string(this->reject_arr));
}

void Server::CheckParse( std::map <std::string,std::pair<int,double>> ticket_quantity,
                         DBDriver* db, 
                         std::string& quote)
{
    /*
        0 = Heartbeat
        1 = Test Request
        2 = Resend Request
        3 = Reject
        4 = Sequence Reset
        5 = Logout
        8 = Execution Report
        A = Logon
        D = Order Single
        R = Quote Request
        S = Quote
    */

    switch (recv_param[35][0])
    {
        //  Heartbeat
        case '0':
        {
            std::cout << "Heartbeat\n";

            quote = quote_string(this->header_tail);

            break;
        }
        // Test Request
        case '1':
        {
            std::cout << "Test Request\n";

            quote = send_reject("Test Request");
            
            break;
        }
        // Resend Request
        case '2':
        {
            std::cout << "Resend Request\n";

            quote = quote_string(this->header_tail);

            break;
        }
        // Reject
        case '3':
        {
            std::cout << "Reject\n";

            quote = send_reject("Reject");

            break;
        }
        // Sequence Reset
        case '4':
        {
            std::cout << "Sequence Reset\n";

            quote = send_reject("Sequence Reset");

            break;
        }
        // Logout
        case '5':
        {
            std::cout << "Logout\n";

            quote = quote_string(this->header_tail);

            break;
        }
        // Execution Report
        case '8':
        {
            quote = send_reject("Execution Report");

            break;
        }
        // Logon
        case 'A':
        {
            std::cout << "Logon\n";

            quote = quote_string(this->logon);

            break;
        }
        // Order Single
        case 'D':
        {
            std::cout << "Order Single\n";

            recv_param[34] = std::to_string(std::stoi(recv_param[34]) + 1);

            recv_param[31] = std::to_string(ticket_quantity[recv_param[55]].second);

            recv_param[32] = std::to_string( db->buy( ticket_quantity, 
                                                      "t1", recv_param[55], 
                                                      std::stoi(recv_param[38])
                                                    )
                                           );
            
            if (recv_param[32] == recv_param[38])
                recv_param[150] = "1";
            else
                recv_param[150] = "0";

            quote = quote_string(this->exec_arr);

            break;
        }
        // Quote Request
        case 'R':
        {
            std::cout << "Quote Request\n";

            auto itTicker = ticket_quantity.find(recv_param[55]);
            if (itTicker == ticket_quantity.cend())
            {
                quote = send_reject("Unknown Ticker");
                //return (quote);
            }

            recv_param[34] = std::to_string(std::stoi(recv_param[34]) + 1);
            recv_param[133] = std::to_string(ticket_quantity[recv_param[55]].second);
            //quote = quote_string(this->quote_arr);
            quote = quote_string(this->quote_arr);

            break;
        }
        // Quote
        case 'S':
        {
            quote = send_reject("Quote");
            break;
        }

        // ERROR TYPE
        default:
        {
            quote = send_reject("Unknown Operation");
            break;
        }
    }
    
}

int Server::process_rcvdata( std::map <std::string,std::pair<int,double>> ticket_quantity, 
                             DBDriver* db, 
                             int ClientSocket)
{
    std::string quote = "";
    parse_rcvdata(std::string( arr.data(), strlen(arr.data()) ) );
    CheckParse( ticket_quantity, db, quote);
    
    std::cout << "process_rcvdata->" << arr.data() << std::endl;
    std::cout << "Qoteprocess_rcvdata:" << quote << std::endl;

    snd_count = write(ClientSocket, quote.c_str(), quote.size());
    std::cout << "verifyR:" << snd_count << std::endl;
    if (snd_count < 0) {
        throw std::runtime_error("No recv_BYTES send: " + errno + '\n');
    }

    
    for (auto it = recv_param.cbegin(); it != recv_param.cend(); ++it)
        std::cout << it->first << " : " << it->second << std::endl;
    

    recv_param.clear();
    return 0;
}
