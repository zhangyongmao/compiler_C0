
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<map>
#include"global_def.h"
#include"lexical_analysis.cpp"
#include"grammatical_anaylysis.h"
#include"code_generator.h"
#include"midcode_optimization.h"

using namespace std;

int main(){

    // 代码文件目录
    string file_name = "test_file\\t.txt";


    // 词法分析
    vector<Token> tokens = word_anaylze(file_name);
    cout<<"word anaylze "<<endl;
    int tmp_row = 0;
    for(int i = 0; i < tokens.size(); i++){
        cout<<tokens[i].type<<" "<<tokens[i].word<<endl;
    }


    // 语法分析
    vector<Midcode> midcodes = grammar_anaylze(tokens);
    cout<<"grammar analyze"<<endl;
    ofstream file1( "midcode.txt" );
    streambuf *buf1 = cout.rdbuf( file1.rdbuf());
    for(int i = 0; i < midcodes.size(); i++){
        cout<<midcodes[i].op<<",";
        if(midcodes[i].s1 != "")cout<<midcodes[i].s1<<",";
        else cout<<" ,";
        if(midcodes[i].s2 != "")cout<<midcodes[i].s2<<",";
        else cout<<" ,";
        if(midcodes[i].s3 != "")cout<<midcodes[i].s3<<endl;
        else cout<<" "<<endl;
    }
    cout.rdbuf(buf1);


    // 代码优化
    vector<Midcode> optim_midcodes = code_optimize(midcodes);
    ofstream file2( "midcode_optim.txt" );
    streambuf *buf2 = cout.rdbuf( file2.rdbuf());
    for(int i = 0; i < optim_midcodes.size(); i++){
        cout<<optim_midcodes[i].op<<",";
        if(optim_midcodes[i].s1 != "")cout<<optim_midcodes[i].s1<<",";
        else cout<<" ,";
        if(optim_midcodes[i].s2 != "")cout<<optim_midcodes[i].s2<<",";
        else cout<<" ,";
        if(optim_midcodes[i].s3 != "")cout<<optim_midcodes[i].s3<<endl;
        else cout<<" "<<endl;
    }
    cout.rdbuf(buf2);


    // mips汇编生成
    code_generator(optim_midcodes, "result.asm");

    return 0;
}