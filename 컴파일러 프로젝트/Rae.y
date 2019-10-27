%{
#define MSDOS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG	0

#define	 MAXSYM	100
#define	 MAXSYMLEN	20
#define	 MAXTSYMLEN	15
#define	 MAXTSYMBOL	MAXSYM/2


int tsymbolcnt=0;
int errorcnt=0;

FILE *fp;

extern char symtbl[MAXSYM][MAXSYMLEN];
extern char sent[MAXSYM][MAXSYM];
extern int maxsym;
extern int maxsent;
extern int lineno;

void	dwgen();
int		gentemp();
void	loopstart(int, int);
void	loopend(int);
void	ifend(int);
void	ifBL(int, int, int);
void	ifBR(int, int, int);
void	ifBLS(int, int, int);
void	ifBRS(int, int, int);
void	ifSAME(int, int, int);
void	inputnumstmt(int);
void	printstmt(int);
void	printsent(int);
void 	linestmt();
void 	spacestmt();
void	assgnstmt(int, int);
void	numassgn(int, int);
void	addstmt(int, int, int);
void	substmt(int, int, int);
void	multistmt(int, int, int);
void	divstmt(int, int, int);
int		insertsym(char *);
%}

%left ADD SUB
%left MULTI DIV
%right ASSGN
%token	MULTI DIV ADD SUB LOOP IF INPUT PRINT LINE SPACE LP RP BLS BRS BL BR SAME ASSGN ID NUM SENTENCE STMTEND START END


%%
program	: START stmt_list END		{ if (errorcnt==0) dwgen(); }
	;

stmt_list: 	stmt_list stmt 
	|	stmt_list loop_stmt	
	|	stmt_list if_stmt
	|	/* null */
	| 	error STMTEND	{ errorcnt++; yyerrok;}
	;

loop_stmt : loop_start stmt_list STMTEND	{loopend($1);}
	;

loop_start : LOOP term ASSGN 				{$$=gentemp(); loopstart($$, $2);}
	;

if_stmt	: if_start stmt_list STMTEND		{ifend($1);}
	;

if_start : IF term BL term	ASSGN		{$$=gentemp(); ifBL($$, $2, $4);}
	|	IF term BR term   	ASSGN		{$$=gentemp(); ifBR($$, $2, $4);}
	|	IF term BLS term	ASSGN		{$$=gentemp(); ifBLS($$, $2, $4);}
	|	IF term BRS term	ASSGN		{$$=gentemp(); ifBRS($$, $2, $4);}
	|	IF term SAME term	ASSGN		{$$=gentemp(); ifSAME($$, $2, $4);}
	;

stmt	: 	ID ASSGN expr STMTEND			{$$=$1; assgnstmt($1, $3);}
	|	INPUT ID STMTEND					{$$=$1; inputnumstmt($2);}
	|	PRINT print_stmt STMTEND
	;

print_stmt	: print_stmt LINE 				{ linestmt();}	
	|	print_stmt SPACE					{ spacestmt();}
	|	print_stmt ID  						{ printstmt($2);}
	|	print_stmt SENTENCE 				{ printsent($2);}
	|	/* null */
	;

expr	:	expr ADD expr	{ $$=gentemp(); addstmt($$, $1, $3); }
	|	expr SUB expr		{ $$=gentemp(); substmt($$, $1, $3); }
	|	expr MULTI expr		{ $$=gentemp(); multistmt($$, $1, $3); }
	|	expr DIV expr		{ $$=gentemp(); divstmt($$, $1, $3); }
	|	LP expr RP			{ $$=$2;}
	|	term
	;


term	:	ID	
	|	NUM		{ $$=gentemp(); numassgn($$, $1); }
	;


%%
main() 
{
	printf("\nRae compiler v1.0\n");

	fp=fopen("a.asm", "w");
	
	yyparse();

	fclose(fp);

	if (errorcnt==0) 
		{ printf("Successfully compiled. Assembly code is in 'a.asm'.\n");}
}

yyerror(s)
char *s;
{
	printf("%s (line %d)\n", s, lineno);
}

void loopstart(lcnt, num)
int lcnt;
int num;
{
		char reloop[26]="reloop";

		strcat(reloop, symtbl[lcnt]);

		fprintf(fp, "$ -- LOOP START --\n");
		fprintf(fp, "LVALUE %s\n", symtbl[lcnt]); 
		fprintf(fp, "RVALUE %s\n", symtbl[num]);  
		fprintf(fp, ":=\n");
		fprintf(fp, "LABEL %s\n", reloop);
}

void loopend(lcnt)
int lcnt;
{	
		char reloop[26]="reloop";
		char outloop[27]="outloop";
		strcat(outloop, symtbl[lcnt]);
		strcat(reloop, symtbl[lcnt]);

		fprintf(fp, "LVALUE %s\n", symtbl[lcnt]); 
		fprintf(fp, "RVALUE %s\n", symtbl[lcnt]); 
		fprintf(fp, "PUSH 1\n"); 
		fprintf(fp, "-\n");
		fprintf(fp, ":=\n");

		fprintf(fp, "RVALUE %s\n", symtbl[lcnt]); 
		fprintf(fp, "GOFALSE %s\n", outloop);
		fprintf(fp, "GOTO %s\n", reloop);
		fprintf(fp, "LABEL %s\n", outloop);
		fprintf(fp, "$ -- LOOP END --\n");
}

void ifend(icnt)
int icnt;
{
		char outif[25]="outif";
		strcat(outif, symtbl[icnt]);
		fprintf(fp, "LABEL %s\n", outif);
		fprintf(fp, "$ -- IF END --\n");
}

void ifBL(icnt, left, right)
int icnt;
int left;
int right;
{
		char outif[25]="outif";
		strcat(outif, symtbl[icnt]);

		fprintf(fp, "$ -- IF START --\n");
		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");
		fprintf(fp, "GOMINUS %s\n", outif);

		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");
		fprintf(fp, "GOFALSE %s\n", outif);
}

void ifBR(icnt, left, right)
int icnt;
int left;
int right;
{
		char outif[25]="outif";
		strcat(outif, symtbl[icnt]);
		fprintf(fp, "$ -- IF START --\n");
		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");
		fprintf(fp, "GOPLUS %s\n", outif);

		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");
		fprintf(fp, "GOFALSE %s\n", outif);
}

void ifBLS(icnt, left, right)
int icnt;
int left;
int right;
{
		char outif[25]="outif";
		strcat(outif, symtbl[icnt]);
		fprintf(fp, "$ -- IF START --\n");
		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");

		fprintf(fp, "GOMINUS %s\n", outif);
}

void ifBRS(icnt, left, right)
int icnt;
int left;
int right;
{
		char outif[25]="outif";
		strcat(outif, symtbl[icnt]);
		fprintf(fp, "$ -- IF START --\n");
		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");

		fprintf(fp, "GOPLUS %s\n", outif);
}

void ifSAME(icnt, left, right)
int icnt;
int left;
int right;
{
		char outif[25]="outif";
		strcat(outif, symtbl[icnt]);
		fprintf(fp, "$ -- IF START --\n");
		fprintf(fp, "RVALUE %s\n", symtbl[left]); 
		fprintf(fp, "RVALUE %s\n", symtbl[right]); 
		fprintf(fp, "-\n");

		fprintf(fp, "GOTRUE %s\n", outif);
}

void inputnumstmt(num)
int num;
{
		fprintf(fp, "$ -- INPUT --\n");
		fprintf(fp, "LVALUE %s\n", symtbl[num]); 
		fprintf(fp, "INNUM\n");
		fprintf(fp, ":=\n");
}

void printstmt(left)
int left;
{
		fprintf(fp, "$ -- PRINT ID --\n");
		fprintf(fp, "RVALUE %s\n", symtbl[left]);
		fprintf(fp, "OUTNUM\n");
}
void printsent(idx)
int idx;
{
	int i;
	fprintf(fp, "$ -- PRINT  SENTENCE--\n");
	for(i=1; i<strlen(sent[idx])-1; i++) {
			fprintf(fp, "PUSH %d\n", sent[idx][i]);
			fprintf(fp, "OUTCH\n");
	}
}
void linestmt()
{
		fprintf(fp, "$ -- LINE --\n");
		fprintf(fp, "PUSH 10\n"); 
		fprintf(fp, "OUTCH\n");
}
void spacestmt()
{
		fprintf(fp, "$ -- SPACE --\n");
		fprintf(fp, "PUSH 32\n"); 
		fprintf(fp, "OUTCH\n");
}
void numassgn(idx, num)
int idx;
int num;
{
	fprintf(fp, "$ -- NUM ASSIGNMENT STMT --\n");
	fprintf(fp, "LVALUE %s\n", symtbl[idx]); 
	fprintf(fp, "PUSH %d\n", num); 
	fprintf(fp, ":=\n");
}

void assgnstmt(left, right)
int left;
int right;
{	
	fprintf(fp, "$ -- ID ASSIGNMENT STMT --\n");
	fprintf(fp, "LVALUE %s\n", symtbl[left]); 
	fprintf(fp, "RVALUE %s\n", symtbl[right]); 
	fprintf(fp, ":=\n");
}

void addstmt(t, first, second)
int t;
int first;
int second;
{
	fprintf(fp, "$ -- ADD STMT --\n");
	fprintf(fp, "LVALUE %s\n", symtbl[t]); 
	fprintf(fp, "RVALUE %s\n", symtbl[first]); 
	fprintf(fp, "RVALUE %s\n", symtbl[second]); 
	fprintf(fp, "+\n");
	fprintf(fp, ":=\n");
}

void substmt(t, first, second)
int t;
int first;
int second;
{
	fprintf(fp, "$ -- SUB STMT --\n");
	fprintf(fp, "LVALUE %s\n", symtbl[t]); 
	fprintf(fp, "RVALUE %s\n", symtbl[first]); 
	fprintf(fp, "RVALUE %s\n", symtbl[second]); 
	fprintf(fp, "-\n");
	fprintf(fp, ":=\n");
}

void multistmt(t, first, second)
int t;
int first;
int second;
{
	fprintf(fp, "$ -- MULTI STMT --\n");
	fprintf(fp, "LVALUE %s\n", symtbl[t]); 
	fprintf(fp, "RVALUE %s\n", symtbl[first]); 
	fprintf(fp, "RVALUE %s\n", symtbl[second]); 
	fprintf(fp, "*\n");
	fprintf(fp, ":=\n");
}

void divstmt(t, first, second)
int t;
int first;
int second;
{
	fprintf(fp, "$ -- DIV STMT --\n");
	fprintf(fp, "LVALUE %s\n", symtbl[t]); 
	fprintf(fp, "RVALUE %s\n", symtbl[first]); 
	fprintf(fp, "RVALUE %s\n", symtbl[second]); 
	fprintf(fp, "/\n");
	fprintf(fp, ":=\n");
}

int gentemp()
{
char buffer[MAXTSYMLEN];
char tempsym[MAXSYMLEN]="TTCBU";
	
	tsymbolcnt++;
	if (tsymbolcnt > MAXTSYMBOL) printf("temp symbol overflow\n");
	itoa(tsymbolcnt, buffer, 10);
	strcat(tempsym, buffer);
	return( insertsym(tempsym) ); // Warning: duplicated symbol is not checked for lazy implementation
}

void dwgen()
{
int i;
	fprintf(fp, "HALT\n");
	fprintf(fp, "$ -- END OF EXECUTION CODE AND START OF VAR DEFINITIONS --\n");

// Warning: this code should be different if variable declaration is supported in the language 
	for(i=0; i<maxsym; i++) 
		fprintf(fp, "DW %s\n", symtbl[i]);
	fprintf(fp, "END\n");
}

