//
// Created by 43741 on 2024/12/22.
//

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <string>
#include <vector>
#include <sstream>
using namespace std;

class Command {
private:
    std::string input = "";//存储输入的原始文本
    size_t position;//当前的解析位置
    char split_char;//分隔字符
public:
    int count = 0;

    Command() = default;

    Command(const Command &rhs);

    Command(char _delimiter);

    Command(const std::string &input, char _delimiter = ' ');

    ~Command() = default;

    void countArguments();

    std::string getNext();

    void reset();

    friend std::istream &operator >> (std::istream &in,Command &obj);

    void setSplit(char new_delimiter);
};
#endif //COMMANDPARSER_H