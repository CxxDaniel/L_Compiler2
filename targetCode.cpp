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

	/*������е���ɨ��symbol*/
	ch = fgetc(fpsymbol);//�ȶ�ȡһ�� 1
	while (ch != EOF) {
		//���
		while (fgetc(fpsymbol) != '\t') {
			//�����в��� ����ȡ��ţ�
		}

		//��ڵ�ַ
		ch = fgetc(fpsymbol);
		while (ch != '\t')
		{
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		symbol_type.push_back(arr.c_str());
		arr = "";

		//����
		ch = fgetc(fpsymbol);
		while (ch != '\n') {
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		/*cout << "����"<<word_num<<":\t" << arr << endl;*///�Ѿ�����ɨ��token�ļ���ȷ��
		symbol_name.push_back(arr.c_str());
		arr = "";
		ch = fgetc(fpsymbol);

	}

	/*������е���ɨ��equ����*/
	ch = fgetc(equ);//�ȶ�ȡһ��
	//equ �ļ�
	while (ch != EOF) {
		//������
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
		/*cout << "����"<<symbol_num<<":\t" << arr << endl;*///�Ѿ�����ɨ��symbol�ļ���ȷ��
		op2.push_back(arr.c_str());
		/*ch = fgetc(equ);*/
		arr = "";

		//addr
		ch = fgetc(equ);
		while (ch != '\n') {
			arr += ch;
			ch = fgetc(equ);
		}
		/*cout << "����"<<symbol_num<<":\t" << arr << endl;*///�Ѿ�����ɨ��symbol�ļ���ȷ��
		addr.push_back(arr.c_str());
		ch = fgetc(equ);
		arr = "";
	}

}

void initialRegs() {
	//��ʼ���Ĵ�������,׼��4��λ��(ax,bx,cx,dx)���ĸ��Ĵ��� ������ŷ��õı����� �ȶ���ʼ��Ϊ""
	//����ֻ�� bx �� dx
	regs.push_back("");		//ax
	regs.push_back("");		//bx *	
	regs.push_back("");		//cx
	regs.push_back("");		//dx *
	//��ʼ��ʱ����ÿ���Ĵ�����ʹ��ʱ�䶼��Ϊ0
	regstime.push_back(0);
	regstime.push_back(0);
	regstime.push_back(0);
	regstime.push_back(0);
}

/* ���� �������� ����Ӧ�÷���ļĴ������� */
string distribute(string str) {
	if ( regs[1] == "" )
	{//BXΪ��
		regs[1] = str;
		regstime[1] = usetime;
		return "BX";
	}
	else if (regs[1] == str) {//BX�ѷ����str
		regstime[1] = usetime;
		return "BX1";
	}
	else if ( regs[3] == "") 
	{//DXΪ�ջ����Ѿ������str
		regs[3] = str;
		regstime[3] = usetime;
		return "DX";
	}
	else if (regs[3] == str) {//DX�Ѿ��������str
		regstime[3] = usetime;
		return "DX1";
	}
	else {//�����������ͨ���ϴ�ʹ��ʱ��������
		if (regstime[1] >= regstime[3] )
		{//usetime Խ��˵��Խ��ʹ�ã�Ӧ�÷�������С���Ǹ�
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
		cout << "equ�ļ���ʧ��" << endl;
	}
	if (fpsymbol != NULL )
	{
	}
	else {
		cout << "�޷���symbol�ļ�" << endl;
	}
	//��ʼ��
	initialRegs();
	word_scanner(fpsymbol, fpequ);
	for (int i = 0; i < op.size(); i++)
	{
		cout << op[i] << "\t" << op1[i] << "\t" << op[2] << "\t" << addr[i] << endl;
	}
	for (int i = 0; i < op.size() ; i++)
	{
		if (op[i] == "41") 
		{//�����Ϊ*
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
		{//�����Ϊ+
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
		else if (op[i] == "45") {//�����Ϊ-
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
		else if (op[i] == "48") {//�����Ϊ/
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
		else if(op[i] == "51"){//�����Ϊ:=
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