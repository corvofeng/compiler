

file(COPY
    ./main.c ./require.l
    ./run.sh ./MyMake
    ./yacc_test.c ./require.y ./lex.out
    DESTINATION ${PROJECT_BINARY_DIR}/input)
