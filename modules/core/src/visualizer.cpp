#include "visualizer.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <fstream>

_START_PSM_NM_

void Visualizer::addArc(Graph &g, ::std::shared_ptr<cpn::Arc> arc)
{
    if (arc->orientation() == cpn::Arc::Orientation::BD)
    {
        std::string sourceName = arc->place()->name();
        std::string targetName = arc->transition()->name();
        auto source = vertexMap_[sourceName];
        auto target = vertexMap_[targetName];
        boost::add_edge(source, target, g);
        boost::add_edge(target, source, g);
    }
    else
    {
        std::string sourceName = arc->orientation() == cpn::Arc::Orientation::P2T ? arc->place()->name() : arc->transition()->name();
        std::string targetName = arc->orientation() == cpn::Arc::Orientation::T2P ? arc->place()->name() : arc->transition()->name();
        auto source = vertexMap_[sourceName];
        auto target = vertexMap_[targetName];
        boost::add_edge(source, target, g);
    }
}

void Visualizer::addPlace(Graph &g, ::std::shared_ptr<cpn::Place> place)
{
    auto v = boost::add_vertex(g);
    auto name = place->name();
    vertexMap_[place->name()] = v;
    vertexTypeMap_[vertexMap_[place->name()]] = "place";
    vertexNameMap_[vertexMap_[place->name()]] = name;
}

void Visualizer::addTransition(Graph &g, ::std::shared_ptr<cpn::Transition> transition)
{
    auto v = boost::add_vertex(g);
    auto name = transition->name();
    vertexMap_[transition->name()] = v;
    vertexTypeMap_[vertexMap_[transition->name()]] = "transition";
    vertexNameMap_[vertexMap_[transition->name()]] = name;
}

void Visualizer::Draw(::std::shared_ptr<cpn::Network> network, bool verbose)
{
    Graph g;
    vertexMap_.clear();
    vertexTypeMap_.clear();
    vertexNameMap_.clear();

    if (verbose)
    {
        // add places
        for (const auto &place : network->places())
        {
            addPlace(g, place);
        }

        // add transitions
        for (const auto &transition : network->transitions())
        {
            addTransition(g, transition);
        }

        // add arcs
        for (const auto &arc : network->arcs())
        {
            addArc(g, arc);
        }
    }
    else // remove unused
    {
        auto dfs = [&](::std::shared_ptr<cpn::Place> place, auto &dfs) -> void
        {
            auto arcs = network->getPlaceOutDegree(place);
            for (auto &arc : arcs)
            {
                auto transition = arc->transition();
                if (vertexMap_.find(transition->name()) == vertexMap_.end())
                {
                    addTransition(g, transition);
                    for (auto &arc2 : network->getTransitionOutDegree(transition))
                    {
                        auto place = arc2->place();
                        if (vertexMap_.find(place->name()) == vertexMap_.end())
                        {
                            addPlace(g, place);
                            dfs(place, dfs);
                        }
                        if(arc2->orientation() != cpn::Arc::Orientation::BD)
                            addArc(g, arc2);
                    }
                }
                addArc(g, arc);
            }
        };

        auto entryPointInfo = network->getEntryPointsInfo();
        for (auto &info : entryPointInfo)
        {
            auto place = network->getPlaceByName(info.first);
            addPlace(g, place);
            dfs(place, dfs);
        }
    }

    // dump
    std::ofstream dotFile(network->name() + ".dot");
    boost::write_graphviz(dotFile, g,
                          vertex_property_writer<decltype(vertexTypeMap_), decltype(vertexNameMap_)>(vertexTypeMap_, vertexNameMap_));
}

_END_PSM_NM_