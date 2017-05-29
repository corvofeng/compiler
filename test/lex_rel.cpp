#include "lex.h"
#include <stdio.h>

void printHelp() {
    printf("Usage: lex_rel INPUT_FILE OUT_C\n");
    exit(0);
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        printHelp();
    }

    Lex lex(argv[1], "lex.yy.c");
    lex.scaner();
    lex.dfaMerge();
    lex.nfa2DFA();
    lex.output();

    return 0;
}
