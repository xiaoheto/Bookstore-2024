#include "BookManager.h"

// BookISBN 实现
BookISBN::BookISBN(const std::string &isbn) {
    if (!Validator::isValidISBN(isbn)) {
        throw Error("Invalid\n");
    }
    strncpy(ISBN, isbn.c_str(), 20);
    ISBN[20] = '\0';
}

bool BookISBN::operator==(const BookISBN &other) const {
    return strcmp(ISBN, other.ISBN) == 0;
}

bool BookISBN::operator<(const BookISBN &other) const {
    return strcmp(ISBN, other.ISBN) < 0;
}

// BookName 实现
BookName::BookName(const std::string &bookname) {
    if (!Validator::isValidBookString(bookname)) {
        throw Error("Invalid\n");
    }
    strncpy(Bookname, bookname.c_str(), 60);
    Bookname[60] = '\0';
}

bool BookName::operator==(const BookName&other)const {
    return strcmp(Bookname,other.Bookname) == 0;
}

bool BookName::operator<(const BookName&other)const {
    return strcmp(Bookname,other.Bookname) < 0;
}

AuthorName::AuthorName(const std::string &authorname) {
    if (!Validator::isValidBookString(authorname)) {
        throw Error("Invalid\n");
    }
    strcpy(Author,authorname.c_str());
    Author[60] = '\0';
}

bool AuthorName::operator==(const AuthorName &other) const {
    return strcmp(Author, other.Author) == 0;
}

bool AuthorName::operator<(const AuthorName &other) const {
    return strcmp(Author, other.Author) < 0;
}

KeyWord::KeyWord(const std::string &keyword) {
    if (!Validator::isValidKeyword(keyword)) {
        throw Error("Invalid\n");
    }
    strcpy(Keyword,keyword.c_str());
    Keyword[60] = '\0';
}

bool KeyWord::operator<(const KeyWord &other) const {
    return strcmp(Keyword, other.Keyword) < 0;
}

bool KeyWord::operator==(const KeyWord &other) const {
    return strcmp(Keyword, other.Keyword) == 0;
}

// Book 实现
Book::Book(int id, const std::string &isbn, const std::string &bookName,
           const std::string &authorName, const std::string &keyWord,
           double price, int quantity, double totalcost)
    : book_id(id), ISBN(isbn), book_name(bookName),
      author_name(authorName), key_word(keyWord),
      Price(price), Quantity(quantity), TotalCost(totalcost) {}

Book::Book(int id, const std::string &isbn)
    : book_id(id), ISBN(isbn), Price(0), Quantity(0), TotalCost(0) {
    book_name.Bookname[0] = '\0';
    author_name.Author[0] = '\0';
    key_word.Keyword[0] = '\0';
}

bool Book::operator<(const Book &rhs) const {
    return ISBN < rhs.ISBN;
}

bool Book::operator>(const Book &rhs) const {
    return rhs < *this;
}

bool Book::operator<=(const Book &rhs) const {
    return !(rhs < *this);
}

bool Book::operator>=(const Book &rhs) const {
    return !(*this < rhs);
}

std::ostream &operator<<(std::ostream &out, const Book &book) {
    out << book.ISBN.ISBN << '\t'
        << book.book_name.Bookname << '\t'
        << book.author_name.Author << '\t'
        << book.key_word.Keyword << '\t'
        << std::fixed << std::setprecision(2) << book.Price << '\t'
        << book.Quantity << '\n';
    return out;
}

// BookManager 实现
BookManager::BookManager() {
    BookStorage.initialise("book_data");
    BookID_pos.init("book_id_to_pos");
    BookISBN_pos.init("isbn_to_pos");
    BookName_pos.init("name_to_pos");
    Author_pos.init("author_to_pos");
    Keyword_pos.init("keyword_to_pos");

    BookStorage.get_info(bookCount, 1);
}

void BookManager::validateSelected(const AccountManager &accounts) const {
    if (accounts.getCurrentPrivilege() < 3) {
        throw Error("Invalid\n");
    }
    if (accounts.loginStack.empty() ||
        accounts.loginStack.back().selectedBookId == 0) {
        throw Error("Invalid\n");
    }
}

void BookManager::Show(Command &input, AccountManager &account, LogManager &log) {
    if (input.count > 2) {
        throw Error("Invalid\n");
    }

    std::string param = input.getNext();
    std::vector<Book> results;

    if (param.empty()) {
        // 显示所有图书
        std::vector<int> positions;
        BookISBN_pos.find_all(positions);
        for (int pos : positions) {
            Book book;
            if (BookStorage.read(book, pos)) {
                results.push_back(book);
            }
        }
    } else {
        if (param.substr(0, 6) == "-ISBN=") {
            std::string isbn = param.substr(6);
            if (!Validator::isValidISBN(isbn)) {
                throw Error("Invalid\n");
            }
            std::vector<int> positions;
            BookISBN_pos.find_node(isbn, positions);
            if (!positions.empty()) {
                Book book;
                BookStorage.read(book, positions[0]);
                results.push_back(book);
            }
        } else if (param.substr(0, 6) == "-name=") {
            if (param.length() < 8 || param[6] != '\"' || param.back() != '\"') {
                throw Error("Invalid\n");
            }
            std::string name = param.substr(7, param.length() - 8);
            if (!Validator::isValidBookString(name)) {
                throw Error("Invalid\n");
            }
            std::vector<int> positions;
            BookName_pos.find_node(name, positions);
            for (int pos : positions) {
                Book book;
                BookStorage.read(book, pos);
                results.push_back(book);
            }
        }
        // ... 其他查询条件类似
    }

    // 按ISBN排序并输出
    std::sort(results.begin(), results.end());
    if (results.empty()) {
        std::cout << '\n';
    } else {
        for (const auto &book : results) {
            std::cout << book;
        }
    }
}

void BookManager::Buy(Command &input, AccountManager &accounts, LogManager &logs) {
    if (input.count != 3) {
        throw Error("Invalid\n");
    }

    std::string isbn = input.getNext();
    if (!Validator::isValidISBN(isbn)) {
        throw Error("Invalid\n");
    }

    std::string quantityStr = input.getNext();
    int quantity;
    try {
        quantity = std::stoi(quantityStr);
        if (quantity <= 0) throw Error("Invalid\n");
    } catch (...) {
        throw Error("Invalid\n");
    }

    std::vector<int> positions;
    BookISBN_pos.find_node(isbn, positions);
    if (positions.empty()) {
        throw Error("Invalid\n");
    }

    Book book;
    BookStorage.read(book, positions[0]);
    if (book.Quantity < quantity) {
        throw Error("Invalid\n");
    }

    book.Quantity -= quantity;
    BookStorage.update(book, positions[0]);

    double totalCost = book.Price * quantity;
    std::cout << std::fixed << std::setprecision(2) << totalCost << '\n';

    Log buyLog;
    buyLog.isIncome = true;
    buyLog.Amount = totalCost;
    buyLog.behavoir = ActionType::BUY;
    logs.AddLog(buyLog);
}

void BookManager::Select(Command &input, AccountManager &accounts, LogManager &logs) {
    if (accounts.getCurrentPrivilege() < 3 || input.count != 2) {
        throw Error("Invalid\n");
    }

    std::string isbn = input.getNext();
    if (!Validator::isValidISBN(isbn)) {
        throw Error("Invalid\n");
    }

    std::vector<int> positions;
    BookISBN_pos.find_node(isbn, positions);

    if (positions.empty()) {
        BookStorage.get_info(bookCount, 1);
        bookCount++;
        Book newBook(bookCount, isbn);

        int pos = BookStorage.write(newBook);
        if (pos <= 0) throw Error("Invalid\n");

        BookStorage.write_info(bookCount, 1);
        BookID_pos.insert_node(DataNode(std::to_string(bookCount), pos));
        BookISBN_pos.insert_node(DataNode(isbn, pos));

        accounts.selectBook(bookCount);
    } else {
        Book book;
        BookStorage.read(book, positions[0]);
        accounts.selectBook(book.book_id);
    }
}

void BookManager::Modify(Command &input, AccountManager &accounts, LogManager &logs) {
    validateSelected(accounts);  // 检查权限和是否已选中图书

    // 获取当前选中的图书
    int id = accounts.loginStack.back().selectedBookId;
    std::vector<int> positions;
    BookID_pos.find_node(std::to_string(id), positions);
    if (positions.empty()) {
        throw Error("Invalid\n");
    }

    Book oldBook, newBook;
    BookStorage.read(oldBook, positions[0]);
    newBook = oldBook;  // 复制原书信息

    // 记录已修改的字段
    bool modified_isbn = false;
    bool modified_name = false;
    bool modified_author = false;
    bool modified_keyword = false;
    bool modified_price = false;

    // 处理每个修改命令
    for (int i = 0; i < input.count - 1; ++i) {
        std::string command = input.getNext();
        if (command.empty()) {
            throw Error("Invalid\n");
        }

        try {
            if (command.substr(0,6) == "-ISBN=") {
                if (modified_isbn) throw Error("Invalid\n");
                if (command.length() <= 7 || command[6] != '\"' || command.back() != '\"') {
                    throw Error("Invalid\n");
                }
                std::string isbn = command.substr(7, command.length() - 8);
                if (!Validator::isValidISBN(isbn)) {
                    throw Error("Invalid\n");
                }

                // 检查新ISBN是否已存在或与原ISBN相同
                if (strcmp(isbn.c_str(), oldBook.ISBN.ISBN) == 0) {
                    throw Error("Invalid\n");
                }
                std::vector<int> check;
                BookISBN_pos.find_node(isbn, check);
                if (!check.empty()) {
                    throw Error("Invalid\n");
                }

                newBook.ISBN = BookISBN(isbn);
                modified_isbn = true;
            }
            else if (command.substr(0,6) == "-name=") {
                if (modified_name) throw Error("Invalid\n");
                if (command.length() <= 7 || command[6] != '\"' || command.back() != '\"') {
                    throw Error("Invalid\n");
                }
                std::string name = command.substr(7, command.length() - 8);
                if (!Validator::isValidBookString(name)) {
                    throw Error("Invalid\n");
                }

                newBook.book_name = BookName(name);
                modified_name = true;
            }
            else if (command.substr(0,8) == "-author=") {
                if (modified_author) throw Error("Invalid\n");
                if (command.length() <= 9 || command[8] != '\"' || command.back() != '\"') {
                    throw Error("Invalid\n");
                }
                std::string author = command.substr(9, command.length() - 10);
                if (!Validator::isValidBookString(author)) {
                    throw Error("Invalid\n");
                }

                newBook.author_name = AuthorName(author);
                modified_author = true;
            }
            else if (command.substr(0,9) == "-keyword=") {
                if (modified_keyword) throw Error("Invalid\n");
                if (command.length() <= 10 || command[9] != '\"' || command.back() != '\"') {
                    throw Error("Invalid\n");
                }
                std::string keywords = command.substr(10, command.length() - 11);
                if (!Validator::isValidKeyword(keywords)) {
                    throw Error("Invalid\n");
                }

                newBook.key_word = KeyWord(keywords);
                modified_keyword = true;
            }
            else if (command.substr(0,7) == "-price=") {
                if (modified_price) throw Error("Invalid\n");

                std::string price_str = command.substr(7);
                if (!Validator::isValidPrice(price_str)) {
                    throw Error("Invalid\n");
                }

                try {
                    double price = std::stod(price_str);
                    if (price < 0) throw Error("Invalid\n");
                    newBook.Price = price;
                    modified_price = true;
                } catch (...) {
                    throw Error("Invalid\n");
                }
            }
            else {
                throw Error("Invalid\n");
            }
        }
        catch (...) {
            throw Error("Invalid\n");
        }
    }

    // 如果有修改，更新图书信息和索引
    if (modified_isbn || modified_name || modified_author ||
        modified_keyword || modified_price) {
        // 更新索引
        if (modified_isbn) {
            BookISBN_pos.delete_node(DataNode(oldBook.ISBN.ISBN, positions[0]));
            BookISBN_pos.insert_node(DataNode(newBook.ISBN.ISBN, positions[0]));
        }
        if (modified_name) {
            BookName_pos.delete_node(DataNode(oldBook.book_name.Bookname, positions[0]));
            BookName_pos.insert_node(DataNode(newBook.book_name.Bookname, positions[0]));
        }
        if (modified_author) {
            Author_pos.delete_node(DataNode(oldBook.author_name.Author, positions[0]));
            Author_pos.insert_node(DataNode(newBook.author_name.Author, positions[0]));
        }
        if (modified_keyword) {
            // 删除旧关键词
            std::string old_keywords(oldBook.key_word.Keyword);
            std::istringstream old_ss(old_keywords);
            std::string old_keyword;
            while (std::getline(old_ss, old_keyword, '|')) {
                Keyword_pos.delete_node(DataNode(old_keyword, positions[0]));
            }

            // 添加新关键词
            std::string new_keywords(newBook.key_word.Keyword);
            std::istringstream new_ss(new_keywords);
            std::string new_keyword;
            while (std::getline(new_ss, new_keyword, '|')) {
                Keyword_pos.insert_node(DataNode(new_keyword, positions[0]));
            }
        }

        // 更新图书信息
        BookStorage.update(newBook, positions[0]);

        // 添加日志
        Log modifyLog;
        modifyLog.behavoir = ActionType::MODIFYBOOK;
        modifyLog.use = &accounts.loginStack.back().account;
        modifyLog.Amount = 0;
        logs.AddLog(modifyLog);
    }
}

void BookManager::ImportBook(Command &input, AccountManager &accounts, LogManager &logs) {
    if (accounts.getCurrentPrivilege() < 3 || input.count != 3) {
        throw Error("Invalid\n");
    }

    // 检查是否已选中图书
    int id = accounts.loginStack.back().selectedBookId;
    if (id == 0) {
        throw Error("Invalid\n");
    }

    // 验证输入
    std::string quantity_str = input.getNext();
    std::string total_cost_str = input.getNext();

    int quantity;
    double total_cost;

    try {
        quantity = std::stoi(quantity_str);
        if (quantity <= 0) throw Error("Invalid\n");

        total_cost = std::stod(total_cost_str);
        if (total_cost < 0) throw Error("Invalid\n");
    } catch (...) {
        throw Error("Invalid\n");
    }

    // 获取图书信息
    std::vector<int> positions;
    BookID_pos.find_node(std::to_string(id), positions);
    if (positions.empty()) {
        throw Error("Invalid\n");
    }

    // 更新库存和总成本
    Book book;
    BookStorage.read(book, positions[0]);
    book.Quantity += quantity;
    book.TotalCost += total_cost;
    BookStorage.update(book, positions[0]);

    // 添加日志
    Log importLog;
    importLog.behavoir = ActionType::IMPORTBOOK;
    importLog.use = &accounts.loginStack.back().account;
    importLog.isIncome = false;
    importLog.Amount = total_cost;
    logs.AddLog(importLog);
}