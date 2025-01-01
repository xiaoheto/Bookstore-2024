//
// Created by 43741 on 2024/12/22.
//

#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <vector>
#include <string>
#include <iomanip>
#include <map>

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
    bool isIncome = false; // 表示是否是收入
    double Amount = 0;
    char owner[31];

    Log() = default;
};

class LogManager {
private:
    MemoryRiver<Log> logStorage;
    int financeCount = 0; // 交易笔数

public:
    LogManager();

    void ShowFinance(int need = -1);

    void ReportFinance();

    void ShowLog();

    void AddLog(Log& log); // 把 log 放进文件的同时还需要检查是否有交易

    void ReportEmployee();
};
#endif //LOGMANAGER_H
