//
// Created by 43741 on 2024/12/18.
//

#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <fstream>
using namespace std;
class MamoryPool {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
    void move_read_pointer(int pos) {
        file.seekg(pos,std::ios::beg);
    }
    void move_write_pointer(int pos) {
        file.seekp(pos,std::ios::beg);
    }
public:
    MemoryPool() = default;

    MemoryPool(const string& file_name) : file_name(file_name) {}

     void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        file.open(file_name,std::ios::in | std::ios::out | std::ios::binary);
        move_read_pointer((n-1) * sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp),sizeof(int));
        file.close();
    }

    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.open(file_name,std::ios::in | std::ios::out | std::ios::binary);
        move_write_pointer((n-1) * sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp),sizeof(int));
        file.close();
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(0, std::ios::end);
        int index = file.tellp();
        file.write(reinterpret_cast<char*>(&t), sizeofT);
        file.close();
        return index;
        /* your code here */
    }

    void update(T &t, const int index) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        move_write_pointer(index);
        file.write(reinterpret_cast<char*>(&t), sizeofT);
        file.close();
    }

    void read(T &t, const int index) {
        file.open(file_name, std::ios::in | std::ios::binary);
        move_read_pointer(index);
        file.read(reinterpret_cast<char*>(&t), sizeofT);
        file.close();
    }

    void Delete(int index) {
        T tmp{};
        update(tmp, index);
    }
};
#endif //MEMORYPOOL_H
