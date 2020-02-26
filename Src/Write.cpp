#include "../Include/Write.h"


Write::~Write()
{
	wr.close();
}

void Write::write_to_file( const std::vector< std::vector< std::string > >& write_data )
{
	try
	{
		if (!wr.is_open())
			throw std::runtime_error("Cannot open write file!");

		for (size_t record = 0; record < write_data.size(); record++)
		{
			for (size_t field = 0; field < write_data[record].size()-1; field++)
			{
				wr << write_data[record][field] <<",";
			}
			wr << write_data[record][write_data[record].size()-1] << std::endl;
		}
	}
	catch (const std::runtime_error & r)
	{
		std::cout << r.what() << std::endl;
	}
}
