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
int const MAX_LEN = 500;


/* ���ڵ�Ľṹ�� */
struct node {
	string name;
	vector<node*> son;
	node* addSon(string s) {
		node* Son = new node();
		Son->name = s;
		son.push_back(Son);
		return Son;
	}
}Node;

typedef struct equ {
	string op;		//������
	string op1;	//������1
	string op2;	//������2
	string result;	//�������
}equ;

//�������ָ����һ���������ɵ���Ԫʽ
int LineOfEqu = 0;

FILE* fptoken;
FILE* fpsymbol;
FILE* fpsymbol2; //׷��д
FILE* fpEqu;
int word_num = 0;
int symbol_num = 0;
//�ʷ�
string* word_list = new string[MAX_LEN];
//���ʵĻ�����
string* word_code = new string[MAX_LEN];
//���ʵ���ڵ�ַ
string* word_addr = new string[MAX_LEN];
//��ʶ��
string* symbol_list = new string[MAX_LEN];
//��ʶ������������
string* symbol_type = new string[MAX_LEN];

//��������Equ�����Ԫʽ
equ* Equ = new  equ[MAX_LEN];

void gen(string op, string a, string b, string r) {
		// ����� ����1  ����2  �������
	Equ[LineOfEqu] = { op,a,b,r };
	LineOfEqu++;	//ָ�����
}

//����һ��ȫ�ֵ�int��¼��ʱ�����ĸ���
int temp_num = 1;
string NewTemp() {
	string name = "T" + to_string(temp_num);
	symbol_num++;
	temp_num++;
	fprintf(fpsymbol2, "%d\t", symbol_num);
	fprintf(fpsymbol2, "%d\t", 18);
	fprintf(fpsymbol2, "%s\n", name.c_str());
	return	to_string(symbol_num);
}

bool IsFilter(char ch) {
	if (ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ' )
	{
		return true;
	}
	return false;
}

			  /*  ����ĵ�ַ ��Ԫʽ����� */
void BackPatch(int addr, string no ) {
	stringstream ss;
	ss << no;
	int no2;
	ss >> no2;
	addr++;
	Equ[no2-1].result = to_string(addr);
}

//ɨ��token��symbol �õ� word_list �� symbol_list
void word_scanner(FILE* fptoken,FILE*fpsymbol) {
	char ch = ' ';
	string arr = "";
	/*������е���ɨ��ʻ�*/
	ch = fgetc(fptoken);//�ȶ�ȡһ�� 1
	//token�ļ���Ϊ�ĸ����֣����-����-������-��ڵ�ַ
	while (ch != EOF ) {
		//���
		while (fgetc(fptoken) != '\t') {
			//�����в��� ����ȡ��ţ�
		}
		
		//����
		ch = fgetc(fptoken);
		while (ch != '\t' ) {
			arr += ch ;
			ch = fgetc(fptoken);
		}
		/*cout << "����"<<word_num<<":\t" << arr << endl;*///�Ѿ�����ɨ��token�ļ���ȷ��
		
		word_list[word_num ] = arr.c_str();
		word_num++;
		arr = "";

		//������
		ch = fgetc(fptoken);
		while (ch != '\t' ) {
			arr += ch;
			ch = fgetc(fptoken);
		}
		word_code[word_num - 1] = arr.c_str();
		arr = "";

		//��ڵ�ַ
		ch = fgetc(fptoken);
		while (ch != '\n' )
		{
			arr += ch;
			ch = fgetc(fptoken);
		}
		word_addr[word_num - 1] = arr.c_str();

		ch = fgetc(fptoken);

		arr = "";
		
	}

	/*������е���ɨ����ű����*/
	ch = fgetc(fpsymbol);//�ȶ�ȡһ�� 1
	//symbol�ļ���Ϊ�ĸ����֣����-����-����
	while (ch != EOF) {
		//���
		while (fgetc(fpsymbol) != '\t') {
			//�����в��� ����ȡ��ţ�
		}

		//����
		ch = fgetc(fpsymbol);
		while ( ch != '\t') {
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		symbol_type[symbol_num] = arr.c_str();
		arr = "";

		//����
		ch = fgetc(fpsymbol);
		while (ch != '\n') {
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		/*cout << "����"<<symbol_num<<":\t" << arr << endl;*///�Ѿ�����ɨ��symbol�ļ���ȷ��
		
		symbol_list[symbol_num] = arr.c_str();
		symbol_num++;
		ch = fgetc(fpsymbol);
		arr = "";

	}

}

//������������ж�ĳ�������Ƿ���symbol���ű�����Ϊ��ʶ���������ڳ�����
bool IsSymbol_ident(string str) {
	for (int i = 0; i < symbol_num; i++)
	{
		if (symbol_list[i] == str && symbol_type[i] == "18") {//����ʾ�ñ�ʶ����symbol�У���Ϊ��ʶ��
			return true;
		}
	}
	return false;
}

bool IsSymbol(string str) {
	for (int i = 0; i < symbol_num; i++)
	{
		if (symbol_list[i] == str) {//����ʾ�ñ�ʶ����symbol�У���Ϊ��ʶ��
			return true;
		}
	}
	return false;
}

//����ʵ��ָ����д��ͬ�������в�ͬ���ֵ��﷨����
int flag = 0;//flag��ʶ��ǰ�Ľ��е���λ��
int error_num = 1;/*error_num������ע�﷨�����д���ĸ�����
				   error_num��ֵ ����1 ��˵���﷨�����д���*/

void L_Begin(struct node* p);
string E_analize(struct node* p);
vector<vector<string>> B_analize(struct node* p);

/* 
   E-> FE1 
   E1->+FE1|-FE1|��
   F->TF1
   F1->*TF1|/TF1|��
   T->id|(E)	idΪ����/��ʶ��
*/

string T_analize(struct node* p) {
	//����T�����ж� flagָ���ַ� Ϊ id/�����������֡�
	struct node* p1 = NULL;
	if (word_list[flag] == "(" )
	{//�ж�Ϊ������Ҫ���ɣ���E���������ӽڵ㣬����ʼ��E����
		p->addSon("(");
		p->addSon("E");
		p->addSon(")");
		flag++;
		p1 = p->son[1];		// ��E�ڵ㸳ֵ��p1����ʼE�Ĺ��̷���
		string temp_addr = E_analize(p1);
		return temp_addr;
	}
	else if(IsSymbol(word_list[flag])) {//�жϴ˴�Ϊһ��ʶ��/����
		//�����ӽڵ㼴�ɡ�flagָ�����һλ��
		p->addSon(word_list[flag]);
		string ch = word_addr[flag];
		flag++;
		return ch;
	}
	else {//�� ��T����ʽ���˴���һ���ַ��Ȳ���id���ֲ��ǣ� ����
		cout << "����" << error_num << ":\t �������ʽ�Ҳ���һ���ַ������������ӣ� " << endl;
		error_num++;
	}
}

vector<string> F1_analize(struct node* p) {
	//����Ļ�����Ϊ ��������� ���Ե�ǰ���ű�Ϊһ���������
	struct node* p1;
	vector<string> str3;//��������
	if (word_list[flag] == "+" || word_list[flag] == "-" )
	{//���Ǵ˴������Ϊ + �� - �������F1Ϊ�� 
		
	}
	else if(word_list[flag] == "*" || word_list[flag] == "/" ) 
	{//�˴������Ϊ*��/ ��T1�Ƴ� *TF1 | /TF1
		p->addSon(word_list[flag]);
		string op = "";		//��¼�����
		if (word_list[flag] == "*")
		{
			op = "41";
		}
		else if (word_list[flag] == "/") {
			op = "48";
		}
		flag++;

		//�� ��T�ķ���
		p->addSon("T");
		p1 = p->son[1];
		string addr1  = T_analize(p1);
		

		//�� ��F1�ķ������ݹ�
		p->addSon("F1");
		p1 = p->son[2];
		vector<string> str2 = F1_analize(p1);	//����F1������string����

		if (str2.size() != 0 ) {
			string op2 = str2[0];
			string addr2 = str2[1];
			string addr3 = NewTemp();
			gen(op2, addr1, addr2, addr3);
			addr1 = addr3;
		}
		
		str3.push_back(op);
		str3.push_back(addr1);
		return str3;
	}
	return str3 ;
}

string F_analize(struct node* p) {
	struct node* p1 = NULL;

	p->addSon("T");
	p->addSon("F1");
	p1 = p->son[0];
	string addr1 = T_analize(p1);
	
	p1 = p->son[1];
	vector<string> str1= F1_analize(p1);
	
	if (str1.size() != 0 )
	{
		string op = str1[0];
		string addr2 = str1[1];
		string addr3 = NewTemp();
		gen(op, addr1, addr2, addr3);
		return addr3;
	}
	return addr1;
}

vector<string> E1_analize(struct node* p) {//ͬ���˴�ҲӦ��Ϊһ�������+ - ��Ϊ�� * / Ϊ��
	struct node* p1 = NULL;
	vector<string> str2;//���ݵ�����
	if (word_list[flag] == "+" || word_list[flag] == "-" )
	{
		string op = "";
		if (word_list[flag] =="+" )
		{
			op = "43";
		}
		else {
			op = "45";
		}
		p->addSon(word_list[flag]);
		flag++;
		//F�ķ���
		p->addSon("F");
		p1 = p->son[1];
		string addr1 = F_analize(p1);

		//E1�ķ���
		p->addSon("E1");
		p1 = p->son[2];
		vector<string> str1 = E1_analize(p1);
		if (str1.size() != 0 )
		{
			string op2 = str1[0];
			string addr2 = str1[1];
			string addr3 = NewTemp();
			gen(op2, addr1, addr2, addr3);
			addr1 = addr3;
		}
		str2.push_back(op);
		str2.push_back(addr1);
		return str2;
	}
	else if(word_list[flag] == "*" || word_list[flag] == "/" ) {//E1Ϊ��
		
	}
	else if (word_list[flag] == ";") {
		//���������ʽ��������������
		
	}
	return str2;
}

string E_analize(struct node* p) {
	struct node* p1 = NULL;
	p->addSon("F");
	p->addSon("E1");
	p1 = p->son[0];		//F��ֵ��p1����ʼ����F
	string addr1 =  F_analize(p1);

	//��ʼ����E1
	p1 = p->son[1];
	vector<string> str1 = E1_analize(p1);
	if (str1.size() != 0 )
	{
		string op = str1[0];
		string addr2 = str1[1];
		string addr3 = NewTemp();
		gen(op, addr1, addr2, addr3);
		addr1 = addr3;
	}
	return addr1;

}

/* �����Ǹ��鷳��bool���ʽ 
	B->CH
	H->or CH|�� 
	C->DK
	K->and DK|��
	D->not D|E
	E->true|false|(B)|F
	F->id G id
	G-> <|<=|=|>=|<>
	*/

/*G-> <|>|<=|>=|<>|=*/
string G_bool_analize(struct node* p) {
	string op = "";
	if (word_list[flag] == "<" || word_list[flag] == "<=" || word_list[flag] == ">" || word_list[flag] == ">=" || word_list[flag] == "<>")
	{//ȷ��G������һ����ϵ�жϷ�< > <= >= <>
		p->addSon(word_list[flag]);
		op = word_list[flag];
		flag++;
		return op;
	}
	else {
		cout << "����" << error_num << ":\t bool���ʽȱ�ٹ�ϵ�жϷ��� " << endl;
		error_num++;
	}
	return op ;
}
/*F->id G id*/
/*��������� (.true)���� (.false)���� (.next)����  */
vector<vector<string>> F_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	if (IsSymbol_ident(word_list[flag]))
	{
		p->addSon(word_list[flag]);
		string id1 = word_addr[flag];
		flag++;
		p->addSon("G");
		p1 = p->son[1];
		string relop = G_bool_analize(p1);
		if (IsSymbol_ident(word_list[flag]))
		{//��һ��Ҳ�Ǳ�ʶ�������ǳ���������ʽ����ȷ
			p->addSon(word_list[flag]);
			string id2 = word_addr[flag];
			flag++;
			gen("j" + relop, id1, id2, "0");
			gen("j", "_", "_", "0");
		}
		else {
			cout << "����" << error_num << ":\t" << endl;
			error_num++;
		}
		string t = to_string(LineOfEqu);	//����nextquad
		string f = to_string(LineOfEqu + 1);//����nextquad+1
		true_no.push_back(t);
		false_no.push_back(f);
	}
	true_or_false.push_back(true_no);
	true_or_false.push_back(false_no);
	return true_or_false;
}
/*E->true|false|(B)|F*/
vector<vector<string>> E_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	if (word_list[flag] == "true" || word_list[flag] == "false" )
	{								 //��������£�E->true|false
		p->addSon(word_list[flag]);
		flag++;
		string t = to_string(LineOfEqu);
		true_no.push_back(t);
	}
	else if (word_list[flag] == "(") {//��������£�E->(B)
		p->addSon("(");
		flag++;
		p->addSon("B");
		p->addSon(")");
		p1 = p->son[1];
		return  B_analize(p1);
	}
	else if (IsSymbol_ident(word_list[flag])) {//��������£������ʽ��ΪE->F ��Ϊ F->id G id
		p->addSon("F");
		p1 = p->son[0];
		return F_bool_analize(p1);
	}
	else {
		//����
		cout << "����" << error_num << ":\t bool���ʽ���� " << endl;
		error_num++;
	}
	true_or_false.push_back(true_no);
	true_or_false.push_back(false_no);
	return true_or_false;
}
/*D->not D|��*/
vector<vector<string>> D_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<vector<string>> tf;
	if (word_list[flag] == "not" )
	{//����not
		p->addSon("not");
		flag++;
		p->addSon("D");
		p1 = p->son[1];
		vector<vector<string>> tf1 =  D_bool_analize(p1);
		if (tf1.size() != 0 )
		{
			tf.push_back(tf1[1]);
			tf.push_back(tf1[0]);
		}
	}
	else {//û��not ��Dת��E
		p->addSon("E");
		p1 = p->son[0];
		tf = E_bool_analize(p1);
	}
	return tf;
}
/*K-> and DK|��*/
vector<vector<string>> K_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	if (word_list[flag] == "and" )
	{//���Ϊ and ����K-> and DK �Ĳ���ʽ
		p->addSon("and");
		flag++;
		p->addSon("D");
		p1 = p->son[1];
		vector<vector<string>> str1 = D_bool_analize(p1);
		p->addSon("K");
		p1 = p->son[2];
		vector<vector<string>> str2 = K_bool_analize(p1);
		//���ж� �ڶ���K�Ƿ�Ϊ��  1.��Ϊ�գ���ֱ�Ӵ���str1��true��false 2.����Ϊ�� �����and������
		if (str2.size() != 0 )
		{//����  and �Ĺ���
			true_or_false.push_back(str2[0]);	//and�� (.true)ΪE2��true����
			vector<string> f;					//fΪ�µ�and���false���� �� E1��E2����false���ϵĲ���
			for (int i = 0; i < str1[1].size(); i++)
			{
				f.push_back(str1[1][i]);
			}
			for (int i = 0; i < str2[1].size(); i++)
			{
				f.push_back(str2[1][i]);
			}//���ˣ��͵õ��� and֮���false����
			true_or_false.push_back(f);
			return true_or_false;
		}
	}
	else {//KΪ��
		return true_or_false;
	}
}
/*C->DK*/
vector<vector<string>> C_bool_analize(struct node* p) {
	struct node* p1 = NULL ;
	vector<vector<string>> true_or_false;
	p->addSon("D");
	p1 = p->son[0];
	vector<vector<string>> str1 = D_bool_analize(p1);
	//�õ�D��������true false���ϣ����п�
	vector<string> t1;
	vector<string> f1;
	if (str1.size() != 0 )
	{
		t1 = str1[0];	//D��.true����
		f1 = str1[0];	//D��.false����
	}

	p->addSon("K");
	p1 = p->son[1];
	vector<vector<string>> str2 = K_bool_analize(p1);
	//�õ�K��������true or false���ϣ����п�
	vector<string> t2;
	vector<string> f2;
	if (str2.size() != 0)
	{
		t2 = str2[0];	//K��.true����
		f2 = str2[1];	//K��.false����
	}
	//�ٶ�D��K�����������
	true_or_false.push_back(t2);	//�����true������E2��K����true����
	//��ȡ�����false����
	vector<string> ff;
	for (int i = 0; i < f1.size() ; i++)
	{
		ff.push_back(f1[i]);
	}
	for (int i = 0; i < f2.size(); i++)
	{
		ff.push_back(f2[i]);
	}
	true_or_false.push_back(ff);
	return true_or_false;
}
/*H->or CH|��*/
vector<vector<string>> H_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	if (word_list[flag] == "or")
	{//���Ϊ and ����K->and DK �Ĳ���ʽ
		p->addSon("or");
		flag++;
		p->addSon("C");
		p1 = p->son[1];
		vector<vector<string>> str1 = C_bool_analize(p1);
		//�õ�H��
		vector<string> t1;
		vector<string> f1;
		if (str1.size() != 0 )
		{
			t1 = str1[0];
			f1 = str1[1];
		}
		p->addSon("H");
		p1 = p->son[2];
		vector<vector<string>> str2 =  H_bool_analize(p1);
		vector<string> t2;
		vector<string> f2;
		if (str2.size() != 0 )
		{
			t2 = str2[0];
			f2 = str2[1];
		}
		//��������� or ����
		//(.true)�����������Ĳ���
		for (int i = 0; i < t1.size(); i++)
		{
			true_no.push_back(t1[i]);
		}
		for (int i = 0; i < t2.size(); i++)
		{
			true_no.push_back(t2[i]);
		}
		true_or_false.push_back(true_no);
		//(.false)������E2��false����
		true_or_false.push_back(f2);
		return true_or_false;
		
	}
	else {//KΪ��

	}
}

vector<vector<string>> B_analize(struct node* p) {
	struct node* p1 = NULL ;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	p->addSon("C");
	p1 = p->son[0];
	vector<vector<string>> tf1 = C_bool_analize(p1);
	vector<string> t1;
	vector<string> f1;
	if (tf1.size() != 0 )
	{
		t1 = tf1[0];
		f1 = tf1[1];
	}

	//��¼��Mquad
	int Mquad = LineOfEqu;

	p->addSon("H");
	p1 = p->son[1];
	vector<vector<string>> tf2 = H_bool_analize(p1);
	vector<string> t2;
	vector<string> f2;
	if (tf2.size() != 0 )
	{
		t2 = tf2[0];
		f2 = tf2[1];
	}

	//�������tf1  tf2 ���� or ����
	//true����
	for (int i = 0; i < t1.size(); i++)
	{
		true_no.push_back(t1[i]);
	}
	for (int i = 0; i < t2.size(); i++)
	{
		true_no.push_back(t2[i]);
	}
	true_or_false.push_back(true_no);
	//false���ϼ� f2
	true_or_false.push_back(f2);
	for (int i = 0; i < f1.size(); i++)
	{
		BackPatch(Mquad, f1[i]);
	}
	return true_or_false;

}
/* S�ȣ���bool�͵Ĳ���ʽ �ͷ���next���� */

vector<string> S_analize(struct node* p) {
	//flagָ����� id/if/while/begin ��S�е�һ���ӽڵ�
	//S�ر��ӣ�����5�п��ܵĺ�ѡʽ�����ͦ�鷳�ģ�������һ��
	/*	1.S-> id:= E

		2.S-> if B then S
		3.S-> if B then S else S

		4.S-> while B do S

		5.S-> begin L end
	�Ĵ��� ��һ��Ϊ1.��ʶ�� 2.if 3.while 4.begin
	*/
	struct node* p1 = NULL;

	if (IsSymbol_ident(word_list[flag]))
	{//�˴���ʾΪһ����ʶ������Ϊid ���1
		p->addSon(word_list[flag]);
		string addr1 = word_addr[flag];		//��һ�������ĵ�ַ 
		flag++;
		//�ж�id �ĺ�һ���ǲ��� ��= ���Ǳ���
		if (word_list[flag] == ":=")
		{//���ڣ�=����û�������E���ж�
			p->addSon(":=");
			string op = "51";					//operation������
			p->addSon("E");
			flag++;
			p1 = p->son[2];		//��p�ĵ�3���ӽڵ�E��ֵ��p��Ȼ�����E����
			string addr2 = E_analize(p1);		//flagָ����� E�еĵ�һ������/���� 
			gen(op, addr2, "0", addr1);
		}
		else {//��ֵ���û�У�=������
			cout << "����" << error_num << ":\t ��ֵ���ȱ�٣�= ���ţ�" << endl;
			error_num++;
		}
	}
	else if (word_list[flag] == "if") {//S�µĵ�һ����Ϊ if ����Ϊ���2/3
		vector<string> next;//Ҫ���ݵ�next����
		p->addSon("if");
		p->addSon("B");
		//������then�ڵ㣬���ں��������B֮�����
		p->addSon("then");
		p1 = p->son[1];
		flag++;
		//�����B���жϡ� flagָ�����B�еĵ�һ��Ԫ�أ���ʶ��
		vector<vector<string>> tf1 = B_analize(p1);		//�õ�b��true_or_false����
		vector<string> t1;
		vector<string> f1;
		if (tf1.size() != 0 )
		{
			t1 = tf1[0];
			f1 = tf1[1];
		}
		//Ҫ���B����������flagָ�����then�����ǵü�һ
		//��then�����ж�
		if (word_list[flag] == "then")
		{
			//ȷ����then�����޴�
			flag++;
		}
		else {
			cout << "����" << error_num << ":\t if���ȱ��then�ؼ��ʣ� " << endl;
			error_num++;
		}

		/* ������M1�ķ��� ��M1quad ��֮���Լ�ΪM1����Ϊ�ۺϿ�����else �����M2 */
		int M1quad = LineOfEqu;

		//�������ֶ�S�����ж�
		p->addSon("S");
		p1 = p->son[3];
		vector<string> next1 = S_analize(p1);	//�õ�S���ص�next����

		//S��������,�ж��Ƿ���else������else����Ϊ���3����û�У������2
		if (word_list[flag] == "else")
		{//���3 if B then S else S
			//�˴��ж�S
			flag++;

			//���������N��next��¼
			int Nnext = LineOfEqu;
			gen("j", "_", "_", "0");	//�Լ����N��emit������ʱ�������

			p->addSon("else");

			//���������M2��nextquad��¼
			int M2quad = LineOfEqu;
			p->addSon("S");
			p1 = p->son[5];
			vector<string> next2 = S_analize(p1);	//������õ�S2��next����
			for (int i = 0; i < t1.size(); i++)
			{
				BackPatch(M1quad, t1[i]);
			}//�˴�back patch ��һ��
			for (int i = 0; i < f1.size() ; i++)
			{
				BackPatch(M2quad, f1[i]);
			}//�˴�back patch �ڶ���

			//�ۺ�next����
			for (int i = 0; i < next1.size() ; i++)
			{
				next.push_back(next1[i]);
			}for (int i = 0; i < next2.size() ; i++)
			{
				next.push_back(next2[i]);
			}//�ֱ�ѹ��next1��next2 �� ���õ�S��next����
			return next;

		}
		else {//��Ϊ���2 if B then S 
			//��ʱflagָ����� S��ĵ�һ�����ţ����ú��ơ�
			for (int  i = 0; i < t1.size() ; i++)
			{
				BackPatch(M1quad, t1[i]);
			}
			for (int i = 0; i < next1.size() ; i++)
			{
				next.push_back(next1[i]);
			}
			return next;
		}


	}

	else if (word_list[flag] == "while") {//�˴��������.4 while B do S
	vector<string> next;
		struct node* p1 = NULL;
		flag++;
		p->addSon("while");
		p->addSon("B");
		p->addSon("do");
		p->addSon("S");
		//���� B 
		p1 = p->son[1];		//��S�ĵڶ����ӽڵ�B��ֵ��p1

		//�� B֮ǰ��M�ڵ���з���
		int M1quad = LineOfEqu;

		vector<vector<string>> tf1 = B_analize(p1);
		vector<string> t1;
		vector<string> f1;
		if (tf1.size() != 0 )
		{
			t1 = tf1[0];
			f1 = tf1[0];
		}//�õ���B���ص�true��false����

		//�ж���û��do 
		if (word_list[flag] == "do")
		{//��do�������û����
			flag++;
		}
		else {//û��do ����
			cout << "����" << error_num << "��\t while���ȱ�ٹؼ���do �� " << endl;
			error_num++;
		}
		//��S֮ǰ��M2���з���
		int M2quad = LineOfEqu;

		//�������ٽ�һ���ж�S
		p1 = p->son[3];
		vector<string> next1 = S_analize(p1);
		for (int i = 0; i < next1.size() ; i++)
		{
			BackPatch(M1quad, next1[i]);
		}
		for (int i = 0; i < t1.size() ; i++)
		{
			BackPatch(M2quad, t1[i]);
		}
		next = f1;
		gen("j", "_", "_", to_string(M1quad));

	}

	else if (word_list[flag] == "begin") {//������һ�������������5.begin L end
		flag++;
		p->addSon("begin");
		//����L����
		p->addSon("L");
		p1 = p->son[1];
		L_Begin(p1);
		p->addSon("end");
		//�ж���û��end
		if (word_list[flag] == "end")
		{//��end
			flag++;
		}
		else {
			cout << "����" << error_num << ":\t begin�Ӿ�ȱ��end�ؼ��֣� " << endl;
			error_num++;
		}
	}

}

void L_Begin(struct node* p) {
	struct node* p1 = NULL;
	//���и���������
	/*��һ��  ���� �ݹ� �� ����
		L->S|S;L */
		/*  1.�Ƚ�S�ĵݹ�����ж�
			2.�� S�жϺ������� �ж���û�зֺţ�������L���޵ݹ�
			3.L�еݹ飬��ʼL�ĵݹ�
		*/
	p->addSon("S");
	p1 = p->son[0];
	vector<string> next1 = S_analize(p1);
	//��S����Ҫ��flagָ��
	if (word_list[flag] == ";")
	{//��ʼL�ݹ�
		flag++;
		p->addSon(";");
		p->addSon("L");
		//�ڽ�S֮ǰ��M���з���
		int Mquad = LineOfEqu;
		p1 = p->son[2];
		L_Begin(p1);
	}
	else {
		//��û�зֺţ���ֻ�ܽ���S�Ĵ��������S��������ݵ�root�����һ���ӽڵ㡣
	}
}

void L_analize(struct node* p) {
	//��ǰ��flagָ�����L������Ҷ�ӽڵ㿴�Ļ�������i ���ж��Ƿ� �ô�ָ����� symbol���еı�ʶ�� 

	struct node* p1;

	if (IsSymbol_ident(word_list[flag]))
	{//��Ϊ�棬��ô�Ϊһ����ʶ������ȷ
		p->addSon(word_list[flag]);
		flag++;
		if (word_list[flag] == ",")//˵���˴���L�еݹ�
		{
			p->addSon(",");
			p->addSon("L");
			flag++; //����ָ���2��L�ĵ�һ���ӽڵ㡣
			p1 = p->son[2];//��Ϊ�еݹ飬��p�ĵ������ӽڵ�L��ֵ��p
			L_analize(p1);
		}
		else {
			//�˴�û�еݹ飬ֻ��һ�������ѭ�����ɡ�
		}
	}
	else {//Ϊ�٣��˴����Ǳ�ʶ��������
		cout << "����" << error_num << ":\t �������var��Ӧ�ý�һ��ʶ�� �� �������ʶ��ȡ��Ϊ������ " << endl;
		error_num++;
	}
}

//дһ��D->L:K;|L:K;D����ʽ�ĵݹ��������
void D_analize(struct node* p) {
	struct node* p1 = NULL;

	p->addSon("L");
	//������ȣ���ʼ��L�������﷨����
	p1 = p->son[0];		//��D�ĵ�һ���ӽڵ�L��ֵ��p������L�����׶Ρ�
	L_analize(p1);		//��L����������flagָ����� ��

	p->addSon(":");
	//: ���ж�
	if (word_list[flag] == ":")
	{
		//�ж��У�������ȷ
	}
	else {
		cout << "����" << error_num << ":\t �����������ȱ�٣��� " << endl;
		error_num++;
	}

	p->addSon("K");
	//	�ȷ���K (�ж�K�ķ����Ƿ��� integer��bool��real �ķ�Χ��
	flag++;		//flagָ����K
	if (word_list[flag] == "integer" || word_list[flag] == "bool" || word_list[flag] == "real")
	{//�ж�K���ķ��ţ�Ϊ��ȷ������
		p->son[2]->addSon(word_list[flag]);
		flag++; //flagָ����ƣ�ָ���ˣ�
	}
	else {
		cout << "����" << error_num << ":\t �������ȱ�� ���ͷ��ţ� " << endl;
		error_num++;
	}

	p->addSon(";");
	//�ж����ޣ�
	if (word_list[flag] == ";")
	{//�зֺţ�û����־λ����һλ����
		flag++;
	}
	else {
		cout << "����" << error_num << ":\t ������������βȱ�ٷֺţ� " << endl;
	}

	/*��ǰ������ ����D->L:K;|L:K;D ������ʽ
		��Ҫ�жϵ��� ð�ţ� �ж����ޣ�����ж�����D�ĵݹ�
		�������������ִεĵݹ���   */
		//flag��ǰָ�򣻵ĺ�һλ�������һλΪ������D�еݹ�
	if (word_list[flag + 1] == ":")
	{
		p->addSon("D");
		p1 = p->son[4];
		D_analize(p1);
	}

}

void Declear(struct node* p) {
	struct node* p1;
	if (word_list[flag] == "var")//�������ж����� var ���ж���û�б����������
	{//		��var �� ��ʼ�� S->var D �����﷨����
		p->addSon("var");
		p->addSon("D");
		//����ʽ D->L : K ;|L : K ; D �����еݹ鰡����
		p1 = p->son[1];		//�� S �ĵڶ����ӽڵ㸳ֵ D ��p������pȥ�ݹ�
		flag++;				//flagָ����var��һ����������Dʱflagָ��L�ĵ�һ���ַ�
		D_analize(p1);
	}
	else {//������û��var�ı���
		cout << "����" << error_num << ":\t �������ȱ��var��ʶ��" << endl;
		error_num++;

	}
}

void parser(struct node* root) {
	struct node* p =NULL ;
	if (word_list[flag] == "program") 
	{//�ж�����program����ʼ��ʶ
		flag++;
		if (IsSymbol_ident(word_list[flag]) ) 
		{//�ж�program��ʶ�������޳��������������Ǹ��Ƿ�һ����ʶ��=�����Ǹ��ַ��Ƿ���symbol�ļ��С�
			//���˼�  ��program���г����� �������Ľ��
			root->addSon(word_list[flag]);
			flag++;				//flagָ�����var
			root->addSon("S");
			p = root->son[1];		//��root�ĵڶ����ӽڵ㸳ֵ��p
			Declear(p);
			//Ȼ���� ����������ģ�� flagָ��id 
			root->addSon("L");
			p = root->son[2];
			L_Begin(p);
			//���������������ֺţ�
			root->addSon(";");
		}
		else 
		{//û�г�����������
			cout << "" << error_num << ":\t ����ȱ�ٳ������� �����������ʹ���˱�ʶ����";
			error_num++;
		}
	}
	else {//û��program��ʶ������
		cout << "����"<<error_num<<":\t ����ȱ�� program �����ʶ����" << endl;
		error_num++;
	}
}

int main() {
	if ((fptoken = fopen("token.txt","r")) == NULL)//�ж��ļ��ɱ���
	{
		cout << "token�ļ���ʧ�ܣ�";
		return 0;
	}
	if ((fpsymbol = fopen("symbol.txt", "r")) == NULL) {
		cout << "symbol�ļ���ʧ�ܣ�";
		return 0;
	}
	if ((fpEqu = fopen("equ.txt", "w")) == NULL)
	{
		cout << "equ�ļ���ʧ�ܣ�";
		return 0;
	}
	/*������token�ļ��ַ���������� word_list �� �����СΪ ȫ�ֱ��� word_num 
			symbol�ļ�������������� symbol_list �� �����СΪ symbol_num
	*/
	word_scanner(fptoken,fpsymbol);
	fclose(fpsymbol);
	fclose(fptoken);

	fpsymbol2 = fopen("symbol.txt", "a+");//�ر�symbol�ļ����ٴ���׷�ӷ�ʽ�򿪣��Ա��������д�롣
	if (fpsymbol == NULL )
	{
		cout << "symbol�ļ���д��ʽ���ڶ��Σ���ʧ�ܡ�" << endl;
		return 0;
	}


	////��ʼ������������������
	struct node* root = new node ;
	root->name = "program";

	flag = 0;
	error_num = 1;

	parser(root);

	for (int i = 0; i < LineOfEqu ; i++)
	{
		cout << Equ[i].op <<"\t"<< Equ[i].op1 <<"\t"<< Equ[i].op2<< "\t"<< Equ[i].result << endl;
		fprintf(fpEqu, "%s\t", Equ[i].op.c_str());
		fprintf(fpEqu, "%s\t", Equ[i].op1.c_str());
		fprintf(fpEqu, "%s\t", Equ[i].op2.c_str());
		fprintf(fpEqu, "%s\n", Equ[i].result.c_str());
	}

	//��β
	fclose(fpEqu);
	fclose(fpsymbol2);
	delete[] Equ;
	delete[] word_list;
	delete[] word_addr;
	delete[] word_code;
	delete[] symbol_type;
	delete[] symbol_list;
	return 0;
}
