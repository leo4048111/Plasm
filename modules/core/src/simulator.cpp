#include "simulator.hpp"

#include "logger.hpp"

#include <functional>

_START_PSM_NM_

void Simulator::Simulate(std::shared_ptr<cpn::Network> network)
{
    LOGI("Starting simulation...");

    network_ = network;

    int totalFirings = 0;
    std::map<std::string, int> transitionFirings;
    bool passed = true;

    // get entry points
    ::std::vector<::std::shared_ptr<cpn::Place>> entryPoints;
    for (auto &place : network->places())
    {
        if (place->entryPoint())
        {
            entryPoints.push_back(place);
        }
    }

    // add control tokens for entry points
            cpn::Token ctrl(cpn::CTRL_COLOR, "()");
        for(auto& entry : entryPoints) entry->push(ctrl);

    // traverse all permutations
    ::std::string lastHash = "";
    do
    {
        LOGI("==================================================");
        LOGI("Initial hash: %s", network_->hash().c_str());
        for (auto &entry : entryPoints)
        {
            LOGI("Entry from: %s", entry->name().c_str());
            dfs(entry);
            LOGI("Entry out: %s", entry->name().c_str());
        }
        LOGI("Final hash: %s", network_->hash().c_str());
        ::std::string curHash = network_->hash();
        if(lastHash != "") {
            if(lastHash != curHash) {
                passed = false;
                LOGE("Hash mismatch: %s != %s", lastHash.c_str(), curHash.c_str());
            }
        }
        lastHash = network_->hash().c_str();
        LOGI("==================================================");

        // revert all states
        while (!revertStack_.empty())
        {
            auto transition = revertStack_.top();
            revertStack_.pop();
            network_->revert(transition);
        }
    } while (std::next_permutation(entryPoints.begin(), entryPoints.end(), [](const std::shared_ptr<cpn::Place> &a, const std::shared_ptr<cpn::Place> &b)
                                   { return a->name() < b->name(); }));

    if(passed) {
        LOGI("Simulation passed, no TOD vulnerabilities found...");
    }
}

void Simulator::dfs(std::shared_ptr<cpn::Place> place)
{
    LOGI("In Place: %s", place->name().c_str());
    auto arcs = network_->getPlaceOutDegree(place);
    for (auto& arc : arcs)
    {
        auto transition = arc->transition();
        bool result = network_->fire(transition);

        if (result)
        {
            LOGI("Transition %s fired.", transition->name().c_str());
            revertStack_.push(transition);
            auto arcs2 = network_->getTransitionOutDegree(transition);
            for (auto& arc2 : arcs2)
            {
                auto place = arc2->place();
                dfs(place);
            }
            // result = network_->revert(transition);
        }
    }
};

_END_PSM_NM_