//
// Created by 43741 on 2024/12/22.
//

#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H
#include <iomanip>
#include <queue>
#include <set>
#include "CommandParser.h"
#include "AccountManager.h"
#include "LogManager.h"
struct BookISBN {
    char ISBN[21];
    BookISBN() = default;
    BookISBN(std::string isbn);
    bool operator == (const BookISBN &other)const;
    bool operator < (const BookISBN &other)const;
};

struct BookName {
    char Bookname[61];
    BookName() = default;
    BookName(std::string bookname);
    bool operator == (const BookName &other)const;
    bool operator < (const BookName &other)const;
};

struct AuthorName {
    char Author[61];
    AuthorName() = default;
    AuthorName(std::string authorname);

    bool operator == (const AuthorName &other)const;
    bool operator < (const AuthorName &other)const;
};

struct KeyWord {
    char Keyword[61];
    KeyWord() = default;
    KeyWord(std::string keyword);
    bool operator == (const KeyWord &other)const;
    bool operator < (const KeyWord &other)const;
};

class Book {
private:
    int book_id;
    BookISBN ISBN;
    BookName book_name;
    AuthorName author_name;
    KeyWord key_word;
    int Quantity = 0;
    int Price = 0;
    int TotalCost = 0;
    friend class BookManager;
public:
    Book() = default;
    Book(int id,std::string &isbn,std::string bookName,std::string authorName,std::string keyWord,
        int quantity,int price,int totalcost);
    Book(int id,std::string isbn);
    bool operator<(const Book &rhs) const;

    bool operator>(const Book &rhs) const;

    bool operator<=(const Book &rhs) const;

    bool operator>=(const Book &rhs) const;

    friend std::ostream &operator << (std::ostream&out,const Book &book);
};

class BookManager {
private:
    int bookCount;
    MemoryRiver<Book>BookStorage;
    DataFile BookID_pos;
    DataFile BookISBN_pos;
    DataFile BookName_pos;
    DataFile Author_pos;
    DataFile Keyword_pos;
public:
    BookManager();

    void Show(Command &input,AccountManager &account,LogManager &log);

    void Buy(Command &input, AccountManager &accounts, LogManager &logs);

    void Select(Command &input, AccountManager &accounts, LogManager &logs); // 检查是否有权限，检查是否有 ISBN，然后选中

    void Modify(Command &input, AccountManager &accounts, LogManager &logs); // 检查是否有权限

    void ImportBook(Command &input, AccountManager &accounts, LogManager &logs); // 检查是否有权限
};
#endif //BOOKMANAGER_H
