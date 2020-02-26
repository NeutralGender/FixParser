#ifndef _DBDRIVER_POSTGRES

#define _DBDRIVER_POSTGRES

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <functional>

#include <pqxx/pqxx>

#include "DBDriver.h"

class DBDriverPostrgres : public DBDriver
{
public:
    DBDriverPostrgres(
            const std::string& dbname_,
            const std::string& user_,
            const std::string& password_,
            const std::string& hostaddr_,
            const std::string& port_
            )
            : DBDriver( dbname_, user_, password_, hostaddr_, port_ )
            {}
    ~DBDriverPostrgres();

    virtual int connect() override;

    virtual void create_table( std::vector< std::vector < std::string > >& parsed_data,
			       const std::string& table) override;

    virtual void drop(const std::string&) override;

    virtual void insert( std::vector< std::vector < std::string > >& parsed_data, 
		         const std::string& table) override;

    virtual void select( std::vector < std::vector < std::string > >& write_data, 
			 const std::string& table ) override;

    virtual int buy(  std::map<std::string,std::pair<int,double>>& ticket_quantity,
	              const std::string& table, 
		      const std::string& from, 
		      size_t stock_count ) override;

    void ticker(const std::string &,size_t stock);
    void stored_procedure(const std::string& a, size_t stock_count);
    int buy_storage_procedure(const std::string& tablename);
    void insert_error(const std::string&, const std::string&);
    void buy_update(size_t, const std::string& table, const std::string& user);
    
    virtual void rise_db( std::map<std::string,std::pair<int,double>>& ticket_quantity,
			  std::vector < std::vector < std::string > >& write_data,
			  const std::string& table) override;

    std::pair<int, std::pair<int, int>> Quantity_position(const std::string& table);

    void if_exist_db(const std::string& user, const std::string& table);

    int if_exist( std::map<std::string,std::pair<int,double>>& ticket_quantity, 
		  const std::string& user, 
		  const std::string& table
                );

};

#endif
