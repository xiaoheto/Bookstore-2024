#ifndef BLOCKLINK_H
#define BLOCKLINK_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include "MemoryRiver.h"
#include "error.h"

constexpr int BLOCK_SIZE = 1000;
constexpr int SPLIT_SIZE = 2000;//块的大小的两倍
constexpr int MERGE_SIZE = 500;//块的大小的一半

class DataNode {
private:
   int pos;
   char str_value[64];//储存在文件当中必须使用char数组，如果传入了string需要转换
   int num_value;
   friend class DataBlock;
   friend class DataFile;
public:
   DataNode() = default;

   DataNode(const std::string &str_, int value, int num_ = 0) {
       if (str_.length() >= 64) throw Error("Invalid\n");
       std::strncpy(str_value, str_.c_str(), 63);
       str_value[63] = '\0';
       num_value = value;
       pos = num_;
   }

   bool operator<(const DataNode &other) const {
       if (strcmp(str_value, other.str_value) == 0) {
           return num_value < other.num_value;
       }
       else {
           return strcmp(str_value, other.str_value) < 0;
       }
   }

   bool operator>(const DataNode &other) const {
       if (strcmp(str_value, other.str_value) == 0) {
           return num_value > other.num_value;
       }
       else {
           return strcmp(str_value, other.str_value) > 0;
       }
   }

   bool operator>=(const DataNode &other) const {
       return !(*this < other);
   }

   bool operator<=(const DataNode &other) const {
       return !(*this > other);
   }

   bool operator==(const DataNode &other) const {
       return (strcmp(str_value, other.str_value) == 0) && (num_value == other.num_value);
   }

   bool operator!=(const DataNode &other) const {
       return !(*this == other);
   }

   DataNode &operator=(const DataNode &right) {
       if (*this == right) {
           return *this;
       }
       strcpy(str_value, right.str_value);
       num_value = right.num_value;
       pos = right.pos;
       return *this;
   }
};

class DataBlock {
private:
   int nextPos, prePos, curPos, size;//分别为下一个、上一个块的位置，当前快的位置，块的大小
   DataNode block[2100];//中间存储了2100个DataNode（BLOCK_SIZE的两倍多一点）
   friend class DataFile;
public:
   DataBlock() : nextPos(0), prePos(0), curPos(0), size(0) {}

   DataBlock(int size_, DataBlock *array) : size(size_) {
       memcpy(block, array, size_ * sizeof(DataNode));
   }

   DataNode getFirst() const {
       if (size == 0) throw Error("Invalid\n");
       return block[0];
   }

   DataNode getLast() const {
       return size > 0 ? block[size - 1] : block[0];
   }

   bool insertNode(const DataNode &node) {
       if (size >= 2100) return false;

       int position = std::lower_bound(block, block + size, node) - block;
       if (position < size && block[position] == node) {
           return false;
       }
       for (int i = size; i > position; --i) {
           block[i] = block[i - 1];
       }
       block[position] = node;
       size++;
       return size >= SPLIT_SIZE;
   }

   void deleteNode(const DataNode &node) {
       int position = std::lower_bound(block, block + size, node) - block;
       if (position == size || block[position] != node) {
           return;
       }
       for (int i = position; i < size - 1; ++i) {
           block[i] = block[i + 1];
       }
       size--;
   }

   void searchByKey(const std::string &key, std::vector<int> &result) {
       for (int i = 0; i < size; ++i) {
           if (strcmp(block[i].str_value, key.c_str()) == 0) {
               result.push_back(block[i].num_value);
           }
       }
   }
};

class DataFile {
private:
   std::string fileName;
   int blockCount, beg;//块的个数以及头指针
   MemoryRiver<DataBlock> blockStorage;

   bool splitBlock(int blockPos) {
       DataBlock oldBlock, newBlock;
       if (!blockStorage.read(oldBlock, blockPos)) {
           return false;
       }

       int len = oldBlock.size / 2;
       newBlock.size = oldBlock.size - len;
       memcpy(newBlock.block, oldBlock.block + len, newBlock.size * sizeof(DataNode));
       oldBlock.size = len;

       newBlock.prePos = oldBlock.curPos;
       newBlock.nextPos = oldBlock.nextPos;
       newBlock.curPos = blockStorage.write(newBlock);
       oldBlock.nextPos = newBlock.curPos;

       if (!blockStorage.update(oldBlock, oldBlock.curPos) ||
           !blockStorage.update(newBlock, newBlock.curPos)) {
           return false;
       }

       blockCount++;
       blockStorage.write_info(blockCount, 1);
       return true;
   }

   bool mergeBlock(const int &pos) {
       if (pos == 8) {
           return true;
       }

       DataBlock cur_block, pre_block, nextPos_block;
       if (!blockStorage.read(cur_block, pos) ||
           !blockStorage.read(pre_block, cur_block.prePos)) {
           return false;
       }

       if (cur_block.nextPos) {
           if (!blockStorage.read(nextPos_block, cur_block.nextPos)) {
               return false;
           }
           nextPos_block.prePos = pre_block.curPos;
           blockStorage.update(nextPos_block, nextPos_block.curPos);
       }

       pre_block.nextPos = cur_block.nextPos;
       memcpy(pre_block.block + pre_block.size,
              cur_block.block,
              cur_block.size * sizeof(DataNode));
       pre_block.size += cur_block.size;

       if (!blockStorage.update(pre_block, pre_block.curPos)) {
           return false;
       }

       blockStorage.Delete(cur_block.curPos);
       blockCount--;
       blockStorage.write_info(blockCount, 1);

       if (pre_block.size >= SPLIT_SIZE) {
           return splitBlock(pre_block.curPos);
       }
       return true;
   }

public:
   DataFile() : blockCount(0), beg(8) {}

   DataFile(const std::string &file_name) : blockCount(0), beg(8) {
       blockStorage.initialise(file_name);
   }

   void init(const std::string &file_name) {
       blockStorage.initialise(file_name);
       blockCount = 0;
       beg = 8;
       blockStorage.write_info(blockCount, 1);
   }

   ~DataFile() = default;

   void find_node(const std::string &key, std::vector<int> &result) {
       if (blockCount == 0) return;

       DataBlock cur_block;
       blockStorage.get_info(blockCount, 1);

       for (int i = beg; i; i = cur_block.nextPos) {
           if (!blockStorage.read(cur_block, i)) break;

           if (strcmp(key.c_str(), cur_block.getFirst().str_value) >= 0 &&
               strcmp(key.c_str(), cur_block.getLast().str_value) <= 0) {
               cur_block.searchByKey(key, result);
           }
           if (cur_block.nextPos == 0) break;
       }
   }

   bool insert_node(const DataNode &node) {
       blockStorage.get_info(blockCount, 1);

       if (blockCount == 0) {
           DataBlock newBlock;
           newBlock.size = 1;
           newBlock.block[0] = node;
           newBlock.curPos = blockStorage.write(newBlock);
           if (newBlock.curPos <= 0) return false;

           blockCount++;
           blockStorage.write_info(blockCount, 1);
           return blockStorage.update(newBlock, newBlock.curPos);
       }
       else {
           DataBlock block;
           for (int i = beg;; i = block.nextPos) {
               if (!blockStorage.read(block, i)) return false;

               if (node < block.getLast() || block.nextPos == 0) {
                   bool needSplit = block.insertNode(node);
                   if (!blockStorage.update(block, i)) return false;
                   if (needSplit) {
                       return splitBlock(i);
                   }
                   return true;
               }
           }
       }
   }

   void delete_node(const DataNode &node) {
       if (blockCount == 0) return;

       blockStorage.get_info(blockCount, 1);
       DataBlock tp_block;

       for (int i = beg; i; i = tp_block.nextPos) {
           if (!blockStorage.read(tp_block, i)) break;

           if (node < tp_block.getFirst() || node > tp_block.getLast()) {
               continue;
           }

           tp_block.deleteNode(node);
           blockStorage.update(tp_block, i);

           if (tp_block.size <= MERGE_SIZE) {
               mergeBlock(i);
           }

           if (node > tp_block.getLast()) break;
       }
   }

   void find_all(std::vector<int> &ans) {
       if (blockCount == 0) return;

       DataBlock tp_block;
       for (int i = beg; i; i = tp_block.nextPos) {
           if (!blockStorage.read(tp_block, i)) break;

           for (int j = 0; j < tp_block.size; ++j) {
               ans.push_back(tp_block.block[j].num_value);
           }

           if (tp_block.nextPos == 0) break;
       }
   }
};

#endif //BLOCKLINK_H