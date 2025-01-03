#include "CommandParser.h"

Command::Command(char _delimiter) {
    split_char = _delimiter;
}

Command::Command(const Command &rhs) {
    input = rhs.input;
    position = rhs.position;
    split_char = rhs.split_char;
    position = rhs.position;
}

Command::Command(const std::string &in, char _split_char) :
    split_char(_split_char), input(in), position(0), count(0) {
    // 跳过开头的分隔符
    while (position < input.length() && input[position] == split_char) {
        position++;
    }
    countArguments();
}

void Command::countArguments() {
    count = 0;
    size_t i = 0;
    while (i < input.length() && input[i] != '\r' && input[i] != '\n') {
        // 跳过分隔符
        while (i < input.length() && input[i] == split_char) {
            i++;
        }

        if (i < input.length() && input[i] != '\r' && input[i] != '\n') {
            count++;
            // 找到下一个分隔符
            while (i < input.length() && input[i] != split_char && input[i] != '\r' && input[i] != '\n') {
                i++;
            }
        }
    }
}

std::string Command::getNext() {
    if (position >= input.length() ||
        input[position] == '\r' || input[position] == '\n') {
        return "";
        }

    std::string result;
    // 读取直到下一个分隔符或结束
    while (position < input.length() &&
           input[position] != split_char &&
           input[position] != '\r' &&
           input[position] != '\n') {
        result += input[position];
        position++;
           }

    // 跳过分隔符
    while (position < input.length() && input[position] == split_char) {
        position++;
    }

    return result;
}

void Command::reset() {
    input.clear();
    position = 0;
    count = 0;
    split_char = ' ';
}

void Command::setSplit(char new_split_char) {
    split_char = new_split_char;
    position = 0;
    countArguments();
}

std::istream &operator>>(std::istream &in, Command &obj) {
    std::getline(in, obj.input);
    obj.position = 0;
    while (obj.position < obj.input.length() && obj.input[obj.position] == obj.split_char) {
        obj.position++;
    }
    obj.countArguments();
    return in;
}
