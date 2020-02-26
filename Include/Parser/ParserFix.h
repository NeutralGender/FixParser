#ifndef _Parser
#define _Parser

#include "ParserAbstract.h"

#include <vector>
#include <string>
#include <regex>

#include "../Read.h"
#include "../DBDriver/DBDriver.h"
/*
#include "DBDriver.h"
class DBDriver;

#include "Server.h"
class Server;
*/

class ParserFix : public ParserAbstract
{
public:
	ParserFix() {};
	~ParserFix() {}

	virtual void parse( const std::vector<std::string>& raw_data,
						std::vector< std::vector <std::string > >& parsed_data  
			  		  ) override;

	//	NEED TO WORK!!!!!!!!!!
	//void merge_insert(Parser& parser) override;
	//

	void create_ticker( std::vector< std::vector <std::string > >& parsed_data,
						std::map<std::string, std::pair<int,double>>& ticket_quantity 
					  ) override;
};

#endif