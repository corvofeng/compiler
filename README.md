# 编译器课程设计


## 程序运行

```bash
mkdir build
cd build
cmake ..
make -j4
```

## Lex运行

```bash
cd bin
./comp

cd ../input
gcc out.c -o out
./out main.c
```

`lex`主要是将`require.l`文件解析, 然后再生成`out.c`文件

