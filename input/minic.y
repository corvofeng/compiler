%{
/*	minic.y(1.9)	17:46:21	97/12/10
*
*	Parser demo of simple symbol table management and type checking.
*/
#include	<stdio.h>	/* for (f)printf() */
#include	<stdlib.h>	/* for exit() */

#include	"symtab.h"
#include	"types.h"
#include	"check.h"

int		lineno	= 1;	/* number of current source line */
extern int	yylex();	/* lexical analyzer generated from lex.l */
extern char	*yytext;	/* last token, defined in lex.l  */
SYM_TAB 	*scope;		/* current symbol table, initialized in lex.l */
char		*base;		/* basename of command line argument */

void
yyerror(char *s)
{
fprintf(stderr,"Syntax error on line #%d: %s\n",lineno,s);
fprintf(stderr,"Last token was \"%s\"\n",yytext);
exit(1);
}

%}

%union	{
	char*		name;
	int		value;
	T_LIST*		tlist;
	T_INFO*		type;
	SYM_INFO*	sym;
	SYM_LIST*	slist;
	}

%token	INT FLOAT NAME STRUCT IF ELSE RETURN NUMBER LPAR RPAR LBRACE RBRACE
%token	LBRACK RBRACK ASSIGN SEMICOLON COMMA DOT PLUS MINUS TIMES DIVIDE EQUAL

%type	<name>	NAME
%type	<value>	NUMBER
%type	<type>	type parameter exp lexp
%type	<tlist>	parameters more_parameters exps
%type	<sym>	field var
%type	<slist>	fields

/*	associativity and precedence: in order of increasing precedence */

%nonassoc	LOW  /* dummy token to suggest shift on ELSE */
%nonassoc	ELSE /* higher than LOW */

%nonassoc	EQUAL
%left		PLUS	MINUS
%left		TIMES	DIVIDE
%left		UMINUS	/* dummy token to use as precedence marker */
%left		DOT	LBRACK	/* C compatible precedence rules */

%%
program		: declarations
		;

declarations	: declaration declarations
		| /* empty */
		;

declaration	: fun_declaration
		| var_declaration
		;

fun_declaration	: type NAME {	/* this is $3 */
			$<sym>$ = symtab_insert(scope,$2,0);
			scope = symtab_open(scope); /* open new scope */
			scope->function = $<sym>$; /* attach to this function */
			}
		  LPAR parameters RPAR {	/* this is $7 */
			$<sym>3->type = types_fun($1,$5);
			}
		  block	{ scope = scope->parent; }
		;

parameters	: more_parameters	{ $$ = $1; }
		|			{ $$ = 0; }
		;

more_parameters	: parameter COMMA more_parameters
					{ $$ = types_list_insert($3,$1); }
		| parameter		{ $$ = types_list_insert(0,$1); }
		;

parameter	: type NAME {
			symtab_insert(scope,$2,$1); /* insert in symbol table */
			$$ = $1; /* remember type info */
			}
		;

block		: LBRACE 		{  scope = symtab_open(scope); }
		  var_declarations statements RBRACE
					{ scope = scope->parent; /* close scope */}
		;

var_declarations: var_declaration var_declarations
		|
		;

var_declaration	: type NAME SEMICOLON	{ symtab_insert(scope,$2,$1); }
		;

type		: INT			{ $$ = types_simple(int_t); }
		| FLOAT			{ $$ = types_simple(float_t); }
		| type TIMES		{ $$ = types_array($1); }
		| STRUCT LBRACE fields RBRACE /* record */
					{ $$ = types_record($3); }
		;

fields		: field fields		{ $$ = symtab_list_insert($2,$1); }
		|			{ $$ = 0; }
		;

field		: type NAME SEMICOLON	{ $$ = symtab_info_new($2,$1); }
		;

statements	: statement SEMICOLON statements
		| /* empty */
		;

statement	: IF LPAR exp RPAR statement 		%prec LOW
		| IF LPAR exp RPAR statement ELSE statement	/* shift */
		| lexp ASSIGN exp	{ check_assignment($1,$3); }
		| RETURN exp /* statements always in scope with function */
			{ check_assignment(scope->function->type->info.fun.target,$2); }
		| block
		;

lexp		: var			{ $$ = $1->type; }
		| lexp LBRACK exp RBRACK{ $$ = check_array_access($1,$3); }
		| lexp DOT NAME		{ $$ = check_record_access($1,$3); }
		;

exp		: exp DOT NAME		{ $$ = check_record_access($1,$3); }
		| exp LBRACK exp RBRACK	{ $$ = check_array_access($1,$3); }
		| exp PLUS exp		{ $$ = check_arith_op(PLUS,$1,$3); }
		| exp MINUS exp		{ $$ = check_arith_op(MINUS,$1,$3); }
		| exp TIMES exp		{ $$ = check_arith_op(TIMES,$1,$3); }
		| exp DIVIDE exp	{ $$ = check_arith_op(DIVIDE,$1,$3); }
		| exp EQUAL exp		{ $$ = check_relop(EQUAL,$1,$3); }
		| LPAR exp RPAR	{ $$ = $2; }
		| MINUS exp 	%prec UMINUS /* this will force a reduce */
					{ $$ = check_arith_op(UMINUS,$2,0); }
		| var			{ $$ = $1->type; }
		| NUMBER 		{ $$ = types_simple(int_t); }
		| NAME LPAR RPAR	{ $$ = check_fun_call(scope,$1,0); }
		| NAME LPAR exps RPAR	{ $$ = check_fun_call(scope,$1,&$3); }
		;

exps		: exp 			{ $$ = types_list_insert(0,$1); }
		| exp COMMA exps	{ $$ = types_list_insert($3,$1); }
		;

var		: NAME 			{ $$ = check_symbol(scope,$1); }
%%

int
main(int argc,char *argv[])
{
if (argc!=2) {
	fprintf(stderr,"Usage: %s base_file_name",argv[0]);
	exit(1);
	}
base = argv[1];
return yyparse();
}
