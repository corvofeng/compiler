%{
/*    minic.y(1.9)    17:46:21    97/12/10
*
*    Parser demo of simple symbol table management and type checking.
*/
#include    <stdio.h>    /* for (f)printf() */
#include    <stdlib.h>    /* for exit() */

#include    "symtab.h"
#include    "types.h"
#include    "check.h"

int        lineno    = 1;    /* number of current source line */
extern int    yylex();    /* lexical analyzer generated from lex.l */
extern char    *yytext;    /* last token, defined in lex.l  */
SYM_TAB     *scope;        /* current symbol table, initialized in lex.l */
char        *base;        /* basename of command line argument */

void yyerror(char *s)
{
    fprintf(stderr,"Syntax error on line #%d: %s\n",lineno,s);
    fprintf(stderr,"Last token was \"%s\"\n",yytext);
    exit(1);
}

%}

%!
%token NUM
%head  expr

%left + -
%left * +
%right UNINUS

%!
%%

expr    : expr '+' expr {$$ = $1 + $3; }
        | expr '-' expr {$$ = $1 - $3; }
        | expr '*' expr {$$ = $1 * $3; }
        | expr '/' expr {$$ = $1 / $3; }
        | '('expr')'      {$$ = $2;}
        | NUM
        |
        ;
%%
