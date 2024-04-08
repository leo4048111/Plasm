#include "simulator.hpp"

#include "logger.hpp"

_START_PSM_NM_

void Simulator::Simulate(std::shared_ptr<cpn::Network> network)
{
    LOGI("Starting simulation...");

    int totalFirings = 0;
    std::map<std::string, int> transitionFirings;
    bool simulationRunning = true;

    while(simulationRunning)
    {
        simulationRunning = false;
        bool deadlock = true;

        for (auto& transition : network->transitions())
        {
            bool canFire = true;

            for (auto& arc : network->arcs())
            {
                if(arc->place() == nullptr || arc->transition() == nullptr) {
                    canFire = false;
                    continue;
                }
                if (arc->transition() == transition && arc->orientation() == cpn::Arc::Orientation::P2T)
                {
                    auto place = arc->place();
                    if (place->tokens().empty())
                    {
                        canFire = false;
                        break;
                    }
                }
            }

            if (canFire)
            {
                transitionFirings[transition->name()] += 1;
                totalFirings++;
                simulationRunning = true;
                deadlock = false;
                LOGI("Transition fired: %s", transition->name().c_str());
                break;
            }
        }

        if (deadlock)
        {
            LOGI("Deadlock detected.");
            break;
        }
    }

    LOGI("Simulation completed. Total firings: %d", totalFirings);
    for (const auto& [transitionName, firings] : transitionFirings)
    {
        LOGI("Transition %s fired %d times.", transitionName.c_str(), firings);
    }

    if (!simulationRunning && totalFirings == 0)
    {
        LOGI("No transitions fired. Possible initial deadlock.");
    }
}

_END_PSM_NM_