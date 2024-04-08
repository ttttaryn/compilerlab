# 使用flex处理lexer.l文件，生成lex.yy.c文件,然后使用gcc编诰lex.yy.c文件，生成可执行文件a.out

mkdir build
cd build
cmake ..
cmake -build .
make
./Compilerlab2 ../../cases/e08.c
