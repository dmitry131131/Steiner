#include <iostream>
#include "Steiner.hpp"

void Steiner::run() {

}



void Steiner::run_modified() {


}

void Steiner::add_point(const std::string& name, int id, int x, int y, Steiner::PointType type) {
    if (points_.find(id) != points_.end()) {
        return;
    }

    Point new_point = {.name_ = name, .id_ = id, .x_ = x, .y_ = y, .type_ = type, .edges_ids_{}};

    // Find related edges
    for (auto E : edges_) {
        auto edge = E.second;
        if (edge.from_ == id || edge.to_ == id) {
            new_point.edges_ids_.push_back(edge.id_);
        }
    }

    points_.insert({id, new_point});
}

void Steiner::add_edge(int id, int from, int to) {
    if (edges_.find(id) != edges_.end()) {
        return;
    }

    Edge edge = {.id_ = id, .from_ = from, .to_ = to};
    edges_.insert({id, edge});

    auto point_from = points_.find(from);
    if (point_from != points_.end()) {
        point_from->second.edges_ids_.push_back(edge.id_);
    }

    auto point_to = points_.find(to);
    if (point_to != points_.end()) {
        point_to->second.edges_ids_.push_back(edge.id_);
    }
}

auto Steiner::get_point(int id) {
    return points_.find(id);
}

auto Steiner::get_edge(int id) {
    return edges_.find(id);
}

void Steiner::dump() {
    // Dump points
    std::cout << "Point count: " << points_.size() << std::endl;
    for (auto N : points_) {
        const auto& node = N.second;
        std::cout << "Point ID=" << node.id_
                  << ", Name='" << node.name_ << "'"
                  << ", Coordinates(x, y) = (" << node.x_ << ", " << node.y_ << ")"
                  << ", type = ";
        if (node.type_ == PointType::BASIC) {
            std::cout << "t', ";
        } else if (node.type_ == PointType::STEINER) {
            std::cout << "s', ";
        }

        std::cout << "Edges id: [ ";
        for (const auto edge_id : node.edges_ids_) {
            std::cout << edge_id << ' ';
        }
        std::cout << "]\n";
    }

    // Dump edges
    std::cout << "\n=== Edges count: " << edges_.size() << " ===" << std::endl;
    for (auto E : edges_) {
        const auto& edge = E.second;
        std::cout << "ID = " << edge.id_
                  << " | linked points: " << edge.from_ << " -> " << edge.to_ << std::endl;
    }
}