#include "LogManager.h"
LogManager::LogManager() {
    logStorage.initialise("log_Storage");
}

void LogManager::ShowFinance(int need) {
    if (need == 0) {
        std::cout << '\n';
    }
    if (need == -1) {
        need = financeCount;
    }
    double income = 0.0,expense = 0.0;
    for (int i = financeCount - need + 1;i <= financeCount;++i) {
        Log temp;
        logStorage.read(temp, 8 + (i - 1) * sizeof(Log));
        if (temp.isIncome) income += temp.Amount;
        else expense += temp.Amount;
    }
    std::cout << "+ " << std::fixed << std::setprecision(2) << income
         << " - " << std::fixed << std::setprecision(2) << expense << "\n";
}

void LogManager::AddLog(Log &log) {
    logStorage.write(log);
}