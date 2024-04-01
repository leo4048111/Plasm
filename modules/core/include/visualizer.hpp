#pragma once

#include "base.hpp"

#include "cpn.hpp"

_START_PSM_NM_

class Visualizer
{
public:
    Visualizer() = default;
    ~Visualizer() = default;

    void Draw(cpn::Network network) const;
};

_END_PSM_NM_