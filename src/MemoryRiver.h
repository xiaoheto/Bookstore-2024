#ifndef MEMORYRIVER_H
#define MEMORYRIVER_H

#include <fstream>
#include <string>
#include <unistd.h>
#include "error.h"

template<class T>
class MemoryRiver {
private:
   std::fstream file;
   std::string file_name;
   int sizeofT = sizeof(T);

   void move_read_pointer(int pos) {
       file.seekg(pos, std::ios::beg);
   }

   void move_write_pointer(int pos) {
       file.seekp(pos, std::ios::beg);
   }

public:
   MemoryRiver() = default;

   explicit MemoryRiver(const std::string &file_name) : file_name(file_name) {}

    void initialise(std::string FN = "") {
       try {
           if (!FN.empty()) file_name = FN;

           // 首先检查文件是否可访问

           // 尝试以只读方式打开检查文件是否存在
           std::ifstream test(file_name);
           if (test.is_open()) {
               test.close();
           }

           // 尝试打开文件
           file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
           if (!file.is_open()) {
               // 文件不存在，创建新文件
               file.clear();
               file.open(file_name, std::ios::out | std::ios::binary);
               if (!file.is_open()) {
                   char cwd[1024];
                   if (getcwd(cwd, sizeof(cwd)) != NULL) {
                       std::cerr << cwd << std::endl;
                   }
                   throw Error("Failed to open file\n");
               }
           }

           // 初始化文件头
           int tmp = 0;
           file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
           if (!file.good()) {
               throw Error("Failed to write file\n");
           }
           file.flush();  // 确保数据写入磁盘
           file.close();
       } catch (const std::exception& e) {
           throw;
       }
   }

   void get_info(int &tmp, const int n) {
       file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
       move_read_pointer((n - 1) * sizeof(int));
       file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
       file.close();
   }

   void write_info(const int tmp, const int n) {
       file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
       move_write_pointer((n - 1) * sizeof(int));
       file.write(reinterpret_cast<const char *>(&tmp), sizeof(int));
       file.close();
   }

    int write(const T &t) {
       file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
       file.seekp(0, std::ios::end);  // 移动到文件末尾
       int index = file.tellp();
       file.write(reinterpret_cast<const char*>(&t), sizeofT);
       bool success = file.good();
       file.close();
       return success ? index : -1;
   }

   bool update(const T &t, const int index) {
       file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
       move_write_pointer(index);
       file.write(reinterpret_cast<const char*>(&t), sizeofT);
       bool success = file.good();
       file.close();
       return success;
   }

   bool read(T &t, const int index) {
       file.open(file_name, std::ios::in | std::ios::binary);
       move_read_pointer(index);
       file.read(reinterpret_cast<char*>(&t), sizeofT);
       bool success = file.good();
       file.close();
       return success;
   }

   void Delete(int index) {
       T tmp{};
       update(tmp, index);
   }
};

#endif //MEMORYRIVER_H