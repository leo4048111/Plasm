#pragma once

#include "base.hpp"
#include "singleton.hpp"

#include "cpn.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <memory>

_START_PSM_NM_

class Visualizer
{
    PSM_SINGLETON(Visualizer)

public:
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                        boost::property<boost::vertex_name_t, std::string>,
                                        boost::property<boost::edge_name_t, std::string>>;
    void Draw(::std::shared_ptr<cpn::Network> network, bool verbose = false);

private:
    void addArc(Graph &g, ::std::shared_ptr<cpn::Arc> arc);
    void addPlace(Graph &g, ::std::shared_ptr<cpn::Place> place);
    void addTransition(Graph &g, ::std::shared_ptr<cpn::Transition> transition);

    template <typename VertexTypeMap, typename VertexNameMap>
    class vertex_property_writer
    {
    public:
        vertex_property_writer(const VertexTypeMap &vertexTypeMap, const VertexNameMap &vertexNameMap)
            : vertexTypeMap_(vertexTypeMap), vertexNameMap_(vertexNameMap) {}

        template <class Vertex>
        void operator()(std::ostream &out, const Vertex &v) const
        {
            auto it = vertexTypeMap_.find(v);
            auto it2 = vertexNameMap_.find(v);
            if (it != vertexTypeMap_.end())
            {
                if (it->second == "place")
                {
                    out << "[shape=circle";
                }
                else if (it->second == "transition")
                {
                    out << "[shape=box";
                }

                out << ", label=\"" << it2->second << "\"]";
            }
        }

    private:
        const VertexTypeMap &vertexTypeMap_;
        const VertexNameMap &vertexNameMap_;
    };

private:
    std::unordered_map<std::string, boost::graph_traits<Graph>::vertex_descriptor> vertexMap_;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor, std::string> vertexTypeMap_;
    std::unordered_map<boost::graph_traits<Graph>::vertex_descriptor, std::string> vertexNameMap_;
};

_END_PSM_NM_