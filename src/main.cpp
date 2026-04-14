#include <iostream>
#include "CLI/CLI.hpp"

#include "Parser.hpp"

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

    if (is_modified_algorithm) {
        std::cout << "Modified algorithm launched!\n";
        // Launch modified algorithm
    } else {
        std::cout << "Basic algorithm launched!\n";
        // Launch basic algorithm
    }

    return 0;
} catch(const std::exception& e) {
    std::cerr << "General error: " << e.what() << '\n';  
}