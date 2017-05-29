

file(COPY
    ./main.c ./require.l
    ./run.sh ./MyMake
    ./yacc_test.c ./require.y
    DESTINATION ${PROJECT_BINARY_DIR}/input)
