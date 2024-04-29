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

    Simulator& SetOnStart(::std::function<void()> onStart) { onStart_ = onStart; return *this; };
    Simulator& SetOnEnd(::std::function<void()> onEnd) { onEnd_ = onEnd; return *this; };
    Simulator& SetOnError(::std::function<void(::std::string, ::std::string, ::std::string)> onError) { onError_ = onError; return *this; };
    Simulator& SetOnSuccess(::std::function<void(::std::string, ::std::string)> onSuccess) { onSuccess_ = onSuccess; return *this; };

private:
    void dfs(::std::shared_ptr<cpn::Place> place, ::std::shared_ptr<cpn::Place> exitPoint);

private:
    ::std::shared_ptr<cpn::Network> network_;
    ::std::stack<::std::shared_ptr<cpn::Place>> functionStack_;

    ::std::function<void()> onStart_{nullptr};
    ::std::function<void()> onEnd_{nullptr};
    ::std::function<void(::std::string, ::std::string, ::std::string)> onError_{nullptr};
    ::std::function<void(::std::string, ::std::string)> onSuccess_{nullptr};
};

_END_PSM_NM_