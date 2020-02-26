#ifndef _Read
#define _Read

#include <iostream>

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Read
{
public:
	explicit Read( const string &path ) : in(path)
	{
	}
	~Read()
	{
		in.close();
	}

	void read_form_file( std::vector<std::string>& );

private:
	ifstream in;

};

#endif