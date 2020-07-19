#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<map>
#include<queue>
#include<set>
#include"global_def.h"
#include"midcode_optimization.h"

using namespace std;

vector<Midcode> original_midcode;
vector<Midcode> optimal_midcode;

set<string> global_var_optim;
map<string, int> global_const_optim;


bool is_number_optim(string s){
    if(s.size() > 0 && s[0] >= '0' && s[0] <= '9'){
        return true;
    }
    if(s.size() > 1 && s[0] == '-' && s[1] >= '0' && s[1] <= '9'){
        return true;
    }
    return false;
}

void calc_use_def(DFG * dfg){
    for(int i = 0; i < dfg->blocks.size(); i++){
        for(string s: global_var_optim){
            dfg->blocks[i].out_active_var.insert(s);
        }
        for(int j = 0; j < dfg->blocks[i].midcodes.size(); j++){
            Midcode mid = dfg->blocks[i].midcodes[j];
            cout<<"op = "<<endl;
            cout<<mid.op<<endl;
            if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" ){
                if( ! is_number_optim(mid.s1) && dfg->blocks[i].def.find(mid.s1) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s1) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s1);
                }
                if( ! is_number_optim(mid.s2) && dfg->blocks[i].def.find(mid.s2) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s2) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s2);
                }
                if( dfg->blocks[i].def.find(mid.s3) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s3) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].def.insert(mid.s3);
                }
            }
            if(mid.op == "="){
                if( ! is_number_optim(mid.s1) && dfg->blocks[i].def.find(mid.s1) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s1) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s1);
                }
                if( dfg->blocks[i].def.find(mid.s3) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s3) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].def.insert(mid.s3);
                }
            }
            if(mid.op == "==" || mid.op == "!=" || mid.op == "<" || mid.op == ">" || mid.op == "<=" || mid.op == ">=" ){
                if( ! is_number_optim(mid.s1) && dfg->blocks[i].def.find(mid.s1) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s1) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s1);
                }
                if( ! is_number_optim(mid.s2) && dfg->blocks[i].def.find(mid.s2) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s2) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s2);
                }
            }
            if(mid.op == "[]"){
                if( ! is_number_optim(mid.s2) && dfg->blocks[i].def.find(mid.s2) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s2) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s2);
                }
                if( dfg->blocks[i].def.find(mid.s3) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s3) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].def.insert(mid.s3);
                }
            }
            if(mid.op == "[]="){
                if( ! is_number_optim(mid.s1) && dfg->blocks[i].def.find(mid.s1) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s1) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s1);
                }
                if( ! is_number_optim(mid.s2) && dfg->blocks[i].def.find(mid.s2) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s2) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s2);
                }
            }
            if(mid.op == "scanf"){
                if( dfg->blocks[i].def.find(mid.s3) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s3) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].def.insert(mid.s3);
                }
            }
            if(mid.op == "printf"){
                if( ! is_number_optim(mid.s2) && dfg->blocks[i].def.find(mid.s2) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s2) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s2);
                }
            }
            if(mid.op == "call_param"){
                if( ! is_number_optim(mid.s3) && dfg->blocks[i].def.find(mid.s3) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s3) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s3);
                }
            }
            if(mid.op == "return"){
                if( ! is_number_optim(mid.s3) && dfg->blocks[i].def.find(mid.s3) == dfg->blocks[i].def.end() && dfg->blocks[i].use.find(mid.s3) == dfg->blocks[i].use.end()){
                    dfg->blocks[i].use.insert(mid.s3);
                }
            }
        }
        for(string s: dfg->blocks[i].use){
            dfg->blocks[i].in_active_var.insert(s);
        }
        for(string s: dfg->blocks[i].out_active_var){
            if(dfg->blocks[i].def.find(s) != dfg->blocks[i].def.end()){
                dfg->blocks[i].in_active_var.insert(s);
            }
        }
    }
}

void calc_active_var(DFG * dfg){
    while(1){
        bool repeat = false;
        for(int i = dfg->blocks.size() - 1 ;i >= 0; i--){
            for(int next_block: dfg->blocks[i].next_block){
                for(string next_active_var: dfg->blocks[next_block].in_active_var){
                    if(dfg->blocks[i].out_active_var.find(next_active_var) == dfg->blocks[i].out_active_var.end()){
                        dfg->blocks[i].out_active_var.insert(next_active_var);
                        repeat = true;
                    }
                }
            }
            for(string s: dfg->blocks[i].out_active_var){
                if(dfg->blocks[i].def.find(s) == dfg->blocks[i].def.end()){
                    dfg->blocks[i].in_active_var.insert(s);
                }
            }
        }
        if(repeat == false){
            break;
        }
    }
}

DFG func_to_DFG(int offset, vector<bool>& first_midcode){
    int cnt_blockid = 0;
    DFG dfg;
    dfg.num_block = 0;
    int cnt = offset;

    // 划分block
    while(cnt < original_midcode.size()){
        Block b;
        b.block_id = cnt_blockid++;

        // block标识，加入第一条四元式
        if(original_midcode[cnt].op == "label"){
            b.label = original_midcode[cnt].s3;
            dfg.label_block[b.label] = b.block_id;
        }
        b.midcode_offset = cnt;
        b.midcodes.push_back(original_midcode[cnt++]);

        // 判断并逐渐加入后面的四元式
        while( cnt < original_midcode.size()){
            if(first_midcode[cnt] || original_midcode[cnt].op == "end_func"|| original_midcode[cnt].op == "exit"){
                break;
            }
            b.midcodes.push_back(original_midcode[cnt++]);
        }

        // 加入dfg图
        dfg.blocks.push_back(b);
        dfg.num_block++;

        if(original_midcode[cnt].op == "end_func" || original_midcode[cnt].op == "exit"){
            break;
        }
    }

    // 计算block间顺序关系 pre  next
    for(int i = 0; i < dfg.blocks.size() ; i++){
        string last_op = dfg.blocks[i].midcodes[dfg.blocks[i].midcodes.size() - 1].op;
        
        // 正常顺序关系
        if(i + 1 < dfg.blocks.size() && last_op != "jmp"){
            dfg.blocks[i].next_block.push_back(i+1);
        }
        if(i - 1 >= 0 && dfg.blocks[i - 1].midcodes[dfg.blocks[i - 1].midcodes.size() - 1].op != "jmp"){
            dfg.blocks[i].pre_block.push_back(i-1);
        }
        // 跳转关系
        if(last_op == "==" || last_op == "!=" || last_op == "<" || last_op == "<=" || last_op == ">" || last_op == ">=" || last_op == "jmp" ){
            string j_label = dfg.blocks[i].midcodes[dfg.blocks[i].midcodes.size() - 1].s3;
            dfg.blocks[i].next_block.push_back(dfg.label_block[j_label]);
            dfg.blocks[dfg.label_block[j_label]].pre_block.push_back(i);
        }
    }

    // 计算每个block的入口出口处活跃变量
    calc_use_def(&dfg);
    calc_active_var(&dfg);

    return dfg;
}

void assign_replace(Block * b){
    
    for(int i = 0; i < b->midcodes.size() ; i++){
        // 如果是赋值语句
        if(optimal_midcode[b->midcode_offset + i].op == "="){
            string origin_var = optimal_midcode[b->midcode_offset + i].s1;
            string assign_var = optimal_midcode[b->midcode_offset + i].s3;
            bool replace = false;

            // 替换之后的四元式中使用的origin_var
            for(int j = i + 1; j < b->midcodes.size(); j++){
                Midcode mid = optimal_midcode[b->midcode_offset + j];

                // 判断是否修改等式两端的值
                if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" ){
                    if(mid.s3 == origin_var || mid.s3 == assign_var){
                        break;
                    }
                }
                if(mid.op == "[]"){
                    if(mid.s3 == origin_var || mid.s3 == assign_var){
                        break;
                    }
                }
                if(mid.op == "="){
                    if(mid.s3 == origin_var || mid.s3 == assign_var){
                        break;
                    }
                }
                if(mid.op == "scanf"){
                    if(mid.s3 == origin_var || mid.s3 == assign_var){
                        break;
                    }
                }
                if(mid.op == "call_r"){
                    if(mid.s3 == origin_var || mid.s3 == assign_var){
                        break;
                    }
                }
                
                // 进行替换
                if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" ){
                    if(mid.s1 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s1 = origin_var;
                    }
                    if(mid.s2 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s2 = origin_var;
                    }
                }
                if(mid.op == "="){
                    if(mid.s1 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s1 = origin_var;
                    }
                }
                if(mid.op == "==" || mid.op == "!=" || mid.op == "<" || mid.op == ">" || mid.op == "<=" || mid.op == ">=" ){
                    if(mid.s1 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s1 = origin_var;
                    }
                    if(mid.s2 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s2 = origin_var;
                    }
                }
                if(mid.op == "[]"){
                    if(mid.s1 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s1 = origin_var;
                    }
                    if(mid.s2 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s2 = origin_var;
                    }
                }
                if(mid.op == "[]="){
                    if(mid.s1 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s1 = origin_var;
                    }
                    if(mid.s2 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s2 = origin_var;
                    }
                }
                if(mid.op == "printf"){
                    if(mid.s2 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s2 = origin_var;
                    }
                }
                if(mid.op == "call_param"){
                    if(mid.s3 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s3 = origin_var;
                    }
                }
                if(mid.op == "return"){
                    if(mid.s3 == assign_var){
                        optimal_midcode[b->midcode_offset + j].s3 = origin_var;
                    }
                }
                b->midcodes[j].op = optimal_midcode[b->midcode_offset + j].op;
                b->midcodes[j].s1 = optimal_midcode[b->midcode_offset + j].s1;
                b->midcodes[j].s2 = optimal_midcode[b->midcode_offset + j].s2;
                b->midcodes[j].s3 = optimal_midcode[b->midcode_offset + j].s3;
            }
        }
    }
}

void delete_unnecessary_midcode(Block * b){
    set<string> use_var;
    for(string s: b->out_active_var){
        use_var.insert(s);
    }
    for(int i = b->midcodes.size() - 1; i >= 0; i--){
        Midcode mid = optimal_midcode[b->midcode_offset + i];

        cout<<mid.op<<" "<<mid.s1<<" "<<mid.s2<<" "<<mid.s3<<endl;
        cout<<b->midcodes[i].op<<" "<<b->midcodes[i].s1<<" "<<b->midcodes[i].s2<<" "<<b->midcodes[i].s3<<" "<<endl;
        // cout<<b->block_id<<"  use_var = "<<use_var.size()<<" ";
        // for(string s: use_var){
        //     cout<<" "<<s;
        // }
        cout<<endl;
        if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" ){
            // 删掉无用指令
            if(use_var.find(mid.s3) == use_var.end()){
                optimal_midcode[i + b->midcode_offset].op = "nop";
                b->midcodes[i].op = "nop";
            }
            else{
                if( ! is_number_optim(mid.s1)){
                    use_var.insert(mid.s1);
                }
                if( ! is_number_optim(mid.s2)){
                    use_var.insert(mid.s2);
                }
            }
        }
        if(mid.op == "="){
            if(use_var.find(mid.s3) == use_var.end()){
                optimal_midcode[i + b->midcode_offset].op = "nop";
                b->midcodes[i].op = "nop";
            }
            else if( ! is_number_optim(mid.s1)){
                use_var.insert(mid.s1);
            }
        }
        if(mid.op == "==" || mid.op == "!=" || mid.op == "<" || mid.op == ">" || mid.op == "<=" || mid.op == ">=" ){
            if( ! is_number_optim(mid.s1)){
                use_var.insert(mid.s1);
            }
            if( ! is_number_optim(mid.s2)){
                use_var.insert(mid.s2);
            }
        }
        if(mid.op == "[]"){
            if(use_var.find(mid.s3) == use_var.end()){
                optimal_midcode[i + b->midcode_offset].op = "nop";
                b->midcodes[i].op = "nop";
            }
            else{
                if( ! is_number_optim(mid.s2)){
                    use_var.insert(mid.s2);
                }
            }
        }
        if(mid.op == "[]="){
            if( ! is_number_optim(mid.s1)){
                use_var.insert(mid.s1);
            }
            if( ! is_number_optim(mid.s2)){
                use_var.insert(mid.s2);
            }
        }
        if(mid.op == "scanf"){
            ;
        }
        if(mid.op == "printf"){
            if( ! is_number_optim(mid.s2) && mid.s2 != ""){
                use_var.insert(mid.s2);
            }
        }
        if(mid.op == "call_param"){
            if( ! is_number_optim(mid.s3)){
                use_var.insert(mid.s3);
            }
        }
        if(mid.op == "return"){
            if( ! is_number_optim(mid.s3)){
                use_var.insert(mid.s3);
            }
        }
    }
}

void block_optimize(Block * b){
    // 常量替换
    for(int i = 0; i < b->midcodes.size(); i++){
        Midcode mid = b->midcodes[i];
        if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/"){
            if(is_number_optim(mid.s1) && is_number_optim(mid.s2)){
                int index = b->midcode_offset + i;
                optimal_midcode[index].op = "=";
                if(mid.op == "+") optimal_midcode[index].s1 = to_string(stoi(mid.s1) + stoi(mid.s2));
                if(mid.op == "-") optimal_midcode[index].s1 = to_string(stoi(mid.s1) - stoi(mid.s2));
                if(mid.op == "*") optimal_midcode[index].s1 = to_string(stoi(mid.s1) * stoi(mid.s2));
                if(mid.op == "/") optimal_midcode[index].s1 = to_string(stoi(mid.s1) / stoi(mid.s2));
                optimal_midcode[index].s2 = "";
                // block里也变换一下
                b->midcodes[i].op = "=";
                b->midcodes[i].s1 = optimal_midcode[index].s1;
                b->midcodes[i].s2 = "";
                // 新生成的常量应用到后面
                for(int j = i + 1; j < b->midcodes.size(); j++){
                    if(b->midcodes[j].op == "+" || b->midcodes[j].op == "-" || b->midcodes[j].op == "*" || b->midcodes[j].op == "/" || b->midcodes[j].op == "[]" || b->midcodes[j].op == "call_r"){
                        if(b->midcodes[j].s3 == mid.s3){
                            break;
                        }
                    }
                    if(b->midcodes[j].op == "+" || b->midcodes[j].op == "-" || b->midcodes[j].op == "*" || b->midcodes[j].op == "/" || b->midcodes[j].op == "[]" || b->midcodes[j].op == "[]="){
                        if(b->midcodes[j].s1 == optimal_midcode[index].s3){
                            b->midcodes[j].s1 = optimal_midcode[index].s1;
                            optimal_midcode[j + b->midcode_offset].s1 = optimal_midcode[index].s1;
                        }
                        else if(b->midcodes[j].s2 == optimal_midcode[index].s3){
                            b->midcodes[j].s2 = optimal_midcode[index].s1;
                            optimal_midcode[j + b->midcode_offset].s2 = optimal_midcode[index].s1;
                        }
                    }
                }
            }
        }
    }

    // 消除局部公共子表达式
    for(int i = 0; i < b->midcodes.size(); i++){
        Midcode mid = b->midcodes[i];
        if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" ){
            if( ! is_number_optim(mid.s1) && ! is_number_optim(mid.s2)){
                for(int j = i + 1; j < b->midcodes.size(); j++){
                    if(b->midcodes[j].op == "+" || b->midcodes[j].op == "-" || b->midcodes[j].op == "*" || b->midcodes[j].op == "/" || b->midcodes[j].op == "[]" || b->midcodes[j].op == "call_r"){
                        if(b->midcodes[j].s3 == mid.s1 || b->midcodes[j].s3 == mid.s2 || b->midcodes[j].s3 == mid.s3){
                            break;
                        }
                    }
                    if(b->midcodes[j].op == mid.op && b->midcodes[j].s1 == mid.s1 && b->midcodes[j].s2 == mid.s2){
                        int index = b->midcode_offset + j;
                        optimal_midcode[index].op = "=";
                        optimal_midcode[index].s1 = mid.s3;
                        optimal_midcode[index].s2 = "";
                        b->midcodes[j].op = "=";
                        b->midcodes[j].s1 = mid.s3;
                        b->midcodes[j].s2 = "";
                    }
                }
            }
        }
    }

    // 等式替换
    // 等式替换后产生的无用语句在下面的无用代码消除中删除
    assign_replace(b);

    // 无用代码消除，根据活跃变量分析
    delete_unnecessary_midcode(b);

}

// 代码移动
void move_code(DFG * dfg, int start_id, int end_id){
    map<string, int> def_var;
    // 对一个循环进行处理，记录所有新赋值的变量
    for(int i = start_id; i <= end_id; i++){
        for(int j = 0; j < dfg->blocks[i].midcodes.size(); j++){
            Midcode mid = dfg->blocks[i].midcodes[j];
            if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" ){
                if( def_var.count(mid.s3) == 0 ){
                    def_var[mid.s3] = 1;
                }
                else{
                    def_var[mid.s3] += 1;
                }
            }
            if(mid.op == "=" || mid.op == "[]" || mid.op == "[]=" || mid.op == "scanf" || mid.op == "call_r"){
                if( def_var.count(mid.s3) == 0){
                    def_var[mid.s3] = 1;
                }
                else{
                    def_var[mid.s3] += 1;
                }
            }
        }
    }

    // 将于循环无关的代码移动
    vector<Midcode> move_midcode;
    for(int i = start_id; i <= end_id; i++){
        for(int j = 0; j < dfg->blocks[i].midcodes.size(); j++){
            Midcode mid = dfg->blocks[i].midcodes[j];
            if(mid.op == "+" || mid.op == "-" || mid.op == "*" || mid.op == "/" || mid.op == "="){
                if( def_var.count(mid.s1) == 0 && def_var.count(mid.s2) == 0 && def_var.count(mid.s3) == 1 && def_var[mid.s3] == 1){
                    move_midcode.push_back(mid);
                    dfg->blocks[i].midcodes[j].op = "nop";
                    optimal_midcode[dfg->blocks[i].midcode_offset + j].op = "nop";
                }
            }
        }
    }

    // 将循环无关代码移动到循环之前
    int index_label = dfg->blocks[start_id].midcode_offset;
    for(int i = 0; i < move_midcode.size(); i++){
        optimal_midcode.insert(optimal_midcode.begin() + index_label + i, move_midcode[i]);
    }
}

// 循环优化
void loop_optim(DFG *dfg){
    for(int i = 0; i < dfg->blocks.size(); i++){
        Midcode mid = dfg->blocks[i].midcodes[dfg->blocks[i].midcodes.size() - 1];
        if(mid.op == "jmp"){
            string j_label = mid.s3;
            int start_blockid = dfg->label_block[j_label];
            if(start_blockid <= i){
                move_code(dfg, start_blockid, i);
            }
        }
    }
}

// DFG 流图优化
void dfg_optimize(DFG dfg){
    
    for(int i = 0; i < dfg.num_block; i++){
        block_optimize(& dfg.blocks[i]);
    }

    // 代码移动
    loop_optim(&dfg);
}

void code_optimize_dfg(){
    // 划分基本块
    vector<bool> first_midcode(original_midcode.size(), false);
    first_midcode[0] = true;
    for(int i = 0; i < original_midcode.size() - 1; i++){
        if(original_midcode[i].op == "==" || 
            original_midcode[i].op == "!=" ||
            original_midcode[i].op == "<" || 
            original_midcode[i].op == "<=" || 
            original_midcode[i].op == ">" || 
            original_midcode[i].op == ">=" || 
            original_midcode[i].op == "jmp" ||
            original_midcode[i].op == "call" ||
            original_midcode[i].op == "call_r"){
                first_midcode[i + 1] = true;
        }
        if(original_midcode[i].op == "label" ){
            first_midcode[i] = true;
        }
    }

    // 构建DFG图并据此进行优化
    for(int i = 0; i < original_midcode.size()-1; i++){
        if(original_midcode[i].op == "func"){
            DFG dfg = func_to_DFG(i + 1, first_midcode);

            cout<<endl;
            cout<<"DFG: "<<original_midcode[i].s3<<endl;
            for(int j = 0 ; j < dfg.blocks.size(); j++){
                cout<<"\t"<<dfg.blocks[j].block_id<<endl;
                cout<<"\tin_active_var = ";
                for(string s: dfg.blocks[j].in_active_var){
                    cout<<s<<" ";
                }
                cout<<endl;
                for(Midcode mid: dfg.blocks[j].midcodes){
                    cout<<"\t"<<mid.op<<","<<mid.s1<<","<<mid.s2<<","<<mid.s3<<endl;
                }
                cout<<endl;
                cout<<"\tuse = ";
                for(string s: dfg.blocks[j].use){
                    cout<<" "<<s;
                }
                cout<<endl;
                cout<<"\tdef = ";
                for(string s: dfg.blocks[j].def){
                    cout<<" "<<s;
                }
                cout<<endl;
                cout<<"\tout_active_var = ";
                for(string s: dfg.blocks[j].out_active_var){
                    cout<<"\t"<<s<<" ";
                }
                cout<<endl<<"\tnext_block = ";
                for(int p : dfg.blocks[j].next_block){
                    cout<<" "<<p;
                }
                cout<<endl;
            }
            dfg_optimize(dfg);
        }
    }
}

void const_replace(){
    // 记录全局变量
    for(int i = 0; i < original_midcode.size(); i++){
        if(original_midcode[i].op == "func"){
            break;
        }
        if(original_midcode[i].op == "const_int"){ 
            global_const_optim[original_midcode[i].s3] = stoi(original_midcode[i].s1);
        }
        else if(original_midcode[i].op == "const_char"){
            global_const_optim[original_midcode[i].s3] = stoi(original_midcode[i].s1);
        }
        else{
            global_var_optim.insert(original_midcode[i].s3);
        }
    }

    // 全局常量直接替换为值
    for(int i = 0; i < original_midcode.size(); i++){
        string tmp_op = original_midcode[i].op;
        if(tmp_op == "+" || tmp_op == "-" || tmp_op == "*" || tmp_op == "/" || tmp_op == "[]" || tmp_op == "[]=" || tmp_op == ">" || tmp_op == "<" || tmp_op == "<=" || tmp_op == ">=" || tmp_op == "=" || tmp_op == "==" || tmp_op == "!="){
            if(global_const_optim.count(original_midcode[i].s1) == 1){
                original_midcode[i].s1 = to_string(global_const_optim[original_midcode[i].s1]);
            }
            if(global_const_optim.count(original_midcode[i].s2) == 1){
                original_midcode[i].s2 = to_string(global_const_optim[original_midcode[i].s2]);
            }
        }
    }
}


vector<Midcode> code_optimize(vector<Midcode> midcodes){

    original_midcode = midcodes;

    // 常量记录、转化
    const_replace();

    // 复制到optim_midcode
    for(int i = 0; i < original_midcode.size(); i++){
        Midcode m;
        m.op = original_midcode[i].op;
        m.s1 = original_midcode[i].s1;
        m.s2 = original_midcode[i].s2;
        m.s3 = original_midcode[i].s3;
        optimal_midcode.push_back(m);
    }

    // DFG流图优化
    code_optimize_dfg();

    return optimal_midcode;
}

