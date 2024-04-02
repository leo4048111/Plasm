#include "visualizer.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <fstream>

_START_PSM_NM_

void Visualizer::Draw(::std::shared_ptr<cpn::Network> network) const
 {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                  boost::property<boost::vertex_name_t, std::string>,
                                  boost::property<boost::edge_name_t, std::string>> Graph;

    Graph g;

    std::unordered_map<std::string, boost::graph_traits<Graph>::vertex_descriptor> vertexMap;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor, std::string> vertexTypeMap;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor, std::string> vertexNameMap;

    // add places
    for (const auto& place : network->places()) {
        auto v = boost::add_vertex(g);
        boost::put(boost::vertex_name, g, v, place->name());
        vertexMap[place->name()] = v;
        vertexTypeMap[vertexMap[place->name()]] = "place";
        vertexNameMap[vertexMap[place->name()]] = place->name();
    }

    // add transitions
    for (const auto& transition : network->transitions()) {
        auto v = boost::add_vertex(g);
        boost::put(boost::vertex_name, g, v, transition->name());
        vertexMap[transition->name()] = v;
        vertexTypeMap[vertexMap[transition->name()]] = "transition";
        vertexNameMap[vertexMap[transition->name()]] = transition->name();
    }

    // add arcs
    for (const auto& arc : network->arcs()) {
        if(arc->orientation() == cpn::Arc::Orientation::BD) {
            std::string sourceName = arc->place()->name();
            std::string targetName = arc->transition()->name();
            auto source = vertexMap[sourceName];
            auto target = vertexMap[targetName];
            boost::add_edge(source, target, g);
            boost::add_edge(target, source, g);
        } else {
            std::string sourceName = arc->orientation() == cpn::Arc::Orientation::P2T ? arc->place()->name() : arc->transition()->name();
            std::string targetName = arc->orientation() == cpn::Arc::Orientation::T2P ? arc->place()->name() : arc->transition()->name();
            auto source = vertexMap[sourceName];
            auto target = vertexMap[targetName];
            boost::add_edge(source, target, g);
        }
    }

    // dump
    std::ofstream dotFile("cpn_network.dot");
    boost::write_graphviz(dotFile, g,
                          vertex_property_writer<decltype(vertexTypeMap), decltype(vertexNameMap)>(vertexTypeMap, vertexNameMap));
}

_END_PSM_NM_