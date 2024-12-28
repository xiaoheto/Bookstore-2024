#ifndef MEMORYRIVER_H
#define MEMORYRIVER_H

#include <fstream>
#include <string>
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
       if (!FN.empty()) file_name = FN;
       file.open(file_name, std::ios::out | std::ios::binary);
       if (!file.is_open()) {
           throw Error("Failed to open file\n");
       }
       int tmp = 0;
       file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
       file.close();
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
       file.seekp(0, std::ios::end);
       int index = file.tellp();
       file.write(reinterpret_cast<const char*>(&t), sizeofT);
       file.close();
       return index;
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