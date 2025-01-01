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

struct FinanceLog{//一个对象表示一次交易
    bool ty = 0;//表示buy or import 0表示buy(收入)， 1表示import(支出)
    double tot = 0;//正数，该笔交易额
    FinanceLog() = default;
    FinanceLog(const bool& type, const double& num):ty(type), tot(num){}
    FinanceLog& operator = (const FinanceLog& rhs){
        ty = rhs.ty;
        tot = rhs.tot;
        return *this;
    }
    friend std::ostream & operator << (std::ostream& ou, const FinanceLog& tmp_fin){
        if(tmp_fin.ty){
            ou << "import ";
        }
        else ou << "buy ";
        ou << tmp_fin.tot;
        return ou;
    }
};

class LogManager {
private:
    MemoryRiver<Log> logStorage;
    int financeCount = 0; // 交易笔数
    int sizeofF = sizeof(FinanceLog);
public:
    LogManager();

    void ReportFinance();

    void ShowLog();

    void AddLog(Log& log); // 把 log 放进文件的同时还需要检查是否有交易

    void ReportEmployee();
};

class Log_System{
private:
    MemoryRiver<FinanceLog> Fin_Log_Data;
    int sizeofF = sizeof(FinanceLog);
public:
    Log_System():Fin_Log_Data("fin_log_data"){};
    void show_fin(){
        int tot_time;
        double income = 0, outcome = 0;
        FinanceLog cur;
        Fin_Log_Data.get_info(tot_time, 1);
        for(int i = 8; i <= 8 + (tot_time - 1) * sizeofF; i += sizeofF){
            Fin_Log_Data.read(cur, i);
            if(!cur.ty){
                income += cur.tot;
            }
            else{
                outcome += cur.tot;
            }
        }
        cout << fixed << std::setprecision(2)<< "+ " << income << " - " << outcome << endl;
    }
    void show_fin(const int& num){
        if(!num){
            cout << endl;
            return;
        }
        int tot_time;
        double income = 0, outcome = 0;
        FinanceLog cur;
        Fin_Log_Data.get_info(tot_time, 1);
        if(num > tot_time){
            cout << "Invalid\n";
            return;
        }
        for(int i = 8 + (tot_time - 1) * sizeofF; i >= 8 + (tot_time - num) * sizeofF; i -= sizeofF){
            Fin_Log_Data.read(cur, i);
            if(!cur.ty){
                income += cur.tot;
            }
            else{
                outcome += cur.tot;
            }
        }
        cout << fixed << std::setprecision(2)<< "+ " << income << " - " << outcome << endl;
    }
    void add_finance(const string& type, const double& num){
        FinanceLog cur;
        if(type == "buy"){
            cur = FinanceLog(0, num);
            Fin_Log_Data.write(cur);
        }
        else{
            cur = FinanceLog(1, num);
            Fin_Log_Data.write(cur);
        }
    }
};
#endif //LOGMANAGER_H
