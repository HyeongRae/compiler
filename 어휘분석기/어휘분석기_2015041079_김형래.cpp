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


vector<SimbolTable*> simboltable; //������ �� ����
string lexeme; //simboltable�� lexptr�� ����ų �ּ��� ���ڿ�
int lexemeLen =0; //lexeme�� ����
vector<SimbolTable*> tokenval; //�Էµ� ���ڿ��� ������� ��ū�� ����

void lexical_analyzer(string input);
char* lookup(string lexbuf);
bool Strcmp(string lexbuf, char x[]);
char* insert(string lexbuf, Token x);
void input_tokenval(string p, Token x);
int Stlen(string x);

int main()
{
	string input; //�Էµ� ���ڿ�

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
	string lexbuf=""; //��ū

	for (int i = 0; i < input.size(); i++) {
		if (input[i] == ' ')  //���� ����
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