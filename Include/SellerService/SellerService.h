#ifndef _SELLER_SERVICE
#define _SELLER_SERVICE

#include "../DBDriver/DBDriver.h"
#include "../Parser/ParserAbstract.h"
#include "../Audit/AuditAbstract.h"
#include "../Server.h"
#include "../Write.h"
#include "../Read.h"

#include <string>
#include <vector>
#include <map>

class SellerService
{
private:
    std::vector < std::string > raw_data;
    std::vector < std::vector < std::string > > parsed_data;
    std::vector < std::vector < std::string > > write_data;
    std::map < std::string, std::pair < int, double > > ticket_quantity;

public:
    SellerService();
    ~SellerService();

    void ReadDataFromFile( const std::string& path_to_read );
    void ParseData( ParserAbstract* parser );
    void MergeParsedData( const std::string& path_to_read );
    void WriteFromTableIntoFile( DBDriver* driver, 
                                 const std::string& table_name,
                                 const std::string& path_to_file );

    // DB functions:
    void ConnectToDB( DBDriver* dbdriver );
    void CreateTable( DBDriver* dbdriver, const std::string& table_name );
    void DropTable( DBDriver* dbdriver, const std::string& table_name );

    void InsertIntoTable( DBDriver* dbdriver,
                          const std::string& table_name 
                        );

    void SelectAllFromTable( DBDriver* dbdriver,
                             const std::string& table_name 
                           );

    // Parser functions:
    void CreateTicker( ParserAbstract* parser );

    void UpdataTicker_quantity( DBDriver* dbdriver, 
                                const std::string table_name );

    void BuyTicker( DBDriver* dbdriver,
                    const std::string& table_name,
                    const std::string& from,
                    const size_t& stock_count );

    // Audit Functions
    void CreateAuditTable( AuditAbstract* audit, 
                           DBDriver* dbdriver, 
                           const std::string& table_name );
    
	void TriggerOnUpdate( AuditAbstract* audit,
                          DBDriver* dbdriver, 
                          const std::string& audit_table, 
                          const std::string& data_table) ;

	void TriggerOnDelete( AuditAbstract* audit, 
                          DBDriver* dbdriver,
                          const std::string& audit_table, 
                          const std::string& data_table);


    // Server function
    void SellTickets( Server* server,
                      DBDriver* dbdriver );
};

#endif
