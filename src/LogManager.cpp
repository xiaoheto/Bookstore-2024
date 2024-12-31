#include "LogManager.h"
LogManager::LogManager() {
    logStorage.initialise("log_Storage");
}

void LogManager::ShowFinance(int need) {
    if (need == 0) {
        std::cout << '\n';
        return;
    }

    logStorage.get_info(financeCount, 1);

    if (financeCount == 0) {
        std::cout << "+ 0.00 - 0.00\n";
        return;
    }

    if (need > financeCount) {
        throw Error("Invalid\n");
    }

    if (need == -1) {
        need = financeCount;
    }

    double inc = 0, dec = 0;
    const int headerSize = 2 * sizeof(int);

    // 确保从后往前读最近的 need 笔交易
    for (int i = financeCount - 1; i >= financeCount - need && i >= 0; i--) {
        Log temp;
        logStorage.read(temp, headerSize + i * sizeof(Log));

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
        logStorage.write_info(financeCount, 1);
        // 直接写在固定位置，确保连续存储
        logStorage.update(log, 2 * sizeof(int) + (financeCount - 1) * sizeof(Log));
    } else {
        logStorage.write(log);
    }
}