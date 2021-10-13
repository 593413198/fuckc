#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h> // for gcc -m32

// how memory used for code?
// text: code and constant data, in FlASH;
//      int main() {...}
// data: initialied data, like global/extern param;
//      int32_t g_word = 64; 
// bss: all uninitialied data
//      int32_t g_flag;
// stack: call stack's data;
// heap: dynamic memory allocation;
/*
+------------------+
|    stack   |     |      high address
|    ...     v     |
|                  |
|                  |
|                  |
|                  |
|    ...     ^     |
|    heap    |     |
+------------------+
| bss  segment     |
+------------------+
| data segment     |
+------------------+
| text segment     |      low address
+------------------+
*/

#define KB *1024
#define FUCKC_ERROR -1
#define FUCKC_SUCC 1
#define FUCKC_UND 0
#define debug 
// #define int int32_t
#define int long long

// We only use: text, data, stack !!!
int *text;
int *old_text;
int *stack;
char *data;

char *src;

// register
int *pc; // point to next operation
int *sp; // stack pointer
int *bp; // point to somewhere in stack
int ax;

// ISA: instruction set arcitecture
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

// LEX Token
enum { 
    Div = 128, Num, Str, Enter, Err, Comment,
};

int poolsize = 256 KB; // size of text, data, stack
int line; // code line

int token;


void lex()
{
    int number; // number
    char string[100]; // string
    while (token = *src)
    {
        src++; // 这是下一个字符
        if (0)
        {
        }
        else if (token == '"')
        {
            int index = 0;
            while (*src != '"')
            {
                string[index ++] = *src++;
            }
            printf("str: %s\n", string);
            src++;
            token = Str;
            return;
        }
        // 数字 number
        // 十进制: 123  TODO 别的进制
        else if (token >= '0' && token <= '9')
        {
            number = 0;
            while (*src >= '0' && *src <= '9')
                number = number * 10 + *src++ - '0';
            token = Num;
            return;
        }
        // 注释 comment 只考虑//
        else if (token == '/')
        {
            if (*src == '/') // 注释
            {
                // 后面直到换行符都不要
                while (*src++ != '\n') 
                token = Comment;
                // src ++;
                return;
            }
            else // 除法
            {
                token = Div;
                return;
            }
        }
        else if (token == '\n')
        {
            token = Enter;
            return;
        }
        else
        {
            token = Err;
            return;
        }
    }
    return;
}


int eval()
{
    int op, *tmp;
    int cycle = 0;
    while (1)
    {
        cycle ++;
        op = *pc++;
#ifdef debug
        printf("%d> %.4s (%d)\n", cycle,
            & "LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,"
            "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
            "OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT"[op * 5], ax);
#endif
        // IMM const: ax <- const, pc++
        if (op == IMM)  { ax = *pc++; }
        // PUSH: ax -> stack top
        else if (op == PUSH) { --sp; *sp = ax; }
        // ADD: ax <- ax + sp; sp++
        else if (op == ADD) { ax += *sp; sp++; }
        // EXIT
        else if (op == EXIT) { printf("exit with sp: %d\n", *sp); return *sp; }
        else {return -1;}
    }
    return 0;
}

void program()
{
    lex();
    while (token > 0)
    {
        printf("%d: \n", token);
        lex();
    }
}

int main(int argc, char **argv)
{
    FILE* fd;
    // char buff[1 KB];
    char buff[1000];
    
    if ((fd = fopen(argv[1], "r")) < 0)
    {
        printf("Open %s Failed.\n", *argv);
        return FUCKC_ERROR;
    }
    printf("Open %s Succ.\n", argv[1]);

    while (fgets(buff, 1024, fd))
    {
        src = buff;
        printf("source code: %s", src);
        program();
    }

    // malloc
    if (!(text = old_text = malloc(poolsize)))
    {
        printf("malloc text failed with size %d.\n", poolsize);
        return FUCKC_ERROR;
    }
    if (!(data = malloc(poolsize)))
    {
        printf("malloc data failed with size %d.\n", poolsize);
        return FUCKC_ERROR;
    }
    if (!(stack = malloc(poolsize)))
    {
        printf("malloc stack failed with size %d.\n", poolsize);
        return FUCKC_ERROR;
    }

    // memset
    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    // 测试一下Lexical Analysis

    // register
    // bp = sp = (int*) ((int)stack + poolsize);
    // ax = 0;

    // int i = 0;
    // text[i++] = IMM;
    // text[i++] = 10;
    // text[i++] = PUSH;
    // text[i++] = IMM;
    // text[i++] = 20;
    // text[i++] = ADD;
    // text[i++] = PUSH;
    // text[i++] = EXIT;
    // pc = text;
    
    // return eval();
}