#include "../../Include/Audit/AuditFix.h"
#include "../../Include/DBDriver/DBDriver.h"

std::string create_audit_table_(std::string table_name)
{
    const std::string sql = "   CREATE TABLE IF NOT EXISTS " + table_name + " \
                                (\
                                " + '"' + "Operation" + '"' + " text   NOT NULL, \
                                " + '"' + "Timestamp" + '"' + " timestamp NOT NULL,\
                                " + '"' + "QuantityReceived" + '"' + "  text,\
                                " + '"' + "Ticker" + '"' + "	      text	NOT NULL\
                                );\
							";
    return (sql);
}

void AuditFix::create_audit_table(  DBDriver* db,
                                    const std::string& table_name)
{        
    auto f = std::bind(create_audit_table_, std::placeholders::_1);

    std::string sql = f(table_name);

    try
    {
        db->conn.prepare(table_name, sql);
        pqxx::work W(db->conn);
        W.exec(sql);
        W.commit();
    }
    catch (const std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}

std::string trigger_on_update_(std::string& trigger_type, const std::string& audit_table, const std::string& data_table)
{
    std::string sql = "\
                        CREATE OR REPLACE FUNCTION audit_" + trigger_type + "() RETURNS TRIGGER AS $audit_" + trigger_type + "$\
                        BEGIN\
                        INSERT INTO " + audit_table + " \
                                (\
                                " + '"' + "Operation" + '"' + ",\
                                " + '"' + "Timestamp" + '"' + ",\
                                " + '"' + "QuantityReceived" + '"' + ",\
                                " + '"' + "Ticker" + '"' + "\
                                )\
                        VALUES ( \
                                " + "'" + trigger_type[0] + "'" + ",\
                                now(),OLD.\
                                " + '"' + "QuantityReceived" + '"' + ",\
                                (Select " + '"' + "Ticker" + '"' + "from " + data_table + " where " + '"' + "Ticker" + '"' + " = OLD." + '"' + "Ticker" + '"' + " )\
                                );\
                        RETURN NEW;\
                        END;\
                        $audit_" + trigger_type + "$ LANGUAGE plpgsql;\
                        CREATE TRIGGER audit_" + trigger_type + " AFTER " + trigger_type + " ON " + data_table + " FOR EACH ROW EXECUTE PROCEDURE audit_" + trigger_type + "();\
                      ";
    //std::cout << sql << std::endl;
    return (sql);
}

void AuditFix::trigger_on_update(   DBDriver* db, 
                                    const std::string& audit_table, 
                                    const std::string& data_table
                                )
{
    /*
    std::vector<std::string> trigger_type = { "UPDATE","INSERT" };

    auto f = std::bind(trigger_on_update_, std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3);

    for (size_t i = 0; i < trigger_type.size(); i++)
    {
        std::string sql = f(trigger_type[i], audit_table, data_table);
        */

    std::string sql = "\
                        CREATE OR REPLACE FUNCTION audit_update() RETURNS TRIGGER AS $audit_update$\
                        BEGIN\
                        INSERT INTO " + audit_table + " \
                                (\"Operation\",\"Timestamp\",\"QuantityReceived\",\"Ticker\")\
                        VALUES ('U',now(),OLD.\"QuantityReceived\",\
                                (Select \"Ticker\" from " + data_table + " where \"Ticker\"  = OLD.\"Ticker\")\
                                );\
                        RETURN NEW;\
                        END;\
                        $audit_update$ LANGUAGE plpgsql;\
                        CREATE TRIGGER audit_update AFTER UPDATE ON " + data_table + " FOR EACH ROW EXECUTE PROCEDURE audit_update();\
                      ";


        try
        {
            pqxx::work W(db->conn);
            W.exec(sql);
            W.commit();
        }
        catch (const std::exception & e)
        {
            std::cout << e.what() << std::endl;
        }
    //}
}

std::string trigger_on_delete_( std::string& trigger_type, 
                                const std::string& table_name, 
                                const std::string& data_table)
{
    std::string sql = "\
                        CREATE OR REPLACE FUNCTION audit_" + trigger_type + "() RETURNS TRIGGER AS $audit_" + trigger_type + "$\
                        BEGIN\
                        INSERT INTO " + table_name + " \
                                (\
                                " + '"' + "Operation" + '"' + ",\
                                " + '"' + "Timestamp" + '"' + ",\
                                " + '"' + "QuantityReceived" + '"' + ",\
                                " + '"' + "Ticker" + '"' + "\
                                )\
                        VALUES ( \
                                " + "'" + trigger_type[0] + "'" + ",\
                                now(),OLD.\
                                " + '"' + "QuantityReceived" + '"' + ",OLD.\
                                " + '"' + "Ticker" + '"' + "\
                                );\
                        RETURN NEW;\
                        END;\
                        $audit_" + trigger_type + "$ LANGUAGE plpgsql;\
                        CREATE TRIGGER audit_" + trigger_type + " AFTER " + trigger_type + " ON " + data_table + " FOR EACH ROW EXECUTE PROCEDURE audit_" + trigger_type + "();\
                      ";
    return (sql);
}

void AuditFix::trigger_on_delete(   DBDriver* db, 
                                    const std::string& table_name, 
                                    const std::string& data_table
                                )
{
    std::string trigger_type = "DELETE";
    auto f = std::bind(trigger_on_delete_, std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3);

    std::string sql = f(trigger_type, table_name, data_table);
    try
    {
        pqxx::work W(db->conn);
        W.exec(sql);
        W.commit();
    }
    catch (const std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}