#pragma once

#include "base.hpp"
#include "singleton.hpp"

#include "cpn.hpp"

#include <memory>

_START_PSM_NM_

class Visualizer
{
    PSM_SINGLETON(Visualizer)

public:
    void Draw(::std::shared_ptr<cpn::Network> network) const;
};

_END_PSM_NM_