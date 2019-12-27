#include<iostream>
#include<stdio.h>
#include<fstream>
#include<vector>
#include<algorithm>
#include<iomanip>
#include <stdlib.h>
#include "cstdlib"
#include "sstream"
#include<string>
using namespace std;

vector<string> regs;

vector<int> regstime;

int usetime = 0;
vector<string> op;
vector<string> op1;
vector<string> op2;
vector<string> addr;
vector<string> symbol_type;
vector<string> symbol_name;

void word_scanner(FILE* fpsymbol, FILE* equ) {
	char ch = ' ';
	string arr = "";

	/*这里进行的是扫描symbol*/
	ch = fgetc(fpsymbol);//先读取一个 1
	while (ch != EOF) {
		//序号
		while (fgetc(fpsymbol) != '\t') {
			//不进行操作 ，不取序号！
		}

		//入口地址
		ch = fgetc(fpsymbol);
		while (ch != '\t')
		{
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		symbol_type.push_back(arr.c_str());
		arr = "";

		//符号
		ch = fgetc(fpsymbol);
		while (ch != '\n') {
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		/*cout << "符号"<<word_num<<":\t" << arr << endl;*///已经检验扫描token文件正确了
		symbol_name.push_back(arr.c_str());
		arr = "";
		ch = fgetc(fpsymbol);

	}

	/*这里进行的是扫描equ符号*/
	ch = fgetc(equ);//先读取一个
	//equ 文件
	while (ch != EOF) {
		//操作符
		while (ch != '\t') {
			arr += ch;
			ch = fgetc(equ);
		}
		op.push_back(arr.c_str());
		arr = "";

		//op1
		ch = fgetc(equ);
		while (ch != '\t') {
			arr += ch;
			ch = fgetc(equ);
		}
		op1.push_back(arr.c_str());
		arr = "";

		//op2
		ch = fgetc(equ);
		while (ch != '\t') {
			arr += ch;
			ch = fgetc(equ);
		}
		/*cout << "符号"<<symbol_num<<":\t" << arr << endl;*///已经检验扫描symbol文件正确了
		op2.push_back(arr.c_str());
		/*ch = fgetc(equ);*/
		arr = "";

		//addr
		ch = fgetc(equ);
		while (ch != '\n') {
			arr += ch;
			ch = fgetc(equ);
		}
		/*cout << "符号"<<symbol_num<<":\t" << arr << endl;*///已经检验扫描symbol文件正确了
		addr.push_back(arr.c_str());
		ch = fgetc(equ);
		arr = "";
	}

}

void initialRegs() {
	//初始化寄存器数组,准备4个位置(ax,bx,cx,dx)放四个寄存器 用来存放放置的变量名 先都初始化为""
	//而且只用 bx 和 dx
	regs.push_back("");		//ax
	regs.push_back("");		//bx *	
	regs.push_back("");		//cx
	regs.push_back("");		//dx *
	//初始化时，将每个寄存器的使用时间都置为0
	regstime.push_back(0);
	regstime.push_back(0);
	regstime.push_back(0);
	regstime.push_back(0);
}

/* 输入 变量名字 返回应该分配的寄存器名字 */
string distribute(string str) {
	if ( regs[1] == "" )
	{//BX为空
		regs[1] = str;
		regstime[1] = usetime;
		return "BX";
	}
	else if (regs[1] == str) {//BX已分配给str
		regstime[1] = usetime;
		return "BX1";
	}
	else if ( regs[3] == "") 
	{//DX为空或者已经分配给str
		regs[3] = str;
		regstime[3] = usetime;
		return "DX";
	}
	else if (regs[3] == str) {//DX已经分配给了str
		regstime[3] = usetime;
		return "DX1";
	}
	else {//这种情况，则通过上次使用时间来分配
		if (regstime[1] >= regstime[3] )
		{//usetime 越大，说明越后被使用，应该分配数字小的那个
			regs[3] = str;
			regstime[3] = usetime;
			return "DX";
		}
		else {
			regs[1] = str;
			regstime[1] = usetime;
			return "BX";
		}
	}
}

int main() {
	FILE* fpequ;
	FILE* fpsymbol;
	fpequ = fopen("equ.txt", "r");
	fpsymbol = fopen("symbol.txt", "r");
	if (fpequ != NULL )
	{
	}
	else {
		cout << "equ文件打开失败" << endl;
	}
	if (fpsymbol != NULL )
	{
	}
	else {
		cout << "无法打开symbol文件" << endl;
	}
	//初始化
	initialRegs();
	word_scanner(fpsymbol, fpequ);
	for (int i = 0; i < op.size(); i++)
	{
		cout << op[i] << "\t" << op1[i] << "\t" << op[2] << "\t" << addr[i] << endl;
	}
	for (int i = 0; i < op.size() ; i++)
	{
		if (op[i] == "41") 
		{//运算符为*
			usetime++;
			stringstream ss;
			ss << op1[i];
			int num1;
			ss >> num1;
			stringstream ss2;
			ss2 << op2[i];
			int num2;
			ss2 >> num2;
			stringstream ss3;
			ss3 << addr[i];
			int num3;
			ss3 >> num3;
			string op1name = symbol_name[(int)num1 - 1];
			string op2name = symbol_name[(int)num2 - 1];
			string addrname = symbol_name[(int)num3 - 1];
			string now_Reg = distribute(op1name);
			if (now_Reg == "BX1")
			{
				cout << "MUL\t" << "BX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tBX" << endl;
			}
			else if (now_Reg == "DX1") {
				cout << "MUL\t" << "DX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tDX" << endl;
			}
			else {
				cout << "MOV\t" << now_Reg << "\t" << op1name << endl;
				cout << "MUL\t" << now_Reg << "\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\t" << now_Reg << endl;
			}
		}
		else if (op[i] == "43" )
		{//运算符为+
			usetime++;
			stringstream ss;
			ss << op1[i];
			int num1;
			ss >> num1;
			stringstream ss2;
			ss2 << op2[i];
			int num2;
			ss2 >> num2;
			stringstream ss3;
			ss3 << addr[i];
			int num3;
			ss3 >> num3;
			string op1name = symbol_name[(int)num1 - 1];
			string op2name = symbol_name[(int)num2 - 1];
			string addrname = symbol_name[(int)num3 - 1];
			string now_Reg = distribute(op1name);
			if (now_Reg == "BX1")
			{
				cout << "ADD\t" << "BX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tBX" << endl;
			}
			else if (now_Reg == "DX1") {
				cout << "ADD\t" << "DX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tDX" << endl;
			}
			else {
				cout << "MOV\t" << now_Reg << "\t" << op1name << endl;
				cout << "ADD\t" << now_Reg << "\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\t" << now_Reg << endl;
			}
		}
		else if (op[i] == "45") {//运算符为-
			usetime++;
			stringstream ss;
			ss << op1[i];
			int num1;
			ss >> num1;
			stringstream ss2;
			ss2 << op2[i];
			int num2;
			ss2 >> num2;
			stringstream ss3;
			ss3 << addr[i];
			int num3;
			ss3 >> num3;
			string op1name = symbol_name[(int)num1 - 1];
			string op2name = symbol_name[(int)num2 - 1];
			string addrname = symbol_name[(int)num3 - 1];
			string now_Reg = distribute(op1name);
			if (now_Reg == "BX1")
			{
				cout << "SUB\t" << "BX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tBX" << endl;
			}
			else if (now_Reg == "DX1") {
				cout << "SUB\t" << "DX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tDX" << endl;
			}
			else {
				cout << "MOV\t" << now_Reg << "\t" << op1name << endl;
				cout << "SUB\t" << now_Reg << "\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\t" << now_Reg << endl;
			}
		}
		else if (op[i] == "48") {//运算符为/
			usetime++;
			stringstream ss;
			ss << op1[i];
			int num1;
			ss >> num1;
			stringstream ss2;
			ss2 << op2[i];
			int num2;
			ss2 >> num2;
			stringstream ss3;
			ss3 << addr[i];
			int num3;
			ss3 >> num3;
			string op1name = symbol_name[(int)num1 - 1];
			string op2name = symbol_name[(int)num2 - 1];
			string addrname = symbol_name[(int)num3 - 1];
			string now_Reg = distribute(op1name);
			if (now_Reg == "BX1")
			{
				cout << "DIV\t" << "BX\t" << op2name << endl;
				cout << "MOV\t" <<addrname << "\tBX" << endl;
			}
			else if (now_Reg == "DX1") {
				cout << "DIV\t" << "DX\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\tDX" << endl;
			}
			else {
				cout << "MOV\t" << now_Reg << "\t" << op1name << endl;
				cout << "DIV\t" << now_Reg << "\t" << op2name << endl;
				cout << "MOV\t" << addrname << "\t"<<now_Reg << endl;
			}
		}
		else if(op[i] == "51"){//运算符为:=
		usetime++;
		stringstream ss;
		ss << op1[i];
		int num1;
		ss >> num1;
		stringstream ss2;
		ss2 << op2[i];
		int num2;
		ss2 >> num2;
		stringstream ss3;
		ss3 << addr[i];
		int num3;
		ss3 >> num3;
		string op1name = symbol_name[(int)num1 - 1];
		string addrname = symbol_name[(int)num3 - 1];
		string now_Reg = distribute(op1name);
		if (now_Reg == "BX1")
		{
			cout << "MOV\t" << addrname << "\tBX" << endl;
		}
		else if (now_Reg == "DX1") {
			cout << "MOV\t" << addrname << "\tDX" << endl;
		}
		else {
			cout << "MOV\t" << now_Reg << "\t" << op1name << endl;
			cout << "MOV\t" << addrname << "\t" << now_Reg << endl;
		}
		}
		else if (op[i] == "j" && op1[i] == "_" && op2[i] == "_") {
		cout << "JMP" <<"\t" << addr[i] << endl;
}
		else if (op[i] == "j>=") {
		stringstream ss;
		ss << op1[i];
		int num1;
		ss >> num1;
		stringstream ss2;
		ss2 << op2[i];
		int num2;
		ss2 >> num2;
		cout << "CMP\t" << symbol_name[(int)num1 - 1] << "\t" << symbol_name[(int)num2 - 1] << endl;
		cout << "JGE" << "\t" << addr[i]<<endl;
		}
		else if (op[i] == "j=") {
		stringstream ss;
		ss << op1[i];
		int num1;
		ss >> num1;
		stringstream ss2;
		ss2 << op2[i];
		int num2;
		ss2 >> num2;
		cout << "CMP\t" << symbol_name[(int)num1 - 1] << "\t" << symbol_name[(int)num2 - 1] << endl;
		cout << "JZ" << "\t" << addr[i]<<endl;
		}
		else if (op[i] == "j!=") {
		stringstream ss;
		ss << op1[i];
		int num1;
		ss >> num1;
		stringstream ss2;
		ss2 << op2[i];
		int num2;
		ss2 >> num2;
		cout << "CMP\t" << symbol_name[(int)num1 - 1] << "\t" << symbol_name[(int)num2 - 1] << endl;
		cout << "JNZ" << "\t" << addr[i]<<endl;
		}
		else if (op[i] == "j>") {
		stringstream ss;
		ss << op1[i];
		int num1;
		ss >> num1;
		stringstream ss2;
		ss2 << op2[i];
		int num2;
		ss2 >> num2;
		cout << "CMP\t" << symbol_name[(int)num1 - 1] << "\t" << symbol_name[(int)num2 - 1] << endl;
		cout << "JG" << "\t" << addr[i]<<endl;
		}
		else if (op[i] == "j<") {
		stringstream ss;
		ss << op1[i];
		int num1;
		ss >> num1;
		stringstream ss2;
		ss2 << op2[i];
		int num2;
		ss2 >> num2;
		cout << "CMP\t" << symbol_name[(int)num1 - 1] << "\t" << symbol_name[(int)num2 - 1] << endl;
		cout << "JL" << "\t" << addr[i]<<endl;
		}

	}
}