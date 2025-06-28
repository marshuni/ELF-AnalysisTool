#include "ElfParser.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: elf-analyzer <elf_file> [output.json]" << std::endl;
        return 1;
    }

    std::string elfPath = argv[1];
    std::string outputPath = (argc >= 3) ? argv[2] : "";

    // 创建并运行解析器
    ElfParser parser(elfPath);
    nlohmann::json result = parser.parseAll();

    // 输出结果
    if (outputPath.empty()) {
        std::cout << result.dump(4) << std::endl;  // 缩进4空格的美化输出
    } else {
        std::ofstream ofs(outputPath);
        if (!ofs) {
            std::cerr << "Error: failed to open output file: " << outputPath << std::endl;
            return 2;
        }
        ofs << result.dump(4);
        std::cout << "JSON written to " << outputPath << std::endl;
    }

    return 0;
}
