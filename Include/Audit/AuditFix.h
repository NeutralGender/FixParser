#ifndef AUDIT_FIX
#define AUDIT_FIX

#include "AuditAbstract.h"

#include <pqxx/pqxx>

#include <iostream>
#include <string>
#include <vector>
#include <functional>

class AuditFix : public AuditAbstract
{
private:

public:
	AuditFix() {}
	~AuditFix() {}

	virtual void create_audit_table( DBDriver* dbdriver, 
									 const std::string& table_name) override;
    
	virtual void trigger_on_update( DBDriver* dbdriver, 
									const std::string& audit_table, 
                                    const std::string& data_table) override;

	virtual void trigger_on_delete( DBDriver* dbdriver, 
                                    const std::string& audit_table, 
									const std::string& data_table) override;


};

#endif