#include "ElfParser.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: elf-analyzer <elf_file> [output.json]" << endl;
        return 1;
    }

    string elfPath = argv[1];
    string outputPath = (argc >= 3) ? argv[2] : "";

    // 创建并运行解析器
    ElfParser parser(elfPath);
    nlohmann::json result = parser.parseAll();

    // 输出结果
    if (outputPath.empty()) {
        cout << result.dump(4) << endl;  // 缩进4空格的美化输出
    } else {
        ofstream ofs(outputPath);
        if (!ofs) {
            cerr << "Error: failed to open output file: " << outputPath << endl;
            return 2;
        }
        ofs << result.dump(4);
        cout << "JSON written to " << outputPath << endl;
    }

    return 0;
}
