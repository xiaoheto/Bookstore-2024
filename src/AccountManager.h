#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "BlockLink.h"
#include "CommandParser.h"
#include "LogManager.h"
#include <vector>

class AccountId {
private:
    char UserId[31];  // 保证有空间存储结束符
    
    friend class Account;
    friend class AccountManager;
public:
    AccountId() = default;
    AccountId(const std::string user_id);
    
    std::string getUserId() const;
    bool operator==(const AccountId &other) const;
    bool operator<(const AccountId &other) const;
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
    Account(const std::string &user_id, const std::string &user_name, 
            const std::string &password_, int privilege_ = 0);
    
    int getPrivilege();
    void changePassword(const std::string &newPassword);
};

class LogInAccount {
private:
    Account account;
    int selectedBookId;
    
    friend class AccountManager;
    friend class BookManager;
public:
    LogInAccount(const Account &acc) : account(acc), selectedBookId(0) {}
};

class AccountManager {
private:
    int account_count;
    std::vector<LogInAccount> loginStack;
    MemoryRiver<Account> accountStorage;
    DataFile userId_pos;

    // 私有辅助函数
    bool isValidUserID(const std::string &id) const;
    bool isValidPassword(const std::string &pwd) const;
    bool isValidUsername(const std::string &name) const;

public:
    AccountManager();
    AccountManager(const std::string &fileName);

    void logIn(Command &input);
    void logOut(Command &input);
    void registerUser(Command &input);
    void changePassword(Command &input);
    void addUser(Command &input, LogManager &log);
    void deleteUser(Command &input, LogManager &log);
    void selectBook(int book_id);
    int getCurrentPrivilege() const;

    // 获取当前登录账户
    Account* getCurrentAccount();
    
    friend class BookManager;
};

#endif // ACCOUNTMANAGER_H