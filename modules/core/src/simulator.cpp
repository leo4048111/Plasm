#include "simulator.hpp"

#include "logger.hpp"
#include "randomizer.hpp"

#include <functional>
#include <regex>

_START_PSM_NM_

void Simulator::Simulate(std::shared_ptr<cpn::Network> network)
{
    if(onStart_) onStart_();
    network_ = network;

    int totalFirings = 0;
    std::map<std::string, int> transitionFirings;
    bool passed = true;

    // get entry points and initialize tokens
    ::std::vector<::std::shared_ptr<cpn::Place>> entryPoints;
    for (auto &entry : network->getEntryPointsInfo())
    {
        auto place = network->getPlaceByName(entry.first);
        entryPoints.push_back(place);
    }

    // traverse all permutations
    ::std::string lastHash = "";
    ::std::string initHash = "";
    ::std::string curHash = "";
    int round = 0;
    do
    {
        network_->reset();
        initHash = network_->hash();

        cpn::Token ctrl(cpn::CTRL_COLOR, "()");

        for (auto &entry : network->getEntryPointsInfo())
        {
            auto place = network->getPlaceByName(entry.first);
            place->push(ctrl);

            for (auto &paramPlace : entry.second)
            {
                if(paramPlace->color() == "address")
                    paramPlace->push(cpn::Token(paramPlace->color(), Randomizer::GetInstance().makeAddress()));
                else
                    paramPlace->push(cpn::Token(paramPlace->color(), Randomizer::GetInstance().makeRandom(0, INT8_MAX)));
            }
        }
        for (auto &entry : entryPoints)
        {
            auto entryName = entry->name();
            ::std::string exitName;
            size_t lastDotPos = entryName.find_last_of('.');
            if (lastDotPos != ::std::string::npos)
            {
                exitName = entryName.substr(0, lastDotPos) + ".out";
            }
            auto exitPlace = network_->getPlaceByName(exitName);

            dfs(entry,exitPlace);
        }
        curHash = network_->hash();
        if (lastHash != "")
        {
            if (lastHash != curHash)
            {
                passed = false;
            }
        }
        lastHash = network_->hash().c_str();

        if(!passed) break;
    } while (std::next_permutation(entryPoints.begin(), entryPoints.end(), [](const std::shared_ptr<cpn::Place> &a, const std::shared_ptr<cpn::Place> &b)
                                   { return a->name() < b->name(); }));

    if (passed)
    {
        if(onSuccess_) onSuccess_(initHash, curHash);
    }
    else {
        if(onError_) onError_(initHash, lastHash, curHash);
    }

    if(onEnd_) onEnd_();
}

void Simulator::dfs(::std::shared_ptr<cpn::Place> place, ::std::shared_ptr<cpn::Place> exitPoint)
{
    if(place.get() == exitPoint.get()) {
        LOGI("Exit Point reached: %s", place->name().c_str());
        place->pop();
        return;
    }
    auto arcs = network_->getPlaceOutDegree(place);
    for (auto &arc : arcs)
    {
        auto transition = arc->transition();
        // check function call return path
        ::std::regex functionCallConLastPattern("\\bFunctionCall\\.[0-9]+\\.con.out\\b");
        bool isFunctionCallCon1 = ::std::regex_search(transition->name(), functionCallConLastPattern);
        bool result;
        if(isFunctionCallCon1)
        {
            auto callerPlace = functionStack_.top();
            auto callerPlaceName = callerPlace->name();
            auto idStartPos = callerPlaceName.find(".");
            auto idEndPos = callerPlaceName.find(".", idStartPos + 1);
            auto idString = callerPlaceName.substr(idStartPos + 1, idEndPos - idStartPos);
            auto callerId = std::stoi(idString);

            if(transition->name() == "FunctionCall." + std::to_string(callerId) + ".con.out")
            {
                result = network_->fire(transition);
            }
            else
                result = false;
        }
        else
            result = network_->fire(transition);

        if (result)
        {
            auto arcs2 = network_->getTransitionOutDegree(transition);
            for (auto &arc2 : arcs2)
            {
                auto place = arc2->place();
                if(place->size())
                {
                    if(place->top().color() == cpn::CTRL_COLOR)
                    {
                        // register caller
                        ::std::regex callerPattern("\\bFunctionCall\\.[0-9]+\\.in\\b");
                        bool isFunctionCall = ::std::regex_search(place->name(), callerPattern);
                        if(isFunctionCall)
                            functionStack_.push(place);
                        dfs(place, exitPoint);
                        if(isFunctionCall)
                            functionStack_.pop();
                    }
                }
            }
        }
    }
};

_END_PSM_NM_