//
// Created by 43741 on 2024/12/22.
//

#include "CommandParser.h"
Command::Command(char _delimiter) {
    split_char = _delimiter;
}

Command::Command(const Command &rhs) {
    input = rhs.input;
    position = rhs.position;
    split_char = rhs.split_char;
    count = rhs.count;
}

Command::Command(const std::string &input_, char _delimiter) {
    split_char = _delimiter;
    input = input;
    count = 0;//过滤行首的分隔符
    while (input[count] == _delimiter) count++;
    countArguments();
}


std::string Command::getNext() {
    int j = position,len = input.length();
    std::string temp = "";
    if (position >= len) return temp;

    while (input[j] != split_char && j < len && input[j] != '\r'){
        temp += input[j];
        j++;
    }

    while (input[j] == split_char && j < len && input[j] != '\r') j++;
    position = j;
    return temp;
}

void Command::countArguments() {
    int len = input.length(),i = 0,j;
    //注意,buffer中有末尾的/r
    while (input[i] == split_char) i++;
    while (i < len && input[i] != '\r') {
        j = i;
        while (input[j] != split_char && j < len && input[j] != '\r'){
            j++;
        }
        if (i != j) count++;
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

void Command::setSplit(char newSplit) {
    split_char = newSplit;
}