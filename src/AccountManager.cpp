#include "AccountManager.h"

#include "validator.h"

AccountId::AccountId(std::string AccountUseerId) {
    strcpy(UserId, AccountUseerId.c_str());
}

std::string AccountId::getUserId() const {
    return UserId;
}

bool AccountId::operator==(const AccountId &rhs) const {
    if (strcmp(UserId, rhs.UserId) == 0) return true;
    else return false;
}

bool AccountId::operator<(const AccountId &rhs) const {
    if (strcmp(UserId, rhs.UserId) < 0) return true;
    else return false;
}

Account::Account(const std::string &_UseerId, const std::string &_name, const std::string &_password, int _priority) {
    strcpy(UserId.UserId, _UseerId.c_str());
    strcpy(UserName, _name.c_str());
    strcpy(password, _password.c_str());
    privilege = _priority;
}

void Account::changePassword(const std::string &newPassword) {
    strcpy(password, newPassword.c_str());
}

int Account::getPrivilege() {
    return privilege;
}

//class AccountManegement

AccountManager::AccountManager() {
    accountStorage.initialise("accountStorage");//MemoryRiver
    userId_pos.init("userId_pos");//ull

    account_count = 0;
    Account root("root", "", "sjtu", 7);
    accountStorage.get_info(account_count, 1);
    if (account_count) return;
    int pos = accountStorage.write(root);

    userId_pos.insert_node(DataNode(root.UserId.UserId, pos));
}

AccountManager::AccountManager(const string &file_name) {
    accountStorage.initialise("accountStorage");
    userId_pos.init(file_name);

}

void AccountManager::logIn(Command &input) {
    if (input.count != 2 && input.count != 3) {
        throw Error("Invalid\n");
    }

    string UseerId = input.getNext();
    if (!Validator::isValidUserID(UseerId) || UseerId.length() > 30) {
        throw Error("Invalid\n");
    }
    vector<int> ans;
    userId_pos.find_node(UseerId, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }//用户不存在,失败

    Account temp;
    accountStorage.read(temp, ans[0]);

    string s = input.getNext();
    if (!s.empty() && (!Validator::isValidUserID(s) || s.length() > 30)) {
        throw Error("Invalid\n");
    }

    // 否则需要检查密码
    if (s.empty()) {
        if (getCurrentPrivilege() >= temp.getPrivilege()) {
            LogInAccount tp;
            tp.account = temp;
            loginStack.push_back(tp);
        }
        else {
            throw Error("Invalid\n");
        }
    }
    else {
        if (strcmp(s.c_str(), temp.password) != 0) {
            throw Error("Invalid\n");
        }
        LogInAccount tp;
        tp.account = temp;
        loginStack.push_back(tp);
    }
}

void AccountManager::logOut(Command &input) {
    if (input.count != 1 || loginStack.empty()) {
        throw Error("Invalid\n");
    }

    loginStack.pop_back();
}

void AccountManager::registerUser(Command &input) {
    string _UseerId = input.getNext();
    string _password = input.getNext();
    string _name = input.getNext();

    if (!Validator::isValidUserID(_UseerId) || !Validator::isValidUserID(_password) || !Validator::isValidUsername(_name) ||
        _UseerId.length() > 30 || _password.length() > 30 || _name.length() > 30) {
        throw Error("Invalid\n");
    }

    if (input.count != 4) {
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(_UseerId, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    }

    Account temp(_UseerId, _name, _password, 1);
    int pos = accountStorage.write(temp);
    userId_pos.insert_node(DataNode(_UseerId, pos));
}

void AccountManager::addUser(Command &input, LogManager &logs) {
    string _UseerId = input.getNext();
    string _password = input.getNext();
    string _priority = input.getNext();
    string _name = input.getNext();

    if (!Validator::isValidUserID(_UseerId) || !Validator::isValidUserID(_password) || !Validator::isValidUsername(_name) ||
        _priority[0] < '0' || _priority[0] > '7' || _priority.length() != 1 ||
        _UseerId.length() > 30 || _password.length() > 30 || _name.length() > 30) {
        throw Error("Invalid\n");
    }

    if (input.count != 5) {
        throw Error("Invalid\n");
    }

    if (getCurrentPrivilege() <= _priority[0] - '0' ||
        getCurrentPrivilege() < 3) {//权限不足,失败
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(_UseerId, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    }

    Account temp(_UseerId, _name, _password, _priority[0] - '0');
    int pos = accountStorage.write(temp);
    userId_pos.insert_node(DataNode(_UseerId, pos));
}

void AccountManager::changePassword(Command &input) {
    string UserID = input.getNext();
    string oldPassword = input.getNext();
    string newPassword;

    if (input.count == 3) {
        newPassword = oldPassword; 
        oldPassword = ""; 
    } else if (input.count == 4) {
        newPassword = input.getNext();
    } else {
        throw Error("Invalid\n");
    }

    if (!Validator::isValidUserID(UserID) || UserID.length() > 30 ||
        (!oldPassword.empty() && (!Validator::isValidUserID(oldPassword) || oldPassword.length() > 30)) ||
        !Validator::isValidUserID(newPassword) || newPassword.length() > 30) {
        throw Error("Invalid\n");
        }

    if (getCurrentPrivilege() < 1) {
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(UserID, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }

    Account temp;
    accountStorage.read(temp, ans[0]);

    if (getCurrentPrivilege() < 7) {
        if (oldPassword.empty() || strcmp(temp.password, oldPassword.c_str()) != 0) {
            throw Error("Invalid\n");
        }
    }

    temp.changePassword(newPassword);
    accountStorage.update(temp, ans[0]);
}

int AccountManager::getCurrentPrivilege() const{
    if (loginStack.empty()) return 0;
    return loginStack.back().account.privilege;
}

void AccountManager::deleteUser(Command &input, LogManager &logs) {
    if (input.count != 2 || getCurrentPrivilege() < 7) {

        throw Error("Invalid\n");
    }

    string UseerId = input.getNext();
    if (!Validator::isValidUserID(UseerId) || UseerId.length() > 30) {
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(UseerId, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }

    for (auto item:loginStack){
        if (item.account.UserId.getUserId() == UseerId) {
            throw Error("Invalid\n");
        }
    }

    accountStorage.Delete(ans[0]);
    userId_pos.delete_node(DataNode(UseerId, ans[0]));
}

void AccountManager::selectBook(int book_id) {
    if (getCurrentPrivilege() < 3) {
        throw Error("Invalid\n");
    }
    loginStack.back().selectedBookId = book_id;
}
