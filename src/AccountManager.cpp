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

void AccountManager::logIn(Command &line) {


    if (line.count < 2 || line.count > 3) {
        throw Error("Invalid\n");
    }

    string UseerId = line.getNext();
    if (!Validator::isValidUserID(UseerId) || UseerId.length() > 30) {
        throw Error("Invalid\n");
    }
    vector<int> ans;
    userId_pos.find_node(UseerId, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }//没注册过,失败

    int cur_priority = getCurrentPrivilege();
    Account temp;
    accountStorage.read(temp, ans[0]);

    string s = line.getNext();
    if (!s.empty() && (!Validator::isValidUserID(s) || s.length() > 30) ) {
        throw Error("Invalid\n");
    }
    //todo:判断顺序要调整，先看密码的正确性
    if (!s.empty()) {
        if (strcmp(s.c_str(), temp.password) != 0) {
            throw Error("Invalid\n");
        } else {
            LogInAccount tp;
            tp.account = temp;
            loginStack.push_back(tp);
            return;
        }
    } else {
        //todo:密码为空，判断权限
        if (cur_priority >= temp.getPrivilege()) {
            LogInAccount tp;
            tp.account = temp;
            loginStack.push_back(tp);
            return;
        } else {
            throw Error("Invalid\n");
        }
    }
}

void AccountManager::logOut(Command &line) {
    if (line.count != 1 || loginStack.empty()) {
        throw Error("Invalid\n");
    }    //没有登录账户,异常

    loginStack.pop_back();
}

void AccountManager::registerUser(Command &line) {
    string _UseerId = line.getNext();
    string _password = line.getNext();
    string _name = line.getNext();

    if (!Validator::isValidUserID(_UseerId) || !Validator::isValidUserID(_password) || !Validator::isValidUsername(_name) ||
        _UseerId.length() > 30 || _password.length() > 30 || _name.length() > 30) {
        throw Error("Invalid\n");
    }

    if (line.count != 4) {
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

void AccountManager::addUser(Command &line, LogManager &logs) {
    string _UseerId = line.getNext();
    string _password = line.getNext();
    string _priority = line.getNext();
    string _name = line.getNext();

    if (!Validator::isValidUserID(_UseerId) || !Validator::isValidUserID(_password) || !Validator::isValidUsername(_name) ||
        _priority[0] < '0' || _priority[0] > '7' || _priority.length() != 1 ||
        _UseerId.length() > 30 || _password.length() > 30 || _name.length() > 30) {
        throw Error("Invalid\n");
    }

    if (line.count != 5) {
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

void AccountManager::changePassword(Command &line) {
    string UseerId = line.getNext();
    string old_password = line.getNext();
    string new_password = line.getNext();

    if (!Validator::isValidUserID(UseerId) || !Validator::isValidUserID(old_password) || !Validator::isValidUserID(new_password) ||
        UseerId.length() > 30 || old_password.length() > 30 || new_password.length() > 30) {
        throw Error("Invalid\n");
    }

    if (line.count < 3 || line.count > 4) {
        throw Error("Invalid\n");
    }

    if (getCurrentPrivilege() < 1) {
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(UseerId, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    } //不存在,失败

    Account temp;
    accountStorage.read(temp, ans[0]);
    if (!new_password.empty()) {
        if (strcmp(temp.password, old_password.c_str() ) != 0) {
            throw Error("Invalid\n");
        } else {
            temp.changePassword(new_password);
            accountStorage.update(temp, ans[0]);
        }
    } else {
        if (getCurrentPrivilege() == 7) {
            temp.changePassword(old_password);
            accountStorage.update(temp, ans[0]);
        } else {
            throw Error("Invalid\n");
        }
    }
}

int AccountManager::getCurrentPrivilege() const{
    if (loginStack.empty()) return 0;
    return loginStack.back().account.privilege;
}

void AccountManager::deleteUser(Command &line, LogManager &logs) {
    if (line.count != 2 || getCurrentPrivilege() < 7) {

        throw Error("Invalid\n");
    }

    string UseerId = line.getNext();
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
    loginStack.back().selectedBookId = book_id;
}