#include<iostream>
#include<stdio.h>
#include<vector>
#include<string>
#include<fstream>
#include<map>
#include"global_def.h"

using namespace std;

string mips_file = "mips.asm";      //输出mips汇编文件名
vector<Midcode> midcodes;           //所有四元式序列
int index_midcode = 0;              //当前处理的四元式索引
Midcode current_midcode;            //当前四元式

vector<string> global_var;                //全局变量，用来判断某个值是否是全局变量
map<string, int> global_const;              //全局常量，变量名-值
map<string, int> local_var_address;         //局部变量的存放地址

vector<pair<string,string>> reg_var;        //寄存器和所存数据的匹配
int reg_count = 1;                          //将要使用的reg号

int address = 0;                            //当前sp相对地址
int func_param_cnt = 0;                     //当前函数参数
int call_param_cnt = 0;                     //当前调用函数的参数数量

bool is_number(string s){
    if(s.size() > 0 && s[0] >= '0' && s[0] <= '9'){
        return true;
    }
    if(s.size() > 1 && s[0] == '-' && s[1] >= '0' && s[1] <= '9'){
        return true;
    }
    return false;
}

bool is_tmp_var(string s){
    if(s.size() > 4 && s[0] == '$' && s[1] == 't' && s[2] == 'm' && s[3] == 'p'){
        return true;
    }
    return false;
}

void push_local_var(string var_name, int size){
    local_var_address[var_name] = address;
    cout<<"\t\tsubi\t$sp\t$sp\t"<<to_string(size)<<endl;
    address += size;
}

string get_localvar_offset(string var_name){
    if(local_var_address.count(var_name) == 1){
        return to_string(-1 * local_var_address[var_name]);
    }
    return "";
}

string get_reg(string var_name){
    string return_reg = "$t" + to_string(reg_count);
    // 使用reg前把值存入内存中
    if(reg_var[reg_count].second != ""){
        if(get_localvar_offset(reg_var[reg_count].second) != ""){
            string offset = get_localvar_offset(reg_var[reg_count].second);
            cout<<"\t\tsw\t"<<reg_var[reg_count].first<<"\t"<<offset<<"($fp)"<<endl;
        }
        else{
            cout<<"\t\tla\t$t0\t"<<reg_var[reg_count].second<<endl;
            cout<<"\t\tsw\t"<<reg_var[reg_count].first<<"\t($t0)"<<endl;
        }
    }

    reg_var[reg_count].second = var_name;
    if(reg_count == 9){
        reg_count = 1;
    }
    else{
        reg_count++;
    }
    return return_reg;
}

string get_var_reg(string var_name){
    if(is_number(var_name))return "";
    for(int i = 1; i <= 9; i++){
        if(reg_var[i].second != "" && reg_var[i].second == var_name && reg_count != i){
            return reg_var[i].first;
        }
    }
    return "";
}

void update_var(string var_name){
    for(int i = 1; i <= 9; i++){
        if(reg_var[i].second == var_name){
            reg_var[i].second = "";
        }
    }
}

void clear_local_data(){
    for(int i = 0; i <= 9; i++){
        reg_var[i].second = "";
    }
    reg_count = 1;
    local_var_address.clear();
}


void next_midcode(){
    index_midcode++;
    if(index_midcode >= midcodes.size())return;
    current_midcode = midcodes[index_midcode];
}

void data(){
    // 全局变量声明
    while(current_midcode.op == "const_int" || current_midcode.op == "const_char" || current_midcode.op == "int" || current_midcode.op == "char" || current_midcode.op == "int_array" || current_midcode.op == "char_array"){
        //const的值直接记录在符号表里，使用时直接赋值，不再lw
        if(current_midcode.op == "const_int"){
            cout<<current_midcode.s3<<":\t.word\t"<<current_midcode.s1<<endl;
            global_const[current_midcode.s3] = (int)current_midcode.s1.c_str();
        }
        else if(current_midcode.op == "const_char"){
            cout<<current_midcode.s3<<":\t.word\t"<<current_midcode.s1<<endl;
            global_const[current_midcode.s3] = stoi(current_midcode.s1);
        }
        else{
            if(current_midcode.op == "int"){
                cout<<current_midcode.s3<<":\t.space\t"<<"4"<<endl;
            }
            if(current_midcode.op == "char"){
                cout<<current_midcode.s3<<":\t.space\t"<<"4"<<endl;
            }
            if(current_midcode.op == "int_array"){
                cout<<current_midcode.s3<<":\t.space\t"<<stoi(current_midcode.s1) * 4 <<endl;
            }
            if(current_midcode.op == "char_array"){
                cout<<current_midcode.s3<<":\t.space\t"<<stoi(current_midcode.s1) * 4 <<endl;
            }
            global_var.push_back(current_midcode.s3); 
        }
        next_midcode();
    }
    int tmp_index = index_midcode;
    int cnt_string = 0;
    while(tmp_index < midcodes.size()){
        if(midcodes[tmp_index].op == "printf"){
            cout<<"string"<<cnt_string<<":\t.asciiz\t\""<<midcodes[tmp_index].s1<<"\""<<endl;
            midcodes[tmp_index].s1 = "string" + to_string(cnt_string);
            cnt_string++;
        }
        tmp_index++;
    }
}

void gen_const_int(){
    global_const[current_midcode.s3] = stoi(current_midcode.s1);
}

void gen_const_char(){
    global_const[current_midcode.s3] = (int)current_midcode.s1.c_str();
}

void gen_int(){
    push_local_var(current_midcode.s3, 4);
}

void gen_char(){
    push_local_var(current_midcode.s3, 4);
}

void gen_int_array(){
    push_local_var(current_midcode.s3, 4 * stoi(current_midcode.s1));
}

void gen_char_array(){
    push_local_var(current_midcode.s3, 4 * stoi(current_midcode.s1));
}

string get_reg1(string var_name){
    string reg_1;
    reg_1 = get_var_reg(var_name);
    if(reg_1 == ""){
        if(is_number(var_name)){
            reg_1 = get_reg("");
            cout<<"\t\tli\t"<<reg_1<<"\t"<<var_name<<endl;
        }
        else if(get_localvar_offset(var_name) != ""){
            reg_1 = get_reg(var_name);
            string offset = get_localvar_offset(var_name);
            cout<<"\t\tlw\t"<<reg_1<<"\t"<<offset<<"($fp)"<<endl;
        }
        else{
            reg_1 = get_reg(var_name);
            cout<<"\t\tla\t$t0\t"<<var_name<<endl;
            cout<<"\t\tlw\t"<<reg_1<<"\t($t0)"<<endl;
        }
    }
    return reg_1;
}

pair<string, string> get_reg2(){
    string reg_1, reg_2;
    reg_1 = get_var_reg(current_midcode.s1);
    if(reg_1 == ""){
        if(is_number(current_midcode.s1)){
            reg_1 = get_reg("");
            cout<<"\t\tli\t"<<reg_1<<"\t"<<current_midcode.s1<<endl;
        }
        else if(get_localvar_offset(current_midcode.s1) != ""){
            reg_1 = get_reg(current_midcode.s1);
            string offset = get_localvar_offset(current_midcode.s1);
            cout<<"\t\tlw\t"<<reg_1<<"\t"<<offset<<"($fp)"<<endl;
        }
        else{
            reg_1 = get_reg(current_midcode.s1);
            cout<<"\t\tla\t$t0\t"<<current_midcode.s1<<endl;
            cout<<"\t\tlw\t"<<reg_1<<"\t($t0)"<<endl;
        }
    }
    
    reg_2 = get_var_reg(current_midcode.s2);
    if(reg_2 == ""){
        if(is_number(current_midcode.s2)){
            reg_2 = get_reg("");
            cout<<"\t\tli\t"<<reg_2<<"\t"<<current_midcode.s2<<endl;
        }
        else if(get_localvar_offset(current_midcode.s2) != ""){
            reg_2 = get_reg(current_midcode.s2);
            string offset = get_localvar_offset(current_midcode.s2);
            cout<<"\t\tlw\t"<<reg_2<<"\t"<<offset<<"($fp)"<<endl;
        }
        else{
            reg_2 = get_reg(current_midcode.s2);
            cout<<"\t\tla\t$t0\t"<<current_midcode.s2<<endl;
            cout<<"\t\tlw\t"<<reg_2<<"\t($t0)"<<endl;
        }
    }
    return make_pair(reg_1, reg_2);
}

vector<string> get_reg3(){
    string reg_1, reg_2, reg_3;
    reg_1 = get_var_reg(current_midcode.s1);
    if(reg_1 == ""){
        if(is_number(current_midcode.s1)){
            reg_1 = get_reg("");
            cout<<"\t\tli\t"<<reg_1<<"\t"<<current_midcode.s1<<endl;
        }
        else if(get_localvar_offset(current_midcode.s1) != ""){
            reg_1 = get_reg(current_midcode.s1);
            string offset = get_localvar_offset(current_midcode.s1);
            cout<<"\t\tlw\t"<<reg_1<<"\t"<<offset<<"($fp)"<<endl;
        }
        else{
            reg_1 = get_reg(current_midcode.s1);
            cout<<"\t\tla\t$t0\t"<<current_midcode.s1<<endl;
            cout<<"\t\tlw\t"<<reg_1<<"\t($t0)"<<endl;
        }
    }
    
    reg_2 = get_var_reg(current_midcode.s2);
    if(reg_2 == ""){
        if(is_number(current_midcode.s2)){
            reg_2 = get_reg("");
            cout<<"\t\tli\t"<<reg_2<<"\t"<<current_midcode.s2<<endl;
        }
        else if(get_localvar_offset(current_midcode.s2) != ""){
            reg_2 = get_reg(current_midcode.s2);
            string offset = get_localvar_offset(current_midcode.s2);
            cout<<"\t\tlw\t"<<reg_2<<"\t"<<offset<<"($fp)"<<endl;
        }
        else{
            reg_2 = get_reg(current_midcode.s2);
            cout<<"\t\tla\t$t0\t"<<current_midcode.s2<<endl;
            cout<<"\t\tlw\t"<<reg_2<<"\t($t0)"<<endl;
        }
    }

    if(is_tmp_var(current_midcode.s3) && get_localvar_offset(current_midcode.s3) == ""){
        push_local_var(current_midcode.s3, 4);
    }
    if(get_var_reg(current_midcode.s3) != ""){
        reg_3 = get_var_reg(current_midcode.s3);
    }
    else{
        reg_3 = get_reg(current_midcode.s3);
    }

    vector<string> result;
    result.push_back(reg_1);
    result.push_back(reg_2);
    result.push_back(reg_3);
    return result;
}

void add_tmp_var(string tmp_var){
    if(is_tmp_var(tmp_var) && get_localvar_offset(tmp_var) == ""){
        push_local_var(tmp_var, 4);
    }
}

void store_reg(string reg_3){
    if(is_tmp_var(current_midcode.s3) && get_localvar_offset(current_midcode.s3) == ""){
        push_local_var(current_midcode.s3, 4);
    }
    if(get_localvar_offset(current_midcode.s3) != ""){
        string offset = get_localvar_offset(current_midcode.s3);
        cout<<"\t\tsw\t"<<reg_3<<"\t"<<offset<<"($fp)"<<endl;
    }
    else{
        cout<<"\t\tla\t$t0\t"<<current_midcode.s3<<endl;
        cout<<"\t\tsw\t"<<reg_3<<"\t($t0)"<<endl;
    }
}

void gen_add(){
    vector<string> tmp_reg = get_reg3();
    cout<<"\t\tadd\t"<<tmp_reg[2]<<"\t"<<tmp_reg[0]<<"\t"<<tmp_reg[1]<<endl;
    // store_reg(tmp_reg[2]);
    add_tmp_var(current_midcode.s3);
}

void gen_sub(){
    vector<string> tmp_reg = get_reg3();
    cout<<"\t\tsub\t"<<tmp_reg[2]<<"\t"<<tmp_reg[0]<<"\t"<<tmp_reg[1]<<endl;
    // store_reg(tmp_reg[2]);
    add_tmp_var(current_midcode.s3);
}

void gen_mul(){
    vector<string> tmp_reg = get_reg3();
    cout<<"\t\tmul\t"<<tmp_reg[2]<<"\t"<<tmp_reg[0]<<"\t"<<tmp_reg[1]<<endl;
    // store_reg(tmp_reg[2]);
    add_tmp_var(current_midcode.s3);
}

void gen_div(){
    vector<string> tmp_reg = get_reg3();
    cout<<"\t\tdiv\t"<<tmp_reg[2]<<"\t"<<tmp_reg[0]<<"\t"<<tmp_reg[1]<<endl;
    // store_reg(tmp_reg[2]);
    add_tmp_var(current_midcode.s3);
}

void gen_assign(){

    string reg_1 = get_reg1(current_midcode.s1);

    update_var(current_midcode.s3);

    store_reg(reg_1);
}

void gen_eq(){
    pair<string,string>tmp_reg = get_reg2();
    cout<<"\t\tbeq\t"<<tmp_reg.first<<"\t"<<tmp_reg.second<<"\t"<<current_midcode.s3<<endl;
}

void gen_neq(){
    pair<string,string>tmp_reg = get_reg2();
    cout<<"\t\tbne\t"<<tmp_reg.first<<"\t"<<tmp_reg.second<<"\t"<<current_midcode.s3<<endl;
}
    
void gen_l(){
    pair<string,string>tmp_reg = get_reg2();
    cout<<"\t\tblt\t"<<tmp_reg.first<<"\t"<<tmp_reg.second<<"\t"<<current_midcode.s3<<endl;
}

void gen_b(){
    pair<string,string>tmp_reg = get_reg2();
    cout<<"\t\tbgt\t"<<tmp_reg.first<<"\t"<<tmp_reg.second<<"\t"<<current_midcode.s3<<endl;
}

void gen_leq(){
    pair<string,string>tmp_reg = get_reg2();
    cout<<"\t\tble\t"<<tmp_reg.first<<"\t"<<tmp_reg.second<<"\t"<<current_midcode.s3<<endl;
}

void gen_beq(){
    pair<string,string>tmp_reg = get_reg2();
    cout<<"\t\tbge\t"<<tmp_reg.first<<"\t"<<tmp_reg.second<<"\t"<<current_midcode.s3<<endl;
}

void gen_get_array(){

    string array_offset_reg = get_reg("");
    if(is_number(current_midcode.s2)){
        cout<<"\t\tli\t"<<array_offset_reg<<"\t"<<stoi(current_midcode.s2) * 4<<endl;
    }
    else{
        string offset_reg = get_reg1(current_midcode.s2);

        cout<<"\t\tmul\t"<<array_offset_reg<<"\t"<<offset_reg<<"\t4"<<endl;
    }

    string reg_1;
    if(get_localvar_offset(current_midcode.s1) != ""){
        reg_1 = get_reg("");
        string offset = get_localvar_offset(current_midcode.s1);
        cout<<"\t\tadd\t$t0\t$fp\t"<<offset<<endl;
        cout<<"\t\tadd\t$t0\t$t0\t"<<array_offset_reg<<endl;
        cout<<"\t\tlw\t"<<reg_1<<"\t($t0)"<<endl;
    }
    else{
        reg_1 = get_reg("");
        cout<<"\t\tla\t$t0\t"<<current_midcode.s1<<endl;
        cout<<"\t\tadd\t$t0\t$t0\t"<<array_offset_reg<<endl;
        cout<<"\t\tlw\t"<<reg_1<<"\t($t0)"<<endl;
    }
    
    update_var(current_midcode.s3);

    store_reg(reg_1);
}

void gen_array_assign(){
    
    string reg_1 = get_reg1(current_midcode.s1);

    string array_offset_reg = get_reg("");
    if(is_number(current_midcode.s2)){
        cout<<"\t\tli\t"<<array_offset_reg<<"\t"<<stoi(current_midcode.s2) * 4<<endl;
    }
    else{
        string offset_reg = get_reg1(current_midcode.s2);

        cout<<"\t\tmul\t"<<array_offset_reg<<"\t"<<offset_reg<<"\t4"<<endl;
    }


    string reg_3;
    if(get_localvar_offset(current_midcode.s3) != ""){
        reg_3 = get_reg("");
        string offset = get_localvar_offset(current_midcode.s3);
        cout<<"\t\tadd\t$t0\t$fp\t"<<offset<<endl;
        cout<<"\t\tadd\t$t0\t$t0\t"<<array_offset_reg<<endl;
        cout<<"\t\tsw\t"<<reg_1<<"\t($t0)"<<endl;
    }
    else{
        reg_3 = get_reg("");
        cout<<"\t\tla\t$t0\t"<<current_midcode.s3<<endl;
        cout<<"\t\tadd\t$t0\t$t0\t"<<array_offset_reg<<endl;
        cout<<"\t\tsw\t"<<reg_1<<"\t($t0)"<<endl;
    }
}

void gen_printf(){
    if(current_midcode.s1 != ""){
        cout<<"\t\tla\t$t0\t"<<current_midcode.s1<<endl;
        cout<<"\t\tmove\t$a0\t$t0"<<endl;
        cout<<"\t\tli\t$v0\t4"<<endl;
        cout<<"\t\tsyscall"<<endl;
    }
    if(current_midcode.s2 != ""){
        if(is_number(current_midcode.s2)){
            cout<<"\t\tli\t$a0\t"<<current_midcode.s2<<endl;
            cout<<"\t\tli\t$v0\t1"<<endl;
            cout<<"\t\tsyscall"<<endl;
        }
        else{
            if(get_localvar_offset(current_midcode.s2) != ""){
                string offset = get_localvar_offset(current_midcode.s2);
                cout<<"\t\tlw\t$a0\t"<<offset<<"($fp)"<<endl;
            }
            else{
                cout<<"\t\tla\t$t0\t"<<current_midcode.s2<<endl;
                cout<<"\t\tlw\t$a0\t($t0)"<<endl;
            }
            if(current_midcode.s3 == "char"){
                cout<<"\t\tli\t$v0\t11"<<endl;
            }
            else{
                cout<<"\t\tli\t$v0\t1"<<endl;
            }
            cout<<"\t\tsyscall"<<endl;
        }
    }
}

void gen_scanf(){
    if(current_midcode.s2 == "char"){
        cout<<"\t\tli\t$v0\t12"<<endl;
    }
    else{
        cout<<"\t\tli\t$v0\t5"<<endl;
    }
    cout<<"\t\tsyscall"<<endl;
    if(get_localvar_offset(current_midcode.s3) != ""){
        string offset = get_localvar_offset(current_midcode.s3);
        cout<<"\t\tsw\t$v0\t"<<offset<<"($fp)"<<endl;
    }
    else{
        cout<<"\t\tla\t$t0\t"<<current_midcode.s1<<endl;
        cout<<"\t\tsw\t$v0\t($t0)"<<endl;
    }
}

void gen_jmp(){
    cout<<"\t\tj\t"<<current_midcode.s3<<endl;
}

void gen_label(){
    cout<<current_midcode.s3<<":"<<endl;
}

//函数调用部分：
void gen_func(){
    cout<<current_midcode.s3<<":"<<endl;
    if(current_midcode.s3 == "main"){
        cout<<"\t\tsubi\t$sp\t$sp\t4"<<endl;
        cout<<"\t\tmove\t$fp\t$sp"<<endl;
        cout<<"\t\tsubi\t$sp\t$sp\t8"<<endl;
    }
    address = 8;
}

void gen_param(){
    string offset = to_string((func_param_cnt + 1) * 4);
    cout<<"\t\tlw\t$a0\t"<<offset<<"($fp)"<<endl;
    push_local_var(current_midcode.s3, 4);
    string local_offset = get_localvar_offset(current_midcode.s3);
    cout<<"\t\tsw\t$a0\t"<<local_offset<<"($fp)"<<endl;
    func_param_cnt++;
}

void clear_reg_var(){
    for(int i = 0; i <= 9; i++){
        if(reg_var[i].second != ""){
            if(get_localvar_offset(reg_var[i].second) != ""){
                string offset = get_localvar_offset(reg_var[i].second);
                cout<<"\t\tsw\t"<<reg_var[i].first<<"\t"<<offset<<"($fp)"<<endl;
            }
            else{
                cout<<"\t\tla\t$t0\t"<<reg_var[i].second<<endl;
                cout<<"\t\tsw\t"<<reg_var[i].first<<"\t($t0)"<<endl;
            }
        }
        reg_var[i].second = "";
    }
    reg_count = 1;
}

void gen_call(){
    clear_reg_var();

    cout<<"\t\tsw\t$fp\t($sp)"<<endl;
    cout<<"\t\tmove\t$fp\t$sp"<<endl;
    cout<<"\t\tsubi\t$sp\t$sp\t4"<<endl;
    cout<<"\t\tsw\t$ra\t($sp)"<<endl;
    cout<<"\t\tsubi\t$sp\t$sp\t4"<<endl;
    cout<<"\t\tjal\t"<<current_midcode.s1<<endl;

    
    string sub_offset = to_string(call_param_cnt * 4);
    cout<<"\t\taddi\t$sp\t$sp\t"<<sub_offset<<endl;
    address -= call_param_cnt * 4;
    call_param_cnt = 0;
}

void gen_call_r(){
    clear_reg_var();

    cout<<"\t\tsw\t$fp\t($sp)"<<endl;
    cout<<"\t\tmove\t$fp\t$sp"<<endl;
    cout<<"\t\tsubi\t$sp\t$sp\t4"<<endl;
    cout<<"\t\tsw\t$ra\t($sp)"<<endl;
    cout<<"\t\tsubi\t$sp\t$sp\t4"<<endl;
    cout<<"\t\tjal\t"<<current_midcode.s1<<endl;

    
    string sub_offset = to_string(call_param_cnt * 4);
    cout<<"\t\taddi\t$sp\t$sp\t"<<sub_offset<<endl;
    address -= call_param_cnt * 4;
    call_param_cnt = 0;

    /*
    if(global_var.count(current_midcode.s3) == 0){
        push_local_var(current_midcode.s3, 4);
    }*/

    store_reg("$v1");
}

void gen_call_param(){
    push_local_var("param_" + current_midcode.s3, 4);
    string tmp_reg = get_reg1(current_midcode.s3);
    string offset_param = get_localvar_offset("param_" + current_midcode.s3);
    cout<<"\t\tsw\t"<<tmp_reg<<"\t"<<offset_param<<"($fp)"<<endl;
    call_param_cnt++;
}

void gen_return(){
    string tmp_reg = get_reg1(current_midcode.s3);
    cout<<"\t\tmove\t$v1\t"<<tmp_reg<<endl;
    cout<<"\t\tmove\t$sp\t$fp"<<endl;
    cout<<"\t\tlw\t$fp\t($sp)"<<endl;
    cout<<"\t\tmove\t$t0\t$ra"<<endl;
    cout<<"\t\tlw\t$ra\t-4($sp)"<<endl;
    cout<<"\t\tjr\t$t0"<<endl;
}

void gen_end(){
    clear_local_data();
    func_param_cnt = 0;
}

void gen_exit(){
    cout<<"\t\tli\t$v0\t10"<<endl;
    cout<<"\t\tsyscall"<<endl;
}

void text(){
    cout<<".global main"<<endl;
    cout<<"\t\tj\tmain"<<endl;
    while(index_midcode < midcodes.size()){
        if(current_midcode.op == "const_int"){
            gen_const_int();
        }
        else if(current_midcode.op == "const_char"){
            gen_const_char();
        }
        else if(current_midcode.op == "int"){
            gen_int();
        }
        else if(current_midcode.op == "char"){
            gen_char();   
        }
        else if(current_midcode.op == "int_array"){
            gen_int_array();    
        }
        else if(current_midcode.op == "char_array"){
            gen_char_array();   
        }
        else if(current_midcode.op == "+"){
            gen_add();
        }
        else if(current_midcode.op == "-"){
            gen_sub();   
        }
        else if(current_midcode.op == "*"){
            gen_mul();   
        }
        else if(current_midcode.op == "/"){
            gen_div();   
        }
        else if(current_midcode.op == "="){
            gen_assign();   
        }
        else if(current_midcode.op == "=="){
            gen_eq();   
        }
        else if(current_midcode.op == "!="){
            gen_neq();
        }
        else if(current_midcode.op == "<"){
            gen_l();
        }
        else if(current_midcode.op == ">"){
            gen_b();
        }
        else if(current_midcode.op == "<="){
            gen_leq();
        }
        else if(current_midcode.op == ">="){
            gen_beq();
        }
        else if(current_midcode.op == "[]"){
            gen_get_array();
        }
        else if(current_midcode.op == "[]="){
            gen_array_assign();
        }
        else if(current_midcode.op == "scanf"){
            gen_scanf();
        }
        else if(current_midcode.op == "printf"){
            gen_printf();
        }
        else if(current_midcode.op == "jmp"){
            gen_jmp();
        }
        else if(current_midcode.op == "label"){
            gen_label();
        }
        else if(current_midcode.op == "func"){
            gen_func();
        }
        else if(current_midcode.op == "param"){
            gen_param();
        }
        else if(current_midcode.op == "call"){
            gen_call();
        }
        else if(current_midcode.op == "call_r"){
            gen_call_r();
        }
        else if(current_midcode.op == "call_param"){
            gen_call_param();
        }
        else if(current_midcode.op == "return"){
            gen_return();
        }
        else if(current_midcode.op == "end_func"){
            gen_end();   
        }
        else if(current_midcode.op == "exit"){
            gen_exit();
        }
        else if(current_midcode.op == "nop") {
            ;
        }
        else{
            cout<<"Error ---- error op type !!!  op = "<<current_midcode.op<<endl;
        }
        next_midcode();
    }
}

void code_generator_init(){
    for(int i = 0; i <= 9; i++){
        reg_var.push_back(make_pair("$t"+to_string(i), ""));
    }
}

void code_generator(vector<Midcode> &midcode, string file_name){
    mips_file = file_name;
    
    code_generator_init();
    ofstream file(mips_file);
    streambuf *x = cout.rdbuf(file.rdbuf());
    midcodes = midcode;
    current_midcode = midcodes[index_midcode];

    cout<<".data"<<endl;
    data();

    cout<<".text"<<endl;
    text();

    cout.rdbuf(x);
    for(int i =0 ;i < reg_var.size(); i++){
        cout<<i<<" "<<reg_var[i].first<<" "<<reg_var[i].second<<endl;
    }
    cout<<"code generator end !"<<endl;
}