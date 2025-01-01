#ifndef BOOKSTORE_VALIDATOR_H
#define BOOKSTORE_VALIDATOR_H

#include <string>
#include <algorithm>
#include <set>

namespace Validator {
    // 用户ID和密码验证（数字、字母、下划线，最大30字符）
    inline bool isValidUserID(const std::string &x) {
        if (x.empty()) return true;
        for (int i = 0;i < x.length(); ++i) {
            if (x[i] != '_' && !(x[i] >= '0' && x[i] <= '9') && !(x[i] >= 'a' && x[i] <= 'z') && !(x[i] >= 'A' && x[i] <= 'Z') ) {
                return false;
            }
        }
        return true;
    }

    // 用户名验证（除不可见字符外的ASCII字符，最大30字符）
    inline bool isValidUsername(const std::string &x) {
        if (x.empty()) return true;
        for (int i = 0;i < x.length(); ++i) {
            if (x[i] < 32 || x[i] > 126) {
                return false;
            }
        }
        return true;
    }

    inline bool isWithinAscll(char input) {
        if (input < 0 || input > 127) {
            return false;
        }
        return true;
    }

    // ISBN验证（除不可见字符外的ASCII字符，最大20字符）
    inline bool isValidISBN(const std::string &str) {
        if (str.empty() || str.length() > 20) return false;
        return std::all_of(str.begin(), str.end(), [](char c) {
            return c >= 32 && c <= 126;
        });
    }

    // 书名和作者名验证（除不可见字符和双引号外的ASCII字符，最大60字符）
    inline bool isValidBookString(const std::string &str) {
        if (str.empty() || str.length() > 60) return false;
        return std::all_of(str.begin(), str.end(), [](char c) {
            return c >= 32 && c <= 126 && c != '\"';
        });
    }

    // 关键词验证
    inline bool isValidKeyword(const std::string &str) {
        if (str.empty() || str.length() > 60) return false;
        if (str[0] == '|' || str.back() == '|') return false;

        std::set<std::string> keywords;
        std::string current;
        for (char c : str) {
            if (c == '|') {
                if (current.empty()) return false;
                if (keywords.find(current) != keywords.end()) return false;
                keywords.insert(current);
                current.clear();
            } else {
                if (c < 32 || c > 126 || c == '\"') return false;
                current += c;
            }
        }
        if (current.empty()) return false;
        if (keywords.find(current) != keywords.end()) return false;
        return true;
    }

    // 数量验证（正整数，不超过2'147'483'647）
    inline bool isValidQuantity(const std::string &str) {
        if (str.empty() || str.length() > 10) return false;
        if (!std::all_of(str.begin(), str.end(), ::isdigit)) return false;
        try {
            long long num = std::stoll(str);
            return num > 0 && num <= 2147483647;
        } catch (...) {
            return false;
        }
    }

    // 价格验证（正数，包含小数点，最大13字符）
    inline bool isValidPrice(const std::string &str) {
        if (str.empty() || str.length() > 13) return false;
        int dotCount = 0;
        for (char c : str) {
            if (c == '.') {
                dotCount++;
                if (dotCount > 1) return false;
            } else if (!isdigit(c)) {
                return false;
            }
        }
        try {
            double price = std::stod(str);
            return price >= 0;
        } catch (...) {
            return false;
        }
    }
}

#endif // BOOKSTORE_VALIDATOR_H