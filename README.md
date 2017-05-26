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


## Yacc运行

> Yacc输入

```
<(,->
<$NUM,23>
<+,->
<$NUM,16>
<),->
<*,->
<$NUM,3>
<-,->
<$NUM,12>
<+,->
<$NUM,4>
```

> Yacc运行结果

```
                  0                                                     s1
(                 0  1                     0                            s6
( a               0  1  6                  0  23                        A->a
( A               0  1  5                  0  23                        s14
( A +             0  1  5 14               0  23   0                    s6
( A + a           0  1  5 14  6            0  23   0  16                A->a
( A + A           0  1  5 14 23            0  23   0  16                A->A+A
( A               0  1  5                  0  39                        s12
( A )             0  1  5 12               0  39   0                    A->(A)
A                 0  2                    39                            s7
A *               0  2  7                 39   0                        s3
A * a             0  2  7  3              39   0   3                    A->a
A * A             0  2  7 17              39   0   3                    A->A*A
A                 0  2                   117                            s9
A -               0  2  9                117   0                        s3
A - a             0  2  9  3             117   0  12                    A->a
A - A             0  2  9 19             117   0  12                    s8
A - A +           0  2  9 19  8          117   0  12   0                s3
A - A + a         0  2  9 19  8  3       117   0  12   0   4            A->a
A - A + A         0  2  9 19  8 18       117   0  12   0   4            A->A+A
A - A             0  2  9 19             117   0  16                    A->A-A
A                 0  2                   101                            Accept
```


