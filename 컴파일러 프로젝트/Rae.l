%{
#include <string.h>
#include "Rae.h"
#define MAXSYM	100
#define	MAXSYMLEN	20
char symtbl[MAXSYM][MAXSYMLEN];
char sent[MAXSYM][MAXSYM];
int lineno=1;
int insertsym();
int insertsent();
int maxsym=0;
int maxsent=0;

char s[30];
char a[100];
%}
sp		[ \t]
ws		{sp}+
nl		\n
eletter		[A-Za-z]
hletter		[\xb0-\xfe][\xa0-\xfe]
letter		({eletter}|{hletter})
digit		[0-9]
id			{letter}({letter}|{digit})*
sentence	\"([^\xb0-\xfe\xa0-\xfe])*\"
%%
{ws}		{/* do nothing */}
{nl}		{lineno++; }
더하기	{return(ADD); }
빼기		{return(SUB);}
곱하기	{return(MULTI);}
나누기	{return(DIV);}
반복		{return(LOOP);}
조건		{return(IF);}
입력		{return(INPUT);}
출력		{return(PRINT);}
개행		{return(LINE);}
공백		{return(SPACE);}
\(		{return(LP);}
\)		{return(RP);}
">="	{return(BLS);}
"<="	{return(BRS);}
\>		{return(BL);}
\<		{return(BR);}
"=="	{return(SAME);}
:=		{return(ASSGN); }
;		{return(STMTEND); }
시작		{return(START);}
끝		{return(END);}
{id}			{strncpy(s, yytext, yyleng); s[yyleng]='\0'; yylval=insertsym(s); return(ID);}
{digit}+		{sscanf(yytext, "%d", &yylval); return(NUM);}
{sentence}		{strncpy(a, yytext, yyleng); a[yyleng]='\0'; yylval=insertsent(a); return(SENTENCE);}
.				{printf("invalid token %s\n", yytext); }
%%

int insertsym(s)
char *s;
{
int i;

	for(i=0;i<maxsym; i++) {
		if(strcmp(s, symtbl[i])==0) return(i);
		}
	if (i<MAXSYM-1) {
		strcpy(symtbl[maxsym],s);
		maxsym++;
		return(maxsym-1);
		}
	else
		{printf("symbol table overflow\n");}
	return(0);
}

int insertsent(s)
char *s;
{
int i;

	for(i=0;i<maxsent; i++) {
		if(strcmp(s, sent[i])==0) return(i);
		}
	if (i<MAXSYM-1) {
		strcpy(sent[maxsent],s);
		maxsent++;
		return(maxsent-1);
		}
	else
		{printf("symbol table overflow\n");}
	return(0);
}
yywrap() {}