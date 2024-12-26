//
// Created by Leonard C on 2021/12/25.
// Updated by GPT on 2023/12/23.
//

#ifndef BOOKSTORE_EXCEPTION_H
#define BOOKSTORE_EXCEPTION_H

#include <string>
#include <exception>

class Error : public std::exception {
private:
    std::string err; // 存储错误信息

public:
    // 默认构造函数
    Error() noexcept : err("Unknown Error") {}

    // 构造函数：接受 std::string
    explicit Error(const std::string& message) noexcept : err(message) {}

    // 构造函数：接受 C 风格字符串
    explicit Error(const char* message) noexcept : err(message ? message : "Unknown Error") {}

    // 析构函数：声明为 noexcept，符合 C++ 标准异常类规范
    ~Error() noexcept override = default;

    // 获取错误信息：声明为 noexcept 和 const
    const char* what() const noexcept override {
        return err.c_str();
    }
};

#endif // BOOKSTORE_EXCEPTION_H
