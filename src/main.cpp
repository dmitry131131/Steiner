#include <iostream>
#include "CLI/CLI.hpp"

#include "Parser.hpp"
#include "Steiner.hpp"

int main(int argc, char** argv) try {
    CLI::App app{"Steiner algorithm implementation"};
    argv = app.ensure_utf8(argv);

    std::string input_filename;
    bool is_modified_algorithm = false;

    app.add_option("input_filename", input_filename, "Input JSON filename")->required();
    app.add_flag("-m", is_modified_algorithm)->default_val(false);

    CLI11_PARSE(app, argc, argv);

    std::cout << "Program started, input filename: " << input_filename << '\n';

    // Parsing input JSON
    Parser parser;
    if (!parser.parse(input_filename)) {
        return 1;
    };

    // Copy data into the steiner algorithm
    Steiner steiner;
    for (auto node = parser.Nodes_begin(); node != parser.Nodes_end(); ++node) {
        Steiner::PointType type = Steiner::PointType::BASIC;

        if (node->type == "t") {
            type = Steiner::PointType::BASIC;
        } else if (node->type == "s") {
            type = Steiner::PointType::STEINER;
        }

        steiner.add_point(node->name, node->id, node->x, node->y, type);
    }

    for (auto edge = parser.Edges_begin(); edge != parser.Edges_end(); ++edge) {
        steiner.add_edge(edge->id, edge->from, edge->to);
    }

    // steiner.dump();

    if (is_modified_algorithm) {
        std::cout << "Modified algorithm launched!\n";
        steiner.run_modified();
    } else {
        std::cout << "Basic algorithm launched!\n";
        steiner.run();
    }

    return 0;
} catch(const std::exception& e) {
    std::cerr << "General error: " << e.what() << '\n';  
}