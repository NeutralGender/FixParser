#ifndef _PARSER_ABSTRACT
#define _PARSER_ABSTRACT

#include <string>
#include <vector>
#include <map>

class ParserAbstract
{
private:
    
public:
    ParserAbstract();
    ~ParserAbstract();

    virtual void parse( const std::vector<std::string>& raw_data,
				        std::vector< std::vector <std::string > >& parsed_data  
			          ) = 0;
    
    //virtual void merge_insert() = 0;

    virtual void create_ticker( std::vector< std::vector <std::string > >& parsed_data,
						        std::map<std::string, std::pair<int,double>>& ticket_quantity 
					          ) = 0;

};

#endif