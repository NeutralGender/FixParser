#ifndef _DBDRIVER
#define _DBDRIVER

#include "../Parser/ParserAbstract.h"
#include "../Audit/AuditAbstract.h"

#include "pqxx/connection.hxx"

#include "vector"
#include "string"
#include "map"

class DBDriver
{
protected:
    pqxx::connection conn;

public:
    DBDriver(const std::string& dbname_,
             const std::string& user_,
             const std::string& password_,
             const std::string& hostaddr_,
             const std::string& port_);
    ~DBDriver();

    virtual int connect() = 0;

    virtual void create_table( std::vector< std::vector < std::string > >& parsed_data,
					           const std::string& table) = 0;
    
    virtual void drop(const std::string&) = 0;

    virtual void insert( std::vector< std::vector < std::string > >& parsed_data, 
						 const std::string& table) = 0;
    
    virtual void select( std::vector < std::vector< std::string > >& write_data, 
				         const std::string& table ) = 0;

    virtual void rise_db( std::map<std::string,std::pair<int,double>>& ticket_quantity,
						  std::vector < std::vector < std::string > >& write_data,
						  const std::string& table) = 0;

    virtual int buy(  std::map<std::string,std::pair<int,double>>& ticket_quantity,
					  const std::string& table, 
					  const std::string& from, 
					  size_t stock_count ) = 0;

    friend class AuditFix;
};


#endif