//
// Created by 43741 on 2024/12/22.
//

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "BlockLink.h"
#include "CommandParser.h"
#include "LogManager.h"

class AccountId {
private:
    char UserId[31];

    AccountId(std::string user_id);

    AccountId() = default;
    friend class Account;
    friend  class AccountManager;
public:
    std::string getUserId()const;
    bool operator == (const AccountId &other)const;
    bool operator < (const AccountId &other)const;
};


class Account {
private:
    AccountId UserId;
    char UserName[31];
    char password[31];
    int privilege;
    friend class AccountManager;
public:
    Account() = default;

    Account(const std::string &user_id,const std::string &user_name,const std::string &password_,int privilege_ = 0);

    int getPrivilege();

    void changePassword(std::string newPassword);
};

struct LogInAccount {
    Account LogAccount;
    int selectBookId = 0;
    friend  class AccountManager;
};

class AccountManager {
private:
    int account_count;
    std::vector<LogInAccount>LogInStorage;//存储正在登录的账户
    MemoryRiver<Account>AccountStorage;
    DataFile userId_pos;
    friend class BookManager;
public:
    AccountManager();

    AccountManager(std::string fileName);

    void LogIn(Command &input);

    void LogOut(Command &input);

    void Register(Command &input);

    void changePassword(Command &input);

    void UserAdd(Command &input,LogManager &log);

    void DeleteUser(Command &input,LogManager &log);

    void selectBook(int book_id);

    int getCurrentPrivilege()const;
};
#endif //ACCOUNTMANAGER_H
