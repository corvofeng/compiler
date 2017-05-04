
# include 文件夹  gcc -I
include_directories(${PROJECT_SOURCE_DIR}/seu_lex ${PROJECT_SOURCE_DIR}/seu_yacc)

# 链接库目录    gcc -L
link_directories(${PROJECT_BINARY_DIR}/seu_lex ${PROJECT_SOURCE_DIR}/seu_yacc)

# 设置源码文件
set(SOURCE_FILES ./main.cpp)

# 设置编译输出文件
add_executable(comp ${SOURCE_FILES})

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


TARGET_LINK_LIBRARIES(comp lex yacc)

