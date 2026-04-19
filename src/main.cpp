#include <iostream>
#include "CLI/CLI.hpp"

#include "Parser.hpp"
#include "Steiner.hpp"

// Generate output filename
std::string generate_output_filename(const std::string& input_filename) {
    size_t sep_pos = input_filename.find_last_of("/\\");
    
    std::string filename = (sep_pos == std::string::npos) ? input_filename 
                            : input_filename.substr(sep_pos + 1);
    
    size_t dot_pos = filename.find_last_of('.');
    
    if (dot_pos == std::string::npos) {
        return filename + "_out";
    } else {
        std::string base = filename.substr(0, dot_pos);
        std::string ext = filename.substr(dot_pos); 
        return base + "_out" + ext;
    }
}

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
        PointType type = PointType::BASIC;

        if (node->type == "t") {
            type = PointType::BASIC;
        } else if (node->type == "s") {
            type = PointType::STEINER;
        }

        steiner.add_point(node->name, node->id, node->x, node->y, type);
    }

    for (auto edge = parser.Edges_begin(); edge != parser.Edges_end(); ++edge) {
        steiner.add_edge(edge->id, edge->from, edge->to);
    }

    steiner.dump();

    if (is_modified_algorithm) {
        std::cout << "Modified algorithm launched!\n";
        steiner.run_modified();
    } else {
        std::cout << "Basic algorithm launched!\n";
        steiner.run();
    }

    std::string output_filename = generate_output_filename(input_filename);
    std::ofstream out(output_filename);
    if (!out) {
        std::cerr << "Cannot open output file: " << output_filename << '\n';
        return 1;
    }
    out << steiner.to_json();
    out.close();

    return 0;
} catch(const std::exception& e) {
    std::cerr << "General error: " << e.what() << '\n';  
}