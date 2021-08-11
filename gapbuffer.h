#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef struct 
// {
//     char *buffer;
//     size_t bufferSize;
//     char *gapStart;
//     char *gapEnd;
// } GapBuffer;

// GapBuffer *gbCreateEmpty() 
// {
//     GapBuffer *newGapBuffer = (GapBuffer*)malloc(sizeof(GapBuffer));
//     newGapBuffer->buffer = (char*)malloc(100);
//     newGapBuffer->bufferSize = 100;
//     newGapBuffer->gapStart = newGapBuffer->buffer;
//     newGapBuffer->gapEnd = newGapBuffer->buffer + newGapBuffer->bufferSize;
//     return newGapBuffer;
// }

// void gbPrint(GapBuffer *gapBuffer) 
// {
//     // Print start buffer
//     printf("\"");
//     int startBufferEnd = gapBuffer->gapStart - gapBuffer->buffer;
//     for (int i = 0; i < startBufferEnd; i++) 
//     {
//         printf("%c", gapBuffer->buffer[i]);
//     }

//     // Print end buffer
//     int endBufferStart = gapBuffer->gapEnd - gapBuffer->buffer;
//     for (int i = endBufferStart; i < gapBuffer->bufferSize; i++) 
//     {
//         printf("%c", gapBuffer->buffer[i]);
//     }
//     printf("\"");
// }

// void gbResize(GapBuffer *gb)
// {
//     char *oldBuffer = gb->buffer;
//     char *newBuffer = (char*)malloc(sizeof(char) * gb->bufferSize + 100);

//     int startBufSize = gb->gapStart - gb->buffer;
//     int endBufSize = (gb->buffer + gb->bufferSize) - gb->gapEnd;
//     int endBufStart = gb->gapEnd - gb->buffer;

//     // Copy start and end buffers over
//     memcpy(newBuffer, oldBuffer, startBufSize);
//     memcpy(newBuffer + endBufStart + 100, oldBuffer + endBufStart, endBufSize);

//     // Update the struct data
//     gb->bufferSize += 100;
//     gb->gapStart = newBuffer + startBufSize;
//     gb->gapEnd = newBuffer + endBufStart + 100;
//     gb->buffer = newBuffer;

//     free(oldBuffer);
// }

// void gbInsertCharacter(GapBuffer *gb, char c)
// {
//     // If gap buffer size is not 0 or 1, just add the character
//     int gapSize = gb->gapEnd - gb->gapStart;
//     if (gapSize > 1) 
//     {
//         *gb->gapStart = c;
//         gb->gapStart++;
//         return;
//     }

//     // Otherwise, we're going to resize the buffer and then add the character
//     // afterwards
//     gbResize(gb);
//     *gb->gapStart = c;
//     gb->gapStart++;
// }

// typedef struct 
// {
//     char *buffer;
//     size_t bufferSize;
//     size_t preSize;
//     size_t postSize;
// } GapBuffer;

typedef struct
{
    char *buffer;
    size_t bufferSize;
    size_t preSize;
    size_t postSize;
} GapBuffer;

GapBuffer *gbCreateEmpty();
GapBuffer *gbCreateFromString(char *string, size_t stringLen);
void gbPrint(GapBuffer *gapBuffer);
void gbResize(GapBuffer *gb);
void gbResizeForAmount(GapBuffer *gb, size_t amount);
void gbInsertCharacter(GapBuffer *gb, char c);
void gbInsertString(GapBuffer *gb, char *s, size_t slen);
void gbMoveGapLeft(GapBuffer *gb);
void gbMoveGapRight(GapBuffer *gb);
void gbMoveGapToIndex(GapBuffer *gb, int index);
void gbMoveGapByDelta(GapBuffer *gb, int delta);
void gbMoveGapUp(GapBuffer *gb);
void gbMoveGapDown(GapBuffer *gb);
void gbBackspace(GapBuffer *gb);
void gbGetCursor(GapBuffer *gb, int *cy, int *cx, int cols);
GapBuffer *gbCreateFromFile(char *filename);
void gbWriteToFile(GapBuffer *gb, char *filename);
char *gbGetString(GapBuffer *gb);
void gbFree(GapBuffer *gb);

#endif