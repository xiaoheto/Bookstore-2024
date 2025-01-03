#ifndef BOOKSTORE_MEMORYRIVER_H
#define BOOKSTORE_MEMORYRIVER_H

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);

public:
    MemoryRiver() = default;

    MemoryRiver(const string &file_name) : file_name(file_name) { initialise(); }

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        std::remove(file_name.c_str());
        file.open(file_name, std::fstream::in | std::fstream::out);
        if (!file) {
            file.open(file_name, std::ofstream::out);
            //新建文件
            int tmp = 0;
            for (int i = 0; i < info_len; ++i)
                file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        }
        file.close();
    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        file.open(file_name);
        file.seekg((n - 1) * sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        return;
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.open(file_name);
        file.seekp((n - 1) * sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        int index;
        int num_T, del_head;//临时声明,从文件中赋值,最后写回文件中
        get_info(num_T, 1);
        get_info(del_head, 2);

        file.open(file_name);
        if (!del_head) {//没有删除过,在文件尾部加
            index = info_len * sizeof(int) + num_T * sizeofT;
            //del_head还是0(相当于上一个节点的指针)
        } else {//在释放掉的空间加
            index = del_head;
            file.seekg(del_head);
            file.read(reinterpret_cast<char *>(&del_head), sizeof(int));
        }
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&t), sizeofT);
        file.close();

        //num_T 更新
        num_T++;
        write_info(num_T, 1);
        write_info(del_head, 2);//更新 del_head 的值
        return index;
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    bool update(T &t, const int index) {
        file.open(file_name);
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&t), sizeofT);
        bool success = file.good();
        file.close();
        return success;
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    bool read(T &t, const int index) {
        file.open(file_name);
        file.seekg(index);
        file.read(reinterpret_cast<char *>(&t), sizeofT);
        bool success = file.good();
        file.close();
        return success;
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        int del_head;
        get_info(del_head, 2);

        file.open(file_name);
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&del_head), sizeof(int));
        del_head = index;
        file.close();

        write_info(del_head, 2);
    }
    void clear() {
        file.open(file_name);
        file.clear();
        file.close();
    }
};

#endif //BOOKSTORE_MEMORYRIVER_H
