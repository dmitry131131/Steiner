#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <cstdlib>
#include <limits>
#include <set>

enum PointType {
    INVALID,
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

    Point() : name_(""), id_(-1), x_(0), y_(0), type_(INVALID) {}
    
    Point(const std::string& name, int id, int x, int y, PointType type)
        : name_(name), id_(id), x_(x), y_(y), type_(type) {}

    bool operator==(const Point& other) const {
        return x_ == other.x_ && y_ == other.y_;
    }
};

struct Edge final {
    int id_;
    int from_;
    int to_;
};

struct ManhattanMetric final {
    static constexpr int calculate(const Point& a, const Point& b) {
        return std::abs(a.x_ - b.x_) + std::abs(a.y_ - b.y_);
    }
};

class Steiner final {
    std::unordered_map<int, Point> points_;
    std::unordered_map<int, Edge> edges_;
    int next_point_id_ = 1000;
    int next_edge_id_ = 0;

private:
    std::pair<int, std::vector<std::pair<int, int>>> compute_mst(const std::vector<int>& point_ids) const;
    std::vector<Point> generate_candidates(const std::vector<int>& terminal_ids) const;
    int compute_gain(const std::vector<int>& terminal_ids, const Point& candidate) const;

public:
    void run();
    void run_modified();

    void add_point(const std::string& name, int id, int x, int y, PointType type);
    void add_edge(int id, int from, int to);

    auto get_point(int id);
    auto get_edge(int id);

    auto point_begin() { return points_.begin(); }
    auto point_end() { return points_.end(); }

    auto edge_begin() { return edges_.begin(); }
    auto edge_end() { return edges_.end(); }

    std::string to_json() const;
    void dump();
};
