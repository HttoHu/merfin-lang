#pragma once
#include <string>
extern int cur_line;
class SyntaxError
{
public:
    SyntaxError(const std::string &_content) : line(cur_line), content(_content) {}
    SyntaxError(int _line_no, const std::string &_content) : line(_line_no), content(_content) {}
    std::string what() const
    {
        return "<Line " + std::to_string(line) + ">: Syntax Error " + content;
    }

private:
    int line;
    std::string content;
};