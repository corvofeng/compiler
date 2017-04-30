set(SOURCE_LIB ./lex.cpp ./lex.h
                ./nfa.cpp ./nfa.h
                ./dfa.cpp ./dfa.h
                ./state.h ./state.cpp)

add_library(lex ${SOURCE_LIB})
