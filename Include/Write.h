#ifndef _Write
#define _Write

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Write
{
public:
	explicit Write( const std::string &path ) : wr( path, std::ios_base::trunc ) {}
	~Write();

	void write_to_file( const std::vector< std::vector< std::string > >& write_data );

private:
	std::ofstream wr;
};

#endif