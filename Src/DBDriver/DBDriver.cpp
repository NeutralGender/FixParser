#include "../../Include/DBDriver/DBDriver.h"

DBDriver::DBDriver( const std::string& dbname_,
                    const std::string& user_,
                    const std::string& password_,
                    const std::string& hostaddr_,
                    const std::string& port_)
                  : conn("dbname=" + dbname_
                       + " user=" + user_
                       + " password=" + password_
                       + " hostaddr=" + hostaddr_
                       + " port=" + port_)
{
}

DBDriver::~DBDriver()
{
}
