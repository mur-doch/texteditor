#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gapbuffer.h"

int main()
{
    // GapBuffer *gb = gbCreateEmpty();
    // char c = 48;
    // for (int i = 0; i < 150; i++)
    // // for (int i = 0; i < 150; i++)
    // {
    //     gbInsertCharacter(gb, c);
    //     gbPrint(gb);
    //     printf("\n\n");

    //     c++;
    //     if (c > 122)
    //     {
    //         c = 48;
    //     }
    // }
    // printf("%zu", gb->bufferSize);
    // char c;
    // scanf("%c", &c);
    // while (c != 'q') 
    // {
    //     gbInsertCharacter(gb, c);
    //     gbPrint(gb);
    //     getchar();
    //     scanf("%c", &c);
    // }

    // char string[] = "hello, world!";
    // size_t stringLen = strlen(string);
    // GapBuffer *gb = gbCreateFromString(string, stringLen);
    // gbPrint(gb);
    // for (int i = 0; i < 10; i++) 
    // {
    //     gbInsertCharacter(gb, 'A');
    // }
    // gbPrint(gb);
    // printf("%zu", gb->bufferSize);
    
    // GapBuffer *gb = gbCreateFromString("AA", 2);
    // gbPrint(gb);
    // printf("\n");

    // gbBackspace(gb);
    // gbPrint(gb);
    // printf("\n");

    // gbBackspace(gb);
    // gbPrint(gb);
    // printf("\n");

    // gbInsertCharacter(gb, 'B');
    // gbPrint(gb);
    // printf("\n");

    // gbMoveGapLeft(gb);
    // gbInsertCharacter(gb, 'B');
    // gbPrint(gb);
    // printf("\n");

    // gbMoveGapRight(gb);
    // gbInsertCharacter(gb, 'C');
    // gbPrint(gb);
    // printf("\n");

    // GapBuffer *gb = gbCreateFromString("AAAAA", 5);
    // gbMoveGapByDelta(gb, -5);
    // gbPrint(gb);
    // printf("\n");

    // gbInsertCharacter(gb, 'B');
    // gbPrint(gb);
    // printf("\n");

    // gbMoveGapByDelta(gb, 5);
    // gbPrint(gb);
    // printf("\n");

    char s[] = "hello\nworld!";
    size_t slen = strlen(s);
    GapBuffer *gb = gbCreateEmpty();
    gb

    // GapBuffer *gb = gbCreateFromString(s, slen);

    // gbMoveGapUp(gb);
    // gbPrint(gb);
    // printf("\n");

    // gbMoveGapDown(gb);
    // gbPrint(gb);
    // printf("\n");

    // gbInsertCharacter(gb, 'A');
    // gbPrint(gb);
    // printf("\n");
}