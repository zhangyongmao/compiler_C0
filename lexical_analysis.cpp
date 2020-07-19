#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include"global_def.h"

using namespace std;

vector<Token> word_result;
ifstream infile;
string buffer;
char current_char;
int num_buffer = 0;
int row = 0;

vector<Token> word_anaylze(string input_file);
void lexical_init(string input_file);
void get_char();
char look_char();

vector<Token> word_anaylze(string input_file){
    lexical_init(input_file);
    get_char();
    while(current_char != EOF){
        if(current_char == ' ' || current_char == '\n' || current_char == '\t'){
            get_char();
            continue;
        }
        Token token;
        token.row = row;
        token.type = DEFAULT;
        token.word = string("");

        if(current_char >= '0' && current_char <= '9'){
            token.type = NUM;
            if(current_char == '0'){
                char next_char = look_char();
                if(next_char >= '0' && next_char <= '9'){
                    cout<<"Error  word anaylze  0 + .. error"<<endl;
                }
                token.word = "0";
                // token.num_value = 0;
                get_char();
            }
            else{
                int tmp_num_value = 0;
                while(current_char >= '0' && current_char <= '9'){
                    token.word += current_char;
                    if(tmp_num_value > INT32_MAX / 10 || tmp_num_value == INT32_MIN / 10 && current_char > '7'){
                        cout<<"Error  word anaylze   int overflow  line = "<<row<<endl;
                    }
                    tmp_num_value = tmp_num_value * 10 + current_char - '0';
                    get_char();
                }
                // token.num_value = tmp_num_value;
            }
            word_result.push_back(token);
        }
        else if(current_char >= 'a' && current_char <= 'z' || current_char >= 'A' && current_char <= 'Z' || current_char == '_'){
            while(current_char >= 'a' && current_char <= 'z' || current_char >= 'A' && current_char <= 'Z' || current_char == '_' 
                    || current_char >= '0' && current_char <= '9'){
                token.word += current_char;
                get_char();
            }
            if(token.word == "if")token.type = KW_IF;
            else if(token.word == "else")token.type = KW_ELSE;
            else if(token.word == "while")token.type = KW_WHILE;
            else if(token.word == "break")token.type = KW_BREAK;
            else if(token.word == "continue")token.type = KW_CONTINUE;
            else if(token.word == "int")token.type = KW_INT;
            else if(token.word == "char")token.type = KW_CHAR;
            else if(token.word == "printf")token.type = KW_PRINTF;
            else if(token.word == "return")token.type = KW_RETURN;
            else if(token.word == "def")token.type = KW_DEF;
            else if(token.word == "const")token.type = KW_CONST;
            else if(token.word == "void")token.type = KW_VOID;
            else token.type = ID;

            word_result.push_back(token);
        }
        else if(current_char == '"'){ // 字符串
            token.type = STR;
            get_char();
            while(current_char != '"'){
                token.word += current_char;
                get_char();
                if(current_char == EOF){
                    cout<<"Error  word anayle  STR end"<<endl;
                }
            }
            word_result.push_back(token);
            get_char();
        }
        else if(current_char == '\''){ // 字符
            token.type = CHAR;
            get_char();
            token.word += current_char;
            get_char();
            if(current_char != '\''){
                cout<<"Error  word anayle  CHAR end"<<endl;
            }
            word_result.push_back(token);
            get_char();
        }
        else{ // 标点符号
            token.type = STR;
            if(current_char == '+')token.word = "+";
            else if(current_char == '-')token.word = "-";
            else if(current_char == '*')token.word = "*";
            else if(current_char == '/')token.word = "/";
            else if(current_char == '(')token.word = "(";
            else if(current_char == ')')token.word = ")";
            else if(current_char == '[')token.word = "[";
            else if(current_char == ']')token.word = "]";
            else if(current_char == '{')token.word = "{";
            else if(current_char == '}')token.word = "}";
            else if(current_char == ',')token.word = ",";
            else if(current_char == '.')token.word = ".";
            else if(current_char == ';')token.word = ";";
            else if(current_char == ':')token.word = ":";
            else if(current_char == '!'){
                if(look_char() == '='){
                    token.word = "!=";
                    get_char();
                }
                else token.word = "!";
            }
            else if(current_char == '='){
                if(look_char() == '='){
                    token.word = "==";
                    get_char();
                }
                else token.word = "=";
            }
            else if(current_char == '>'){
                if(look_char() == '='){
                    token.word = ">=";
                    get_char();
                }
                else token.word = ">";
            }
            else if(current_char == '<'){
                if(look_char() == '='){
                    token.word = "<=";
                    get_char();
                }
                else token.word = "<";
            }
            else if(current_char == '&'){
                if(look_char() == '&'){
                    token.word = "&&";
                    get_char();
                }
            }
            else if(current_char == '|'){
                if(look_char() == '|'){
                    token.word = "||";
                    get_char();
                }
            }
            else{
                cout<<"Error  word anayle  STR  No def char: "<<current_char<<endl;
            }
            word_result.push_back(token);
            get_char();
        }
    }
    return word_result;
}

// 初始化，打开文件
void lexical_init(string input_file){
    infile.open(input_file.data());
}

void get_char(){
    if(num_buffer >= buffer.size()){
        if(getline(infile, buffer)){
            num_buffer = 0;
            current_char = '\n';
            row++;
        }
        else{
            current_char = EOF;
            return;
        }
    }
    else{
        current_char = buffer[num_buffer++];
    }
}

char look_char(){
    if(num_buffer < buffer.size() - 1){
        return buffer[num_buffer ];
    }
    return '\n';
}


// int main(){
//     string file1 = "C:\\Users\\ZYM\\Desktop\\compiler-zym\\test_file\\t.txt";
//     vector<Token> tmp_token = word_anaylze(file1);
//     for(int i = 0; i < tmp_token.size(); i++){
//         cout<<tmp_token[i].row<<" "<<tmp_token[i].type<<" "<<tmp_token[i].word<<endl;
//     }
//     return 0;
// }

