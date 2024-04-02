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
    void Draw(::std::shared_ptr<cpn::Network> network) const;

private:
    template <typename VertexTypeMap, typename VertexNameMap>
    class vertex_property_writer
    {
    public:
        vertex_property_writer(const VertexTypeMap &vertexTypeMap, const VertexNameMap& vertexNameMap)
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
};

_END_PSM_NM_