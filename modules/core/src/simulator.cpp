#include "simulator.hpp"

#include "logger.hpp"

#include <functional>
#include <regex>

_START_PSM_NM_

void Simulator::Simulate(std::shared_ptr<cpn::Network> network)
{
    LOGI("Starting simulation...");

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
    int round = 0;
    do
    {
        LOGI("==================================================");
        network_->reset();

        cpn::Token ctrl(cpn::CTRL_COLOR, "()");
        LOGI("Simulation round: %d", round++);

        for (auto &entry : network->getEntryPointsInfo())
        {
            auto place = network->getPlaceByName(entry.first);
            LOGI("Adding control to entry: %s", place->name().c_str());
            place->push(ctrl);

            for (auto &paramPlace : entry.second)
            {
                LOGI("Adding token to param place: %s", paramPlace->name().c_str());
                paramPlace->push(cpn::Token("int", 1));
            }
        }
        LOGI("Initial hash: %s", network_->hash().c_str());
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

            LOGI("Entry from: %s", entry->name().c_str());
            dfs(entry,exitPlace);
            LOGI("Entry out: %s", entry->name().c_str());
        }
        LOGI("Final hash: %s", network_->hash().c_str());
        ::std::string curHash = network_->hash();
        if (lastHash != "")
        {
            if (lastHash != curHash)
            {
                passed = false;
                LOGE("Hash mismatch: %s != %s", lastHash.c_str(), curHash.c_str());
            }
        }
        lastHash = network_->hash().c_str();
        LOGI("==================================================");
    } while (std::next_permutation(entryPoints.begin(), entryPoints.end(), [](const std::shared_ptr<cpn::Place> &a, const std::shared_ptr<cpn::Place> &b)
                                   { return a->name() < b->name(); }));

    if (passed)
    {
        LOGI("Simulation passed, no TOD vulnerabilities found...");
    }
}

void Simulator::dfs(::std::shared_ptr<cpn::Place> place, ::std::shared_ptr<cpn::Place> exitPoint)
{
    LOGI("In Place: %s", place->name().c_str());
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
        ::std::regex functionCallCon1Pattern("\\bFunctionCall\\.[0-9]+\\.con1\\b");
        bool isFunctionCallCon1 = ::std::regex_search(transition->name(), functionCallCon1Pattern);
        bool result;
        if(isFunctionCallCon1)
        {
            auto callerPlace = functionStack_.top();
            auto callerPlaceName = callerPlace->name();
            auto idStartPos = callerPlaceName.find("FunctionCall.");
            auto idEndPos = callerPlaceName.find(".", idStartPos);
            auto idString = callerPlaceName.substr(idStartPos, idEndPos - idStartPos);
            auto callerId = std::stoi(idString);

            if(transition->name() == "FunctionCall." + std::to_string(callerId) + ".con1")
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
            LOGI("Transition %s fired.", transition->name().c_str());
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