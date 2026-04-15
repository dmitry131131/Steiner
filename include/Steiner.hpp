#pragma once
#include <vector>
#include <unordered_map>
#include <string>

class Steiner final {
public:
    enum PointType {
        BASIC,
        STEINER
    };

    struct Point final {
        std::string name_;
        int id_;
        int x_;
        int y_;
        PointType type_;
        std::vector<int> edges_ids_;
    };

    struct Edge final {
        int id_;
        int from_;
        int to_;
    };

private:
    std::unordered_map<int, Point> points_;
    std::unordered_map<int, Edge> edges_;

public:
    void run();
    void run_modified();

    void add_point(const std::string& name, int id, int x, int y, Steiner::PointType type);
    void add_edge(int id, int from, int to);

    auto get_point(int id);
    auto get_edge(int id);

    auto point_begin() { return points_.begin(); }
    auto point_end() { return points_.end(); }

    auto edge_begin() { return edges_.begin(); }
    auto edge_end() { return edges_.end(); }

    void dump();
};