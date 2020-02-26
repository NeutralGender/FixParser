#include <iostream>
#include "Include/DBDriver/DBDriverPostgres.h"
#include "Include/Parser/ParserFix.h"
#include "Include/Parser/ParserAbstract.h"
#include "Include/SellerService/SellerService.h"
#include "Include/Audit/AuditFix.h"
#include "Include/Server.h"

int main()
{

    DBDriverPostrgres postgres("postgres","postgres","postgres","127.0.0.1","5432");
    AuditFix audit;
    ParserFix parser;

    SellerService seller;

    seller.ReadDataFromFile("IO_files/copy.csv");

    seller.ParseData( &parser );
    seller.CreateTicker( &parser );

    seller.ConnectToDB( &postgres );
    seller.DropTable( &postgres, "t2" );
    seller.CreateTable( &postgres, "t2" );
    seller.InsertIntoTable( &postgres, "t2" );

    seller.CreateAuditTable( &audit, &postgres, "audit1" );
    seller.TriggerOnUpdate( &audit, &postgres, "audit1", "t2" );
    seller.WriteFromTableIntoFile( &postgres, "t2", "Out.txt" );
    //seller.TriggerOnDelete( &audit, &postgres, "audit1", "t2" );

    std::cout << "waiting for client!" << std::endl;

    Server s("127.0.0.1","8000");
    s.create_socket();
    s.sockaddr_init(8000);
    s.bind_();
    s.listen_();

    seller.SellTickets( &s, &postgres );

}

/*
g++ -std=c++17 main.cpp -I Include/Audit/ -I Include/DBDriver/ -I Include/Parser/ 
                        -I Include/SellerService/ -I Include/ 
                        Src/Audit/*.cpp Src/DBDriver/*.cpp Src/Parser/*.cpp 
                        Src/SellerService/*.cpp Src/*.cpp -lpqxx -o main
*/