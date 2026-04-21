#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include "BS_thread_pool.hpp"
#include "Steiner.hpp"

void Steiner::run() {
    edges_.clear();
    
    std::vector<int> terminal_ids;
    for (const auto& [id, p] : points_) {
        if (p.type_ == BASIC) {
            terminal_ids.push_back(id);
        }
    }
    
    if (terminal_ids.size() < 2) {
        return;  
    }
    
    while (true) {
        auto [current_cost, current_edges] = compute_mst(terminal_ids);
        
        auto candidates = generate_candidates(terminal_ids);
        
        int best_gain = 0;
        Point best_candidate;
        for (const auto& cand : candidates) {
            int gain = compute_gain(terminal_ids, cand);
            if (gain > best_gain) {
                best_gain = gain;
                best_candidate = cand;
            }
        }
        
        if (best_gain <= 0) {
            break;
        }
        
        int new_id = next_point_id_++;
        best_candidate.id_ = new_id;
        best_candidate.name_ = "steiner_" + std::to_string(new_id);
        points_[new_id] = best_candidate;
        terminal_ids.push_back(new_id);
    }
    
    auto [final_cost, final_edges] = compute_mst(terminal_ids);

    std::cout << "Final Cost: " << final_cost << '\n';
    
    for (const auto& e : final_edges) {
        int edge_id = next_edge_id_++;
        edges_[edge_id] = Edge{edge_id, e.first, e.second};
        points_[e.first].edges_ids_.push_back(edge_id);
        points_[e.second].edges_ids_.push_back(edge_id);
    }
}

void Steiner::run_modified() {
   edges_.clear();
    
    std::vector<int> terminal_ids;
    for (const auto& [id, p] : points_) {
        if (p.type_ == BASIC) {
            terminal_ids.push_back(id);
        }
    }
    
    if (terminal_ids.size() < 2) {
        return;  
    }
    
    while (true) {
        auto [current_cost, current_edges] = compute_mst(terminal_ids);
        
        auto candidates = generate_candidates(terminal_ids);

        BS::thread_pool pool;  
        std::vector<std::future<int>> gain_futures;
        gain_futures.reserve(candidates.size());

        for (size_t i = 0; i < candidates.size(); ++i) {
            gain_futures.push_back(
                pool.submit_task([this, &terminal_ids, &candidates, i]() -> int {
                    return compute_gain(terminal_ids, candidates[i]);
                })
            );
        }

        int best_gain = 0;
        Point best_candidate;
        for (size_t i = 0; i < candidates.size(); ++i) {
            int gain = gain_futures[i].get(); 
            if (gain > best_gain) {
                best_gain = gain;
                best_candidate = candidates[i];
            }
        }
        
        if (best_gain <= 0) {
            break;
        }
        
        int new_id = next_point_id_++;
        best_candidate.id_ = new_id;
        best_candidate.name_ = "steiner_" + std::to_string(new_id);
        points_[new_id] = best_candidate;
        terminal_ids.push_back(new_id);
    }
    
    auto [final_cost, final_edges] = compute_mst(terminal_ids);

    std::cout << "Final Cost: " << final_cost << '\n';
    
    for (const auto& e : final_edges) {
        int edge_id = next_edge_id_++;
        edges_[edge_id] = Edge{edge_id, e.first, e.second};
        points_[e.first].edges_ids_.push_back(edge_id);
        points_[e.second].edges_ids_.push_back(edge_id);
    }
}

void Steiner::add_point(const std::string& name, int id, int x, int y, PointType type) {
    if (points_.find(id) != points_.end()) {
        return;
    }

    Point new_point{name, id, x, y, type};

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

int Steiner::compute_gain(const std::vector<int>& terminal_ids, const Point& candidate) const {
    auto [cost_before, _] = compute_mst(terminal_ids);
    
    int temp_id = -1;
    std::vector<int> ids_with_candidate = terminal_ids;
    ids_with_candidate.push_back(temp_id);
    
    auto temp_points = points_;
    temp_points[temp_id] = candidate;
    
    Steiner temp_steiner;
    temp_steiner.points_ = temp_points;
    auto [cost_after, tmp] = temp_steiner.compute_mst(ids_with_candidate);
    
    return cost_before - cost_after;
}

std::vector<Point> Steiner::generate_candidates(const std::vector<int>& terminal_ids) const {
    std::set<int> xs, ys;
    for (int id : terminal_ids) {
        const Point& p = points_.at(id);
        xs.insert(p.x_);
        ys.insert(p.y_);
    }
    
    std::vector<Point> candidates;
    for (int x : xs) {
        for (int y : ys) {
            bool exists = false;
            for (const auto& [id, p] : points_) {
                if (p.x_ == x && p.y_ == y) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                candidates.emplace_back("candidate", -1, x, y, STEINER);
            }
        }
    }
    return candidates;
}

std::pair<int, std::vector<std::pair<int, int>>> Steiner::compute_mst(const std::vector<int>& point_ids) const {
    if (point_ids.empty()) return {0, {}};
    
    int n = point_ids.size();
    std::vector<bool> in_tree(n, false);
    std::vector<int> min_dist(n, std::numeric_limits<int>::max());
    std::vector<int> parent(n, -1);
    
    min_dist[0] = 0;
    int total_cost = 0;
    std::vector<std::pair<int, int>> edges;
    
    for (int i = 0; i < n; ++i) {
        int u = -1;
        for (int j = 0; j < n; ++j) {
            if (!in_tree[j] && (u == -1 || min_dist[j] < min_dist[u])) {
                u = j;
            }
        }
        
        in_tree[u] = true;
        total_cost += min_dist[u];
        if (parent[u] != -1) {
            edges.emplace_back(point_ids[parent[u]], point_ids[u]);
        }
        
        // Обновляем расстояния
        const Point& pu = points_.at(point_ids[u]);
        for (int v = 0; v < n; ++v) {
            if (!in_tree[v]) {
                const Point& pv = points_.at(point_ids[v]);
                int d = ManhattanMetric::calculate(pu, pv);
                if (d < min_dist[v]) {
                    min_dist[v] = d;
                    parent[v] = u;
                }
            }
        }
    }
    return {total_cost, edges};
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

std::string Steiner::to_json() const {
    using json = nlohmann::json;
    json result;

    // Point array
    std::vector<json> nodes_json;
    for (const auto& [id, point] : points_) {
        std::string type_str;
        switch (point.type_) {
            case BASIC:   type_str = "t"; break;
            case STEINER: type_str = "s"; break;
            default:      type_str = "u"; break; // INVALID
        }

        nodes_json.push_back({
            {"id", point.id_},
            {"name", point.name_},
            {"type", type_str},
            {"x", point.x_},
            {"y", point.y_},
            {"edges", point.edges_ids_} 
        });
    }
    result["node"] = nodes_json;

    // Edges array
    std::vector<json> edges_json;
    for (const auto& [id, edge] : edges_) {
        edges_json.push_back({
            {"id", edge.id_},
            {"vertices", {edge.from_, edge.to_}}
        });
    }
    result["edge"] = edges_json;

    return result.dump();
}
