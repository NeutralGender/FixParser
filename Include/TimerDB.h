#ifndef _TimerDB
#define _TimerDB

#include <iostream>
#include <chrono>
#include <functional>
#include <thread>

class TimerDB
{
public:
    TimerDB();
    ~TimerDB();
    void activate(std::chrono::milliseconds delay,
                  std::string script_name,
                  std::string db,
                  std::string mode);

private:


};

#endif