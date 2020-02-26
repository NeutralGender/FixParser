#include "../Include/Read.h"


void Read::read_form_file(std::vector<std::string> &raw_data)
{
	try
	{
		if (!in.is_open())
			throw std::runtime_error("Error, cannot open file!\n");

		std::string str = "";

		while (getline(in, str))
		{
			raw_data.push_back(str);
		}
	}
	catch (const std::runtime_error &r)
	{
		cout << r.what() << endl;
	}
}