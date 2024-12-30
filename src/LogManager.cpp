#include "LogManager.h"
LogManager::LogManager() {
    logStorage.initialise("log_Storage");
}

void LogManager::ShowFinance(int need) {
    if (need == 0) {
        printf("\n");
        return;
    }
    logStorage.get_info(financeCount, 1);
    if (need > financeCount || need > 2147483647 || financeCount > 2147483647) {
        throw Error("Invalid\n");
    }
    if (need == -1) {
        need = financeCount;
    }
    double inc = 0, dec = 0;
    for (int i = financeCount - need + 1; i <= financeCount; ++i){
        class Log temp;
        logStorage.read(temp, 8 + (i - 1) * sizeof(Log));

        if (temp.isIncome) inc += temp.Amount;
        else dec += temp.Amount;
    }

    cout << "+ " << fixed << setprecision(2) << inc
         << " - " << fixed << setprecision(2) << dec << "\n";
}

void LogManager::AddLog(Log &log) {
    logStorage.write(log);
}