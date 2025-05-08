#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <filesystem>
#include <unordered_map>
#include "parser/parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Wrong input.\nExample: xor.exe file.ext\n";
        return 1;
    }
    
    std::ifstream ifs(argv[argc - 1]);
    std::string file_content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();
    
    if (file_content.empty()) {
        std::cerr << "Wrong input.\nEmpty File\n";
        return 1;
    }

    auto indices = index_quotes(file_content);
    handle_content(file_content, indices);

    std::filesystem::path input_path = argv[argc - 1];
    std::filesystem::path parent_dir = input_path.parent_path();
    std::filesystem::path output_path = parent_dir / "XorYourFile output";
    std::filesystem::path output_file = output_path / input_path.filename();

    try {
        std::filesystem::create_directory(output_path);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to create output directory: " << e.what() << "\n";
        return 1;
    }

    std::ofstream ofs(output_file);
    ofs << file_content;
    ofs.close();

    return 0;
}