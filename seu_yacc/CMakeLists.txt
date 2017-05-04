set(SOURCE_LIB ./yacc.cpp ./yacc.h
                ./lr1.cpp ./lr1.h
                ./grammar.cpp ./grammar.h
                ./expression.cpp ./expression.h)

add_library(yacc ${SOURCE_LIB})
