#pragma once
#include <string>
#include <vector>

class Parser final {
public:
    struct Node {
        std::string name;
        int x;
        int y;
        int id;
        std::string type;
    };
    struct Edge {
        int id_0;
        int id_1;
    };
private:
    std::vector<Node> Nodes;
    std::vector<Edge> Edges;

public:
    // Make parsing input JSON file into vectors of Nodes and Edges
    bool parse(const std::string& filename);

    // Getters
    auto Nodes_begin() { return Nodes.begin(); }
    auto Nodes_end() { return Nodes.end(); }

    auto Edges_begin() { return Edges.begin(); }
    auto Edges_end() { return Edges.end(); }
};