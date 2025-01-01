#include "LogManager.h"
LogManager::LogManager() {
    logStorage.initialise("log_Storage");
}

void LogManager::ReportFinance() {
    logStorage.get_info(financeCount, 1);
    const int headerSize = 2 * sizeof(int);

    for(int i = 0; i < financeCount; i++) {  // 注意这里从0开始遍历
        Log temp;
        logStorage.read(temp, headerSize + i * sizeof(Log));

        if (temp.behavoir == ActionType::BUY) {
            std::cout << "用户购买图书消费" << std::fixed << std::setprecision(2) << temp.Amount << "元\n";
        }
        else if (temp.behavoir == ActionType::IMPORTBOOK) {
            std::cout << "进货支出" << std::fixed << std::setprecision(2) << temp.Amount << "元\n";
        }
    }
}
void LogManager::ShowLog() {
    // 先读取所有财务日志
    logStorage.get_info(financeCount, 1);
    const int headerSize = 2 * sizeof(int);

    for(int i = 0; i < financeCount; i++) {
        Log log;
        logStorage.read(log, headerSize + i * sizeof(Log));

        switch(log.behavoir) {
            case ActionType::BUY:
                std::cout << "[-] 购书记录" << '\n'
                         << "    操作者：" << log.owner << '\n'
                         << "    交易金额：" << std::fixed << std::setprecision(2)
                         << log.Amount << " 元" << '\n'
                         << "    收支类型：" << (log.isIncome ? "收入" : "支出") << "\n\n";
                break;

            case ActionType::IMPORTBOOK:
                std::cout << "[-] 进货记录" << '\n'
                         << "    操作者：" << log.owner << '\n'
                         << "    进货支出：" << std::fixed << std::setprecision(2)
                         << log.Amount << " 元" << '\n'
                         << "    收支类型：" << (log.isIncome ? "收入" : "支出") << "\n\n";
                break;

            case ActionType::ADDUSER:
                std::cout << "[-] 创建用户" << '\n'
                         << "    操作者：" << log.owner << "\n\n";
                break;

            case ActionType::DELETEUSER:
                std::cout << "[-] 删除用户" << '\n'
                         << "    操作者：" << log.owner << "\n\n";
                break;

            case ActionType::MODIFYBOOK:
                std::cout << "[-] 修改图书信息" << '\n'
                         << "    操作者：" << log.owner << "\n\n";
                break;

            case ActionType::SELECT:
                std::cout << "[-] 选择图书" << '\n'
                         << "    操作者：" << log.owner << "\n\n";
                break;

            case ActionType::SHOW:
                std::cout << "[-] 查询操作" << '\n'
                         << "    操作者：" << log.owner << "\n\n";
                break;
        }
    }

    // 输出统计信息
    double totalIncome = 0, totalExpense = 0;
    for(int i = 0; i < financeCount; i++) {
        Log log;
        logStorage.read(log, headerSize + i * sizeof(Log));
        if(log.isIncome) {
            totalIncome += log.Amount;
        } else {
            totalExpense += log.Amount;
        }
    }

    std::cout << "\n=== 财务统计 ===" << '\n'
              << "总收入：" << std::fixed << std::setprecision(2) << totalIncome << " 元\n"
              << "总支出：" << std::fixed << std::setprecision(2) << totalExpense << " 元\n"
              << "净收入：" << std::fixed << std::setprecision(2)
              << (totalIncome - totalExpense) << " 元\n";
}

void LogManager::AddLog(Log &log) {
    if (log.behavoir == ActionType::BUY || log.behavoir == ActionType::IMPORTBOOK) {
        financeCount++;
        // 使用固定位置写入
        int pos = 2 * sizeof(int) + (financeCount - 1) * sizeof(Log);
        logStorage.update(log, pos);  // 使用update而不是write
        logStorage.write_info(financeCount, 1);
    } else {
        logStorage.write(log);  // 非财务记录才用write
    }
}
