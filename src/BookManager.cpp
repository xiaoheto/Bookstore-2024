#include "BookManager.h"

bool isKwBnAnValid(std::string &input) {
    for (int i = 0;i < input.size(); ++i) {
        if (input[i] < 32 || input[i] > 126 || input[i] == '\"') {
            return false;
        }
    }
}

BookISBN::BookISBN(std::string isbn) {
    strcpy(ISBN,isbn.c_str());
}

bool BookISBN::operator==(const BookISBN &other)const {
    return strcmp(ISBN,other.ISBN) == 0;
}

bool BookISBN::operator<(const BookISBN &other)const {
    return strcmp(ISBN,other.ISBN) < 0;
}


BookName::BookName(std::string bookname) {
    strcpy(Bookname,bookname.c_str());
}

bool BookName::operator==(const BookName &other)const {
    return strcmp(Bookname,other.Bookname) == 0;
}

bool BookName::operator<(const BookName &other)const {
    return strcmp(Bookname,other.Bookname) < 0;
}

AuthorName::AuthorName(std::string authorname) {
    strcpy(Author,authorname.c_str());
}


bool AuthorName::operator==(const AuthorName &other)const {
    return strcmp(Author,other.Author) == 0;
}

bool AuthorName::operator<(const AuthorName &other)const {
    return strcmp(Author,other.Author) < 0;
}

KeyWord::KeyWord(std::string keyword) {
    strcpy(Keyword,keyword.c_str());
}

bool KeyWord::operator==(const KeyWord &other)const {
    return strcmp(Keyword,other.Keyword) == 0;
}

bool KeyWord::operator<(const KeyWord &other)const {
    return strcmp(Keyword,other.Keyword) < 0;
}

Book::Book(int id, std::string &isbn, std::string bookName, std::string authorName, std::string keyWord, int quantity, int price, int totalcost)
    :book_id(id),ISBN(isbn),book_name(bookName),author_name(authorName),key_word(keyWord),Quantity(quantity),Price(price),TotalCost(totalcost) {}

Book::Book(int id, std::string isbn) {
    book_id = id;
    ISBN = isbn;
}

bool Book::operator<(const Book &rhs) const {
    return ISBN < rhs.ISBN;
}

bool Book::operator>(const Book &rhs) const {
    return rhs.ISBN < ISBN;
}

bool Book::operator<=(const Book &rhs) const {
    return ISBN == rhs.ISBN || ISBN < rhs.ISBN;
}

bool Book::operator>=(const Book &rhs) const {
    return rhs.ISBN < ISBN || ISBN == rhs.ISBN;
}

std::ostream &operator<<(std::ostream &out, const Book &book) {
    out << book.ISBN.ISBN << '\t' << book.book_name.Bookname << '\t' <<
        book.author_name.Author << '\t' << book.key_word.Keyword << '\t' <<
            book.Price << '\t' << book.Quantity << '\n';
}

BookManager::BookManager() {
    BookStorage.initialise("book_data");
    BookID_pos.init("book_id_to_pos");
    BookISBN_pos.init("isbn_to_pos");
    BookName_pos.init("name_toAuthorName(std::string author);_pos");
    Author_pos.init("author_to_pos");
    Keyword_pos.init("keyword_to_pos");
}

void BookManager::Show(Command &input, AccountManager &account, LogManager &log) {
    if (input.count != 2 && input.count != 1) {
        throw Error("Invalid\n");
    }
    std::string temp = input.getNext();
    std::vector<int>result;
    if (temp.empty()) {
        BookISBN_pos.find_all(result);
        //按照ISBN升序输出
        if (result.empty()) {
            std::cout << '\n';
        }
        else {
            for (int i = 0; i < result.size(); ++i) {
                Book temp;
                BookStorage.read(temp, result[i]);
                std::cout << temp;
            }
        }
    }
    else if (temp.substr(0,6) == "-ISBN=") {
        std::string tmp = temp.substr(6,temp.length() - 6);
        BookISBN_pos.find_node(tmp,result);
        Book searchedBook;
        if (result.empty()) {
            std::cout << '\n';
        }
        else {
            for(int i = 0;i < result.size();++i) {
                BookStorage.read(searchedBook,result[i]);
                std::cout << searchedBook;
            }
        }
    }
    else if (temp.substr(0,6) == "-name=") {
        std::string tmp = temp.substr(7,temp.length() - 8);
        BookID_pos.find_node(tmp,result);
        Book searchedBook;
        if (result.empty()) {
            std::cout << '\n';
        }
        else {
            for(int i = 0;i < result.size();++i) {
                BookStorage.read(searchedBook,result[i]);
                std::cout << searchedBook;
            }
        }
    }
    else if (temp.substr(0,8) == "-author=") {
        std::string tmp = temp.substr(9,temp.length() - 10);
        Author_pos.find_node(tmp,result);
        Book searchedBook;
        if (result.empty()) {
            std::cout << '\n';
        }
        else {
            for(int i = 0;i < result.size();++i) {
                BookStorage.read(searchedBook,result[i]);
                std::cout << searchedBook;
            }
        }
    }
    else if (temp.substr(0, 9) == "-keyword=" ) {
        std::string _keyword = "";
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
        if (_keyword.empty() || _keyword.length() > 60) {
            throw Error("Invalid\n");
        }
        else {
            Keyword_pos.find_node(_keyword, result);
            if (!result.empty()) {
                std::priority_queue<Book,std::vector<Book>,std::greater<Book> > output;
                for (int i = 0; i < result.size(); ++i) {
                    Book temp;
                    BookStorage.read(temp, result[i]);
                    output.push(temp);
                }
                while (!output.empty()) {
                    std::cout << output.top();
                    output.pop();
                }
            }
            else {
                std::cout << '\n';
            }
        }
    }
}

void BookManager::Buy(Command &input, AccountManager &accounts, LogManager &logs) {
    if (input.count != 3) {
        throw Error("Invalid\n");
    }
    std::string buyISBN = input.getNext();
    int buyQuantity = std::stoi(input.getNext());
    std::vector<int>result;
    BookISBN_pos.find_node(buyISBN,result);
    if (result.empty() || buyQuantity <= 0 || buyQuantity > 2147483647) {
        throw Error("Invalid\n");
    }
    else {
        Book temp;
        BookStorage.read(temp,result[0]);
        if (temp.Quantity < buyQuantity) {
            throw Error("Invalid\n");
        }
        temp.Quantity -= buyQuantity;
        BookStorage.update(temp,result[0]);
        std::cout << std::fixed << std::setprecision(2) << buyQuantity * temp.Price << '\n';
        Log tp_log;
        tp_log.isIncome = true;
        tp_log.Amount = (double) temp.Price * buyQuantity;
        logs.AddLog(tp_log);
    }
}

void BookManager::Select(Command &input, AccountManager &accounts, LogManager &logs) {
    if (input.count != 2 || accounts.getCurrentPrivilege() < 3) {
        throw Error("invalid\n");
    }
    std::string temp = input.getNext();
    std::vector<int>result;
    BookISBN_pos.find_node(temp,result);
    if (result.empty()) {
        BookStorage.get_info(bookCount, 1);
        bookCount++;
        Book book(bookCount, temp);
        int pos = BookStorage.write(book);
        BookID_pos.insert_node(DataNode(std::to_string(bookCount), pos));
        BookISBN_pos.insert_node(DataNode(temp, pos));
        accounts.selectBook(book.book_id);
        return;
    }
    else {
        Book book;
        BookStorage.read(book,result[0]);
        accounts.selectBook(book.book_id);
    }
}

void BookManager::Modify(Command &input, AccountManager &accounts, LogManager &logs) {
    if (accounts.getCurrentPrivilege() < 3) {
        throw Error("Invalid\n");
    }
    std::string ISBN_ = "";
    std::string name_ = "";
    std::string author_ = "";
    std::set<std::string> keyword_;
    int price_ = 0;
    int id_ = accounts.LogInStorage.back().selectBookId;
    std::string idd = std::to_string(id_);
    if (id_ == 0) {
        throw Error("Invalid\n");
    }
    Book book;
    std::vector<int> resultt;
    std::string s;
    BookID_pos.find_node(idd,resultt);
    BookStorage.read(book,resultt[0]);
    for(int i = 0;i < input.count - 1;++i) {
        std::string command = input.getNext();
        if (i == 0 && command.empty()) {
            throw Error("Invalid\n");
        }
        std::vector<int>commandStorage;
        if (command.substr(0,6) == "-ISBN=") {
            if (std::find(commandStorage.begin(),commandStorage.end(),1) != commandStorage.end()) {
                throw Error("Invalid\n");
            }
            ISBN_ = command.substr(7,command.length() - 8);
            if (strcmp(ISBN_.c_str(),book.ISBN.ISBN) == 0) {
                throw Error("Invalid\n");
            }//与原来的ISBN相同则无效
            strcpy(book.ISBN.ISBN,ISBN_.c_str());
            commandStorage.push_back(1);
        }
        if (command.substr(0,6) == "-name=") {
            if (std::find(commandStorage.begin(),commandStorage.end(),2) != commandStorage.end()) {
                throw Error("Invalid\n");
            }
            name_ = command.substr(7,command.length() - 8);
            strcpy(book.book_name.Bookname,name_.c_str());
            commandStorage.push_back(2);
        }
        if (command.substr(0,8) == "author") {
            if (std::find(commandStorage.begin(),commandStorage.end(),3) != commandStorage.end()) {
                throw Error("Invalid\n");
            }
            author_ = command.substr(9,command.length() - 10);
            strcpy(book.author_name.Author,author_.c_str());
            commandStorage.push_back(3);
        }
        if (command.substr(0,9) == "-keyword=") {
            if (std::find(commandStorage.begin(),commandStorage.end(),4) != commandStorage.end()) {
                throw Error("Invalid\n");
            }
            s = command.substr(10, command.length() - 11);
            if (s.empty() || s.length() > 60) {
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

            Command new_key(s, '|'), old_key(book.key_word.Keyword, '|');
            //old是旧的keyword
            std::string ns = new_key.getNext(), os = old_key.getNext();

            while (!ns.empty()) {
                //判断ns中的关键词是否重复,重复则不合法
                if (std::find(keyword_.begin(),keyword_.end(),ns) != keyword_.end()) {
                    throw Error("Invalid\n");
                }
                keyword_.insert(ns);
                ns = new_key.getNext();
            }
        }
        if (command.substr(0,7) == "-price=") {
            if (std::find(commandStorage.begin(),commandStorage.end(),5) != commandStorage.end()) {
                throw Error("Invalid\n");
            }
            price_ = std::stoi(command.substr(7,command.length() - 7));
            book.Price = price_;
            commandStorage.push_back(5);
        }
    }


    if (! ISBN_.empty()) {
        BookISBN_pos.delete_node(DataNode(book.ISBN.ISBN,resultt[0]));
        BookISBN_pos.insert_node(DataNode(ISBN_,resultt[0]));
    }
    if (! name_.empty()) {
        BookName_pos.delete_node(DataNode(book.book_name.Bookname,resultt[0]));
        BookName_pos.insert_node(DataNode(name_,resultt[0]));
    }
    if (! author_.empty()) {
        Author_pos.delete_node(DataNode(book.author_name.Author,resultt[0]));
        Author_pos.insert_node(DataNode(author_,resultt[0]));
    }
    if (!keyword_.empty()) {
        Command new_key(s, '|'), old_key(book.key_word.Keyword, '|');
        std::string ns = new_key.getNext(), os = old_key.getNext();
        //暴力删除,之后重新插入
        while (!os.empty()) {
            Keyword_pos.delete_node(DataNode(os, resultt[0]));
            os = old_key.getNext();
        }
        while (!ns.empty()) {
            Keyword_pos.insert_node(DataNode(ns, resultt[0]));
            ns = new_key.getNext();
        }
        strcpy(book.key_word.Keyword, s.c_str());
    }
    if (price_ != 0) {
        book.Price = price_;
    }
}
void BookManager::ImportBook(Command &input, AccountManager &accounts, LogManager &logs) {
    if (accounts.getCurrentPrivilege() < 3 || input.count != 3) {
        throw Error("Invalid\n");
    }
    int importQuantity = stoi(input.getNext());
    double TotalCoast = stoi(input.getNext());
    if (importQuantity < 0 || TotalCoast < 0.0) {
        throw Error("Invalid\n");
    }
    int id = accounts.LogInStorage.back().selectBookId;
    if (id == 0) {
        throw Error("Invalid\n");
    }
    Book book;
    std::vector<int> result;
    std::string id_ = std::to_string(id);
    BookID_pos.find_node(id_,result);
    BookStorage.read(book,result[0]);
    book.Quantity += importQuantity;
    book.TotalCost += TotalCoast;

    Log tp_log;
    tp_log.isIncome = false;
    tp_log.Amount = TotalCoast;
    logs.AddLog(tp_log);

    BookStorage.update(book, result[0]);
}
