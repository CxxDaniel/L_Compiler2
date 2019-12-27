#include<iostream>
#include<stdio.h>
#include<fstream>
#include<vector>
#include<algorithm>
#include<iomanip>
#include <stdlib.h>
#include<string>

using namespace std;

//token文件结构
typedef struct token {
	int label;//单词的序号
	char name[30];//单词本身
	int code;//单词的机内码
	int addr; //单词为保留字时为-1，为标识符或常数时为大于1的数，（符号表入口的地址）
}token;

//符号表文件结构
typedef struct symbol {
	int number;//序号
	int type;//类型
	char name[30];//名字
}symbol;

/*
机内码  分别判断 是否保留字，是否运算符 是否常数和标识符
	   1--- 保留字 17个
         and begin bool do else end false if integer not or program real then true var while
	   2--- 运算符 17个
	     （ ） + - * / . , : ; := = <= >= < > <> 
	   3--- 标识符 
	   4--- 常数： 实数、整数
	     
*/




string KEYWORD[20] = {"and","begin","bool","do","else","end","false",
    "if","integer","not","or","program","real","then","var","while",":=","<=",">=","<>"};
char SEPARATER[6] = { ';',',','(',')',':','.' };    //分隔符
char OPERATOR[7] = { '+','-','*','/','>','<','='};     //运算符
char FILTER[4] = { ' ','\t','\r','\n' };                    //过滤符

//想一个寻找机内码的方法
string search_code[37] = {"and","begin","bool","do","else","end","false","if","integer","not","or","program",
"real","then","true","var","while","标识符","整数","实数","(",")","+","-","*","/",".",",",":",";",":=","=","<=","<","<>",">",">="};
/*返回机内码*/
int get_code(string arr) {
    for (int i = 0; i < 37; i++)
    {
        if (arr == search_code[i])
        {
            return (i + 1);
        }

    }
    return -1;
}

//建立一个100单位的符号表数组，这个数组存放加入符号表的标识符和常数
string symbol_list[100] = {};
//这个int型的用来记录符号表的符号的个数，也用来对每一将加入符号表时对数组扫描，减少冗余时间。
int symbol_num = 0;
//这个函数用来返回想要输入符号表的字符在符号表的位置，不存在返回-1.
int get_symbol_addr(string arr) {
    for (int i = 0; i < symbol_num; i++)
    {
        if (arr == symbol_list[i])
        {
            return i;
        }
    }
    return -1;
}

//机内码
const int IDENTIFIER = 18;         //标识符机内码
const int CONSTANT = 19;           //整数机内码
const int FILTER_VALUE = 20;       //实数机内码




/**判断是否为关键字**/
bool IsKeyword(string word) {
    for (int i = 0; i < 20; i++) {
        if (KEYWORD[i] == word) {
            return true;
        }
    }
    return false;
}

/**判断是否为分隔符**/
bool IsSeparater(char ch) {
    for (int i = 0; i < 6; i++) {
        if (SEPARATER[i] == ch) {
            return true;
        }
    }
    return false;
}

/**判断是否为运算符**/
bool IsOperator(char ch) {
    for (int i = 0; i < 7; i++) {
        if (OPERATOR[i] == ch) {
            return true;
        }
    }
    return false;
}

/**判断是否为过滤符**/
bool IsFilter(char ch) {
    for (int i = 0; i < 4; i++) {
        if (FILTER[i] == ch) {
            return true;
        }
    }
    return false;
}


/**判断是否为大写字母**/
bool IsUpLetter(char ch) {
    if (ch >= 'A' && ch <= 'Z') return true;
    return false;
}

/**判断是否为小写字母**/
bool IsLowLetter(char ch) {
    if (ch >= 'a' && ch <= 'z') return true;
    return false;
}

/**判断是否为数字**/
bool IsDigit(char ch) {
    if (ch >= '0' && ch <= '9') return true;
    return false;
}


/**返回每个字的值**/
template <class T>
int value(T* a, int n, T str) {
    for (int i = 0; i < n; i++) {
        if (a[i] == str) return i + 1;
    }
    return -1;
}


/**词法分析**/
void analyse(FILE* fpin,FILE* fpout,FILE* fpsymbol) {
    char ch = ' ';
    string arr = "";
    //token文件中的序号
    int com_label = 1;
    //机内码 ，初始值赋-1 ，再根据get_code的函数获取值
    int code = -1;
    //错误编号 
    int error_no = 0;
    //做一个int行号，用于那啥错误处理
    int line_no = 1;


    //

    while ((ch = fgetc(fpin)) != EOF) {
        arr = "";
        if (IsFilter(ch)) {     //判断是否为过滤符
            if (ch == '\n')
            {
                line_no++;//遇到换行，行数加一
            }

        }
        else if (IsLowLetter(ch)) {       //判断是否为关键字
            while (IsLowLetter(ch)) {
                arr += ch;
                ch = fgetc(fpin);
            }
            fseek(fpin,-1L,SEEK_CUR);
            if (IsKeyword(arr)) {   //保留字
                
                //在这儿输出token  固定的格式
                fprintf(fpout,"%d\t",com_label);    //label 序号
                com_label++;                        
                fprintf(fpout, "%s\t",arr.c_str()); //name 单词
                fprintf(fpout, "%d\t", get_code(arr));//code 机内码
                fprintf(fpout,"%d\n",-1);             //addr 符号表中的入口
                
            }
            else
            {                       //标识符

                // 1.symbol符号表文件
                //先检查符号表是否有出现过同一符号表
                if (get_symbol_addr(arr) != -1)
                {//该标识符已经存在 符号表
                    //已存在符号表的标识符则不继续进行操作。
                }
                else {//符号表还未记录该符号
                    symbol_list[symbol_num] = arr;
                    symbol_num++;
                    fprintf(fpsymbol, "%d\t", symbol_num);
                    fprintf(fpsymbol, "%d\t", 18);
                    fprintf(fpsymbol, "%s\n", arr.c_str());
                }

                // 2.token文件
                fprintf(fpout, "%d\t", com_label);    //label 序号
                com_label++;
                fprintf(fpout, "%s\t", arr.c_str()); //name 单词
                fprintf(fpout, "%d\t", 18);//code 机内码
                fprintf(fpout, "%d\n", get_symbol_addr(arr)+1  );             //addr 符号表中的入口

                
            }
        }
        else if (IsDigit(ch)) {           //判断是否为数字

            //此处加一flag标志，判断有无多个小数点的错误！
            int dot_flag = 0;
            //此处加一flag标志，判断有没有数字开头的标识符！
            int ident_flag = 0;
            //若出现错误，则换一个error_arr来记录下改正的数字。
            string error_arr = "";
            //再多加一个flag标志用于判断，实数的小数部分出现字母的错误。
            int flag_dot_and_ident = 0;
            while (IsDigit(ch) || (ch == '.') || IsUpLetter(ch) || IsLowLetter(ch) ) {//不知道啥意思的   && IsDigit(fgetc(fpin))

                //该数字每多一个小数点flag加1
                if (ch == '.')
                {
                    dot_flag++;
                    if (dot_flag == 2)
                    {
                        error_arr = arr;
                    }
                }
                if ( IsUpLetter(ch) || IsLowLetter(ch) && dot_flag > 0 )
                {
                    flag_dot_and_ident++;       //标识出小数部分有字母的错误！
                    ident_flag++;               //如果数字后面有字母，说明出现数字开头标识符问题
                    error_arr = arr;
                }
                else if (IsUpLetter(ch) || IsLowLetter(ch) && dot_flag == 0) {
                    ident_flag++;               //如果数字后面有字母，说明出现数字开头标识符问题
                    error_arr = arr;
                }

                arr += ch;
                ch = fgetc(fpin);
            }
            fseek(fpin, -1L, SEEK_CUR);

            if (flag_dot_and_ident != 0 )
            {
                error_no++;
                cout << "错误序号:" << error_no << "\t" << arr << "\t行号：" << line_no << "\t" << "详细说明 ：常数的小数部分不能存在字母！" << endl;
                if (dot_flag == 0)
                {       //标识符出错，改为的整数

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 19);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name 单词
                    fprintf(fpout, "%d\t", 19);//code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr)+1 );             //addr 符号表中的入口
                    
                }
                else {//标识符出错，改为的实数

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name 单词
                    fprintf(fpout, "%d\t", 20);//code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr)+1 );             //addr 符号表中的入口
                    
                }
            }
            else if (flag_dot_and_ident == 0 && ident_flag != 0) {
                error_no++;
                cout << "错误序号:" << error_no << "\t" << arr << "\t行号：" << line_no << "\t" << "详细说明 ：标识符不能以数字开头！" << endl;
                if (dot_flag == 0)
                {       //标识符出错，改为的整数

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 19);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name 单词
                    fprintf(fpout, "%d\t", 19);//code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr) + 1);             //addr 符号表中的入口

                }
                else {//标识符出错，改为的实数

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name 单词
                    fprintf(fpout, "%d\t", 20);//code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr) + 1);             //addr 符号表中的入口

                }
            
            }
            else {

                if (dot_flag >= 2  )
                {//出现多个小数点的错误
                    error_no++;
                    cout << "错误序号:" << error_no << "\t" << arr << "\t行号：" << line_no << "\t" << "详细说明 ：数字不能存在多个小数点！" << endl;

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    //自动改错后加入token文件，  且一定是实数，不需要再去判断
                    fprintf(fpout, "%d\t", com_label);    //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name 单词
                    fprintf(fpout, "%d\t", 20);//code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr)+1 );             //addr 符号表中的入口
                    

                }
                else if (dot_flag == 0)
                {  
                    if (get_symbol_addr(arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 19);
                        fprintf(fpsymbol, "%s\n", arr.c_str());
                    }

                    //整数
                    fprintf(fpout, "%d\t", com_label);    //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", arr.c_str()); //name 单词
                    fprintf(fpout, "%d\t", 19);//code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(arr)+1  );             //addr 符号表中的入口
                    

                }
                else
                {
                    if (get_symbol_addr(arr) != -1)
                    {
                        //该常数已存在符号表，不进行任何操作
                    }
                    else {
                        symbol_list[symbol_num] = arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", arr.c_str());
                    }

                    //实数
                    fprintf(fpout, "%d\t", com_label);                      //label 序号
                    com_label++;
                    fprintf(fpout, "%s\t", arr.c_str());                     //name 单词
                    fprintf(fpout, "%d\t", 20);                             //code 机内码
                    fprintf(fpout, "%d\n", get_symbol_addr(arr)+1 );             //addr 符号表中的入口

                    
                }
            }

        }
        else if (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_') {
            while (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_' || IsDigit(ch)) {
                arr += ch;
                ch = fgetc(fpin);
            }
            fseek(fpin, -1L, SEEK_CUR);
            // 1.symbol符号表文件
                //先检查符号表是否有出现过同一符号表
            if (get_symbol_addr(arr) != -1)
            {//该标识符已经存在 符号表
                //已存在符号表的标识符则不继续进行操作。
            }
            else {//符号表还未记录该符号
                symbol_list[symbol_num] = arr;
                symbol_num++;
                fprintf(fpsymbol, "%d\t", symbol_num);
                fprintf(fpsymbol, "%d\t", 18);
                fprintf(fpsymbol, "%s\n", arr.c_str());
            }

            // 2.token文件
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", arr.c_str()); //name 单词
            fprintf(fpout, "%d\t", 18);//code 机内码
            fprintf(fpout, "%d\n", get_symbol_addr(arr) + 1);             //addr 符号表中的入口

        }
        else if (ch == ':') {
        if (fgetc(fpin) == '=' )
        {   // 判断为 :=
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", ":="); //name 单词
            fprintf(fpout, "%d\t", get_code(":="));//code 机内码
            fprintf(fpout, "%d\n", -1 );             //addr 符号表中的入口
        }
        else {//判断为 ：
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", ":"); //name 单词
            fprintf(fpout, "%d\t", get_code(":"));//code 机内码
            fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
            fseek(fpin, -1L, SEEK_CUR);
        }
        }
        else if (ch == '<'  ) {
            //判断是否为 <=   <>
            char tem = fgetc(fpin);
            if (tem == '=') {//判断为 <=
                fprintf(fpout, "%d\t", com_label);    //label 序号
                com_label++;
                fprintf(fpout, "%s\t", "<="); //name 单词
                fprintf(fpout, "%d\t", get_code("<="));//code 机内码
                fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
            }
            else if(tem == '>' ) {
                fprintf(fpout, "%d\t", com_label);    //label 序号
                com_label++;
                fprintf(fpout, "%s\t", "<>"); //name 单词
                fprintf(fpout, "%d\t", get_code("<>"));//code 机内码
                fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
            }
            else {
                fprintf(fpout, "%d\t", com_label);    //label 序号
                com_label++;
                fprintf(fpout, "%s\t", "<"); //name 单词
                fprintf(fpout, "%d\t", get_code("<"));//code 机内码
                fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
                fseek(fpin, -1L, SEEK_CUR);
            }
        }
        else if (ch == '>') {
        if (fgetc(fpin) == '=') {
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", ">="); //name 单词
            fprintf(fpout, "%d\t", get_code(">="));//code 机内码
            fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
        }
        else {
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", ">"); //name 单词
            fprintf(fpout, "%d\t", get_code(">"));//code 机内码
            fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
            fseek(fpin, -1L, SEEK_CUR);
        }
        }
        else switch (ch) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        {
            arr += ch;
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", arr.c_str()); //name 单词
            fprintf(fpout, "%d\t", get_code(arr.c_str()));//code 机内码
            fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
            break;
        }
        case ';':
        case ',':
        case '(':
        case ')':
        {
            arr += ch;
            fprintf(fpout, "%d\t", com_label);    //label 序号
            com_label++;
            fprintf(fpout, "%s\t", arr.c_str()); //name 单词
            fprintf(fpout, "%d\t", get_code(arr));//code 机内码
            fprintf(fpout, "%d\n", -1);             //addr 符号表中的入口
            break;
        }
        default:cout <<  line_no <<"\t"<<ch<< endl;
        }
    }


}


int main()
{
    char inFile[40];
    string outfilename = "token.txt";
    string symbolfilename = "symbol.txt";
    FILE* fpin;
    FILE* fpout;
    FILE* fpsymbol;
    //判断token文件能被打开
    if ((fpout = fopen(outfilename.data(),"w")) == NULL)
    {
        cout << "token文件被打开或是被删除，出现错误请检查！" << endl;
        return 0;
    }
    //判断symbol文件能被打开
    if ((fpsymbol = fopen(symbolfilename.data(), "w")) == NULL)
    {
        cout << "符号表symbol文件被打开或是被删除，出现错误请检查！" << endl;
        return 0;
    }


    cout << "请输入L语言文件名（路径+后缀）:";
    while (true) {
        cin >> inFile;
        if ((fpin = fopen(inFile, "r")) != NULL)
            break;
        else {
            cout << "文件名错误！" << endl;
            cout << "请输入L语言文件名（路径+后缀）:";
        }

    }


    cout << "------正在进行词法分析------" << endl;
    analyse(fpin,fpout,fpsymbol);
    fclose(fpin);
    fclose(fpout);
    return 0;
}
