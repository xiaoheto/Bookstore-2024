#include <exception>
#include <iostream>

#include "AccountManager.h"
#include "CommandParser.h"
#include "BookManager.h"
#include "error.h"
#include "MemoryRiver.h"
#include "BlockLink.h"
#include "validator.h"

int main() {
   std::string input;
   LogManager logs;
   BookManager books;
   AccountManager accounts;

   while(getline(std::cin,input)) {
       try {
           Command command(input);
           for (int i = 0;i < input.length(); ++i) {
               if (!Validator::isWithinAscll(input[i])) {
                   throw Error("Invalid\n");
               }
           }
           if(command.count == 0) {
               continue;
           }//是否是空行
           std::string order = command.getNext();
           if (order == "quit" || order == "exit") {
               return 0;
           }
           if (order == "su") {
               accounts.logIn(command);
           }
           else if (order == "logout") {
               accounts.logOut(command);
           }
           else if (order == "register") {
               accounts.registerUser(command);
           }
           else if (order == "passwd") {
               accounts.changePassword(command);
           }
           else if (order == "useradd") {
               accounts.addUser(command,logs);
           }
           else if (order == "delete") {
               accounts.deleteUser(command,logs);
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
                           logs.ShowFinance(-1,books.financeRecords);
                       }
                       else {
                           for(int i = 0;i < temp.size();++i) {
                               if (!isdigit(temp[i])) {
                                   throw Error("Invalid\n");
                               }
                           }
                           logs.ShowFinance(stoi(temp), books.financeRecords);
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
           else if (order == "report") {
               std::string temp = command.getNext();
               if (temp == "finance") {
                   logs.ReportFinance();
               }
               else if (temp == "employee") {
                   logs.ReportEmployee();
               }
           }
           else if (order == "log") {
               if (accounts.getCurrentPrivilege() != 7) {
                   throw Error("Invalid\n");
               }
               logs.ShowLog();
           }
           else {
               throw Error("Invalid\n");
           }
       }
       catch(Error &message) {
           std::cout << message.what();
       }
       catch (std::invalid_argument &x) {
           std::cout << "Invalid\n";
       }
       catch(...) {
           std::cout << "Invalid\n";
       }
   }
   return 0;
}
