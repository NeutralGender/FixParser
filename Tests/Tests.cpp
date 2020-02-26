#define CATCH_CONFIG_MAIN

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include "../DBDriver.h"
#include "../Parser.h"

/*
SCENARIO("Testing DBDriver")
{
    GIVEN("DBDriver object")
    {
        DBDriver db("postgres", "postgres", "postgres", "127.0.0.1", "5432");

        REQUIRE(db.connect() == 0);

        WHEN("Create storege procedure")
        {
            db.buy_storage_procedure("t1");

            THEN("Test Ticker buy method")
            {
                REQUIRE(db.buy("AA", "A", 1) == -1); // false: A not exists
                REQUIRE(db.buy("AA", "AAP", 1) > -1); // true: evrth is fine
                REQUIRE(db.buy("AAP", "AA", 1) > -1); // true: evrth is fine
                REQUIRE(db.buy("AA", "AAOI", 1) == -1); // false: AAOI NOT NUM
                REQUIRE(db.buy("AA", "AMZN", 2500) == -1); // true: < 0, but return OK
                REQUIRE(db.buy("AMZN", "AA", 2000) == -1); // true: no QuanRecv remained,return -1
            }
        }
    }
}


    DBDriver db("postgres","postgres","postgres","127.0.0.1","5432");
    db.connect();
    //db.drop("t1");
    //db.create_table(parse, "t1");
    //db.insert(parse, "t1");
    //db.select(parse, "t1");
    db.buy_storage_procedure("t1"); // create storage procedure
    */

TEST_CASE(" Benchmark DBDriver::buy() ") 
{
    Parser parse("IO_files\\copy.csv");

    parse.read_file();
    parse.parse();

    Parser parse1("C:\\Users\\wsu\\Desktop\\copy1.csv");
    parse1.read_file();
    parse1.parse();

    Parser parse2("C:\\Users\\wsu\\Desktop\\copy2.csv");
    parse2.read_file();
    parse2.parse();

    parse.merge(parse1);
    parse.merge(parse2);

    parse.create_ticker();

    DBDriver db("postgres","postgres","postgres","127.0.0.1","5432");
    REQUIRE(db.connect() == 0);
    db.buy_storage_procedure("t1");

    /*
    BENCHMARK("db.buy(t1, A, AA, 1)")
    {
        return db.buy(parse, "t1", "AAN", "AA", 1); // OK: evrtng is fine
    };
    
    BENCHMARK("db.buy(t1, A, AAfdr, 1)")
    {
        return db.buy(parse, "t1", "ABC", "AA", 13500); // false < ZERO
    };
    BENCHMARK("db.buy(AA, A, 1)")
    {
        return db.buy(parse, "t1", "AAPL", "AA", 8000); // true: ZERO
    };
    */
    /*
    BENCHMARK("db.buy(AAP, AA, 1)") 
    {
        return db.buy(parse, "t1", "A", "AAfdr", 1); // true: evrth is fine
    };

    BENCHMARK("db.buy(AA, AAOI, 1)") 
    {
        return db.buy(parse, "t1", "A", "AAfdr", 1); // false: AAOI NOT NUM
    };

    BENCHMARK("db.buy(AA, AMZN, 2500)") 
    {
        return db.buy(parse, "t1", "A", "AAfdr", 1); // true: < 0, but return OK
    };

    BENCHMARK("db.buy(AMZN, AA, 2000)") 
    {
        return db.buy(parse, "t1", "A", "AAfdr", 1); // true: no QuanRecv remained,return -1
    };
    */
}