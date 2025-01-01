#include "LogManager.h"
LogManager::LogManager() {
    logStorage.initialise("log_Storage");
}

// LogManager.cpp
void LogManager::ShowFinance(int need) {  // 删除这里的默认参数
    if (need == 0) {
        std::cout << '\n';
        return;
    }

    logStorage.get_info(financeCount, 1);

    if (financeCount == 0) {
        std::cout << "+ 0.00 - 0.00\n";
        return;
    }

    if (need == -1) {
        need = financeCount;
    }
    else if (need > financeCount) {
        throw Error("Invalid\n");
    }

    double inc = 0, dec = 0;

    // 改为用固定值 2
    const int headerSize = 2 * sizeof(int);
    for (int i = financeCount - need; i < financeCount; i++) {
        Log temp;
        int pos = headerSize + i * sizeof(Log);
        logStorage.read(temp, pos);

        if (temp.behavoir == ActionType::BUY) {
            inc += temp.Amount;
        }
        else if (temp.behavoir == ActionType::IMPORTBOOK) {
            dec += temp.Amount;
        }
    }

    std::cout << "+ " << std::fixed << std::setprecision(2) << inc
              << " - " << std::fixed << std::setprecision(2) << dec << "\n";
}

void LogManager::AddLog(Log &log) {
    if (log.behavoir == ActionType::BUY || log.behavoir == ActionType::IMPORTBOOK) {
        financeCount++;
        int pos = 2 * sizeof(int) + (financeCount - 1) * sizeof(Log);
        logStorage.update(log, pos);  // 使用update到确定位置
        logStorage.write_info(financeCount, 1);
    } else {
        logStorage.write(log);  // 只有非财务日志才使用write
    }
}
