#include "AccountManager.h"

//判断是否是下划线,数字,字母
bool is_valid(const string &x) {
    if (x.empty()) return true; //不对空串做判断
    for (int i = 0;i < x.length(); ++i) {
        //不是可见字符
        if (x[i] != '_' && !(x[i] >= '0' && x[i] <= '9') &&
            !(x[i] >= 'a' && x[i] <= 'z') && !(x[i] >= 'A' && x[i] <= 'Z') ) {
            return false;
        }
    }
    return true;
}

//是否是可见字符
bool is_visible(const string &x) {
    if (x.empty()) return true; //不对空串做判断
    for (int i = 0;i < x.length(); ++i) {
        if (x[i] < 32 || x[i] > 126) {
            return false;
        }
    }
    return true;
}

//class AccountId

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
    //加入root账户,注意判断是否已经存在root,防止重复加入
    //root是UseerId
    Account root("root", "", "sjtu", 7);
    accountStorage.get_info(account_count, 1);
    if (account_count) return; //已经存在
    int pos = accountStorage.write(root);

    userId_pos.insert_node(DataNode(root.UserId.UserId, pos));
}

AccountManager::AccountManager(const string &file_name) {
    accountStorage.initialise("accountStorage");
    userId_pos.init(file_name);

}

void AccountManager::logIn(Command &line) {
    //登录的时候,不仅要查找是否注册过,
    //还要在登录栈中查找是否已经登录过?
    //似乎可以重复登录同一账户?

    //判断有无多余的参数
    if (line.count < 2 || line.count > 3) {
        throw Error("Invalid\n");
    }

    string UseerId = line.getNext();
    if (!is_valid(UseerId) || UseerId.length() > 30) {
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
    if (!s.empty() && (!is_valid(s) || s.length() > 30) ) {
        throw Error("Invalid\n");
    }
    //todo:判断顺序要调整，先看密码的正确性
    if (!s.empty()) {
        if (strcmp(s.c_str(), temp.password) != 0) {
            //异常判断:密码错误
            throw Error("Invalid\n");
        } else {//正确,登录成功
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

    //提前判断合法性
    if (!is_valid(_UseerId) || !is_valid(_password) || !is_visible(_name) ||
        _UseerId.length() > 30 || _password.length() > 30 || _name.length() > 30) {
        throw Error("Invalid\n");
    }

    //不能缺省参数
    if (line.count != 4) {
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(_UseerId, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    } //重复,失败

    Account temp(_UseerId, _name, _password, 1);
    //只能注册权限为1的顾客账户
    int pos = accountStorage.write(temp);
    userId_pos.insert_node(DataNode(_UseerId, pos));
}

void AccountManager::addUser(Command &line, LogManager &logs) {
    string _UseerId = line.getNext();
    string _password = line.getNext();
    string _priority = line.getNext();
    string _name = line.getNext();

    //提前判断合法性
    if (!is_valid(_UseerId) || !is_valid(_password) || !is_visible(_name) ||
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
    } //重复,失败

    Account temp(_UseerId, _name, _password, _priority[0] - '0');
    int pos = accountStorage.write(temp);
    userId_pos.insert_node(DataNode(_UseerId, pos));
}

void AccountManager::changePassword(Command &line) {
    string UseerId = line.getNext();
    string old_password = line.getNext();
    string new_password = line.getNext();

    //提前判断合法性
    if (!is_valid(UseerId) || !is_valid(old_password) || !is_valid(new_password) ||
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
    //有密码,那么就判断正误
    if (!new_password.empty()) {
        if (strcmp(temp.password, old_password.c_str() ) != 0) {
            //密码错误
            throw Error("Invalid\n");
        } else {
            temp.changePassword(new_password);
            accountStorage.update(temp, ans[0]);
        }
    } else {
        if (getCurrentPrivilege() == 7) {
            //权限为7,上方的old读到的就是new
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
        //权限不足
        throw Error("Invalid\n");
    }

    string UseerId = line.getNext();
    if (!is_valid(UseerId) || UseerId.length() > 30) {
        throw Error("Invalid\n");
    }

    vector<int> ans;
    userId_pos.find_node(UseerId, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    } //不存在,失败

    //已经登录,失败
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