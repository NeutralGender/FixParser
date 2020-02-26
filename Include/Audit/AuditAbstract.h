#ifndef AUDIT_ABSTRACT
#define AUDIT_ABSTRACT

#include <iostream>

class DBDriver;

class AuditAbstract
{
private:
    
public:
    AuditAbstract();
    ~AuditAbstract();

    virtual void create_audit_table( DBDriver* dbdriver,
                                     const std::string& table_name) = 0;

	virtual void trigger_on_update( DBDriver* dbdriver, const std::string&, 
                                    const std::string&) = 0;

	virtual void trigger_on_delete( DBDriver* dbdriver, 
                                    const std::string&, const std::string&) = 0;

};

#endif