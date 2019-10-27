#include <iostream>
#include <vector>
#include <string>
using namespace std;



typedef enum Token {
	PLUS = 1,
	MINUS,
	MULTI,
	DIV,
	LP,
	RP,
	ID,
	NUM
} Token;
typedef struct SimbolTable {
	char *lexptr;
	Token token;
} SimbolTable;


vector<SimbolTable*> simboltable; //변수와 수 저장
string lexeme; //simboltable의 lexptr이 가리킬 주소의 문자열
int lexemeLen =0; //lexeme의 길이
vector<SimbolTable*> tokenval; //입력된 문자열의 순서대로 토큰을 저장

void lexical_analyzer(string input);
char* lookup(string lexbuf);
bool Strcmp(string lexbuf, char x[]);
char* insert(string lexbuf, Token x);
void input_tokenval(string p, Token x);
int Stlen(string x);

int main()
{
	string input; //입력된 문자열

	getline(cin,input);

	lexical_analyzer(input);

	for (int i = 0; i < tokenval.size(); i++) {
		cout << tokenval[i]->token << ' ';
	}

	return 0;
}
void lexical_analyzer(string input)
{
	int count = 0;
	string lexbuf=""; //토큰

	for (int i = 0; i < input.size(); i++) {
		if (input[i] == ' ')  //공백 무시
			continue;
		else if (input[i] > '0' && input[i] < '9') {       //num
			while (1) {
				if (input[i] > '0' && input[i] < '9') {
					lexbuf+=input[i];
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
		else if ((input[i] > 'a'&& input[i]<'z') || (input[i]>'A' && input[i] < 'Z')) {  //id
			while (1) {
				if ((input[i] > '0' && input[i] < '9') || (input[i]>'a'&& input[i]<'z') || (input[i]>'A' && input[i] < 'Z')) {
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
	if(lexemeLen!=0)
		lexeme.push_back(NULL);
	lexeme = lexeme + lexbuf;
	if (lexemeLen == 0)
		lexemeLen += Stlen(lexbuf);
	else 
		lexemeLen += Stlen(lexbuf)+1;
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
	if ((p[0] > '0' && p[0] < '9') || (p[0]>'a'&& p[0]<'z') || (p[0]>'A' && p[0] < 'Z'))
		temp->lexptr = lookup(p);
	temp->token = x;
	tokenval.push_back(temp);
}
int Stlen(string x)
{
	int i =0;
	while (1) {
		if (x[i] == NULL)
			break;
		i++;
	}
	return i;
}