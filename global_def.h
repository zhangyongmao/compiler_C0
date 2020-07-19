#ifndef GLOBAL_DEF
#define GLOBAL_DEF
#include<map>
#include<string>

// 全局常量、结构体、类的定义


enum TYPE{      // token 类型
    DEFAULT,    //初始状态
    END,        //文件结束
    NUM,        //数字
    CHAR,       //字符
    STR,        //字符串
    ID,         //标识符
    PUN,        //标点符号
    KW_CHAR,    //字符类型
    KW_INT,     //int类型
    KW_IF,      //if
    KW_ELSE,    //else
    KW_CONST,   //const
    KW_WHILE,   //while
    KW_VOID,    //void
    KW_RETURN,  //return
    KW_DEF,     //def
    KW_BREAK,   //break
    KW_CONTINUE,//continue
    KW_PRINTF   //printf
};


struct Token{
    TYPE type;
    std::string word;
    int row;
    // int num_value;
};


struct Midcode{
    std::string op;
    std::string s1;
    std::string s2;
    std::string s3;
};


struct Symbol{
    std::string name;    //变量名
    std::string type;    //类型
    int value;      //值
    int size;       //大小(变量为0，数组为数组大小)
    int bytes;      //每个数据的字节数(int 为 4， char 为 1)
    int address;    //偏移字节数，地址

};

struct Func{
    std::string name;    //函数名
    int param_num;      //参数数量
    int param_bytesize;  //参数字节大小
    bool is_return;      //是否有返回值
};

class Symbol_Table{
    public:
        int Num_var;
        std::map<std::string, Func> map_func;
        std::map<std::string, int> index_var; // const、global、func_name   + ":" + var_name
        std::vector<Symbol> All_var;
};

#endif