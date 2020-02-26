#include "../../Include/SellerService/SellerService.h"

#include "../../Include/DBDriver/DBDriver.h"
#include "../../Include/Parser/ParserAbstract.h"


SellerService::SellerService()
{
}

SellerService::~SellerService()
{
}

void SellerService::ReadDataFromFile( const std::string& path_to_read )
{
    Read read( path_to_read );
    read.read_form_file( raw_data );
}

void SellerService::ConnectToDB(DBDriver* dbdriver)
{
    dbdriver->connect();
}

void SellerService::ParseData(ParserAbstract* parser)
{
    parser->parse( raw_data, parsed_data );
}

void SellerService::CreateTable( DBDriver* dbdriver, 
                                 const std::string& table_name 
                               )
{
    dbdriver->create_table( parsed_data, table_name );
}

void SellerService::DropTable( DBDriver* dbdriver, const std::string& table_name )
{
    dbdriver->drop( table_name );
}

void SellerService::InsertIntoTable( DBDriver* dbdriver,
                                     const std::string& table_name 
                                   )
{
    dbdriver->insert( parsed_data, table_name );
}

void SellerService::SelectAllFromTable( DBDriver* dbdriver,
                                        const std::string& table_name 
                                      )
{
    dbdriver->select( write_data, table_name );
}

void SellerService::CreateTicker( ParserAbstract* parser )
{
    parser->create_ticker( parsed_data, ticket_quantity );
}

void SellerService::UpdataTicker_quantity( DBDriver* dbdriver, 
                                           const std::string table_name )
{
    dbdriver->rise_db( ticket_quantity, write_data, table_name );
}

void SellerService::BuyTicker( DBDriver* dbdriver,
                               const std::string& table_name,
                               const std::string& from,
                               const size_t& stock_count )
{
    dbdriver->buy( ticket_quantity, table_name, from, stock_count );
}

void SellerService::CreateAuditTable( AuditAbstract* audit,
                                      DBDriver* dbdriver,
                                      const std::string& table_name )
{
    audit->create_audit_table( dbdriver, table_name );
}

void SellerService::TriggerOnUpdate( AuditAbstract* audit,
                                     DBDriver* dbdriver, 
                                     const std::string& audit_table, 
                                     const std::string& data_table
                                    )
{
    audit->trigger_on_update( dbdriver, audit_table, data_table );
}

void SellerService::TriggerOnDelete( AuditAbstract* audit,
                                     DBDriver* dbdriver, 
                                     const std::string& audit_table, 
                                     const std::string& data_table
                                    )
{
    audit->trigger_on_delete( dbdriver, audit_table, data_table );
}

void SellerService::SellTickets( Server* server,
                                 DBDriver* dbdriver )
{
    server->select_( ticket_quantity, dbdriver );
}

void SellerService::WriteFromTableIntoFile( DBDriver* driver, 
                                            const std::string& table_name,
                                            const std::string& path_to_file )
{
    std::vector< std::vector< std::string > > write_to_file;
    driver->select( write_to_file, table_name );

    Write wr( path_to_file );
    wr.write_to_file( write_to_file );
}
