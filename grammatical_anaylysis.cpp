#include<iostream>
#include<vector>
#include<string>
#include<map>
#include"global_def.h"

using namespace std;

vector<Token> grammar_tokens;       //token序列
vector<Midcode> grammar_midcode;    //语法分析结果

int index_token;                    //当前token的索引
Token current_token;                //当前token

Symbol_Table symbol_table;          //符号表
string current_func;                //当前函数名 const、 global、 func_name

int count_tmp_var = 0;              //临时变量计数
string current_tmp_var;             //当前最新临时变量

int label_num = 0;                  //跳转点计数

// 非终结符函数声明
int number();
void Factor();
void Term();
void Expression();
void const_def();
void const_describe();
void variable_describe();
void if_statement();
void while_statement();
void scanf_statement();
void printf_statement();
void return_statement();
void assign_statement();
void Statement();
void Statement_list();
bool is_func_def();
void Parameter();
void void_func_def();
void return_func_def();
void func_def();
void call_func();
void call_func_void();
string call_func_return();
string condition();


void next_Token(){
    index_token++;
    if(index_token >= grammar_tokens.size()){
        cout<<"Grammar anaylze End, No Tokens"<<endl;
    }
    else{
        current_token = grammar_tokens[index_token];
        cout<<"current token: "<<current_token.word<<" "<<current_token.type <<endl;
    }
}

string next_tmp_var(){
    current_tmp_var = "$tmp" + to_string(count_tmp_var);
    count_tmp_var++;
    return current_tmp_var;
}

string next_label(){
    label_num++;
    return "LABEL_" + to_string(label_num - 1);
}

bool match(TYPE type, string s, bool next_token){
    if(current_token.type == type && current_token.word == s){
        if(next_token) next_Token();
        return true;
    }
    return false;
}

bool match_s(string s, bool next_token){
    if(current_token.word == s){
        if(next_token) next_Token();
        return true;
    }
    return false;
}

bool match_t(TYPE type, bool next_token){
    if(current_token.type == type){
        if(next_token) next_Token();
        return true;
    }
    return false;
}

// 断言 用来处理错误
void assert(TYPE type, string s){
    if(current_token.type != type || current_token.word != s){
        cout<<"Error  assert error : word = "<<current_token.word<<" want word = "<<s<<" row = "<<current_token.row<<endl;
    }
}
void assert_t(TYPE type){
    if(current_token.type != type){
        cout<<"Error  assert_t error : word = "<<current_token.word<<" want type = "<<type<<" row = "<<current_token.row<<endl;
    }
}
void assert_s(string s){
    if(current_token.word != s){
        cout<<"Error  assert_s error : word = "<<current_token.word<<" want word = "<<s<<" row = "<<current_token.row<<endl;
    }
}

void new_midcode(string op, string s1, string s2, string s3){
    Midcode m;
    m.op = op;
    m.s1 = s1;
    m.s2 = s2;
    m.s3 = s3;
    grammar_midcode.push_back(m);
    cout<<"new midcode: "<<op<<" "<<s1<<" "<<s2<<" "<<s3<<endl;
}

void insert_func(string key, Func &f){
    symbol_table.map_func[key] = f;
}

void insert_symbol(Symbol &sym){
    string key = current_func + ":" + sym.name;
    symbol_table.Num_var = symbol_table.All_var.size();
    symbol_table.All_var.push_back(sym);
    symbol_table.index_var[key] = symbol_table.Num_var;
    symbol_table.Num_var += 1;
    if(symbol_table.Num_var != symbol_table.All_var.size()){
        cout<<"Error  grammar anaylze error  symbol index error ! "<<endl;
    }
}

bool contain_symbol(string name){
    string key;
    key = "global_const:"+ name;
    if(symbol_table.index_var.count(key) == 1){
        return true;
    }
    key = "global_var:"+ name;
    if(symbol_table.index_var.count(key) == 1){
        return true;
    }
    if(current_func != "global_const" && current_func != "global_var"){
        key = current_func + ":" + name;
        if(symbol_table.index_var.count(key) == 1){
            return true;
        }
    }
    return false;
}

Symbol get_symbol(string name){
    string key;
    key = "global_const:"+ name;
    if(symbol_table.index_var.count(key) == 1){
        return symbol_table.All_var[symbol_table.index_var[key]];
    }
    key = "global_var:"+ name;
    if(symbol_table.index_var.count(key) == 1){
        return symbol_table.All_var[symbol_table.index_var[key]];
    }
    if(current_func != "global_const" && current_func != "global_var"){
        key = current_func + ":" + name;
        if(symbol_table.index_var.count(key) == 1){
            return symbol_table.All_var[symbol_table.index_var[key]];
        }
    }
    Symbol s;
    return s;
}

// 通用非终结符

int number(){
    int result = 1;
    if(match_s("+", true)){
        result = 1;
    }
    else if(match_s("-", true)){
        result = -1;
    }
    
    if(match_t(NUM, false)){
        result *= stoi(current_token.word);
        next_Token();
        return result;
    }
    cout<<"Error  grammar Number error "<<endl;
    return -1;
}

void Factor(){
    if(match_t(ID, false)){
        if(symbol_table.map_func.count(current_token.word) == 1){
            string func_name = call_func_return();
            new_midcode("call_r", func_name,"",next_tmp_var());
        }
        else if(contain_symbol(current_token.word)){
            Symbol tmp_s = get_symbol(current_token.word);
            if(tmp_s.size == 0){
                current_tmp_var = current_token.word;
                // new_midcode("=", current_token.word, "", next_tmp_var());
                next_Token();
            }
            else{
                string array_name = current_token.word;
                next_Token();
                assert_s("[");
                match_s("[", true);
                Expression();
                string tmp_var = current_tmp_var;
                new_midcode("[]",array_name,tmp_var, next_tmp_var());
                assert_s("]");
                match_s("]", true);
            }
        }
        else{
            cout<<"Error  grammar Factor error  no type"<<endl;
        }
    }
    else if(match_t(NUM, false)){
        current_tmp_var = current_token.word;
        // new_midcode("=", current_token.word, "", next_tmp_var());
        next_Token();
    }
    else if(match_s("(", true)){
        Expression();
        assert_s(")");
        match_s(")", true);
    }
    else{
        cout<<"Error grammar Factor type error"<<endl;
    }
}

void Term(){
    Factor();
    if(match_s("*", false) || match_s("/",false)){
        if(match_s("*", true)){
            string tmp_var1 = current_tmp_var;
            Term();
            string tmp_var = current_tmp_var;
            new_midcode("*", tmp_var1, tmp_var, next_tmp_var());
        }
        else if(match_s("/", true)){
            string tmp_var1 = current_tmp_var;
            Term();
            string tmp_var = current_tmp_var;
            new_midcode("/", tmp_var1, tmp_var, next_tmp_var());
        }
    }
}

void Expression(){
    if(match_s("+",true)){
        Term();
    }
    else if(match_s("-",true)){
        Term();
        string tmp_var = current_tmp_var;
        new_midcode("-", "0", tmp_var, next_tmp_var());
    }
    else{
        Term();
    }
    
    if(match_s("+",false) || match_s("-", false)){
        if(match_s("+", true)){
            string tmp_var1 = current_tmp_var;
            Expression();
            string tmp_var = current_tmp_var;
            new_midcode("+", tmp_var1, tmp_var, next_tmp_var());
        }
        else if(match_s("-", true)){
            string tmp_var1 = current_tmp_var;
            Expression();
            string tmp_var = current_tmp_var;
            new_midcode("-", tmp_var1, tmp_var, next_tmp_var());
        }
    }
}

void assign_statement(){
    if(match_t(ID, false)){
        string to_id = current_token.word;
        next_Token();
        if(match_s("[", true)){
            Expression();
            string index_array = current_tmp_var;
            assert_s("]");
            match_s("]", true);
            assert_s("=");
            match_s("=", true);
            Expression();
            new_midcode("[]=", current_tmp_var, index_array, to_id);
        }
        else{
            assert_s("=");
            match_s("=", true);
            Expression();
            new_midcode("=", current_tmp_var, "", to_id);
        }
    }
}

// const定义

void const_def(){
    assert_t(ID);
    if(match_t(ID, false)){
        Symbol tmp_s;
        tmp_s.name = current_token.word;
        tmp_s.size = 0;
        tmp_s.bytes = 4;
        next_Token();
        assert_s("=");
        if(match_s("=",true)){
            if(match_t(CHAR, false)){
                tmp_s.type = "char";
                tmp_s.value = (int)current_token.word.c_str()[0];
                new_midcode("const_char", to_string(tmp_s.value), "" , tmp_s.name);
                next_Token();
            }
            else if(match_s("+",false) || match_s("-", false) || match_t(NUM, false)){
                tmp_s.type = "int";
                tmp_s.value = number();
                new_midcode("const_int", to_string(tmp_s.value), "" , tmp_s.name);
            }
            
            insert_symbol(tmp_s);

            if(match_s(",", true)){
                const_def();
            }
        }
    }
}

void const_describe(){
    if(match_s("const", true)){
        if(match_s("int", true) || match_s("char",true)){
            const_def();
        }
        assert_s(";");
        match_s(";", true);
    }
    if(match_s("const", false)){
        const_describe();
    }
}

// gloabl var 定义
void variable_describe(){
    if(is_func_def())return ;
    if(match_s("int", true)){
        while(match_t(ID, false)){
            Symbol tmp_s;
            tmp_s.name = current_token.word;
            tmp_s.bytes = 4;
            tmp_s.type = "int";
            next_Token();
            
            if(match_s("[",true)){
                assert_t(NUM);
                new_midcode("int_array", current_token.word, "", tmp_s.name);
                tmp_s.size = stoi(current_token.word);
                insert_symbol(tmp_s);
                next_Token();
                assert_s("]");
                match_s("]",true);
            }
            else{
                new_midcode("int", "", "", tmp_s.name);
                tmp_s.size = 0;
                insert_symbol(tmp_s);
            }
            if(match_s(";", true)){
                break;
            }
            else{
                assert_s(",");
                match_s(",",true);
            }
        }
    }
    else if(match_s("char", true)){
        while(match_t(ID, false)){
            Symbol tmp_s;
            tmp_s.name = current_token.word;
            tmp_s.bytes = 4;
            tmp_s.type = "char";
            next_Token();
            
            if(match_s("[", true)){
                assert_t(NUM);
                new_midcode("char_array", current_token.word, "", tmp_s.name);
                tmp_s.size = stoi(current_token.word);
                insert_symbol(tmp_s);
                next_Token();
                assert_s("]");
                match_s("]",true);
            }
            else{
                new_midcode("char", "", "", tmp_s.name);
                tmp_s.size = 0;
                insert_symbol(tmp_s);
            }
            if(match_s(";", true)){
                break;
            }
            else{
                assert_s(",");
                match_s(",",true);
            }
        }
    }
    if(match_s("int",false) || match_s("char", false)){
        variable_describe();
    }
}

// func 定义

string condition(){  //条件
    Expression();
    string first_exp = current_tmp_var;
    string new_label = next_label();

    if(match_s("==",true)){
        Expression();
        new_midcode("!=", first_exp, current_tmp_var, new_label);
    }
    else if(match_s("!=", true)){
        Expression();
        new_midcode("==", first_exp, current_tmp_var, new_label);
    }
    else if(match_s("<",true)){
        Expression();
        new_midcode(">=", first_exp, current_tmp_var, new_label);
    }
    else if(match_s("<=",true)){
        Expression();
        new_midcode(">", first_exp, current_tmp_var, new_label);
    }
    else if(match_s(">", true)){
        Expression();
        new_midcode("<=",first_exp, current_tmp_var, new_label);
    }
    else if(match_s(">=", true)){
        Expression();
        new_midcode("<", first_exp, current_tmp_var, new_label);
    }
    else{
        new_midcode("==", first_exp, "0", new_label);
    }
    return new_label;
}

void sub_bool_statement(string stage_label, string start_label, string end_label){

    while(1){
        string new_label = next_label();

        if(match_s("!", true)){
            Expression();
            string first_exp = current_tmp_var;

            if(match_s("==",true)){
                Expression();
                new_midcode("!=", first_exp, current_tmp_var, new_label);
            }
            else if(match_s("!=", true)){
                Expression();
                new_midcode("==", first_exp, current_tmp_var, new_label);
            }
            else if(match_s("<",true)){
                Expression();
                new_midcode(">=", first_exp, current_tmp_var, new_label);
            }
            else if(match_s("<=",true)){
                Expression();
                new_midcode(">", first_exp, current_tmp_var, new_label);
            }
            else if(match_s(">", true)){
                Expression();
                new_midcode("<=",first_exp, current_tmp_var, new_label);
            }
            else if(match_s(">=", true)){
                Expression();
                new_midcode("<", first_exp, current_tmp_var, new_label);
            }
            else{
                new_midcode("==", first_exp, "0", new_label);
            }
        }
        else{
            Expression();
            string first_exp = current_tmp_var;

            if(match_s("==",true)){
                Expression();
                new_midcode("==", first_exp, current_tmp_var, new_label);
            }
            else if(match_s("!=", true)){
                Expression();
                new_midcode("!=", first_exp, current_tmp_var, new_label);
            }
            else if(match_s("<",true)){
                Expression();
                new_midcode("<", first_exp, current_tmp_var, new_label);
            }
            else if(match_s("<=",true)){
                Expression();
                new_midcode("<=", first_exp, current_tmp_var, new_label);
            }
            else if(match_s(">", true)){
                Expression();
                new_midcode(">",first_exp, current_tmp_var, new_label);
            }
            else if(match_s(">=", true)){
                Expression();
                new_midcode(">=", first_exp, current_tmp_var, new_label);
            }
            else{
                new_midcode("!=", first_exp, "0", new_label);
            }
        }
        
        new_midcode("jmp","","", stage_label);
        new_midcode("label", "","", new_label);

        if( ! match_s("&&",true)){
            break;
        }
    }
}

string bool_statement(){
    // True跳转点
    string start_label = next_label();
    // Flase跳转点
    string end_label = next_label();

    while( ! match_s(";",false) && ! match_s(")",false)){
        string stage_label = next_label();

        sub_bool_statement(stage_label, start_label, end_label);

        new_midcode("jmp","","", start_label);
        new_midcode("label", "","", stage_label);
        match_s("||", true);
    }
    new_midcode("jmp", "","", end_label);
    new_midcode("label","","",start_label);
    return end_label;
}

void if_statement(){
    if(match_s("if", true)){
        assert_s("(");
        next_Token();
        string tmp_label1 = bool_statement();
        assert_s(")");
        next_Token();
        Statement();
        
        if(match_s("else", true)){
            string tmp_label2 = next_label();
            new_midcode("jmp", "", "", tmp_label2);
            new_midcode("label", "", "", tmp_label1);
            Statement();
            new_midcode("label", "", "", tmp_label2);
        }
        else{
            new_midcode("label", "", "", tmp_label1);
        }
    }
}

Midcode for_assign_statement(){
    Midcode mid;
    if(match_t(ID, false)){
        string to_id = current_token.word;
        next_Token();
        if(match_s("[", true)){
            Expression();
            string index_array = current_tmp_var;
            assert_s("]");
            match_s("]", true);
            assert_s("=");
            match_s("=", true);
            Expression();
            mid.op = "[]=";
            mid.s1 = current_tmp_var;
            mid.s2 = index_array;
            mid.s3 = to_id;
        }
        else{
            assert_s("=");
            match_s("=", true);
            Expression();
            mid.op = "=";
            mid.s1 = current_tmp_var;
            mid.s2 = "";
            mid.s3 = to_id;
        }
    }
    return mid;
}

void for_statement(){
    if(match_s("for", true)){
        assert_s("(");
        next_Token();
        assign_statement();
        assert_s(";");
        next_Token();
        string tmp_label1 = next_label();
        new_midcode("label","","",tmp_label1);
        string tmp_label2 = condition();
        assert_s(";");
        next_Token();
        Midcode mid = for_assign_statement();
        assert_s(")");
        next_Token();
        Statement();
        if(mid.op != "") new_midcode(mid.op, mid.s1, mid.s2, mid.s3);
        new_midcode("jmp", "","", tmp_label1);
        new_midcode("label","","", tmp_label2);
    }
}

void while_statement(){
    if(match_s("while", true)){
        string tmp_label1 = next_label();
        new_midcode("label","","", tmp_label1);
        assert_s("(");
        next_Token();
        string tmp_label2 = condition();
        assert_s(")");
        next_Token();
        Statement();
        new_midcode("jmp","","", tmp_label1);
        new_midcode("label","","", tmp_label2);
    }
}

void case_list(vector<pair<string,string>> & case_vec){
    for(int i = 0; i < case_vec.size(); i++){
        assert_s("case");
        new_midcode("label","","",case_vec[i].second);
        next_Token();
        if(match_t(NUM, false) || match_t(CHAR, false)){
            next_Token();
            assert_s(":");
            next_Token();
            
            Statement();
        }
        else{
            cout<<"Error grammar switch-case const type error"<<endl;
        }
    }
}

vector<pair<string,string>> get_case_vec(){
    vector<pair<string,string>> result;
    int flag = 1;
    int tmp_index_token = index_token;
    while(flag > 0 && tmp_index_token < grammar_tokens.size()){
        if(grammar_tokens[tmp_index_token].word == "case"){
            result.push_back(make_pair(grammar_tokens[tmp_index_token+1].word, next_label()));
        }
        if(grammar_tokens[tmp_index_token].word == "{"){
            flag++;
        }
        if(grammar_tokens[tmp_index_token].word == "}"){
            flag--;
        }
        tmp_index_token++;
    }
    return result;
}

void switch_statement(){
    if(match_s("switch",true)){
        assert_s("(");
        next_Token();
        Expression();
        string exp = current_tmp_var;
        assert_s(")");
        next_Token();
        assert_s("{");
        next_Token();

        vector<pair<string,string>> case_vec = get_case_vec();
        for(int i = 0; i < case_vec.size(); i++){
            new_midcode("==", exp, case_vec[i].first, case_vec[i].second);
        }
        case_list(case_vec);

        assert_s("}");
        next_Token();
    }
}

void scanf_statement(){
    if(match_s("scanf", true)){
        assert_s("(");
        next_Token();
        while(match_t(ID, false)){
            string type = "int";
            if(contain_symbol(current_token.word)){
                type = get_symbol(current_token.word).type;
            }
            new_midcode("scanf","",type, current_token.word);
            next_Token();
            match_s(",", true);
        }
        assert_s(")");
        next_Token();
    }
}

void printf_statement(){
    if(match_s("printf", true)){
        assert_s("(");
        next_Token();
        string s1 = "";
        if(match_t(STR, false)){
            s1 = current_token.word;
            next_Token();
        }
        match_s(",",true);
        if(match_s(")", false)){
            new_midcode("printf",s1, "", "");
            next_Token();
        }
        else{
            Expression();
            string type = "int";
            if(contain_symbol(current_tmp_var)){
                type = get_symbol(current_tmp_var).type;
            }
            new_midcode("printf",s1, current_tmp_var, type);
            assert_s(")");
            next_Token();
        }
    }
}

void return_statement(){
    if(match_s("return", true)){
        if(match_s("(", true)){
            Expression();
            if(current_func != "main") new_midcode("return", "", "", current_tmp_var);
            assert_s(")");
            next_Token();
        }
    }
}

void Statement(){
    if(match_s("if", false)){
        if_statement();
    }
    else if(match_s("for", false)){
        for_statement();
    }
    else if(match_s("while", false)){
        while_statement();
    }
    else if(match_s("switch", false)){
        switch_statement();
    }
    else if(match_s("scanf",false)){
        scanf_statement();
        assert_s(";");
        next_Token();
    }
    else if(match_s("printf", false)){
        printf_statement();
        assert_s(";");
        next_Token();
    }
    else if(match_s("return", false)){
        return_statement();
        assert_s(";");
        next_Token();
    }
    else if(match_s("{", true)){
        Statement_list();
        assert_s("}");
        next_Token();
    }
    else if(match_s(";", true)){
        ;
    }
    else{
        if(match_t(ID, false)){
            if(contain_symbol(current_token.word)){
                assign_statement();
                assert_s(";");
                next_Token();
            }
            else if(symbol_table.map_func.count(current_token.word) == 1){
                call_func();
                assert_s(";");
                next_Token();
            }
            else{
                cout<<"Error grammer statement keys error"<<endl;
            }
        }
    }
}

void Statement_list(){  //语句列的follow集只有 “}”，遇到 } 就是语句结束了
    while( ! match_s("}", false)){
        // cout<<"statement"<<current_token.word<<endl;
        Statement();
    }
}

void composite_statement(){
    if(match_s("const", false)){
        const_describe();
    }
    if(match_s("int",false) || match_s("char",false)){
        variable_describe();
    }
    Statement_list();
}

bool is_func_def(){     //判断是否为函数定义(与变量定义区分)
    if(current_token.word == "void"){
        return true;
    }
    if(current_token.word == "int" || current_token.word == "char"){
        if(index_token <= grammar_tokens.size() - 6){
            if(grammar_tokens[index_token + 1].type == ID && 
                grammar_tokens[index_token + 2].word == "(" ){
                return true;
            }
        }
    }
    return false;
}

void Parameter(){
    if(match_s(")", false)){
        return ;
    }
    if(match_s("int", true)){
        match_t(ID, false);
        new_midcode("param", "int", "", current_token.word);
        Symbol tmp_s;
        tmp_s.name = current_token.word;
        tmp_s.size = 0;
        tmp_s.type = "int";
        insert_symbol(tmp_s);
        next_Token();
    }
    else if(match_s("char", true)){
        match_t(CHAR, false);
        new_midcode("param", "char", "", current_token.word);
        Symbol tmp_s;
        tmp_s.name = current_token.word;
        tmp_s.size = 0;
        tmp_s.type = "char";
        insert_symbol(tmp_s);
        next_Token();
    }
    else{
        cout<<"Error grammar Parameter type error"<<endl;
    }

    if(match_s(",", true)){
        Parameter();
    }
}

void void_func_def(){
    if(match_s("void", true)){
        assert_t(ID);
        string func_name = current_token.word;
        current_func = func_name;
        Func tmp_func;
        tmp_func.is_return = false;
        symbol_table.map_func[func_name] = tmp_func;
        new_midcode("func","void", "", func_name);
        next_Token();
        assert_s("(");
        next_Token();

        Parameter();

        assert_s(")");
        next_Token();
        assert_s("{");
        next_Token();
        
        composite_statement();

        assert_s("}");
        next_Token();
        new_midcode("end_func","","","");
    }
}

void return_func_def(){
    if(match_s("int", true)){
        assert_t(ID);
        string func_name = current_token.word;
        current_func = func_name;
        Func tmp_func;
        tmp_func.is_return = true;
        symbol_table.map_func[func_name] = tmp_func;
        new_midcode("func","int", "", func_name);
        next_Token();
        assert_s("(");
        next_Token();

        Parameter();

        assert_s(")");
        next_Token();
        assert_s("{");
        next_Token();
        
        composite_statement();

        assert_s("}");
        next_Token();
        new_midcode("end_func","","","");
    }
    else if(match_s("char", true)){
        assert_t(ID);
        string func_name = current_token.word;
        current_func = func_name;
        Func tmp_func;
        tmp_func.is_return = true;
        symbol_table.map_func[func_name] = tmp_func;
        new_midcode("func","char", "", func_name);
        next_Token();
        assert_s("(");
        next_Token();

        Parameter();

        assert_s(")");
        next_Token();
        assert_s("{");
        next_Token();
        
        composite_statement();

        assert_s("}");
        next_Token();
        new_midcode("end_func","","","");
    }
    else{
        cout<<"Error grammer return func def error"<<endl;
    }
}

void func_def(){
    if(match_s("void", false)){
        if(grammar_tokens[index_token + 1].word == "main"){
            return ;
        }
        void_func_def();
    }
    else if(match_s("int",false) || match_s("char", false)){
        return_func_def();
    }
    else{
        cout<<"Error func def  head error"<<endl;
        return ;
    }
    func_def();
}

void call_func_void(){
    if(match_t(ID, false)){
        string func_name = current_token.word;
        if(symbol_table.map_func.count(current_token.word) == 1){
            
            next_Token();
            assert_s("(");
            next_Token();
            vector<string> call_param_vec;
            while( ! match_s(")", false)){
                Expression();
                call_param_vec.push_back(current_tmp_var);
                
                match_s(",", true);
            }
            for(int i = call_param_vec.size() - 1; i >= 0; i--){
                new_midcode("call_param","","", call_param_vec[i]);
            }
            new_midcode("call", func_name,"", "");
            
            assert_s(")");
            next_Token();
        }
    }
}

string call_func_return(){
    if(match_t(ID, false)){
        string func_name = current_token.word;
        if(symbol_table.map_func.count(current_token.word) == 1){
            // string return_tmp_name = next_tmp_var();
            // new_midcode("call_r", current_token.word,"", return_tmp_name);
            next_Token();
            assert_s("(");
            next_Token();
            vector<string> call_param_vec;
            while( ! match_s(")", false)){
                Expression();
                call_param_vec.push_back(current_tmp_var);
                
                match_s(",", true);
            }
            for(int i = call_param_vec.size() - 1; i >= 0; i--){
                new_midcode("call_param","","", call_param_vec[i]);
            }
            assert_s(")");
            next_Token();
        }
        return func_name;
    }
    return "";
}

void call_func(){
    if(match_t(ID, false)){
        if(symbol_table.map_func[current_token.word].is_return){
            string func_name = call_func_return();
            new_midcode("call_r", func_name, "","");
        }
        else{
            call_func_void();
        }
    }
}

// main 函数定义
void main_func(){
    assert_s("void");
    next_Token();
    assert_s("main");
    next_Token();
    assert_s("(");
    next_Token();
    assert_s(")");
    next_Token();
    assert_s("{");
    next_Token();

    new_midcode("func","","","main");
    composite_statement();
    new_midcode("exit","","","");

    assert_s("}");
    next_Token();
}


vector<Midcode> grammar_anaylze(vector<Token> words){
    // 初始化部分
    grammar_tokens = words;
    index_token = 0;
    current_token = grammar_tokens[0];
    symbol_table.Num_var = 0;

    // 开始语法分析
    current_func = "global_const";
    const_describe();

    current_func = "global_var";
    variable_describe();

    for(int i = 0; i < symbol_table.Num_var; i++){
        cout<<symbol_table.All_var[i].name<<"  ";
        cout<<contain_symbol(symbol_table.All_var[i].name)<<endl;
    }

    current_func = "func";
    func_def();

    current_func = "main";
    main_func();

    cout<<" sym :"<<symbol_table.Num_var<<endl;
    for(int i = 0; i < symbol_table.Num_var; i++){
        cout<<symbol_table.All_var[i].name<<"  ";
        cout<<contain_symbol(symbol_table.All_var[i].name)<<endl;
    }

    return grammar_midcode;
}

