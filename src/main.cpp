#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <fstream>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "../includes/lexer.h"
#include "../includes/env.h"
#include "../includes/stmts.h"
#include "../includes/function.h"
#include "../includes/class.h"

int cur_line = 1;
Env env;

std::string read_file(const std::string &filename)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs)
        throw std::runtime_error("open file " + filename + " failed\n");
    char *file_content;
    ifs.seekg(0, std::ios::end);
    size_t file_length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    file_content = new char[file_length];

    ifs.read(file_content, file_length);
    ifs.close();

    std::string str(file_content, file_content + file_length);
    delete[] file_content;
    return str;
}
/*
/usr/local/Cellar/llvm/15.0.7_1/bin/llc -march=arm64 -filetype=obj test.ll -o temp.o
clang++ test.cpp temp.o
./a
*/
int main(int argc, char **argv)
{
    using namespace std;
    using namespace llvm;
    using namespace HLex;
    // input
    try
    {
        string filename = argv[1];

        env.load_file(read_file(filename));
        Parser::parse_program();

        std::error_code ec;
        llvm::raw_fd_ostream output_file("test.ll", ec);
        if (ec)
        {
            std::cerr << "failed to open test.ll";
        }
        env.the_module->print(output_file, nullptr);
    }
    catch (SyntaxError &e)
    {
        std::cout << e.what() << "\n";
    }
    return 0;
}