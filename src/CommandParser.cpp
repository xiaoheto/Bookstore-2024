#include "CommandParser.h"

Command::Command(const Command &rhs) {
    input = rhs.input;
    position = rhs.position;
    split_char = rhs.split_char;
    count = rhs.count;
}

Command::Command(char _delimiter) {
    split_char = _delimiter;
}

Command::Command(const std::string &input_, char delimiter_) {
    split_char = delimiter_;
    input = input_;
    count = 0;//过滤行首的分隔符
    while (input[count] == delimiter_) {
        count++;
    }
    countArguments();
}


std::string Command::getNext() {
    int len = input.length();
    std::string temp = "";
    // 检查是否已经到达字符串末尾
    if (position >= len) {
        return temp;
    }
    // 确保首先跳过多余的分隔符
    while (position < len && input[position] == split_char) position++;
    // 提取下一个参数
    int j = position;
    while (j < len && input[j] != split_char && input[j] != '\r') {
        temp += input[j];
        j++;
    }
    // 跳过下一个参数后的分隔符
    while (j < len && input[j] == split_char && input[j] != '\r') {
        j++;
    }
    position = j;
    return temp;
}


void Command::countArguments() {
    int len = input.length(),i = 0,j;
    //注意,buffer中有末尾的/r
    while (input[i] == split_char) {
        i++;
    }
    while (i < len && input[i] != '\r') {
        j = i;
        while (input[j] != split_char && j < len && input[j] != '\r'){
            j++;
        }
        if (i != j) {
            count++;
        }
        while (input[j] == split_char && j < len) j++;
        i = j;
    }
}

void Command::reset() {
    split_char = ' ';
    input = "";
    position = 0;
    count = 0;
}

std::istream &operator>>(std::istream &in, Command &obj) {
    in >> obj.input;
    return in;
}


void Command::setSplit(char newSplit) {
    split_char = newSplit;
}