#ifndef CODE_GEN
#define CODE_GEN

#include<iostream>
#include<map>
#include<set>
#include<string>
#include<vector>
#include"global_def.h"

using namespace std;

class RegManager{
    public:
        map<string, int> global_var;                //全局变量
        map<string, int> local_var_address;         //局部变量的存放地址
        vector<pair<string,string>> reg_data;       //寄存器和所存数据的匹配
        map<string, bool> used_reg;                 //已经使用的reg

};

void code_generator(std::vector<Midcode> &midcode, std::string file_name);

#endif