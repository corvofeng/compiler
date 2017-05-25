%{

#define IF 5
#define ID 12
#define INTEGER 13
#define REAL 14

const int KEY_NUM = 32;
const char* KEY_SET[] = {
    "auto", "break", "case", "char", "const", "continue",
    "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "int", "long", "register",
    "return", "short", "signed", "sizeof", "static", "struct",
    "switch", "typedef", "union", "unsigned", "void", "volatile",
    "while"
};

int isDigit(char ch)
{
    if(ch <= '9' && ch >= '0')
        return 1;
    return 0;
}

int isLetter(char ch)
{
    if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        return 1;
    return 0;
}

int getKeyId(char *str)
{
    for(int i=0; i<KEY_NUM; i++)
    {
        if(strcmp(KEY_SET[i], str) == 0)
            return i+1;
    }
    return 0;
}

int isNoOne(char ch)
{
    if(ch != '\'' && ch != '\0')
        return 1;
    return 0;
}

int isNoTow(char ch)
{
    if(ch != '\"' && ch != '\0')
        return 1;
    return 0;
}

int isTab(char ch)
{
    if(ch == '\t')
        return 1;
    return 0;
}
int isNextLine(char ch)
{
    if(ch == '\n')
        return 1;
    return 0;
}

int isSingleQuote(char ch)
{
    if (ch == '\'')
        return 1;
    return 0;
}

int isDoubleQuote(char ch)
{
    if (ch == '\"')
        return 1;
    return 0;
}
%}

// 此处定义了一些常量以供后面匹配时使用
%!
noone = isNoOne
notow=isNoTow
letter=isLetter
digit=isDigit
\t=isTab
\n=isNextLine
\'=isSingleQuote
\"=isDoubleQuote
%!

%%

({letter}|_)({letter}|_|{digit})* {
    int id = getKeyId(SYLEX_TEXT);
    if(id != 0)
        printf("<%s,->\n", SYLEX_TEXT);
    else {
        printf("<$ID,%s>\n", SYLEX_TEXT);
    }
}
//[\+\-]?[\d]+([\.][\d]*)?([Ee][+-]?[\d]+)?
// 不支持负数(-)?
%$
{digit}+(%.{digit}*)?((E|e)(%+|-)?{digit}+)? {
    printf("<$NUM,%s>\n", SYLEX_TEXT);
}
%$
%(|%)|%{|%}|[|]|;|,|. {
    printf("<%s,->\n", SYLEX_TEXT);
}
%$
% |{\t}|{\n} {

}
%$
(%*=?)|(/=?)|(%+(%+|=)?)|(-(-|=)?)|(<(<|=)?)|(>(>|=))|(==?)|(&(&|=))|(%|(%||=))|(^=?)|(~=?) {
    printf("<%s,->\n", SYLEX_TEXT);
}
%$
({\'}{noone}*{\'})|({\"}{notow}*{\"}) {
    printf("<$STR,%s>\n", SYLEX_TEXT);
}
%$
#include(% )*((<({letter}|_)({letter}|_|{digit})*.h>)|("({letter}|_)({letter}|_|{digit})*.h")) {
    printf("%s  //应该预处理的，暂时先忽略\n", SYLEX_TEXT);
}
%%
