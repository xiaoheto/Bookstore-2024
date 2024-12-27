#include "CommandParser.h"

//class Command

Command::Command(char _split_char) {
    split_char = _split_char;
}

Command::Command(const Command &rhs) {
    input = rhs.input;
    position = rhs.position;
    split_char = rhs.split_char;
    count = rhs.count;
}

Command::Command(const std::string &in, char _split_char) {
    split_char = _split_char;
    input = in;
    position = 0;//过滤行首的分隔符
    while (input[position] == split_char) position++;
    countArguments();
}

string Command::getNext() {
    int j = position,len = input.length();
    string temp = "";
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
    //注意,input中有末尾的/r
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
    input = "";
    position = 0;
    count = 0;
    split_char = ' ';
    return;
}

istream &operator>>(istream &input, Command &obj) {
    input >> obj.input;
    return input;
}

void Command::setSplit(char new_split_char) {
    split_char = new_split_char;
}
