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


/* 树节点的结构体 */
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
	string op;		//操作码
	string op1;	//操作数1
	string op2;	//操作数2
	string result;	//结果变量
}equ;

//用这个来指向下一个即将生成的四元式
int LineOfEqu = 0;

FILE* fptoken;
FILE* fpsymbol;
FILE* fpsymbol2; //追加写
FILE* fpEqu;
int word_num = 0;
int symbol_num = 0;
//词法
string* word_list = new string[MAX_LEN];
//单词的机内码
string* word_code = new string[MAX_LEN];
//单词的入口地址
string* word_addr = new string[MAX_LEN];
//标识符
string* symbol_list = new string[MAX_LEN];
//标识符的类型数组
string* symbol_type = new string[MAX_LEN];

//定义数组Equ存放四元式
equ* Equ = new  equ[MAX_LEN];

void gen(string op, string a, string b, string r) {
		// 运算符 变量1  变量2  结果变量
	Equ[LineOfEqu] = { op,a,b,r };
	LineOfEqu++;	//指针后移
}

//定义一个全局的int记录临时变量的个数
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

			  /*  回填的地址 四元式的序号 */
void BackPatch(int addr, string no ) {
	stringstream ss;
	ss << no;
	int no2;
	ss >> no2;
	addr++;
	Equ[no2-1].result = to_string(addr);
}

//扫描token和symbol 得到 word_list 和 symbol_list
void word_scanner(FILE* fptoken,FILE*fpsymbol) {
	char ch = ' ';
	string arr = "";
	/*这里进行的是扫描词汇*/
	ch = fgetc(fptoken);//先读取一个 1
	//token文件分为四个部分，序号-符号-机内码-入口地址
	while (ch != EOF ) {
		//序号
		while (fgetc(fptoken) != '\t') {
			//不进行操作 ，不取序号！
		}
		
		//符号
		ch = fgetc(fptoken);
		while (ch != '\t' ) {
			arr += ch ;
			ch = fgetc(fptoken);
		}
		/*cout << "符号"<<word_num<<":\t" << arr << endl;*///已经检验扫描token文件正确了
		
		word_list[word_num ] = arr.c_str();
		word_num++;
		arr = "";

		//机内码
		ch = fgetc(fptoken);
		while (ch != '\t' ) {
			arr += ch;
			ch = fgetc(fptoken);
		}
		word_code[word_num - 1] = arr.c_str();
		arr = "";

		//入口地址
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

	/*这里进行的是扫描符号表符号*/
	ch = fgetc(fpsymbol);//先读取一个 1
	//symbol文件分为四个部分，序号-类型-符号
	while (ch != EOF) {
		//序号
		while (fgetc(fpsymbol) != '\t') {
			//不进行操作 ，不取序号！
		}

		//类型
		ch = fgetc(fpsymbol);
		while ( ch != '\t') {
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		symbol_type[symbol_num] = arr.c_str();
		arr = "";

		//符号
		ch = fgetc(fpsymbol);
		while (ch != '\n') {
			arr += ch;
			ch = fgetc(fpsymbol);
		}
		/*cout << "符号"<<symbol_num<<":\t" << arr << endl;*///已经检验扫描symbol文件正确了
		
		symbol_list[symbol_num] = arr.c_str();
		symbol_num++;
		ch = fgetc(fpsymbol);
		arr = "";

	}

}

//这个函数用来判断某个符号是否在symbol符号表中且为标识符（区分于常数！
bool IsSymbol_ident(string str) {
	for (int i = 0; i < symbol_num; i++)
	{
		if (symbol_list[i] == str && symbol_type[i] == "18") {//即表示该标识符在symbol中，且为标识符
			return true;
		}
	}
	return false;
}

bool IsSymbol(string str) {
	for (int i = 0; i < symbol_num; i++)
	{
		if (symbol_list[i] == str) {//即表示该标识符在symbol中，且为标识符
			return true;
		}
	}
	return false;
}

//根据实验指导来写不同函数进行不同部分的语法分析
int flag = 0;//flag标识当前的进行到的位置
int error_num = 1;/*error_num用来标注语法分析中错误的个数。
				   error_num的值 大于1 则说明语法分析有错误！*/

void L_Begin(struct node* p);
string E_analize(struct node* p);
vector<vector<string>> B_analize(struct node* p);

/* 
   E-> FE1 
   E1->+FE1|-FE1|空
   F->TF1
   F1->*TF1|/TF1|空
   T->id|(E)	id为常数/标识符
*/

string T_analize(struct node* p) {
	//进入T首先判断 flag指向字符 为 id/（来进行区分。
	struct node* p1 = NULL;
	if (word_list[flag] == "(" )
	{//判断为（，则要生成（、E、）三个子节点，并开始对E分析
		p->addSon("(");
		p->addSon("E");
		p->addSon(")");
		flag++;
		p1 = p->son[1];		// 将E节点赋值给p1，开始E的过程分析
		string temp_addr = E_analize(p1);
		return temp_addr;
	}
	else if(IsSymbol(word_list[flag])) {//判断此处为一标识符/常数
		//加上子节点即可、flag指针后移一位。
		p->addSon(word_list[flag]);
		string ch = word_addr[flag];
		flag++;
		return ch;
	}
	else {//即 在T产生式，此处第一个字符既不是id、又不是（ 报错
		cout << "错误" << error_num << ":\t 算数表达式右部第一个字符不是算数因子！ " << endl;
		error_num++;
	}
}

vector<string> F1_analize(struct node* p) {
	//这里的话，因为 到运算符了 所以当前符号必为一个运算符。
	struct node* p1;
	vector<string> str3;//传递向量
	if (word_list[flag] == "+" || word_list[flag] == "-" )
	{//若是此处运算符为 + 或 - ，则这个F1为空 
		
	}
	else if(word_list[flag] == "*" || word_list[flag] == "/" ) 
	{//此处运算符为*或/ 则T1推出 *TF1 | /TF1
		p->addSon(word_list[flag]);
		string op = "";		//记录运算符
		if (word_list[flag] == "*")
		{
			op = "41";
		}
		else if (word_list[flag] == "/") {
			op = "48";
		}
		flag++;

		//是 对T的分析
		p->addSon("T");
		p1 = p->son[1];
		string addr1  = T_analize(p1);
		

		//是 对F1的分析，递归
		p->addSon("F1");
		p1 = p->son[2];
		vector<string> str2 = F1_analize(p1);	//接收F1传来的string数组

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

vector<string> E1_analize(struct node* p) {//同理，此处也应该为一个运算符+ - 不为空 * / 为空
	struct node* p1 = NULL;
	vector<string> str2;//传递的向量
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
		//F的分析
		p->addSon("F");
		p1 = p->son[1];
		string addr1 = F_analize(p1);

		//E1的分析
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
	else if(word_list[flag] == "*" || word_list[flag] == "/" ) {//E1为空
		
	}
	else if (word_list[flag] == ";") {
		//即算术表达式结束，不操作。
		
	}
	return str2;
}

string E_analize(struct node* p) {
	struct node* p1 = NULL;
	p->addSon("F");
	p->addSon("E1");
	p1 = p->son[0];		//F赋值给p1，开始分析F
	string addr1 =  F_analize(p1);

	//开始分析E1
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

/* 下面是更麻烦的bool表达式 
	B->CH
	H->or CH|空 
	C->DK
	K->and DK|空
	D->not D|E
	E->true|false|(B)|F
	F->id G id
	G-> <|<=|=|>=|<>
	*/

/*G-> <|>|<=|>=|<>|=*/
string G_bool_analize(struct node* p) {
	string op = "";
	if (word_list[flag] == "<" || word_list[flag] == "<=" || word_list[flag] == ">" || word_list[flag] == ">=" || word_list[flag] == "<>")
	{//确定G能生成一个关系判断符< > <= >= <>
		p->addSon(word_list[flag]);
		op = word_list[flag];
		flag++;
		return op;
	}
	else {
		cout << "错误" << error_num << ":\t bool表达式缺少关系判断符！ " << endl;
		error_num++;
	}
	return op ;
}
/*F->id G id*/
/*向量存的是 (.true)向量 (.false)向量 (.next)向量  */
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
		{//后一个也是标识符或者是常数，生成式才正确
			p->addSon(word_list[flag]);
			string id2 = word_addr[flag];
			flag++;
			gen("j" + relop, id1, id2, "0");
			gen("j", "_", "_", "0");
		}
		else {
			cout << "错误" << error_num << ":\t" << endl;
			error_num++;
		}
		string t = to_string(LineOfEqu);	//代表nextquad
		string f = to_string(LineOfEqu + 1);//代表nextquad+1
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
	{								 //这种情况下，E->true|false
		p->addSon(word_list[flag]);
		flag++;
		string t = to_string(LineOfEqu);
		true_no.push_back(t);
	}
	else if (word_list[flag] == "(") {//这种情况下，E->(B)
		p->addSon("(");
		flag++;
		p->addSon("B");
		p->addSon(")");
		p1 = p->son[1];
		return  B_analize(p1);
	}
	else if (IsSymbol_ident(word_list[flag])) {//这种情况下，则产生式子为E->F 因为 F->id G id
		p->addSon("F");
		p1 = p->son[0];
		return F_bool_analize(p1);
	}
	else {
		//报错
		cout << "错误" << error_num << ":\t bool表达式有误！ " << endl;
		error_num++;
	}
	true_or_false.push_back(true_no);
	true_or_false.push_back(false_no);
	return true_or_false;
}
/*D->not D|空*/
vector<vector<string>> D_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<vector<string>> tf;
	if (word_list[flag] == "not" )
	{//存在not
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
	else {//没有not 从D转到E
		p->addSon("E");
		p1 = p->son[0];
		tf = E_bool_analize(p1);
	}
	return tf;
}
/*K-> and DK|空*/
vector<vector<string>> K_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	if (word_list[flag] == "and" )
	{//如果为 and 才是K-> and DK 的产生式
		p->addSon("and");
		flag++;
		p->addSon("D");
		p1 = p->son[1];
		vector<vector<string>> str1 = D_bool_analize(p1);
		p->addSon("K");
		p1 = p->son[2];
		vector<vector<string>> str2 = K_bool_analize(p1);
		//再判断 第二个K是否为空  1.若为空，则直接传递str1的true、false 2.若不为空 则进行and操作。
		if (str2.size() != 0 )
		{//根据  and 的规则
			true_or_false.push_back(str2[0]);	//and中 (.true)为E2的true集合
			vector<string> f;					//f为新的and后的false集合 即 E1和E2两个false集合的并集
			for (int i = 0; i < str1[1].size(); i++)
			{
				f.push_back(str1[1][i]);
			}
			for (int i = 0; i < str2[1].size(); i++)
			{
				f.push_back(str2[1][i]);
			}//至此，就得到了 and之后的false集合
			true_or_false.push_back(f);
			return true_or_false;
		}
	}
	else {//K为空
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
	//得到D传过来的true false集合，先判空
	vector<string> t1;
	vector<string> f1;
	if (str1.size() != 0 )
	{
		t1 = str1[0];	//D的.true集合
		f1 = str1[0];	//D的.false集合
	}

	p->addSon("K");
	p1 = p->son[1];
	vector<vector<string>> str2 = K_bool_analize(p1);
	//得到K传过来的true or false集合，先判空
	vector<string> t2;
	vector<string> f2;
	if (str2.size() != 0)
	{
		t2 = str2[0];	//K的.true集合
		f2 = str2[1];	//K的.false集合
	}
	//再对D和K进行与操作。
	true_or_false.push_back(t2);	//结果的true集合是E2（K）的true集合
	//求取结果的false集合
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
/*H->or CH|空*/
vector<vector<string>> H_bool_analize(struct node* p) {
	struct node* p1 = NULL;
	vector<string> true_no;
	vector<string> false_no;
	vector<vector<string>> true_or_false;
	if (word_list[flag] == "or")
	{//如果为 and 才是K->and DK 的产生式
		p->addSon("or");
		flag++;
		p->addSon("C");
		p1 = p->son[1];
		vector<vector<string>> str1 = C_bool_analize(p1);
		//得到H的
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
		//这下面进行 or 操作
		//(.true)集合是两个的并集
		for (int i = 0; i < t1.size(); i++)
		{
			true_no.push_back(t1[i]);
		}
		for (int i = 0; i < t2.size(); i++)
		{
			true_no.push_back(t2[i]);
		}
		true_or_false.push_back(true_no);
		//(.false)集合是E2的false集合
		true_or_false.push_back(f2);
		return true_or_false;
		
	}
	else {//K为空

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

	//记录下Mquad
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

	//下面针对tf1  tf2 进行 or 操作
	//true集合
	for (int i = 0; i < t1.size(); i++)
	{
		true_no.push_back(t1[i]);
	}
	for (int i = 0; i < t2.size(); i++)
	{
		true_no.push_back(t2[i]);
	}
	true_or_false.push_back(true_no);
	//false集合即 f2
	true_or_false.push_back(f2);
	for (int i = 0; i < f1.size(); i++)
	{
		BackPatch(Mquad, f1[i]);
	}
	return true_or_false;

}
/* S等（非bool型的产生式 就返回next集合 */

vector<string> S_analize(struct node* p) {
	//flag指向的是 id/if/while/begin 即S中第一个子节点
	//S特别复杂，它有5中可能的候选式，这个挺麻烦的，我先想一下
	/*	1.S-> id:= E

		2.S-> if B then S
		3.S-> if B then S else S

		4.S-> while B do S

		5.S-> begin L end
	四大类 第一个为1.标识符 2.if 3.while 4.begin
	*/
	struct node* p1 = NULL;

	if (IsSymbol_ident(word_list[flag]))
	{//此处表示为一个标识符，即为id 情况1
		p->addSon(word_list[flag]);
		string addr1 = word_addr[flag];		//第一个变量的地址 
		flag++;
		//判断id 的后一个是不是 ：= 不是报错
		if (word_list[flag] == ":=")
		{//存在：=，则没错，进入对E的判断
			p->addSon(":=");
			string op = "51";					//operation操作符
			p->addSon("E");
			flag++;
			p1 = p->son[2];		//将p的第3个子节点E赋值给p，然后进行E分析
			string addr2 = E_analize(p1);		//flag指向的是 E中的第一个变量/符号 
			gen(op, addr2, "0", addr1);
		}
		else {//赋值语句没有：=，报错
			cout << "错误" << error_num << ":\t 赋值语句缺少：= 符号！" << endl;
			error_num++;
		}
	}
	else if (word_list[flag] == "if") {//S下的第一符号为 if ，则为情况2/3
		vector<string> next;//要传递的next集合
		p->addSon("if");
		p->addSon("B");
		//先生成then节点，再在后面分析完B之后分析
		p->addSon("then");
		p1 = p->son[1];
		flag++;
		//进入对B的判断。 flag指向的是B中的第一个元素（标识符
		vector<vector<string>> tf1 = B_analize(p1);		//得到b的true_or_false集合
		vector<string> t1;
		vector<string> f1;
		if (tf1.size() != 0 )
		{
			t1 = tf1[0];
			f1 = tf1[1];
		}
		//要求从B分析出来，flag指向的是then，即记得加一
		//对then进行判断
		if (word_list[flag] == "then")
		{
			//确定有then，则无错。
			flag++;
		}
		else {
			cout << "错误" << error_num << ":\t if语句缺少then关键词！ " << endl;
			error_num++;
		}

		/* 这里是M1的分析 即M1quad （之所以记为M1是因为综合考虑了else 后面的M2 */
		int M1quad = LineOfEqu;

		//在这里又对S进行判断
		p->addSon("S");
		p1 = p->son[3];
		vector<string> next1 = S_analize(p1);	//得到S返回的next集合

		//S分析出来,判断是否有else，若有else，则为情况3，若没有，则情况2
		if (word_list[flag] == "else")
		{//情况3 if B then S else S
			//此处判断S
			flag++;

			//在这里进行N的next记录
			int Nnext = LineOfEqu;
			gen("j", "_", "_", "0");	//以及这个N的emit，考试时差点忘了

			p->addSon("else");

			//在这里进行M2的nextquad记录
			int M2quad = LineOfEqu;
			p->addSon("S");
			p1 = p->son[5];
			vector<string> next2 = S_analize(p1);	//在这里得到S2的next集合
			for (int i = 0; i < t1.size(); i++)
			{
				BackPatch(M1quad, t1[i]);
			}//此处back patch 第一个
			for (int i = 0; i < f1.size() ; i++)
			{
				BackPatch(M2quad, f1[i]);
			}//此处back patch 第二个

			//聚合next集合
			for (int i = 0; i < next1.size() ; i++)
			{
				next.push_back(next1[i]);
			}for (int i = 0; i < next2.size() ; i++)
			{
				next.push_back(next2[i]);
			}//分别压入next1和next2 的 ，得到S的next集合
			return next;

		}
		else {//则为情况2 if B then S 
			//此时flag指向的是 S后的第一个符号，不用后移。
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

	else if (word_list[flag] == "while") {//此处进入情况.4 while B do S
	vector<string> next;
		struct node* p1 = NULL;
		flag++;
		p->addSon("while");
		p->addSon("B");
		p->addSon("do");
		p->addSon("S");
		//分析 B 
		p1 = p->son[1];		//把S的第二个子节点B赋值给p1

		//在 B之前对M节点进行分析
		int M1quad = LineOfEqu;

		vector<vector<string>> tf1 = B_analize(p1);
		vector<string> t1;
		vector<string> f1;
		if (tf1.size() != 0 )
		{
			t1 = tf1[0];
			f1 = tf1[0];
		}//得到了B返回的true、false集合

		//判断有没有do 
		if (word_list[flag] == "do")
		{//有do，则语句没问题
			flag++;
		}
		else {//没有do 报错
			cout << "错误" << error_num << "：\t while语句缺少关键字do ！ " << endl;
			error_num++;
		}
		//在S之前对M2进行分析
		int M2quad = LineOfEqu;

		//在这里再进一步判断S
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

	else if (word_list[flag] == "begin") {//满足这一条件，则是情况5.begin L end
		flag++;
		p->addSon("begin");
		//分析L部分
		p->addSon("L");
		p1 = p->son[1];
		L_Begin(p1);
		p->addSon("end");
		//判断有没有end
		if (word_list[flag] == "end")
		{//有end
			flag++;
		}
		else {
			cout << "错误" << error_num << ":\t begin子句缺少end关键字！ " << endl;
			error_num++;
		}
	}

}

void L_Begin(struct node* p) {
	struct node* p1 = NULL;
	//进行复合语句分析
	/*第一句  存在 递归 和 回溯
		L->S|S;L */
		/*  1.先进S的递归回溯判断
			2.从 S判断函数出来 判断有没有分号；来决定L有无递归
			3.L有递归，则开始L的递归
		*/
	p->addSon("S");
	p1 = p->son[0];
	vector<string> next1 = S_analize(p1);
	//从S出来要求flag指向；
	if (word_list[flag] == ";")
	{//则开始L递归
		flag++;
		p->addSon(";");
		p->addSon("L");
		//在进S之前对M进行分析
		int Mquad = LineOfEqu;
		p1 = p->son[2];
		L_Begin(p1);
	}
	else {
		//若没有分号，则只管进行S的处理分析，S出来后回溯到root的最后一个子节点。
	}
}

void L_analize(struct node* p) {
	//当前的flag指向的是L处，向叶子节点看的话，就是i 先判断是否 该处指向的是 symbol表中的标识符 

	struct node* p1;

	if (IsSymbol_ident(word_list[flag]))
	{//若为真，则该处为一个标识符，正确
		p->addSon(word_list[flag]);
		flag++;
		if (word_list[flag] == ",")//说明此处的L有递归
		{
			p->addSon(",");
			p->addSon("L");
			flag++; //现在指向第2个L的第一个子节点。
			p1 = p->son[2];//因为有递归，将p的第三个子节点L赋值给p
			L_analize(p1);
		}
		else {
			//此处没有递归，只有一个，则出循环即可。
		}
	}
	else {//为假，此处不是标识符，报错
		cout << "错误" << error_num << ":\t 声明语句var后应该接一标识符 或 请勿给标识符取名为保留字 " << endl;
		error_num++;
	}
}

//写一个D->L:K;|L:K;D产生式的递归分析函数
void D_analize(struct node* p) {
	struct node* p1 = NULL;

	p->addSon("L");
	//深度优先，开始对L处进行语法分析
	p1 = p->son[0];		//将D的第一个子节点L赋值给p，进入L分析阶段。
	L_analize(p1);		//从L分析出来，flag指向的是 ：

	p->addSon(":");
	//: 的判断
	if (word_list[flag] == ":")
	{
		//判断有：，则正确
	}
	else {
		cout << "错误" << error_num << ":\t 变量声明语句缺少：！ " << endl;
		error_num++;
	}

	p->addSon("K");
	//	先分析K (判断K的符号是否在 integer，bool，real 的范围内
	flag++;		//flag指向了K
	if (word_list[flag] == "integer" || word_list[flag] == "bool" || word_list[flag] == "real")
	{//判断K处的符号，为正确的类型
		p->son[2]->addSon(word_list[flag]);
		flag++; //flag指针后移，指向了；
	}
	else {
		cout << "错误" << error_num << ":\t 声明语句缺少 类型符号！ " << endl;
		error_num++;
	}

	p->addSon(";");
	//判断有无；
	if (word_list[flag] == ";")
	{//有分号，没错，标志位后移一位即可
		flag++;
	}
	else {
		cout << "错误" << error_num << ":\t 变量声明语句结尾缺少分号； " << endl;
	}

	/*超前搜索把 根据D->L:K;|L:K;D 这个表达式
		需要判断的是 冒号： 判断有无：则可判断有无D的递归
		再来看看往后几轮次的递归中   */
		//flag当前指向；的后一位，如果下一位为：，则D有递归
	if (word_list[flag + 1] == ":")
	{
		p->addSon("D");
		p1 = p->son[4];
		D_analize(p1);
	}

}

void Declear(struct node* p) {
	struct node* p1;
	if (word_list[flag] == "var")//在这里判断有无 var 来判断有没有变量声明语句
	{//		有var 则 开始对 S->var D 进行语法分析
		p->addSon("var");
		p->addSon("D");
		//产生式 D->L : K ;|L : K ; D 这里有递归啊。。
		p1 = p->son[1];		//将 S 的第二个子节点赋值 D 给p，方便p去递归
		flag++;				//flag指向了var后一个，即进入D时flag指向L的第一个字符
		D_analize(p1);
	}
	else {//这里是没有var的报错
		cout << "错误" << error_num << ":\t 声明语句缺少var标识！" << endl;
		error_num++;

	}
}

void parser(struct node* root) {
	struct node* p =NULL ;
	if (word_list[flag] == "program") 
	{//判断有无program程序开始标识
		flag++;
		if (IsSymbol_ident(word_list[flag]) ) 
		{//判断program标识后面有无程序名，即后面那个是否一个标识符=后面那个字符是否在symbol文件中。
			//到此即  有program且有程序名 加上树的结点
			root->addSon(word_list[flag]);
			flag++;				//flag指向的是var
			root->addSon("S");
			p = root->son[1];		//把root的第二个子节点赋值给p
			Declear(p);
			//然后是 复合语句分析模块 flag指向id 
			root->addSon("L");
			p = root->son[2];
			L_Begin(p);
			//差点儿忘了最后这个分号；
			root->addSon(";");
		}
		else 
		{//没有程序名，报错
			cout << "" << error_num << ":\t 程序缺少程序名称 ，或程序名称使用了标识符！";
			error_num++;
		}
	}
	else {//没有program标识，报错
		cout << "错误"<<error_num<<":\t 程序缺少 program 程序标识符！" << endl;
		error_num++;
	}
}

int main() {
	if ((fptoken = fopen("token.txt","r")) == NULL)//判断文件可被打开
	{
		cout << "token文件打开失败！";
		return 0;
	}
	if ((fpsymbol = fopen("symbol.txt", "r")) == NULL) {
		cout << "symbol文件打开失败！";
		return 0;
	}
	if ((fpEqu = fopen("equ.txt", "w")) == NULL)
	{
		cout << "equ文件打开失败！";
		return 0;
	}
	/*用来把token文件字符输出到数组 word_list 中 数组大小为 全局变量 word_num 
			symbol文件符号输出到数组 symbol_list 中 数组大小为 symbol_num
	*/
	word_scanner(fptoken,fpsymbol);
	fclose(fpsymbol);
	fclose(fptoken);

	fpsymbol2 = fopen("symbol.txt", "a+");//关闭symbol文件后，再次以追加方式打开，以便语义分析写入。
	if (fpsymbol == NULL )
	{
		cout << "symbol文件读写方式（第二次）打开失败。" << endl;
		return 0;
	}


	////开始构建树，首先是树根
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

	//收尾
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
