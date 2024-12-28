#ifndef BOOKSTORE_ERROR_H
#define BOOKSTORE_ERROR_H

#include <string>
#include <exception>

class Error : public std::exception {
private:
    std::string err;

public:
    Error() noexcept : err("Unknown Error") {}
    explicit Error(const std::string& message) noexcept : err(message) {}
    explicit Error(const char* message) noexcept : err(message ? message : "Unknown Error") {}
    ~Error() noexcept override = default;

    const char* what() const noexcept override {
        return err.c_str();
    }
};

#endif // BOOKSTORE_ERROR_H