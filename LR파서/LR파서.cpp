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
	string action[9]; //action 부분
	int GOTO[3];  //goto 부분 ETF순
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
	{"Fi"},  //i가 id이다.
	{"Fn"} }; //n이 num이다.

stack<string> Stack; //스택

vector<SimbolTable*> simboltable; //변수와 수 저장
string lexeme; //simboltable의 lexptr이 가리킬 주소의 문자열
int lexemeLen = 0; //lexeme의 길이
vector<SimbolTable*> tokenval; //입력된 문자열의 순서대로 토큰을 저장
void lexical_analyzer(string input);
char* lookup(string lexbuf);
bool Strcmp(string lexbuf, char x[]);
char* insert(string lexbuf, Token x);
void input_tokenval(string p, Token x);
int Stlen(string x);
void LRParser();
int changenum(string x); //문자열로 된 숫자를 int로 바꿔주는 함수
string changeTokenToString(Token x); //token을 문자열로 바꿔주는 함수
string changeNumToString(int x);//숫자를 문자열로 바꿔주는 함수

int main()
{
	string input; //입력된 문자열
	Stack.push("0");//스택에는 일단 0이 들어있다.

	getline(cin, input);

	lexical_analyzer(input);
	input_tokenval("&", ENDINPUT); //입력의 마지막에는 $가 들어간다.
	LRParser();

	return 0;
}
void lexical_analyzer(string input)
{
	int count = 0;
	string lexbuf = ""; //토큰

	for (int i = 0; i < input.size(); i++) {
		if (input[i] == ' ')  //공백 무시
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
char* lookup(string lexbuf) //문자열 lexbuf를 찾아서 그 위치를 돌려줌 없으면 NULL
{
	for (int i = 0; i < simboltable.size(); i++) {
		if (Strcmp(lexbuf, simboltable[i]->lexptr)) {
			return simboltable[i]->lexptr;
		}
	}
	return NULL;
}
bool Strcmp(string lexbuf, char x[]) //문자열 비교
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
char* insert(string lexbuf, Token x) //새로운 문자열 p와 토큰 x를 저장하고 그 위치를 돌려줌
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

		switch (parsingtable[num].action[tokenval[Tnum]->token - 1][0]) { //스택에 쌓인거 빼서 파싱테이블에서 어떤 액션을 해야하는지 확인
		case 's': //shift이면
			Stack.push(changeTokenToString(tokenval[Tnum]->token));
			state = &parsingtable[num].action[tokenval[Tnum]->token - 1][1];
			//parsingtable[num].action[tokenval[Tnum]->token - 1][1] = 파싱테이블의 입력의 Tnum번째 액션이 가리키는 상태
			Stack.push(state);
			cout << "shift " << changenum(state) << endl;
			Tnum++;
			break;
		case 'r': 
			//1개짜리는 간단한데 3개짜리가 문제
			state = &parsingtable[num].action[tokenval[Tnum]->token - 1][1];
			len = Stlen(grammar[changenum(state)-1])-1;
			for (int i=0; i < len * 2; i++) //빼내고
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