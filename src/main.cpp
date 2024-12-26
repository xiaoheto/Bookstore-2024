//
// Created by 43741 on 2024/12/22.
//
#include <exception>
#include <iostream>

#include "AccountManager.h"
#include "CommandParser.h"
#include "BookManager.h"
#include "error.h"
#include "MemoryRiver.h"
#include "BlockLink.h"
int main() {
    int lineCount = 0;
    std::string input;
    LogManager logs;
    BookManager books;
    AccountManager accounts;
    while(getline(std::cin,input)) {
        lineCount++;
        try {
            Command command(input);
            for (int i = 0;i < input.length(); ++i) {
                if (input[i] < 0 || input[i] > 127) {
                    throw Error("Invalid\n");
                }
            }
            //是否都在ASCLL范围内
            if(command.count == 0) {
                continue;
            }
            //是否是空行
            std::string order = command.getNext();
            if (order == "quit" || order == "exit") {
                return 0;
            }
            if (order == "su") {
                accounts.LogIn(command);
            }
            else if (order == "logout") {
                accounts.LogOut(command);
            }
            else if (order == "register") {
                accounts.Register(command);
            }
            else if (order == "passwd") {
                accounts.changePassword(command);
            }
            else if (order == "useradd") {
                accounts.UserAdd(command,logs);
            }
            else if (order == "delete") {
                accounts.DeleteUser(command,logs);
            }
            else if (order == "show") {
                Command cur_command(command);
                if (cur_command.getNext() == "finance") {
                    if (accounts.getCurrentPrivilege() != 7) {
                        throw Error("Invalid\n");
                    }
                    else {
                        std::string temp = cur_command.getNext();
                        if (temp.empty()) {
                            logs.ShowFinance();
                        }
                        else {
                            for(int i = 0;i < temp.size();++i) {
                                if (!isdigit(temp[i])) {
                                    throw Error("Invalid\n");
                                }
                            }
                            logs.ShowFinance(stoi(temp));
                        }
                    }
                }
                else {
                    books.Show(command,accounts,logs);
                }
            }
            else if (order == "buy") {
                books.Buy(command,accounts,logs);
            }
            else if (order == "select") {
                books.Select(command,accounts,logs);
            }
            else if (order == "modify") {
                books.Modify(command,accounts,logs);
            }
            else if (order == "import") {
                books.ImportBook(command,accounts,logs);
            }
            else if (order == "report") {//TODO完成LogManager后补充
            }
            else if (order == "log") {//TODO
            }
        }
        catch(Error &message) {
            std::cout << message.what();
        }
    }
    return 0;
}