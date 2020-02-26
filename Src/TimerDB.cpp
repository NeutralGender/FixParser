#include "../Include/TimerDB.h"

TimerDB::TimerDB()
{
}

TimerDB::~TimerDB()
{
}

void TimerDB::activate(std::chrono::milliseconds delay,
                        std::string script_name,
                        std::string db,
                        std::string mode)
{
    std::string script = script_name + " " + db + " " + mode;    
    for(;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::system( script.c_str() );
    }
}
