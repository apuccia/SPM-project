#include <iostream>
#include <chrono>
#include <thread>

class Utimer
{
private:
    std::chrono::system_clock::time_point s;
    std::chrono::system_clock::time_point e;
    std::string message;

public:
    void start()
    {
        s = std::chrono::system_clock::now();
    }

    long stop()
    {
        e = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed =
            e - s;
        long musec =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        return musec;
    }
};
