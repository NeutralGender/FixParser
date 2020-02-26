#include "../../Include/Parser/ParserFix.h"

void ParserFix::parse( const std::vector<std::string>& raw_data,
					   std::vector< std::vector <std::string > >& parsed_data  
					 )
{
	for (auto& raw_string : raw_data)
	{
		std::vector<string> temp;

		size_t start = 0;
		size_t current = raw_string.find(',');

		while (current != std::string::npos)
		{
			temp.push_back(raw_string.substr(start, current - start));
			start = current + 1;
			current = raw_string.find(',', current + 1);
		}
		temp.push_back(raw_string.substr(start, current - start));

		parsed_data.push_back(temp);
	}

}


/* NEEED TO WORK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// FUNCTION FOR MERGE NEW FILE INTO TABLE(NOT CONCAT)
void ParserFix::merge_insert(std::vector< std::vector <std::string > >& parsed_data)
{
	// TEMP VECTOR FOR COLLECTING NEW COLUMNS IN CORRECT SEQUENCE
	std::vector<std::vector<std::string>> temp;

	// RESIZE VECTOR FOR PUSHBACK
	temp.resize(parser.parse_data.size());

	for (size_t i = 0; i < this->parse_data[0].size(); i++)
	{
		auto it = std::find(parser.parse_data[0].cbegin(),
							parser.parse_data[0].cend(),
							this->parse_data[0][i]);

		if (it != parser.parse_data[0].cend()) // SO, COLUMN WAS FOUND
		{
			for (size_t j = 0; j < parser.parse_data.size(); j++) // PUSHBACK WHOLE COLUMN INTO VECTOR
			{
				temp[j].push_back(parser.parse_data[j][std::distance(parser.parse_data[0].cbegin(), it)]);
			}
		}
		else
		{
			for (size_t j = 0; j < parser.parse_data.size(); j++) // NO SUCH COLUMN
			{
				temp[j].push_back("N/A");
			}
		}
	}
}
*/


inline bool isNumber(string s)
{
	static const std::regex UNSIGNED_INT_TYPE("[+]?[0-9]+");

	return std::regex_match(s, UNSIGNED_INT_TYPE);
}

inline double rate(std::string& rate)
{
	static const std::regex UNSIGNED_DOUBLE_TYPE("[+]?[0-9]+[.]?[0-9]+");

	if (std::regex_match(rate, UNSIGNED_DOUBLE_TYPE))
	{
		return (std::stod(rate));
	}
	else if (rate == "N/A")
		return (0);
	else
		return (-2);


}

void ParserFix::create_ticker( std::vector< std::vector <std::string > >& parsed_data,
							   std::map<std::string, std::pair<int,double>>& ticket_quantity 
							 )
{
	parsed_data[0].push_back("Ticker");

	// Find in first vector( with columns names ) position of each Column below:
	auto itDescription = std::find(parsed_data[0].cbegin(),parsed_data[0].cend(),"Description");
	auto itQuantity = std::find(parsed_data[0].cbegin(), parsed_data[0].cend(), "QuantityReceived");
	auto itRate = std::find(parsed_data[0].cbegin(), parsed_data[0].cend(), "Rate");
	auto itTicker = std::find(parsed_data[0].cbegin(), parsed_data[0].cend(), "Ticker");

	for (size_t i = 1; i < parsed_data.size(); i++)
	{
		// if error return -1;
		int pos = static_cast<int>(parsed_data[i][std::distance(parsed_data[0].cbegin(),
																itDescription)].find(' '));

		parsed_data[i].push_back(parsed_data[i][std::distance(parsed_data[0].cbegin(),
															  itDescription)].substr(0, pos));

		// check if "QuantityReceived" is integer, precheck for db operations
		if (isNumber(parsed_data[i][std::distance(parsed_data[0].cbegin(),itQuantity)]))
		{
			// insert into std::map pair < ticket, (int)QuantityRecv >;
			ticket_quantity.insert
			(
				std::make_pair
				(
					parsed_data[i][std::distance(parsed_data[0].cbegin(), itTicker)].substr(0, pos),
					// make_pair<int,double> for <QuontityReceived,Rate>
					std::make_pair
					(
						std::stoi(parsed_data[i][std::distance(parsed_data[0].cbegin(), itQuantity)]),
						rate(parsed_data[i][std::distance(parsed_data[0].cbegin(), itRate)])
					)
					
				)
			);
		}
		else
			continue;

	}
	/*
	for (auto it = ticket_quantity.cbegin(); it != ticket_quantity.cend(); it++)
	{
		std::cout << it->first << " : " << it->second.first << " : " << it->second.second << std::endl;
	}
	*/
	
	/*
	for (auto it = ticket_quantity.begin(); it != ticket_quantity.end(); it++)
		std::cout << it->first << "" << it->second << std::endl;
		*/
}
