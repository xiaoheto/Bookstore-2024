//
// Created by 43741 on 2024/12/22.
//

#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <vector>
#include <string>
#include <iomanip>

#include "BlockLink.h"
#include "MemoryRiver.h"
#include "CommandParser.h"
#include "error.h"

class AccountManager;
class Account;
enum class ActionType {
    ADDUSER,
    DELETEUSER,
    SHOW,
    IMPORTBOOK,
    MODIFYBOOK,
    SELECT,
    BUY
};

class Log {
public:
    ActionType behavoir;
    char description[150];
    bool isIncome = false; // 表示是否是收入
    double Amount = 0;

    Log() = default;
};

class LogManager {
private:
    MemoryRiver<Log> logStorage;
    int financeCount = 0; // 交易笔数

public:
    LogManager();

    void ShowFinance(int need = -1);

    void Report_employee(Command& line, AccountManager& accounts);

    void Report_myself(Command &line, AccountManager& accounts);

    void AddLog(Log& log); // 把 log 放进文件的同时还需要检查是否有交易

    void Log_ch(Command& line); // log command，检查有无额外的 token
};
#endif //LOGMANAGER_H
