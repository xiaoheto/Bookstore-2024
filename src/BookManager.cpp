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

KeyWord::KeyWord(const std::string &key_word) {
    if (!Validator::isValidKeyword(key_word)) {
        throw Error("Invalid\n");
    }
    strcpy(Keyword,key_word.c_str());
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
    BookStorage.initialise("BookStorage");
    BookID_pos.init("book_BookID_pos");
    BookISBN_pos.init("BookISBN_pos");
    BookName_pos.init("BookName_pos");
    Author_pos.init("Author_pos");
    Keyword_pos.init("Keyword_pos");

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

void BookManager::Show(Command &line, AccountManager &accounts, LogManager &logs) {
    if (accounts.getCurrentPrivilege() < 1 ||
        (line.count != 2 && line.count != 1)) {
        throw Error("Invalid\n");
    }

    string temp = line.getNext();
    string _name, _isbn, _author, _keyword;
    vector<int> ans;

    //没有参数,说明全部都要输出
    if (temp.empty()) {
        BookISBN_pos.find_all(ans);
        //按照ISBN升序输出
        if (!ans.empty()) {
            for (int i = 0; i < ans.size(); ++i) {
                Book temp;
                BookStorage.read(temp, ans[i]);
                cout << temp;
            }
        } else {
            cout << '\n';
        }
    }
    else//否则开始匹配
    if (temp.substr(0, 6) == "-name=" &&
        temp[6] == '\"' && temp.back() == '\"') {
        _name = temp.substr(7, temp.length() - 8);
        if (_name.empty() || !Validator::isValidBookString(_name) || _name.length() > 60) {
            throw Error("Invalid\n");
        } else {
            BookName_pos.find_node(_name, ans);
            if (!ans.empty()) {
                priority_queue<Book,vector<Book>,greater<Book> > output;
                for (int i = 0; i < ans.size(); ++i) {
                    Book temp;
                    BookStorage.read(temp, ans[i]);
                    output.push(temp);
                }
                while (!output.empty()) {
                    cout << output.top();
                    output.pop();
                }
            } else {
                cout << '\n';
            }
        }
    }
    else
    if (temp.substr(0, 9) == "-keyword=" &&
        temp[9] == '\"' && temp.back() == '\"') {
        for (int i = 10; i < temp.length() - 1; ++i) {
            if (temp[i] == '|') {
                throw Error("Invalid\n");
            }//判断 | ,此处只会有一个关键词
            _keyword += temp[i];
        }
        //开头结尾
        if (_keyword[0] == '|' || _keyword.back() == '|') {
            throw Error("Invalid\n");
        }
        if (_keyword.empty() || !Validator::isValidKeyword(_keyword) || _keyword.length() > 60) {
            throw Error("Invalid\n");
        } else {
            Keyword_pos.find_node(_keyword, ans);
            if (!ans.empty()) {
                priority_queue<Book,vector<Book>,greater<Book> > output;
                for (int i = 0; i < ans.size(); ++i) {
                    Book temp;
                    BookStorage.read(temp, ans[i]);
                    output.push(temp);
                }
                while (!output.empty()) {
                    cout << output.top();
                    output.pop();
                }
            } else {
                cout << '\n';
            }
        }
    }
    else
    if (temp.substr(0, 6) == "-ISBN=") {
        _isbn = temp.substr(6, temp.length() - 6);
        if (_isbn.empty() || !Validator::isValidISBN(_isbn) || _isbn.length() > 20) {
            throw Error("Invalid\n");
        } else {
            BookISBN_pos.find_node(_isbn, ans);
            if (!ans.empty()) {
                for (int i = 0; i < ans.size(); ++i) {
                    Book temp;
                    BookStorage.read(temp, ans[i]);
                    cout << temp;
                }
            } else {
                cout << '\n';
            }
        }
    }
    else
    if (temp.substr(0,8) == "-author=" &&
        temp[8] == '\"' && temp.back() == '\"') {
        _author = temp.substr(9, temp.length() - 10);
        if (_author.empty() || !Validator::isValidBookString(_author) || _author.length() > 60) {
            throw Error("Invalid\n");
        } else {
            Author_pos.find_node(_author, ans);
            if (!ans.empty()) {
                priority_queue<Book,vector<Book>,greater<Book> > output;
                for (int i = 0; i < ans.size(); ++i) {
                    Book temp;
                    BookStorage.read(temp, ans[i]);
                    output.push(temp);
                }
                while (!output.empty()) {
                    cout << output.top();
                    output.pop();
                }
            } else {
                cout << '\n';
            }
        }
    }
    else//否则就是不合法
        throw Error("Invalid\n");
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

void BookManager::Modify(Command &line, AccountManager &accounts, LogManager &logs) {
    if (accounts.getCurrentPrivilege() < 3 || line.count <= 1) {
        throw Error("Invalid\n");
    }

    string temp_command = line.getNext();
    string _name, _isbn, _author, _key_word, _price;

    //没有参数,说明没有选中书
    if (temp_command.empty() ||
        !accounts.loginStack.back().selectedBookId) {
        throw Error("Invalid\n");
    }

    int _id = accounts.loginStack.back().selectedBookId;
    vector<int> ans;
    BookID_pos.find_node(to_string(_id), ans);
    Book new_book;
    BookStorage.read(new_book, ans[0]);
    string s,ns,os;
    set<string> tong;


    while (!temp_command.empty()) {
        if (temp_command.substr(0, 6) == "-name=" &&
            temp_command[6] == '\"' && temp_command.back() == '\"') {
            if (_name.empty()) {
                _name = temp_command.substr(7, temp_command.length() - 8);
                if (_name.empty() || !Validator::isValidBookString(_name) || _name.length() > 60) {
                    throw Error("Invalid\n");
                }
            }
            else { //重复指定参数
                throw Error("Invalid\n");
            }
        }
        else
        if (temp_command.substr(0, 9) == "-key_word=" &&
            temp_command[9] == '\"' && temp_command.back() == '\"') {
            //更改分隔符
            s = temp_command.substr(10, temp_command.length() - 11);
            if (s.empty() || !Validator::isValidBookString(s) || s.length() > 60) {
                throw Error("Invalid\n");
            }
            //判断是否有连续多个|出现
            for (int i = 1;i < s.length(); ++i) {
                if (s[i] == '|' && s[i - 1] == '|')
                    throw Error("Invalid\n");
            }
            //判断开头结尾是否有 '|'
            if (s[0] == '|' || s.back() == '|')
                throw Error("Invalid\n");

            Command new_key(s, '|'), old_key(new_book.key_word.Keyword, '|');
            //old是旧的key_word
            ns = new_key.getNext(), os = old_key.getNext();

            while (!ns.empty()) {
                //判断ns中的关键词是否重复,重复则不合法
                if (tong.find(ns) != tong.end()) {
                    throw Error("Invalid\n");
                }
                tong.insert(ns);
                ns = new_key.getNext();
            }
        }
        else
        if (temp_command.substr(0, 6) == "-ISBN=") {
            if (_isbn.empty()) {
                _isbn = temp_command.substr(6, temp_command.length() - 6);
                if (_isbn.empty() || !Validator::isValidISBN(_isbn) || _isbn.length() > 20) {
                    throw Error("Invalid\n");
                }
                //不能修改相同的isbn
                if (strcmp(new_book.ISBN.ISBN, _isbn.c_str()) == 0) {
                    throw Error("Invalid\n");
                }
                //不能有两本ISBN相同的书
                vector<int> tp_ans;
                BookISBN_pos.find_node(_isbn, tp_ans);
                if (!tp_ans.empty()) {
                    throw Error("Invalid\n");
                }
            } else {
                throw Error("Invalid\n");
            }
        }
        else
        if (temp_command.substr(0, 8) == "-author=" &&
            temp_command[8] == '\"' && temp_command.back() == '\"') {
            if (_author.empty()) {
                _author = temp_command.substr(9, temp_command.length() - 10);
                if (_author.empty() || !Validator::isValidBookString(_author) || _author.length() > 60) {
                    throw Error("Invalid\n");
                }
            } else {
                throw Error("Invalid\n");
            }
        }
        else
        if (temp_command.substr(0, 7) == "-price=") {
            if (_price.empty()) {
                _price = temp_command.substr(7, temp_command.length() - 7);
                //没有映射关系,不用修改
                if (_price.empty()  || _price.length() > 13 || stod(_price) < 0) {
                    throw Error("Invalid\n");
                }
            } else {
                throw Error("Invalid\n");
            }
        }
        else {
            throw Error("Invalid\n");
        }

        temp_command = line.getNext();
    }

    if (!Validator::isValidISBN(_isbn) || _isbn.length() > 20 ||
        !Validator::isValidBookString(_name) || _name.length() > 60 ||
        !Validator::isValidBookString(_author) || _author.length() > 60 ||
        !Validator::isValidBookString(_key_word) || _key_word.length() > 60 ||
        _price.length() > 13 ) {
        throw Error("Invalid\n");
    }
    //合法，则更新数据
    if (!_name.empty()) {
        BookName_pos.delete_node(DataNode(new_book.book_name.Bookname, ans[0]));
        BookName_pos.insert_node(DataNode(_name, ans[0]));
        strcpy(new_book.book_name.Bookname, _name.c_str());
    }
    if (!_author.empty()) {
        Author_pos.delete_node(DataNode(new_book.author_name.Author, ans[0]));
        Author_pos.insert_node(DataNode(_author, ans[0]));
        strcpy(new_book.author_name.Author, _author.c_str());
    }
    if (!_isbn.empty()) {
        BookISBN_pos.delete_node(DataNode(new_book.ISBN.ISBN, ans[0]));
        BookISBN_pos.insert_node(DataNode(_isbn, ans[0]));
        strcpy(new_book.ISBN.ISBN, _isbn.c_str());
    }
    if (!_price.empty()) {
        new_book.Price = stod(_price);
    }

    if (!tong.empty()) {
        Command new_key(s, '|'), old_key(new_book.key_word.Keyword, '|');
        ns = new_key.getNext(), os = old_key.getNext();
        //暴力删除,之后重新插入
        while (!os.empty()) {
            Keyword_pos.delete_node(DataNode(os, ans[0]));
            os = old_key.getNext();
        }
        while (!ns.empty()) {
            Keyword_pos.insert_node(DataNode(ns, ans[0]));
            ns = new_key.getNext();
        }
        strcpy(new_book.key_word.Keyword, s.c_str());
    }
        BookStorage.update(new_book, ans[0]);
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

    // 使用 Validator 进行验证
    if (!Validator::isValidQuantity(quantity_str) || !Validator::isValidPrice(total_cost_str)) {
        throw Error("Invalid\n");
    }

    int quantity = std::stoi(quantity_str);
    double total_cost = std::stod(total_cost_str);

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
    importLog.use = nullptr;
    importLog.isIncome = false;
    importLog.Amount = total_cost;
    snprintf(importLog.description, sizeof(importLog.description),
             "Import %d copies of book (ISBN: %s) with total cost %.2f",
             quantity, book.ISBN.ISBN, total_cost);
    logs.AddLog(importLog);
}
