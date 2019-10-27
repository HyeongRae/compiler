#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <math.h>

using namespace std;
#define STATE 17


typedef enum Token {
	PLUS = 1,
	MINUS,
	MULTI,
	DIV,
	LP,
	RP,
	ID,
	NUM,
	ENDINPUT
} Token;
typedef struct SimbolTable {
	char *lexptr;
	Token token;
} SimbolTable;

typedef struct ParsingTable {
	string action[9]; //action �κ�
	int GOTO[3];  //goto �κ� ETF��
} ParsingTable;

ParsingTable parsingtable[STATE] = {
	{ { "","","","","s4","","s5","s6","" }, {1,2,3} },
	{ { "s7","s8","","","","","","","acc" }, {0,0,0} },
	{ { "r3","r3","s9","s10","","r3","","","r3" }, {0,0,0} },
	{ { "r6","r6","r6","r6","","r6","","","r6" }, {0,0,0} },
	{ { "","","","","s4","","s5","s6","" }, {11,2,3} },
	{ { "r8","r8","r8","r8","","r8","","","r8" }, {0,0,0} },
	{ { "r9","r9","r9","r9","","r9","","","r9" }, {0,0,0} },
	{ { "","","","","s4","","s5","s6","" }, {0,12,3} },
	{ { "","","","","s4","","s5","s6","" }, {0,13,3} },
	{ { "","","","","s4","","s5","s6","" }, {0,0,14} },
	{ { "","","","","s4","","s5","s6","" }, {0,0,15} },
	{ { "s7","s8","","","","s16","","","" }, {0,0,0} },
	{ { "r1","r1","s9","s10","","r1","","","r1" }, {0,0,0} },
	{ { "r2","r2","s9","s10","","r2","","","r2" }, {0,0,0} },
	{ { "r4","r4","r4","r4","","r4","","","r4" }, {0,0,0} },
	{ { "r5","r5","r5","r5","","r5","","","r5" }, {0,0,0} },
	{ { "r7","r7","r7","r7","","r7","","","r7" }, {0,0,0} } };

string grammar[9] = {
	{"EE+T"},
	{"EE-T"},
	{"ET"},
	{"TT*F"},
	{"TT/F"},
	{"TF"},
	{"F(E)"},
	{"Fi"},  //i�� id�̴�.
	{"Fn"} }; //n�� num�̴�.

stack<string> Stack; //����

vector<SimbolTable*> simboltable; //������ �� ����
string lexeme; //simboltable�� lexptr�� ����ų �ּ��� ���ڿ�
int lexemeLen = 0; //lexeme�� ����
vector<SimbolTable*> tokenval; //�Էµ� ���ڿ��� ������� ��ū�� ����
void lexical_analyzer(string input);
char* lookup(string lexbuf);
bool Strcmp(string lexbuf, char x[]);
char* insert(string lexbuf, Token x);
void input_tokenval(string p, Token x);
int Stlen(string x);
void LRParser();
int changenum(string x); //���ڿ��� �� ���ڸ� int�� �ٲ��ִ� �Լ�
string changeTokenToString(Token x); //token�� ���ڿ��� �ٲ��ִ� �Լ�
string changeNumToString(int x);//���ڸ� ���ڿ��� �ٲ��ִ� �Լ�

int main()
{
	string input; //�Էµ� ���ڿ�
	Stack.push("0");//���ÿ��� �ϴ� 0�� ����ִ�.

	getline(cin, input);

	lexical_analyzer(input);
	input_tokenval("&", ENDINPUT); //�Է��� ���������� $�� ����.
	LRParser();

	return 0;
}
void lexical_analyzer(string input)
{
	int count = 0;
	string lexbuf = ""; //��ū

	for (int i = 0; i < input.size(); i++) {
		if (input[i] == ' ')  //���� ����
			continue;
		else if (input[i] >= '0' && input[i] <= '9') {       //num
			while (1) {
				if (input[i] >= '0' && input[i] <= '9') {
					lexbuf += input[i];
					i++;
				}
				else
					break;
			}
			i--;
			insert(lexbuf, NUM);
			input_tokenval(lexbuf, NUM);
			lexbuf.clear();
		}
		else if ((input[i] >= 'a'&& input[i]<='z') || (input[i]>='A' && input[i] <= 'Z')) {  //id
			while (1) {
				if ((input[i] >= '0' && input[i] <= '9') || (input[i]>='a'&& input[i]<='z') || (input[i]>='A' && input[i] <= 'Z')) {
					lexbuf += input[i];
					i++;
				}
				else
					break;
			}
			i--;
			char *p = lookup(lexbuf);
			if (p == NULL) {
				p = insert(lexbuf, ID);
			}
			input_tokenval(lexbuf, ID);
			lexbuf.clear();
		}
		else {
			switch (input[i]) {
			case '+':
				lexbuf += "+";
				input_tokenval(lexbuf, PLUS);
				lexbuf.clear();
				break;
			case '-':
				lexbuf += "-";
				input_tokenval(lexbuf, MINUS);
				lexbuf.clear();
				break;
			case '*':
				lexbuf += "*";
				input_tokenval(lexbuf, MULTI);
				lexbuf.clear();
				break;
			case '/':
				lexbuf += "/";
				input_tokenval(lexbuf, DIV);
				lexbuf.clear();
				break;
			case '(':
				lexbuf += "(";
				input_tokenval(lexbuf, LP);
				lexbuf.clear();
				break;
			case ')':
				lexbuf += ")";
				input_tokenval(lexbuf, RP);
				lexbuf.clear();
				break;
			default:
				break;
			}

		}
	}
}
char* lookup(string lexbuf) //���ڿ� lexbuf�� ã�Ƽ� �� ��ġ�� ������ ������ NULL
{
	for (int i = 0; i < simboltable.size(); i++) {
		if (Strcmp(lexbuf, simboltable[i]->lexptr)) {
			return simboltable[i]->lexptr;
		}
	}
	return NULL;
}
bool Strcmp(string lexbuf, char x[]) //���ڿ� ��
{
	int i = 0;
	while (1) {
		if (lexbuf[i] == NULL || x[i] == NULL)
			break;
		else if (lexbuf[i] != x[i])
			return false;
		i++;
	}
	return true;
}
char* insert(string lexbuf, Token x) //���ο� ���ڿ� p�� ��ū x�� �����ϰ� �� ��ġ�� ������
{
	if (lexemeLen != 0)
		lexeme.push_back(NULL);
	lexeme = lexeme + lexbuf;
	if (lexemeLen == 0)
		lexemeLen += Stlen(lexbuf);
	else
		lexemeLen += Stlen(lexbuf) + 1;
	int ind = lexemeLen - Stlen(lexbuf);

	SimbolTable *temp = new SimbolTable;
	temp->lexptr = &lexeme[ind];
	temp->token = x;
	simboltable.push_back(temp);

	return temp->lexptr;
}
void input_tokenval(string p, Token x)
{
	SimbolTable *temp = new SimbolTable;
	if ((p[0] >= '0' && p[0] <= '9') || (p[0]>='a'&& p[0]<='z') || (p[0]>='A' && p[0] <= 'Z'))
		temp->lexptr = lookup(p);
	temp->token = x;
	tokenval.push_back(temp);
}
int Stlen(string x)
{
	int i = 0;
	while (1) {
		if (x[i] == NULL)
			break;
		i++;
	}
	return i;
}
void LRParser()
{
	int Tnum=0, AND=0;

	while(1) {
		int num = changenum(Stack.top());
		string state;
		int len;
		int g;
		string x;

		switch (parsingtable[num].action[tokenval[Tnum]->token - 1][0]) { //���ÿ� ���ΰ� ���� �Ľ����̺��� � �׼��� �ؾ��ϴ��� Ȯ��
		case 's': //shift�̸�
			Stack.push(changeTokenToString(tokenval[Tnum]->token));
			state = &parsingtable[num].action[tokenval[Tnum]->token - 1][1];
			//parsingtable[num].action[tokenval[Tnum]->token - 1][1] = �Ľ����̺��� �Է��� Tnum��° �׼��� ����Ű�� ����
			Stack.push(state);
			cout << "shift " << changenum(state) << endl;
			Tnum++;
			break;
		case 'r': 
			//1��¥���� �����ѵ� 3��¥���� ����
			state = &parsingtable[num].action[tokenval[Tnum]->token - 1][1];
			len = Stlen(grammar[changenum(state)-1])-1;
			for (int i=0; i < len * 2; i++) //������
				Stack.pop();
			x = Stack.top();
			Stack.push(grammar[changenum(state)-1][0]+"");
			switch (grammar[changenum(state)-1][0]) {
				case 'E': Stack.push(changeNumToString(g = parsingtable[changenum(x)].GOTO[0])); break;
				case 'T': Stack.push(changeNumToString(g = parsingtable[changenum(x)].GOTO[1])); break;
				case 'F': Stack.push(changeNumToString(g = parsingtable[changenum(x)].GOTO[2])); break;
			}
			cout << "reduce " << changenum(state);
			cout << " goto " << g << endl;
			break;
		case 'a': 
			AND = 1;
			cout << "accept " << endl;
			break;
		default : 
			cout << "Error" <<endl;
			AND = 1;
			break;
		}
		if (AND == 1)
			break;
	}
}
int changenum(string x)
{
	int num =0;
	int i=0;
	while (1) {
		if (x[i] >= '0' && x[i] <= '9') {
			num = num * 10 + (x[i] - '0');
			i++;
		}
		else
			break;
	}
	return num;
}
string changeNumToString(int x)
{
	string a;
	int len;
	
	len = (int)(log10(x) + 1);

	for (int i = 0; i < len;i++){
		char b = x % 10 +'0';
		a =  b + a;
		x = x / 10;
	}
	return a;
}
string changeTokenToString(Token x)
{
	switch (x) {
	case PLUS: 	return "+";	break;
	case MINUS : return "-"; break;
	case MULTI : return "*"; break;
	case DIV : return "/"; break;
	case LP : return "("; break;
	case RP : return ")"; break;
	case ID : return "i"; break;
	case NUM : return "n"; break;
	case ENDINPUT: return "$"; break;
	default: break;
	}
}