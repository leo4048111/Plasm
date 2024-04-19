#pragma once

#include "base.hpp"
#include "singleton.hpp"

#include <string>
#include <random>

_START_PSM_NM_

class Randomizer
{
    PSM_SINGLETON(Randomizer)

public:
    ::std::string makeAddress();

    template<typename T>
    T makeRandom(T min, T max)
    {
        ::std::mt19937 gen;
        ::std::random_device rd;
        gen.seed(rd());
        if constexpr (::std::is_integral<T>::value) {
            ::std::uniform_int_distribution<T> dis(min, max);
            return dis(gen);
        } else if constexpr (::std::is_floating_point<T>::value) {
            ::std::uniform_real_distribution<T> dis(min, max);
            return dis(gen);
        } else {
            return (T)0;
        }
    }
};

_END_PSM_NM_