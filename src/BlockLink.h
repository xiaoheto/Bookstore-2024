//
// Created by 43741 on 2024/12/22.
//

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

constexpr int BLOCK_SIZE = 1000;
constexpr int SPLIT_SIZE = 2000;//块的大小的两倍
constexpr int MERGE_SIZE = 500;//块的大小的一半

class DataNode {
    int pos;
    char str_value[64];//储存在文件当中必须使用char数组，如果传入了string需要转换
    int num_value;
    friend class DataBlock;
    friend class DataFile;
public:
    DataNode() = default;

    DataNode(const std::string &str_,int value,int num_ = 0) {
        std::strcpy(str_value,str_.c_str());
        num_value = value;
        pos = num_;
    }

    bool operator<(const DataNode & other)const {
        if (strcmp(str_value,other.str_value) == 0) {
            return num_value < other.num_value;
        }
        else {
            return strcmp(str_value,other.str_value) < 0;
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

    DataNode &operator = (const DataNode&right) {
        if (*this == right) {
            return *this;
        }
        strcpy(str_value,right.str_value);
        num_value = right.num_value;
        pos = right.pos;
        return *this;
    }
};
class DataBlock {
private:
    int nextPos,prePos,curPos,size;//分别为下一个、上一个块的位置，当前快的位置，块的大小
    DataNode block[2100];//中间存储了2100个DataNode（BLOCK_SIZE的两倍多一点）
    friend class DataFile;
public:
    DataBlock():nextPos(0),prePos(0),curPos(0),size(0) {}
    DataBlock(int size_,DataBlock *array):size(size_) {
        memcpy(block,array,size_ * sizeof(DataNode));
    }

    DataNode getFirst() const {
        return block[0];
    }

    DataNode getLast() const {
        return size > 0 ? block[size - 1] : block[0];
    }

    bool insertNode(const DataNode &node) {
        int position = std::lower_bound(block,block + size,node) - block;
        if (position < size && block[position] == node) {
            return false;
        }
        for (int i = size; i > position; --i) {
            block[i] = block[i - 1];
        }
        block[position] = node;//把目标块插入position位置
        size++;
        return size >= SPLIT_SIZE;
    }

    void deleteNode(const DataNode &node) {
        int position = std::lower_bound(block, block + size, node) - block;
        if (position == size || block[position] != node) {
            return; //没有找到块
        }
        for (int i = position; i < size - 1; ++i) {
            block[i] = block[i + 1];
        }//把目标快从原来位置移除
        size--;
        return;
    }

    void searchByKey(const std::string &key,std::vector<int>result) {
        for(int i = 0;i < size;++i) {
            if (block[i].str_value == key) {
                result.push_back(block[i].num_value);
            }
        }
    }
};
class DataFile {
private:
    std::string fileName;
    int blockCount,beg;//块的个数以及头指针
    //希望把块的个数储存在文件的头部，这样需要指定头指针初始的偏移量
    MemoryRiver<DataBlock> blockStorage;

    void splitBlock(int blockPos) {
        DataBlock oldBlock,newBlock;

        blockStorage.read(oldBlock, blockPos);

        int len = oldBlock.size / 2;
        newBlock.size = oldBlock.size - len;
        memcpy(newBlock.block, oldBlock.block + len, newBlock.size * sizeof(DataNode));
        oldBlock.size = len;

        newBlock.prePos = oldBlock.curPos;//新的块接到后面
        newBlock.nextPos = oldBlock.nextPos;
        newBlock.curPos = blockStorage.write(newBlock);
        oldBlock.nextPos = newBlock.curPos;

        blockStorage.update(oldBlock, oldBlock.curPos);
        blockStorage.update(newBlock, newBlock.curPos);
    }

    void mergeBlock(const int &pos) {//把pos合并到pre
        if (pos == 8) {
            return;
        }//头节点
        DataBlock cur_block, pre_block, nextPos_block;
        blockStorage.read(cur_block, pos);
        blockStorage.read(pre_block, cur_block.prePos);

        if (cur_block.nextPos) {//不是尾节点
            blockStorage.read(nextPos_block, cur_block.nextPos);
            nextPos_block.prePos = pre_block.curPos;
            blockStorage.update(nextPos_block, nextPos_block.curPos);
        }

        pre_block.nextPos = cur_block.nextPos;
        memcpy(pre_block.block + pre_block.size, cur_block.block, cur_block.size * sizeof(DataNode));
        pre_block.size += cur_block.size;

        blockStorage.update(pre_block, pre_block.curPos);
        blockStorage.Delete(cur_block.curPos);

        if (pre_block.size >= MERGE_SIZE)
            splitBlock(pre_block.curPos);
    }
public:
    DataFile():blockCount(0),beg(8) {}

    DataFile(const std::string &file_name):blockCount(0),beg(8) {
        blockStorage.initialise(file_name);
    }

    void init(std::string file_name) {
        blockStorage.initialise(file_name);
        blockCount = 0;
        beg = 0;
    }

    ~DataFile() = default;

    void find_node(std::string &key, std::vector<int> &result) {
        DataBlock cur_block;
        blockStorage.get_info(blockCount, 1);
        for (int i = beg; i; i = cur_block.nextPos) {
            blockStorage.read(cur_block, i);
            if (key >= cur_block.getFirst().str_value && key <= cur_block.getLast().str_value) {
                cur_block.searchByKey(key, result);
            }
        }
    }

    void insert_node(const DataNode &node) {
        if (blockCount == 0) {
            DataBlock newBlock;
            newBlock.size = 1;
            newBlock.block[0] = node;
            newBlock.curPos = blockStorage.write(newBlock);//返回写入块newBlock的位置
            blockStorage.update(newBlock,newBlock.curPos);//用newBlock的值更新其对应位置的对象
        }
        else {
            DataBlock block;
            for(int i = beg;;i = block.nextPos) {
                blockStorage.read(block,i);
                if (node < block.getLast() || block.nextPos == 0) {
                    block.insertNode(node);
                    blockStorage.update(block, i);
                    break;
                }
            }
            if (block.size > SPLIT_SIZE)
                splitBlock(block.curPos);
        }
    }

    void delete_node(const DataNode &node) {
        blockStorage.get_info(blockCount, 1);
        DataBlock tp_block;
        for (int i = beg; i; i = tp_block.nextPos) {
            blockStorage.read(tp_block, i);
            //可能会发生跨越情况
            if (node < tp_block.getFirst() || node > tp_block.getLast())
                continue;
            else {
                tp_block.deleteNode(node);
                blockStorage.update(tp_block, i);
                if (node > tp_block.getLast())
                    break;
            }
        }
        if (tp_block.size <= MERGE_SIZE)
            mergeBlock(tp_block.curPos);
    }

    void find_all(std::vector<int> &ans){
        DataBlock tp_block;
        for (int i = beg; i ; i = tp_block.nextPos) {
            blockStorage.read(tp_block, i);
            for (int j = 0; j < tp_block.size; ++j) {
                ans.push_back(tp_block.block[j].num_value);
            }
        }
    }
};
#endif //BLOCKLINK_H
