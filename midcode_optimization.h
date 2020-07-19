#ifndef OPTIMIZE_DEF
#define OPTIMIZE_DEF
#include<vector>
#include<string>
#include<map>
#include<set>
#include"global_def.h"

using namespace std;

struct Block{
    int block_id;       
    int midcode_offset;    // 第一条四元式在总四元式中的偏移
    std::string label;
    std::vector<Midcode> midcodes;

    // 前后基本块
    std::vector<int> pre_block;
    std::vector<int> next_block;

    // 基本块入口出口活跃变量
    std::set<string> in_active_var;
    std::set<string> out_active_var;
    
    // 基本块的use集 def集
    std::set<string> use; //引用
    std::set<string> def; //定值
};

struct DFG{
    int num_block;
    std::vector<Block> blocks;
    std::map<std::string, int> label_block;
};


std::vector<Midcode> code_optimize(std::vector<Midcode> midcodes);


#endif