#include<iostream>
#include<stdio.h>
#include<fstream>
#include<vector>
#include<algorithm>
#include<iomanip>
#include <stdlib.h>
#include<string>

using namespace std;

//token�ļ��ṹ
typedef struct token {
	int label;//���ʵ����
	char name[30];//���ʱ���
	int code;//���ʵĻ�����
	int addr; //����Ϊ������ʱΪ-1��Ϊ��ʶ������ʱΪ����1�����������ű���ڵĵ�ַ��
}token;

//���ű��ļ��ṹ
typedef struct symbol {
	int number;//���
	int type;//����
	char name[30];//����
}symbol;

/*
������  �ֱ��ж� �Ƿ����֣��Ƿ������ �Ƿ����ͱ�ʶ��
	   1--- ������ 17��
         and begin bool do else end false if integer not or program real then true var while
	   2--- ����� 17��
	     �� �� + - * / . , : ; := = <= >= < > <> 
	   3--- ��ʶ�� 
	   4--- ������ ʵ��������
	     
*/




string KEYWORD[20] = {"and","begin","bool","do","else","end","false",
    "if","integer","not","or","program","real","then","var","while",":=","<=",">=","<>"};
char SEPARATER[6] = { ';',',','(',')',':','.' };    //�ָ���
char OPERATOR[7] = { '+','-','*','/','>','<','='};     //�����
char FILTER[4] = { ' ','\t','\r','\n' };                    //���˷�

//��һ��Ѱ�һ�����ķ���
string search_code[37] = {"and","begin","bool","do","else","end","false","if","integer","not","or","program",
"real","then","true","var","while","��ʶ��","����","ʵ��","(",")","+","-","*","/",".",",",":",";",":=","=","<=","<","<>",">",">="};
/*���ػ�����*/
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

//����һ��100��λ�ķ��ű����飬��������ż�����ű�ı�ʶ���ͳ���
string symbol_list[100] = {};
//���int�͵�������¼���ű�ķ��ŵĸ�����Ҳ������ÿһ��������ű�ʱ������ɨ�裬��������ʱ�䡣
int symbol_num = 0;
//�����������������Ҫ������ű���ַ��ڷ��ű��λ�ã������ڷ���-1.
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

//������
const int IDENTIFIER = 18;         //��ʶ��������
const int CONSTANT = 19;           //����������
const int FILTER_VALUE = 20;       //ʵ��������




/**�ж��Ƿ�Ϊ�ؼ���**/
bool IsKeyword(string word) {
    for (int i = 0; i < 20; i++) {
        if (KEYWORD[i] == word) {
            return true;
        }
    }
    return false;
}

/**�ж��Ƿ�Ϊ�ָ���**/
bool IsSeparater(char ch) {
    for (int i = 0; i < 6; i++) {
        if (SEPARATER[i] == ch) {
            return true;
        }
    }
    return false;
}

/**�ж��Ƿ�Ϊ�����**/
bool IsOperator(char ch) {
    for (int i = 0; i < 7; i++) {
        if (OPERATOR[i] == ch) {
            return true;
        }
    }
    return false;
}

/**�ж��Ƿ�Ϊ���˷�**/
bool IsFilter(char ch) {
    for (int i = 0; i < 4; i++) {
        if (FILTER[i] == ch) {
            return true;
        }
    }
    return false;
}


/**�ж��Ƿ�Ϊ��д��ĸ**/
bool IsUpLetter(char ch) {
    if (ch >= 'A' && ch <= 'Z') return true;
    return false;
}

/**�ж��Ƿ�ΪСд��ĸ**/
bool IsLowLetter(char ch) {
    if (ch >= 'a' && ch <= 'z') return true;
    return false;
}

/**�ж��Ƿ�Ϊ����**/
bool IsDigit(char ch) {
    if (ch >= '0' && ch <= '9') return true;
    return false;
}


/**����ÿ���ֵ�ֵ**/
template <class T>
int value(T* a, int n, T str) {
    for (int i = 0; i < n; i++) {
        if (a[i] == str) return i + 1;
    }
    return -1;
}


/**�ʷ�����**/
void analyse(FILE* fpin,FILE* fpout,FILE* fpsymbol) {
    char ch = ' ';
    string arr = "";
    //token�ļ��е����
    int com_label = 1;
    //������ ����ʼֵ��-1 ���ٸ���get_code�ĺ�����ȡֵ
    int code = -1;
    //������ 
    int error_no = 0;
    //��һ��int�кţ�������ɶ������
    int line_no = 1;


    //

    while ((ch = fgetc(fpin)) != EOF) {
        arr = "";
        if (IsFilter(ch)) {     //�ж��Ƿ�Ϊ���˷�
            if (ch == '\n')
            {
                line_no++;//�������У�������һ
            }

        }
        else if (IsLowLetter(ch)) {       //�ж��Ƿ�Ϊ�ؼ���
            while (IsLowLetter(ch)) {
                arr += ch;
                ch = fgetc(fpin);
            }
            fseek(fpin,-1L,SEEK_CUR);
            if (IsKeyword(arr)) {   //������
                
                //��������token  �̶��ĸ�ʽ
                fprintf(fpout,"%d\t",com_label);    //label ���
                com_label++;                        
                fprintf(fpout, "%s\t",arr.c_str()); //name ����
                fprintf(fpout, "%d\t", get_code(arr));//code ������
                fprintf(fpout,"%d\n",-1);             //addr ���ű��е����
                
            }
            else
            {                       //��ʶ��

                // 1.symbol���ű��ļ�
                //�ȼ����ű��Ƿ��г��ֹ�ͬһ���ű�
                if (get_symbol_addr(arr) != -1)
                {//�ñ�ʶ���Ѿ����� ���ű�
                    //�Ѵ��ڷ��ű�ı�ʶ���򲻼������в�����
                }
                else {//���ű�δ��¼�÷���
                    symbol_list[symbol_num] = arr;
                    symbol_num++;
                    fprintf(fpsymbol, "%d\t", symbol_num);
                    fprintf(fpsymbol, "%d\t", 18);
                    fprintf(fpsymbol, "%s\n", arr.c_str());
                }

                // 2.token�ļ�
                fprintf(fpout, "%d\t", com_label);    //label ���
                com_label++;
                fprintf(fpout, "%s\t", arr.c_str()); //name ����
                fprintf(fpout, "%d\t", 18);//code ������
                fprintf(fpout, "%d\n", get_symbol_addr(arr)+1  );             //addr ���ű��е����

                
            }
        }
        else if (IsDigit(ch)) {           //�ж��Ƿ�Ϊ����

            //�˴���һflag��־���ж����޶��С����Ĵ���
            int dot_flag = 0;
            //�˴���һflag��־���ж���û�����ֿ�ͷ�ı�ʶ����
            int ident_flag = 0;
            //�����ִ�����һ��error_arr����¼�¸��������֡�
            string error_arr = "";
            //�ٶ��һ��flag��־�����жϣ�ʵ����С�����ֳ�����ĸ�Ĵ���
            int flag_dot_and_ident = 0;
            while (IsDigit(ch) || (ch == '.') || IsUpLetter(ch) || IsLowLetter(ch) ) {//��֪��ɶ��˼��   && IsDigit(fgetc(fpin))

                //������ÿ��һ��С����flag��1
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
                    flag_dot_and_ident++;       //��ʶ��С����������ĸ�Ĵ���
                    ident_flag++;               //������ֺ�������ĸ��˵���������ֿ�ͷ��ʶ������
                    error_arr = arr;
                }
                else if (IsUpLetter(ch) || IsLowLetter(ch) && dot_flag == 0) {
                    ident_flag++;               //������ֺ�������ĸ��˵���������ֿ�ͷ��ʶ������
                    error_arr = arr;
                }

                arr += ch;
                ch = fgetc(fpin);
            }
            fseek(fpin, -1L, SEEK_CUR);

            if (flag_dot_and_ident != 0 )
            {
                error_no++;
                cout << "�������:" << error_no << "\t" << arr << "\t�кţ�" << line_no << "\t" << "��ϸ˵�� ��������С�����ֲ��ܴ�����ĸ��" << endl;
                if (dot_flag == 0)
                {       //��ʶ��������Ϊ������

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 19);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name ����
                    fprintf(fpout, "%d\t", 19);//code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr)+1 );             //addr ���ű��е����
                    
                }
                else {//��ʶ��������Ϊ��ʵ��

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name ����
                    fprintf(fpout, "%d\t", 20);//code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr)+1 );             //addr ���ű��е����
                    
                }
            }
            else if (flag_dot_and_ident == 0 && ident_flag != 0) {
                error_no++;
                cout << "�������:" << error_no << "\t" << arr << "\t�кţ�" << line_no << "\t" << "��ϸ˵�� ����ʶ�����������ֿ�ͷ��" << endl;
                if (dot_flag == 0)
                {       //��ʶ��������Ϊ������

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 19);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name ����
                    fprintf(fpout, "%d\t", 19);//code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr) + 1);             //addr ���ű��е����

                }
                else {//��ʶ��������Ϊ��ʵ��

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    fprintf(fpout, "%d\t", com_label);    //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name ����
                    fprintf(fpout, "%d\t", 20);//code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr) + 1);             //addr ���ű��е����

                }
            
            }
            else {

                if (dot_flag >= 2  )
                {//���ֶ��С����Ĵ���
                    error_no++;
                    cout << "�������:" << error_no << "\t" << arr << "\t�кţ�" << line_no << "\t" << "��ϸ˵�� �����ֲ��ܴ��ڶ��С���㣡" << endl;

                    if (get_symbol_addr(error_arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = error_arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", error_arr.c_str());
                    }

                    //�Զ��Ĵ�����token�ļ���  ��һ����ʵ��������Ҫ��ȥ�ж�
                    fprintf(fpout, "%d\t", com_label);    //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", error_arr.c_str()); //name ����
                    fprintf(fpout, "%d\t", 20);//code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(error_arr)+1 );             //addr ���ű��е����
                    

                }
                else if (dot_flag == 0)
                {  
                    if (get_symbol_addr(arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 19);
                        fprintf(fpsymbol, "%s\n", arr.c_str());
                    }

                    //����
                    fprintf(fpout, "%d\t", com_label);    //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", arr.c_str()); //name ����
                    fprintf(fpout, "%d\t", 19);//code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(arr)+1  );             //addr ���ű��е����
                    

                }
                else
                {
                    if (get_symbol_addr(arr) != -1)
                    {
                        //�ó����Ѵ��ڷ��ű��������κβ���
                    }
                    else {
                        symbol_list[symbol_num] = arr;
                        symbol_num++;
                        fprintf(fpsymbol, "%d\t", symbol_num);
                        fprintf(fpsymbol, "%d\t", 20);
                        fprintf(fpsymbol, "%s\n", arr.c_str());
                    }

                    //ʵ��
                    fprintf(fpout, "%d\t", com_label);                      //label ���
                    com_label++;
                    fprintf(fpout, "%s\t", arr.c_str());                     //name ����
                    fprintf(fpout, "%d\t", 20);                             //code ������
                    fprintf(fpout, "%d\n", get_symbol_addr(arr)+1 );             //addr ���ű��е����

                    
                }
            }

        }
        else if (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_') {
            while (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_' || IsDigit(ch)) {
                arr += ch;
                ch = fgetc(fpin);
            }
            fseek(fpin, -1L, SEEK_CUR);
            // 1.symbol���ű��ļ�
                //�ȼ����ű��Ƿ��г��ֹ�ͬһ���ű�
            if (get_symbol_addr(arr) != -1)
            {//�ñ�ʶ���Ѿ����� ���ű�
                //�Ѵ��ڷ��ű�ı�ʶ���򲻼������в�����
            }
            else {//���ű�δ��¼�÷���
                symbol_list[symbol_num] = arr;
                symbol_num++;
                fprintf(fpsymbol, "%d\t", symbol_num);
                fprintf(fpsymbol, "%d\t", 18);
                fprintf(fpsymbol, "%s\n", arr.c_str());
            }

            // 2.token�ļ�
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", arr.c_str()); //name ����
            fprintf(fpout, "%d\t", 18);//code ������
            fprintf(fpout, "%d\n", get_symbol_addr(arr) + 1);             //addr ���ű��е����

        }
        else if (ch == ':') {
        if (fgetc(fpin) == '=' )
        {   // �ж�Ϊ :=
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", ":="); //name ����
            fprintf(fpout, "%d\t", get_code(":="));//code ������
            fprintf(fpout, "%d\n", -1 );             //addr ���ű��е����
        }
        else {//�ж�Ϊ ��
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", ":"); //name ����
            fprintf(fpout, "%d\t", get_code(":"));//code ������
            fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
            fseek(fpin, -1L, SEEK_CUR);
        }
        }
        else if (ch == '<'  ) {
            //�ж��Ƿ�Ϊ <=   <>
            char tem = fgetc(fpin);
            if (tem == '=') {//�ж�Ϊ <=
                fprintf(fpout, "%d\t", com_label);    //label ���
                com_label++;
                fprintf(fpout, "%s\t", "<="); //name ����
                fprintf(fpout, "%d\t", get_code("<="));//code ������
                fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
            }
            else if(tem == '>' ) {
                fprintf(fpout, "%d\t", com_label);    //label ���
                com_label++;
                fprintf(fpout, "%s\t", "<>"); //name ����
                fprintf(fpout, "%d\t", get_code("<>"));//code ������
                fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
            }
            else {
                fprintf(fpout, "%d\t", com_label);    //label ���
                com_label++;
                fprintf(fpout, "%s\t", "<"); //name ����
                fprintf(fpout, "%d\t", get_code("<"));//code ������
                fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
                fseek(fpin, -1L, SEEK_CUR);
            }
        }
        else if (ch == '>') {
        if (fgetc(fpin) == '=') {
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", ">="); //name ����
            fprintf(fpout, "%d\t", get_code(">="));//code ������
            fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
        }
        else {
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", ">"); //name ����
            fprintf(fpout, "%d\t", get_code(">"));//code ������
            fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
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
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", arr.c_str()); //name ����
            fprintf(fpout, "%d\t", get_code(arr.c_str()));//code ������
            fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
            break;
        }
        case ';':
        case ',':
        case '(':
        case ')':
        {
            arr += ch;
            fprintf(fpout, "%d\t", com_label);    //label ���
            com_label++;
            fprintf(fpout, "%s\t", arr.c_str()); //name ����
            fprintf(fpout, "%d\t", get_code(arr));//code ������
            fprintf(fpout, "%d\n", -1);             //addr ���ű��е����
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
    //�ж�token�ļ��ܱ���
    if ((fpout = fopen(outfilename.data(),"w")) == NULL)
    {
        cout << "token�ļ����򿪻��Ǳ�ɾ�������ִ������飡" << endl;
        return 0;
    }
    //�ж�symbol�ļ��ܱ���
    if ((fpsymbol = fopen(symbolfilename.data(), "w")) == NULL)
    {
        cout << "���ű�symbol�ļ����򿪻��Ǳ�ɾ�������ִ������飡" << endl;
        return 0;
    }


    cout << "������L�����ļ�����·��+��׺��:";
    while (true) {
        cin >> inFile;
        if ((fpin = fopen(inFile, "r")) != NULL)
            break;
        else {
            cout << "�ļ�������" << endl;
            cout << "������L�����ļ�����·��+��׺��:";
        }

    }


    cout << "------���ڽ��дʷ�����------" << endl;
    analyse(fpin,fpout,fpsymbol);
    fclose(fpin);
    fclose(fpout);
    return 0;
}
