#include "AccountManager.h"

bool isPasswordValid(std::string &input) {
    if (input.size() > 30) {
        return false;
    }
    for(int i = 0;i < input.size();++i) {
        if (input[i] != '_' && !(input[i]>='0' && input[i] <= '9') && !(input[i] >= 'a' && input[i] <= 'z') && !(input[i] >= 'A' && input[i] <= 'Z')) {
            return false;
        }
    }
    return true;
}

bool isUsernameValid(std::string input) {
    for (int i = 0;i < input.size(); ++i) {
        if (input[i] < 32 || input[i] > 126) {
            return false;
        }
    }
    return true;
}

AccountId::AccountId(std::string user_id) {
    strcpy(UserId,user_id.c_str());
}

std::string AccountId::getUserId() const {
    return UserId;
}

bool AccountId::operator<(const AccountId&other)const {
    return strcmp(UserId,other.UserId) < 0;
}

bool AccountId::operator==(const AccountId &other) const {
    return strcmp(UserId,other.UserId) == 0;
}

Account::Account(const std::string &user_id, const std::string &user_name,const std::string &password_, int privilege_) {
    UserId = user_id;
    strcpy(UserName,user_id.c_str());
    strcpy(password,password_.c_str());
    privilege = privilege_;
}

int Account::getPrivilege() {
    return privilege;
}

void Account::changePassword(std::string newPassword) {
    strcpy(password,newPassword.c_str());
}

AccountManager::AccountManager() {
    AccountStorage.initialise("account_data");
    userId_pos.init("account_to_pos");
}

AccountManager::AccountManager(std::string fileName) {
    AccountStorage.initialise(fileName);
}

void AccountManager::LogIn(Command &input) {
    if (input.count != 2 && input.count != 3) {
        throw Error("Invalid\n");
    }
    std::string user_id = input.getNext();
    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }//此用户不存在

    int curPrivilege = getCurrentPrivilege();
    Account temp;
    AccountStorage.read(temp, ans[0]);

    string s = input.getNext();
    if (!s.empty() && (!isPasswordValid(s))) {
        throw Error("Invalid\n");
    }
    if (!s.empty()) {
        if (strcmp(s.c_str(), temp.password) != 0) {
            //异常判断:密码错误
            throw Error("Invalid\n");
        }
        else {//正确,登录成功
            LogInAccount tp;
            tp.LogAccount = temp;
            LogInStorage.push_back(tp);
            return;
        }
    }
    else {
        if (curPrivilege >= temp.getPrivilege()) {
            LogInAccount tp;
            tp.LogAccount = temp;
            LogInStorage.push_back(tp);
            return;
        }
        else {
            throw Error("Invalid\n");
        }
    }
}

void AccountManager::LogOut(Command &input) {
    if (LogInStorage.empty()) {
        throw Error("Invalid\n");
    }
    LogInStorage.pop_back();
}

void AccountManager::Register(Command &input) {
    std::string user_id = input.getNext();
    std::string password = input.getNext();
    std::string user_name = input.getNext();

    if (!isUsernameValid(user_name)) {
        throw Error("Invalid\n");
    }
    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    }
    Account temp(user_id, user_name, password, 1);
    //只能注册权限为1的顾客账户
    int pos = AccountStorage.write(temp);
    userId_pos.insert_node(DataNode(user_id, pos));
}

void AccountManager::changePassword(Command &input) {
    std::string user_id = input.getNext();
    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }
    if (getCurrentPrivilege() != 7) {
        if (input.count != 4) {
            throw Error("Invalid\n");
        }
        Account temp;
        AccountStorage.read(temp, ans[0]);
        std::string CurrentPassword = input.getNext();
        std::string NewPassword = input.getNext();
        if (strcmp(CurrentPassword.c_str(),temp.password) != 0) {
            throw Error("Invalid\n");
        }
        if (!isPasswordValid(NewPassword)) {
            throw Error("Invalid\n");
        }
        temp.changePassword(NewPassword);
    }
    else if (getCurrentPrivilege() == 7) {
        if (input.count != 4 && input.count != 3) {
            throw Error("Invalid\n");
        }
        if (input.count == 4) {
            Account temp;
            AccountStorage.read(temp, ans[0]);
            std::string CurrentPassword = input.getNext();
            std::string NewPassword = input.getNext();
            if (strcmp(CurrentPassword.c_str(),temp.password) != 0) {
                throw Error("Invalid\n");
            }
            if (!isPasswordValid(NewPassword)) {
                throw Error("Invalid\n");
            }
            temp.changePassword(NewPassword);
        }
        else {
            Account temp;
            AccountStorage.read(temp, ans[0]);
            std::string NewPassword = input.getNext();
            if (!isPasswordValid(NewPassword)) {
                throw Error("Invalid\n");
            }
            temp.changePassword(NewPassword);
        }
    }
}

void AccountManager::UserAdd(Command &input, LogManager &log) {
    if (input.count != 5) {
        throw Error("Invalid\n");
    }
    std::string UserID = input.getNext();
    std::string PassWord = input.getNext();
    int Privilege = stoi(input.getNext());
    std::string Username = input.getNext();
    if (getCurrentPrivilege() < 3 || Privilege >= getCurrentPrivilege()) {
        throw Error("Invalid\n");
    }
    std::vector<int> ans;
    userId_pos.find_node(UserID, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    }//账户已经存在
    Account temp(UserID,Username,PassWord,Privilege);
    int pos = AccountStorage.write(temp);
    userId_pos.insert_node(DataNode(UserID, pos));
}

void AccountManager::DeleteUser(Command &input, LogManager &log) {
    if (getCurrentPrivilege() != 7) {
        throw Error("Invalid\n");
    }
    if (input.count != 2) {
        throw Error("Invalid\n");
    }
    std::vector<int>ans;
    std::string UserID = input.getNext();
    userId_pos.find_node(UserID,ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }
    for (int i = 0;i < LogInStorage.size();++i) {
        if (LogInStorage[i].LogAccount.UserId == UserID) {
            throw Error("Invalid\n");
        }
    }
    AccountStorage.Delete(ans[0]);
    userId_pos.delete_node(DataNode(UserID, ans[0]));
}

void AccountManager::selectBook(int book_id) {
    LogInStorage.back().selectBookId = book_id;
}

int AccountManager::getCurrentPrivilege() const {
    if (LogInStorage.empty()) {
        return 0;
    }
    else {
        return LogInStorage.back().LogAccount.privilege;
    }
}