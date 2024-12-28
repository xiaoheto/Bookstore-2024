#include "AccountManager.h"
#include "validator.h"

AccountId::AccountId(std::string user_id) {
    if (!Validator::isValidUserID(user_id)) {
        throw Error("Invalid\n");
    }
    strcpy(UserId, user_id.c_str());
}

std::string AccountId::getUserId() const {
    return std::string(UserId);
}

bool AccountId::operator<(const AccountId &other) const {
    return strcmp(UserId, other.UserId) < 0;
}

bool AccountId::operator==(const AccountId &other) const {
    return strcmp(UserId, other.UserId) == 0;
}

Account::Account(const std::string &user_id, const std::string &user_name,
                const std::string &password_, int privilege_) {
    // 验证所有输入
    if (!Validator::isValidUserID(user_id) ||
        !Validator::isValidUsername(user_name) ||
        !Validator::isValidUserID(password_) ||
        (privilege_ != 0 && privilege_ != 1 && privilege_ != 3 && privilege_ != 7)) {
        throw Error("Invalid\n");
    }

    UserId = AccountId(user_id);
    strncpy(UserName, user_name.c_str(), 30);
    UserName[30] = '\0';
    strncpy(password, password_.c_str(), 30);
    password[30] = '\0';
    privilege = privilege_;
}

int Account::getPrivilege() {
    return privilege;
}

void Account::changePassword(const std::string &newPassword) {
    if (!Validator::isValidUserID(newPassword)) {
        throw Error("Invalid\n");
    }
    strncpy(password, newPassword.c_str(), 30);
    password[30] = '\0';
}

AccountManager::AccountManager() {
    accountStorage.initialise("account_data");
    userId_pos.init("account_to_pos");
    account_count = 0;

    accountStorage.get_info(account_count, 1);
    if (account_count == 0) {
        // 创建 root 账户
        Account root("root", "", "sjtu", 7);
        int pos = accountStorage.write(root);
        if (pos <= 0) throw Error("Invalid\n");

        account_count = 1;
        accountStorage.write_info(account_count, 1);

        if (!userId_pos.insert_node(DataNode("root", pos))) {
            throw Error("Invalid\n");
        }
    }
}

void AccountManager::logIn(Command &input) {
    if (input.count != 2 && input.count != 3) {
        throw Error("Invalid\n");
    }

    std::string user_id = input.getNext();
    if (!Validator::isValidUserID(user_id)) {
        throw Error("Invalid\n");
    }

    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }

    Account temp;
    accountStorage.read(temp, ans[0]);
    std::string password = input.getNext();

    int currentPrivilege = getCurrentPrivilege();

    if (!password.empty()) {
        // 提供了密码的情况
        if (!Validator::isValidUserID(password) ||
            strcmp(password.c_str(), temp.password) != 0) {
            throw Error("Invalid\n");
        }
    } else if (currentPrivilege < temp.privilege) {
        // 没提供密码且权限不足
        throw Error("Invalid\n");
    }

    loginStack.push_back(LogInAccount(temp));
}

void AccountManager::logOut(Command &input) {
    if (input.count != 1 || loginStack.empty()) {
        throw Error("Invalid\n");
    }

    // 清除选中的图书
    loginStack.back().selectedBookId = 0;
    loginStack.pop_back();
}

void AccountManager::registerUser(Command &input) {
    if (input.count != 4) {
        throw Error("Invalid\n");
    }

    std::string user_id = input.getNext();
    std::string password = input.getNext();
    std::string user_name = input.getNext();

    if (!Validator::isValidUserID(user_id) ||
        !Validator::isValidUserID(password) ||
        !Validator::isValidUsername(user_name)) {
        throw Error("Invalid\n");
    }

    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    }

    accountStorage.get_info(account_count, 1);
    Account temp(user_id, user_name, password, 1);
    int pos = accountStorage.write(temp);
    if (pos <= 0) throw Error("Invalid\n");

    account_count++;
    accountStorage.write_info(account_count, 1);

    if (!userId_pos.insert_node(DataNode(user_id, pos))) {
        throw Error("Invalid\n");
    }
}

void AccountManager::changePassword(Command &input) {
    if (input.count != 3 && input.count != 4) {
        throw Error("Invalid\n");
    }

    std::string user_id = input.getNext();
    if (!Validator::isValidUserID(user_id)) {
        throw Error("Invalid\n");
    }

    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }

    Account temp;
    accountStorage.read(temp, ans[0]);

    if (getCurrentPrivilege() == 7) {
        // root 用户修改密码
        std::string newPassword = input.getNext();
        if (!Validator::isValidUserID(newPassword)) {
            throw Error("Invalid\n");
        }
        temp.changePassword(newPassword);
    } else {
        // 普通用户修改密码
        if (input.count != 4) {
            throw Error("Invalid\n");
        }
        std::string currentPassword = input.getNext();
        std::string newPassword = input.getNext();

        if (!Validator::isValidUserID(currentPassword) ||
            !Validator::isValidUserID(newPassword)) {
            throw Error("Invalid\n");
        }

        if (strcmp(currentPassword.c_str(), temp.password) != 0) {
            throw Error("Invalid\n");
        }

        temp.changePassword(newPassword);
    }

    accountStorage.update(temp, ans[0]);
}

void AccountManager::addUser(Command &input, LogManager &log) {
    if (input.count != 5) {
        throw Error("Invalid\n");
    }

    std::string user_id = input.getNext();
    std::string password = input.getNext();
    std::string privilege_str = input.getNext();
    std::string user_name = input.getNext();

    // 验证所有输入
    if (!Validator::isValidUserID(user_id) ||
        !Validator::isValidUserID(password) ||
        !Validator::isValidUsername(user_name)) {
        throw Error("Invalid\n");
    }

    int privilege;
    try {
        privilege = std::stoi(privilege_str);
        if (privilege != 1 && privilege != 3 && privilege != 7) {
            throw Error("Invalid\n");
        }
    } catch (...) {
        throw Error("Invalid\n");
    }

    if (getCurrentPrivilege() < 3 || privilege >= getCurrentPrivilege()) {
        throw Error("Invalid\n");
    }

    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (!ans.empty()) {
        throw Error("Invalid\n");
    }

    Account temp(user_id, user_name, password, privilege);
    int pos = accountStorage.write(temp);
    if (pos <= 0) throw Error("Invalid\n");

    account_count++;
    accountStorage.write_info(account_count, 1);

    if (!userId_pos.insert_node(DataNode(user_id, pos))) {
        throw Error("Invalid\n");
    }

    // 添加日志
    Log addUserLog;
    addUserLog.behavoir = ActionType::ADDUSER;
    addUserLog.use = &loginStack.back().account;
    snprintf(addUserLog.description, 150, "Added user %s with privilege %d",
             user_id.c_str(), privilege);
    log.AddLog(addUserLog);
}

void AccountManager::deleteUser(Command &input, LogManager &log) {
    if (getCurrentPrivilege() != 7 || input.count != 2) {
        throw Error("Invalid\n");
    }

    std::string user_id = input.getNext();
    if (!Validator::isValidUserID(user_id)) {
        throw Error("Invalid\n");
    }

    std::vector<int> ans;
    userId_pos.find_node(user_id, ans);
    if (ans.empty()) {
        throw Error("Invalid\n");
    }

    // 检查用户是否已登录
    for (const auto &login : loginStack) {
        if (login.account.UserId == AccountId(user_id)) {
            throw Error("Invalid\n");
        }
    }

    accountStorage.Delete(ans[0]);
    userId_pos.delete_node(DataNode(user_id, ans[0]));

    account_count--;
    accountStorage.write_info(account_count, 1);

    // 添加日志
    Log deleteUserLog;
    deleteUserLog.behavoir = ActionType::DELETEUSER;
    deleteUserLog.use = &loginStack.back().account;
    snprintf(deleteUserLog.description, 150, "Deleted user %s", user_id.c_str());
    log.AddLog(deleteUserLog);
}

void AccountManager::selectBook(int book_id) {
    if (loginStack.empty()) {
        throw Error("Invalid\n");
    }
    loginStack.back().selectedBookId = book_id;
}

int AccountManager::getCurrentPrivilege() const {
    if (loginStack.empty()) {
        return 0;
    }
    return loginStack.back().account.privilege;
}

Account* AccountManager::getCurrentAccount() {
    if (loginStack.empty()) {
        return nullptr;
    }
    return &loginStack.back().account;
}