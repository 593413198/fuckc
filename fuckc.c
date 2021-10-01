#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

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

// We only use: text, data, stack !!!
int *text;
int *old_text;
int *stack;
char *data;

// register
int *pc; // point to next operation
int *sp; // stack pointer
int *bp; // point to somewhere in stack
int ax;

// ISA: instruction set arcitecture
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

int poolsize = 256 KB; // size of text, data, stack
int line; // code line


int main(int argc, char **argv)
{
    FILE* fd;
    if ((fd = fopen(*argv, 0)) < 0)
    {
        printf("Open %s Failed.\n", *argv);
        return FUCKC_ERROR;
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

    // register
    bp = sp = (int*) (&stack + poolsize);
    ax = 0;

    return FUCKC_SUCC;
}