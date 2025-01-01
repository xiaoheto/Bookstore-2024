#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include <iomanip>
#include <cmath>
#include "CommandParser.h"
#include "AccountManager.h"
#include "LogManager.h"
#include "validator.h"

struct BookISBN {
    char ISBN[21];  // 20字符 + 结束符

    BookISBN() = default;
    explicit BookISBN(const std::string &isbn);

    bool operator==(const BookISBN &other) const;
    bool operator<(const BookISBN &other) const;
};

struct BookName {
    char Bookname[61];  // 60字符 + 结束符

    BookName() = default;
    explicit BookName(const std::string &bookname);

    bool operator==(const BookName &other) const;
    bool operator<(const BookName &other) const;
};

struct AuthorName {
    char Author[61];

    AuthorName() = default;
    explicit AuthorName(const std::string &authorname);

    bool operator==(const AuthorName &other) const;
    bool operator<(const AuthorName &other) const;
};

struct KeyWord {
    char Keyword[61];

    KeyWord() = default;
    explicit KeyWord(const std::string &keyword);

    bool operator==(const KeyWord &other) const;
    bool operator<(const KeyWord &other) const;
};

class Book {
private:
    int book_id;
    BookISBN ISBN;
    BookName book_name;
    AuthorName author_name;
    KeyWord key_word;
    double Price;
    int Quantity;
    double TotalCost;

    friend class BookManager;
public:
    Book() : book_id(0), Price(0), Quantity(0), TotalCost(0) {}

    // 完整构造函数
    Book(int id, const std::string &isbn, const std::string &bookName,
         const std::string &authorName, const std::string &keyWord,
         double price, int quantity, double totalcost);

    // 仅ISBN构造函数
    Book(int id, const std::string &isbn);

    bool operator<(const Book &rhs) const;
    bool operator>(const Book &rhs) const;
    bool operator<=(const Book &rhs) const;
    bool operator>=(const Book &rhs) const;

    friend std::ostream &operator<<(std::ostream &out, const Book &book);
};

class BookManager {
private:
    int bookCount;
    MemoryRiver<Book> BookStorage;
    DataFile BookID_pos;
    DataFile BookISBN_pos;
    DataFile BookName_pos;
    DataFile Author_pos;
    DataFile Keyword_pos;
    Log_System financeLog;

    friend class LogManager;
    // 私有辅助函数
    bool isValidBookString(const std::string &str, int maxLen) const;
    void processKeywords(const std::string &keywords, std::vector<std::string> &result) const;
    void updateIndexes(const Book &oldBook, const Book &newBook, int pos);
    Book* findBookByISBN(const std::string &isbn) const;
    void validateSelected(const AccountManager &accounts) const;

public:
    Log_System& getFinanceLog() { return financeLog; }

    const std::vector<long long>& getFinanceRecords() const {
        return financeRecords;
    }
    MemoryRiver<long long> financeFile;
    std::vector<long long >financeRecords;

    void addFinanceRecord(long long amount);

    BookManager();

    // 图书查询
    void Show(Command &input, AccountManager &account, LogManager &log);

    // 图书购买
    void Buy(Command &input, AccountManager &accounts, LogManager &logs);

    // 选择图书
    void Select(Command &input, AccountManager &accounts, LogManager &logs);

    // 修改图书信息
    void Modify(Command &input, AccountManager &accounts, LogManager &logs);

    // 图书进货
    void ImportBook(Command &input, AccountManager &accounts, LogManager &logs);

    // 获取当前图书总数
    int getBookCount() const { return bookCount; }
};

#endif // BOOKMANAGER_H