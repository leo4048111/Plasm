#include "randomizer.hpp"

#include <random>
#include <string>

_START_PSM_NM_

::std::string Randomizer::makeAddress()
{
    std::string address = "";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    for (int i = 0; i < 40; ++i)
    {
        int digit = dis(gen);
        if (digit < 10)
            address += ('0' + digit);
        else
            address += ('a' + (digit - 10));
    }

    return address;
}

_END_PSM_NM_