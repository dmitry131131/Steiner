#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Parser.hpp"

using json = nlohmann::json;

bool Parser::parse(const std::string& filename) {

    // Open file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error! Can't open file: '" << filename << "'\n";
        return false;
    }

    // Parse JSON
    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << '\n';
        return false;
    }

    file.close();

    // Check JSON contains "node"
    if (data.contains("node") && data["node"].is_array()) {
        for (const auto& item : data["node"]) {
            Node node;

            node.name = item.value("name", "");
            node.x    = item.value("x", 0);
            node.y    = item.value("y", 0);
            node.id   = item.value("id", -1);
            node.type = item.value("type", "");
            Nodes.push_back(node);
        }
    } else {
        std::cerr << "Error JSON doesn't contain 'node' field!\n";
        return false;
    }

    // Check JSON contains 'edge' field
    if (data.contains("edge") && data["edge"].is_array()) {
        for (const auto& item : data["edge"]) {
            // Parse edges
        }
    } else {
        std::cerr << "Error JSON doesn't contain 'edge' field!\n";
        return false;
    }

    // Выводим загруженные узлы для проверки
    std::cout << "Загружено узлов: " << Nodes.size() << std::endl;
    for (const auto& node : Nodes) {
        std::cout << "Узел ID=" << node.id
                  << ", имя='" << node.name << "'"
                  << ", координаты=(" << node.x << ", " << node.y << ")"
                  << ", тип='" << node.type << "'" << std::endl;
    }


    return true;
}