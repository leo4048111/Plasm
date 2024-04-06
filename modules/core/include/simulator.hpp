#pragma once

#include "base.hpp"
#include "singleton.hpp"

#include "cpn.hpp"

#include <memory>

_START_PSM_NM_

class Simulator
{
    PSM_SINGLETON(Simulator)

public:
    void Simulate(::std::shared_ptr<cpn::Network> network);
};

_END_PSM_NM_