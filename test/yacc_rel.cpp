#include "yacc.h"
#include <stdio.h>

void printHelp() {
    printf("Usage: yacc_rel INPUT_FILE PARSE_FILE\n");
    exit(0);
}

int main(int argc, char *argv[])
{

    if(argc < 3) {
        printHelp();
    }

    Yacc *yacc = new Yacc(argv[1], "y.tab.c");
    yacc->scanner();
    yacc->buildTable();
    yacc->parse(argv[2]);
    delete yacc;


    return 0;
}
