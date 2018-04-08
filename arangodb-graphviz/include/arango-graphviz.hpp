#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include <tao/json.hpp>
namespace arangodb::gparser {
using Vertices = std::unordered_set<std::string>;
using Edge =  std::tuple<std::string, std::string, std::string>; // id from to

//expecting unique ids
namespace detail {
struct EdgeHasher {
    std::size_t operator()(Edge const& edge) const {
        return std::hash<std::string>()(std::get<0>(edge));
    };
};
} // detail

using Edges = std::unordered_set<Edge,detail::EdgeHasher>;
using Graph = std::tuple<Vertices,Edges>;

inline Edges const& get_edges(Graph const& graph) {
    return std::get<1>(graph);
}

inline Vertices const& get_vertices(Graph const& graph) {
    return std::get<0>(graph);
}

inline std::string const& get_id(Edge const& edge){
    return std::get<0>(edge);
}

inline std::string const& get_from(Edge const& edge){
    return std::get<1>(edge);
}

inline std::string const& get_to(Edge const& edge){
    return std::get<2>(edge);
}

namespace detail {
inline std::string get_attribute(tao::json::value const& json, std::string const& attr) {
    if(!json.is_object()){
        throw std::logic_error("no vaild object");
    }
    auto result = json.find(attr);
    if (!result) {
        throw std::logic_error(std::string("object does not contain ") + attr);
    }
    return result->as<std::string>();
}

inline Graph parse_object(tao::json::value const& json){
    //using namespace std::string_literals;
    if(!json.is_object()){
        throw std::logic_error("no vaild object");
    }

    auto edgesArray = json.find("edges");
    auto verticesArray = json.find("vertices");

    if(!edgesArray || !edgesArray->is_array() || !verticesArray || !verticesArray->is_array() ){
        throw std::logic_error("object does not contain vertices and edges arrays");
    }

    Vertices vertices;
    for (auto const& v : verticesArray->get_array()){
        vertices.emplace(get_attribute(v,std::string("_id")));
    }

    Edges from_to;
    for (auto const& e : edgesArray->get_array()){
        Edge edge = std::make_tuple(get_attribute(e,std::string("_id"))
                                   ,get_attribute(e,std::string("_from"))
                                   ,get_attribute(e,std::string("_to"))
                                   );
        from_to.emplace(std::move(edge));
    }

    return {std::move(vertices),std::move(from_to)};
}
} //detail

inline std::vector<Graph> parse(tao::json::value const& json){
    bool merge = true;

    if(json.is_object()){
        return {detail::parse_object(json)};
    }

    if(!json.is_array()){
        throw std::logic_error("there was no object or arrary of objects provided");
    }

    Vertices vertices;
    Edges edges;
    std::vector<Graph> result;

    for(auto const& element: json.get_array()){
        if(!element.is_object()){
            throw std::logic_error("there was no object or arrary of objects provided");
        }
        auto [v, e] = detail::parse_object(element);
        if (merge) {
            std::move(e.begin(),e.end(),std::inserter(edges,edges.begin()));
            std::move(v.begin(),v.end(),std::inserter(vertices,vertices.begin()));
        } else {
            throw std::logic_error("not implemented - the not merging mode needs to be implemented");
        }
    }

    if (merge){
        Graph g = std::make_tuple(std::move(vertices),std::move(edges));
        result.emplace_back(std::move(g));
    }

    return result;

}

inline void to_graphviz_stream(std::ostream& os, std::vector<Graph> const& graphs){
    os << "digraph test {" << std::endl;
    auto const& graph = graphs.front();
    for(auto const& edge  : get_edges(graph)) {
        os << "\t" << get_from(edge) << " -> " << get_to(edge) << std::endl;
    }
    os << "}" << std::endl;
}

} // arangodb
