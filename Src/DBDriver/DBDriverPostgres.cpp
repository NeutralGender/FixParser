#include "../../Include/DBDriver/DBDriverPostgres.h"
#include "../../Include/Parser/ParserAbstract.h"
class Parser;

DBDriverPostrgres::~DBDriverPostrgres()
{
	conn.disconnect();
}

int DBDriverPostrgres::connect()
{
	try
	{
		if (!conn.is_open())
			throw std::runtime_error("Cannot open DB\n");
	}
	catch (const std::runtime_error &r)
	{
		std::cout << r.what() << std::endl;
		return -1;
	}
	return 0;
}

void DBDriverPostrgres::create_table( std::vector< std::vector < std::string > >& parsed_data,
									  const std::string& table)
{
	// string for select querry
	std::string sql = "CREATE TABLE IF NOT EXISTS " + table + "(";

	for (size_t field = 0; field < (parsed_data[0].size() - 1); ++field)
	{
		// For "QuantityReceived" integer
		if (parsed_data[0][field] == "QuantityReceived" || parsed_data[0][field] == "Ticker")
		{	
			sql += '"' + parsed_data[0][field] + '"' + " integer,";
			continue;
		}
		sql += '"' + parsed_data[0][field] + '"' + " TEXT,";
	}
	sql += '"' + parsed_data[0][parsed_data[0].size()-1] + '"' + " TEXT );";

	pqxx::work W(conn);

	try
	{
		std::string table_exist = "SELECT EXISTS (\
								   SELECT 1\
								   FROM   information_schema.tables\
								   WHERE  table_name = 't1'\
								   ); ";
		pqxx::result r = W.exec(sql);
		W.commit();

		std::cout << "Create_Table: " << r[0][0] << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

std::string drop_(const std::string& table)
{
	std::string sql = "DROP TABLE IF EXISTS " + table;
	return (sql);
}

void DBDriverPostrgres::drop(const std::string &table)
{
	auto f = std::bind(drop_, std::placeholders::_1);

	std::string sql = f(table);

	pqxx::work Drop(conn);

	pqxx::result r = Drop.exec(sql);
	Drop.commit();
}

void DBDriverPostrgres::insert( std::vector< std::vector < std::string > >& parsed_data, 
								const std::string& table)
{

	//"INSERT INTO public.table () VALUES(), VALUES();"
	std::string sql = "INSERT INTO public." + table + " (";
	for (size_t column = 0; column < parsed_data[0].size()-1; ++column)
	{
		sql += '"' + parsed_data[0][column] + '"' + ",";
	}
	sql += '"' + parsed_data[0][parsed_data[0].size() - 1] + '"' + ") VALUES";

	for (size_t record = 1; record < parsed_data.size(); ++record)
	{
		sql += " (";
		for (size_t field = 0; field < parsed_data[record].size()-1; ++field)
		{
			sql += "'" + parsed_data[record][field] + "'" + ",";
		}
		sql += "'" +  parsed_data[record][parsed_data[record].size()-1] + "'" + "),";
	}
	
	// change to ';' last symbol
	sql[sql.size() - 1] = ';';

	pqxx::work W(conn);

	try
	{
		W.exec(sql);
		W.commit();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

}

std::string select_column_name_(const std::string& table)
{
	std::string sql = "select column_name,data_type \
					   from information_schema.columns \
					   where table_name = '" + table + "'; ";

	return (sql);
}

void select_column_name(pqxx::connection &conn, const std::string& table,
						std::vector <std::vector<std::string>>& write_data)
{
	auto f = std::bind(select_column_name_, std::placeholders::_1);

	std::string sql = f(table);

	pqxx::nontransaction nontransact(conn);
	pqxx::result res(nontransact.exec(sql));

	std::vector<std::string> temp;
	for (pqxx::result::const_iterator c = res.begin(); c != res.end(); ++c) 
	{
		temp.push_back(c[0].as<std::string>());
	}
	write_data.push_back(temp);
}

std::string select_(const std::string& table)
{
	std::string sql = "SELECT * FROM " + table;
	return (sql);
}

void DBDriverPostrgres::select( std::vector < std::vector < std::string > >& write_data, 
								const std::string& table )
{
	select_column_name(conn, table, write_data);

	auto f = std::bind(select_, std::placeholders::_1);

	std::string sql = f(table);

	pqxx::nontransaction nontransact(conn);
	pqxx::result res(nontransact.exec(sql));

	for (pqxx::result::const_iterator c = res.begin(); c != res.end(); ++c) 
	{
		std::vector<std::string> temp;
		for (int i = 0; i < static_cast<int>(c.size()); ++i)
		{
			temp.push_back(c[i].as<std::string>());
		}

		write_data.push_back(temp);
	}

}

std::string ticker_(const std::string& ticker, size_t stock)
{
	std::string sql = "SELECT ticker('" + ticker + "', " + std::to_string(stock) + ")";
	return (sql);
}

void DBDriverPostrgres::ticker(const std::string& ticker, size_t stock)
{
	// 1 - NEGATIVE
	if (((stock >> 31) & 0x1))
	{
		return;
	}
	else
	{
		typedef std::function<std::string(const std::string&, size_t)> ExampleFunction;
		ExampleFunction f = std::bind(ticker_, std::placeholders::_1,
			std::placeholders::_2);

		std::string sql = f(ticker, stock);

		pqxx::work W(conn);

		pqxx::result r = W.exec(sql);
		W.commit();

		std::cout << r[0][0].as<std::string>() << std::endl;
	}
}

std::string stored_procedure_(const std::string& a, size_t stock_count)
{
	std::string sql = "\
						CREATE FUNCTION ticker1(IN " + a + " text) RETURNS integer			 \
						AS $$																	 \
						DECLARE																	 \
							c integer:=(SELECT " + '\"' + "QuantityReceived" + '\"' + " FROM t1 WHERE " + '\"' + "Ticker" + '\"' + "  '=" + a + "' );	 \
						BEGIN																	 \
							IF(c - " + std::to_string(stock_count) + " ) > 0 THEN													 \
								RAISE NOTICE 'c is greater than b';								 \
								UPDATE t1 SET " + '"' + "QuantityReceived" + '"' + " = (c - " + std::to_string(stock_count) + " ) WHERE " + '"' + "Ticker" + '"' + " =  '" + a + "' ;	 \
								RETURN(c - " + std::to_string(stock_count) + " );													 \
							END IF;																 \
																								 \
							IF(c - " + std::to_string(stock_count) + " ) < 0 THEN													 \
								RAISE NOTICE 'c is less than b';								 \
								UPDATE t1 SET " + '"' + "QuantityReceived" + '"' + " = (c)WHERE " + '"' + "Ticker" + '"' + " =  '" + a + "' ;		 \
								RETURN(c);														 \
							END IF;																 \
																								 \
							IF(c - " + std::to_string(stock_count) + " ) = 0 THEN													 \
							   RAISE NOTICE 'c is equal to b';									 \
							   UPDATE t1 SET " + '"' + "QuantityReceived" + '"' + " = 0 WHERE " + '"' + "Ticker" + '\"' + " = '" + a + "' ;		 \
							   RETURN(0);														 \
							END IF;																 \
					   END;																		 \
					$$ LANGUAGE plpgsql;\
	";
	
	return (sql);
}

void DBDriverPostrgres::stored_procedure(const std::string& a, size_t stock_count)
{
	typedef std::function<std::string(const std::string&, size_t)> ExampleFunction;
	ExampleFunction f = std::bind(stored_procedure_, std::placeholders::_1,
														   std::placeholders::_2);

	std::string sql = f( a, stock_count );

	try
	{
		pqxx::work W(conn);
		W.exec(sql);
		W.commit();
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
}

std::string buy_storage_procedure_(const std::string& tableName, const char quote, const std::string& QuantityReceived, const std::string& Ticker)
{
std::string sql = "CREATE FUNCTION buy(IN from_u text, IN to_u text,IN b integer) RETURNS integer\
			AS $buy$ DECLARE \
				c integer:=0;\
				to_u_stock integer:=0;\
	BEGIN\
		c :=(SELECT " + (quote + QuantityReceived + quote) + " FROM " + tableName + " WHERE  " + (quote + Ticker + quote) + "  = from_u);\
		to_u_stock:=(SELECT " + (quote + QuantityReceived + quote) + " FROM " + tableName + " WHERE  " + (quote + Ticker + quote) + "  = to_u);\
	\
	IF(c - b) > 0 THEN\
		UPDATE " + tableName + " SET " + (quote + QuantityReceived + quote) + " = (c - b) WHERE  " + (quote + Ticker + quote) + "  = from_u;\
		UPDATE " + tableName + " SET " + (quote + QuantityReceived + quote) + " = (to_u_stock + b) WHERE  " + (quote + Ticker + quote) + "  = to_u;\
		RETURN(c - b);\
	END IF;\
	\
	IF(c - b) < 0 THEN\
		UPDATE " + tableName + " SET " + (quote + QuantityReceived + quote) + " = (c) WHERE  " + (quote + Ticker + quote) + "  = from_u;\
		RETURN(-4);\
	END IF;\
	\
	IF(c - b) = 0 THEN\
		UPDATE " + tableName + " SET " + (quote + QuantityReceived + quote) + " = (0) WHERE  " + (quote + Ticker + quote) + "  = from_u;\
		UPDATE " + tableName + " SET " + (quote + QuantityReceived + quote) + " = (to_u_stock + b) WHERE  " + (quote + Ticker + quote) + "  = to_u;\
		RETURN(0);\
	END IF;\
	END;\
	$buy$ LANGUAGE plpgsql;\
	";
	

	return (sql);
}

int DBDriverPostrgres::buy_storage_procedure(const std::string& tablename)
{
	auto f = std::bind(buy_storage_procedure_, std::placeholders::_1,
											   std::placeholders::_2,
											   std::placeholders::_3,
											   std::placeholders::_4);
	std::string sql = f(tablename,'\"',"QuantityReceived", "Ticker");

	try
	{
		pqxx::work W(conn);
		W.exec(sql);
		W.commit();
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}

std::string buy_(const std::string& from, const std::string& to, size_t stock_count)
{
	std::string sql = "SELECT buy('" + from + "','" + to + "'," + std::to_string(stock_count) + ");";

	return (sql);
}

std::string if_exist_(const std::string& table, const char quote, const std::string& Ticker, const std::string& user)
{
	std::string sql = "select exists(select 1 from t1 where "+(quote+Ticker+quote)+"='" + user + "');";

	return sql;
}

void DBDriverPostrgres::if_exist_db(const std::string& user, const std::string& table)
{
	try
	{
		auto f = std::bind(if_exist_, std::placeholders::_1,
									  std::placeholders::_2,
									  std::placeholders::_3,
									  std::placeholders::_4);

		std::string sql = f(table, '\"', "Ticker",user);
		pqxx::work W(conn);
		pqxx::result r = W.exec(sql);
		W.commit();

		//return r[0][0].as<bool>();

		std::cout << "if_exist: " << r[0][0].as<bool>() << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
}

void DBDriverPostrgres::insert_error(const std::string& user, const std::string& table)
{
	/*
	CREATE TABLE IF NOT EXISTS error_log\
									(\"Function\" text, \"QuantityReceived\" text,\
									\"Ticker\" text, \"Date\" timestamp NOT NULL);\
	*/


	try
	{
		pqxx::work w(conn);
		
		std::string sql = "\
								PREPARE insert_error1 (text, text, text, timestamp) AS\
									INSERT INTO error_log VALUES($1, $2, $3, $4);\
								EXECUTE insert_error('NotEXISTS', 'NotExist', '"+user+"', now());\
						  ";
		w.exec(sql);
		w.commit();
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
}

inline int DBDriverPostrgres::if_exist(	std::map<std::string,std::pair<int,double>>& ticket_quantity, 
										const std::string& user, 
										const std::string& table)
{
	auto it = ticket_quantity.find(user);
	if (it != ticket_quantity.cend())
	{
		// < Ticker, < Quantity , Rate > >
		return it->second.first;
	}
	else
	{
		//insert_error(user, table);
		return false;
	}
}

inline void DBDriverPostrgres::buy_update(	size_t from_user_stock, 
											const std::string& table, 
								 			const std::string& from_user)
{
	try
	{
		pqxx::work w(conn);
		/*std::string sql = "UPDATE " + table + " SET \"QuantityReceived\" = \'" + std::to_string(from_user_stock) + "\' WHERE \"Ticker\" = '" + from_user + "';\
						   UPDATE " + table + " SET \"QuantityReceived\" = \'" + std::to_string(to_user_stock) + "\' WHERE \"Ticker\" = '" + to_user + "';";*/
		
		std::string sql = "UPDATE " + table + " SET \"QuantityReceived\" = " + std::to_string(from_user_stock) + " WHERE \"Ticker\" = '" + from_user + "';";
		pqxx::result r = w.exec(sql);
		w.commit();

		//std::cout << r[0][0].as<std::string>() << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
}

int DBDriverPostrgres::buy(	std::map<std::string,std::pair<int,double>>& ticket_quantity,
							const std::string& table, 
							const std::string& from, 
							size_t stock_count)
{
	int current_from_user_stock = 0;

	// Check if Ticker is exists
	if (!(current_from_user_stock = if_exist(ticket_quantity, from, table) )// ||
		//!( current_to_user_stock = if_exist(parser, to, table) ) 
		)
	{
		return false;
	}

	current_from_user_stock -= static_cast<int>(stock_count);

	// EVRTHG IS OK, FROM_STOCK_COUNT > TO_STOCK_COUNT
	if (current_from_user_stock > 0)
	{
		ticket_quantity[from].first = current_from_user_stock;

		//std::thread th1(&DBDriver::buy_update, this, std::ref(current_from_user_stock), std::ref(table), std::ref(from), std::ref(to), std::ref(current_to_user_stock));
		//th1.join();
		buy_update(current_from_user_stock, table, from );
		return (stock_count);
	}
	
	else if (current_from_user_stock < 0)
	{
		ticket_quantity[from].first = 0;// stock_count + current_from_user_stock;

		//std::thread th1(&DBDriver::buy_update, this, std::ref(current_from_user_stock), std::ref(table), std::ref(from), std::ref(to), std::ref(current_to_user_stock));
		//th1.join();

		buy_update(0, table, from);
		return (stock_count + current_from_user_stock);
	}
	
	// FROM_STOCK_COUNT HASN`T STOCK ANY MORE
	else if(current_from_user_stock == 0)
	{
		ticket_quantity[from].first = current_from_user_stock;

		buy_update(current_from_user_stock, table, from);
		return (stock_count);
	}

	return 0;
}

std::pair<int, std::pair<int, int>> DBDriverPostrgres::Quantity_position(const std::string& table)
{
	std::pair<int, std::pair<int,int>> quantity_ticker;

	try
	{
		pqxx::work w(conn);

		conn.prepare("position", "select column_name,data_type \
								  from information_schema.columns \
								  where table_name = $1;");
		pqxx::result r = w.exec_prepared("position", table);

		for (auto it = r.begin(); it != r.end(); ++it)
		{
			if (it[0].as<std::string>() == "Ticker")
			{
				quantity_ticker.first = std::distance(r.cbegin(), it);
			}
			else if (it[0].as<std::string>() == "QuantityReceived")
			{
				quantity_ticker.second.first = std::distance(r.cbegin(), it);
			}
			else if (it[0].as<std::string>() == "Rate")
			{
				quantity_ticker.second.second = std::distance(r.cbegin(), it);
			}
			else if(it == r.cend())
				return (std::make_pair(-1,std::make_pair(-1,-1)));
		}
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

	return (quantity_ticker);
}

void DBDriverPostrgres::rise_db( std::map<std::string,std::pair<int,double>>& ticket_quantity,
								 std::vector < std::vector < std::string > >& write_data,
								 const std::string& table)
{
	// SELECT "QuantityReeived" AND "Ticker" POSITION FROM DB(IF -1 TICKET OR QUANTITY NOT EXISTS)
	std::pair<int, std::pair<int, int>> quantity_ticker = Quantity_position(table);
	if ((quantity_ticker.first || quantity_ticker.second.first) == false)
		return;
	
	// CLEAR STD::MAP ( OLD VALUE )
	ticket_quantity.clear();

	// UPDATE STD::MAP FROM DB( IF APP CLOSED OR FAILED )
	try
	{
		select_column_name(conn, table, write_data);

		auto f = std::bind(select_, std::placeholders::_1);

		std::string sql = f(table);

		pqxx::nontransaction nontransact(conn);
		pqxx::result res(nontransact.exec(sql));

		for (pqxx::result::const_iterator c = res.begin(); c != res.end(); ++c)
		{
			ticket_quantity.try_emplace(c[quantity_ticker.first].as<std::string>(),
							   			std::make_pair(c[quantity_ticker.second.first].as<int>(),
											  		   c[quantity_ticker.second.second].as<double>())
										);
		}
	}
	catch (const std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
}