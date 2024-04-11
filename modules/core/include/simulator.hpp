#pragma once

#include "base.hpp"
#include "singleton.hpp"

#include "cpn.hpp"

#include <memory>
#include <stack>

_START_PSM_NM_

class Simulator
{
    PSM_SINGLETON(Simulator)

public:
    void Simulate(::std::shared_ptr<cpn::Network> network);

private:
    void dfs(::std::shared_ptr<cpn::Place> place);

private:
    ::std::shared_ptr<cpn::Network> network_;
    ::std::stack<::std::shared_ptr<cpn::Transition>> revertStack_;
};

_END_PSM_NM_