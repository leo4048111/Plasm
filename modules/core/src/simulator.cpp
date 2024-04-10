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
    bool simulationRunning = true;

    // get entry points
    ::std::vector<::std::shared_ptr<cpn::Place>> entryPoints;
    for (auto &place : network->places())
    {
        if (place->entryPoint())
        {
            entryPoints.push_back(place);
        }
    }

    cpn::Token ctrl(cpn::CTRL_COLOR, "");
    for (auto &entry : entryPoints)
    {
        LOGI("==================================================");
        LOGI("Entry from: %s", entry->name().c_str());
        entry->addToken(ctrl);
        dfs(entry);
        LOGI("Entry out: %s", entry->name().c_str());
        LOGI("==================================================");
    }

    // LOGI("Simulation completed. Total firings: %d", totalFirings);
    // for (const auto& [transitionName, firings] : transitionFirings)
    // {
    //     LOGI("Transition %s fired %d times.", transitionName.c_str(), firings);
    // }

    // if (!simulationRunning && totalFirings == 0)
    // {
    //     LOGI("No transitions fired. Possible initial deadlock.");
    // }
}

void Simulator::dfs(std::shared_ptr<cpn::Place> place)
{
    LOGI("In Place: %s", place->name().c_str());
    auto transitions = network_->getPlaceOutDegree(place);
    for (auto transition : transitions)
    {
        bool result = network_->fire(transition);

        if (result)
        {
            LOGI("Transition %s fired.", transition->name().c_str());
            for (auto place : network_->getTransitionOutDegree(transition))
            {
                dfs(place);
            }
            result = network_->revert(transition);
        }
    }
};

_END_PSM_NM_