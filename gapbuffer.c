#include "gapbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef struct 
// {
//     char *buffer;
//     size_t bufferSize;
//     size_t preSize;
//     size_t postSize;
// } GapBuffer;

GapBuffer *gbCreateEmpty() 
{
    GapBuffer *newGapBuffer = (GapBuffer*)malloc(sizeof(GapBuffer));
    newGapBuffer->buffer = (char*)malloc(100);
    newGapBuffer->bufferSize = 100;
    newGapBuffer->preSize = 0;
    newGapBuffer->postSize = 0;
    return newGapBuffer;
}

GapBuffer *gbCreateFromString(char *string, size_t stringLen)
{
    GapBuffer *newGapBuffer = (GapBuffer*)malloc(sizeof(GapBuffer));
    newGapBuffer->buffer = (char*)malloc(100 + stringLen);
    newGapBuffer->bufferSize = 100 + stringLen;
    strncpy(newGapBuffer->buffer, string, stringLen);
    newGapBuffer->preSize = stringLen;
    newGapBuffer->postSize = 0;
    return newGapBuffer;
}

void gbPrint(GapBuffer *gapBuffer) 
{
    // Print start buffer
    printf("\"(PreBuffer)");
    for (int i = 0; i < gapBuffer->preSize; i++) 
    {
        printf("%c", gapBuffer->buffer[i]);
    }

    // Print end buffer
    printf("(PostBuffer)");
    int endBufferStart = gapBuffer->bufferSize - gapBuffer->postSize;
    for (int i = endBufferStart; i < gapBuffer->bufferSize; i++) 
    {
        printf("%c", gapBuffer->buffer[i]);
    }
    printf("\"");
}

void gbResize(GapBuffer *gb)
{
    char *oldBuffer = gb->buffer;
    char *newBuffer = (char*)malloc(sizeof(char) * gb->bufferSize + 100);

    int endBufStart = gb->bufferSize - gb->postSize;

    // Copy start and end buffers over
    memcpy(newBuffer, oldBuffer, gb->preSize);
    memcpy(newBuffer + endBufStart + 100, oldBuffer + endBufStart, gb->postSize);

    // Update the struct data
    gb->bufferSize += 100;
    gb->buffer = newBuffer;
    // Don't need to update preSize or postSize, because only the gapSize changed here

    free(oldBuffer);
}

void gbResizeForAmount(GapBuffer *gb, size_t amount)
{
    char *oldBuffer = gb->buffer;
    char *newBuffer = (char*)malloc(gb->bufferSize + 100 + amount);

    int endBufStart = gb->bufferSize - gb->postSize;

    // Copy start and end buffers over
    memcpy(newBuffer, oldBuffer, gb->preSize);
    memcpy(newBuffer + endBufStart + 100 + amount, oldBuffer + endBufStart, gb->postSize);

    // Update the struct data
    gb->bufferSize += 100 + amount;
    gb->buffer = newBuffer;
    // Don't need to update preSize or postSize, because only the gapSize changed here

    free(oldBuffer);
}

void gbInsertCharacter(GapBuffer *gb, char c)
{
    // If gap buffer size is not 0 or 1, just add the character
    int gapSize = gb->bufferSize - (gb->preSize + gb->postSize);
    if (gapSize > 1) 
    {
        gb->buffer[gb->preSize] = c;
        gb->preSize++;
        return;
    }

    // Otherwise, we're going to resize the buffer and then add the character
    // afterwards
    gbResize(gb);
    gb->buffer[gb->preSize] = c;
    gb->preSize++;
}

void gbInsertString(GapBuffer *gb, char *s, size_t slen)
{
    int gapSize = gb->bufferSize - (gb->preSize + gb->postSize);
    if (gapSize > slen)
    {
        // we'll just copy s into prebuffer
        memcpy(gb->buffer + gb->preSize, s, slen);
        gb->preSize += slen;
    }
    else
    {
        // we'll resize gb
        gbResizeForAmount(gb, slen);
        memcpy(gb->buffer + gb->preSize, s, slen);
        gb->preSize += slen;
    }
}

void gbMoveGapLeft(GapBuffer *gb)
{
    if (gb->preSize > 0)
    {
        int postBufferStart = gb->bufferSize - (gb->postSize + 1);
        gb->buffer[postBufferStart] = gb->buffer[gb->preSize-1];
        gb->preSize--;
        gb->postSize++;
    }
}

void gbMoveGapRight(GapBuffer *gb)
{
    if (gb->postSize > 0)
    {
        int postBufferStart = gb->bufferSize - (gb->postSize);
        gb->buffer[gb->preSize] = gb->buffer[postBufferStart];
        gb->preSize++;
        gb->postSize--; 
    }
}

void gbMoveGapToIndex(GapBuffer *gb, int index)
{
    if (index == gb->preSize)
    {
        return;
    }
    else if (index < gb->preSize)
    {
        int bytesToMove = gb->preSize - index;
        gb->preSize -= bytesToMove;
        gb->postSize += bytesToMove;
        int postBufferStart = gb->bufferSize - (gb->postSize);
        memcpy(gb->buffer + postBufferStart, gb->buffer + index, bytesToMove);
    }
    else    // index > gb->preSize
    {
        int postBufferStart = gb->bufferSize - (gb->postSize);
        int indexDelta = index - gb->preSize;
        // int postBufferStart = gb->bufferSize - gb->postSize;
        memcpy(gb->buffer + gb->preSize, gb->buffer + postBufferStart, indexDelta);
        gb->preSize += indexDelta;
        gb->postSize -= indexDelta;
    }
}

void gbMoveGapByDelta(GapBuffer *gb, int delta)
{
    if (delta == 0)
    {
        return;
    }
    else if (delta < 0)
    {
        delta *= -1;
        gb->preSize -= delta;
        gb->postSize += delta;
        int postBufferStart = gb->bufferSize - (gb->postSize);
        memcpy(gb->buffer + postBufferStart, gb->buffer + gb->preSize, delta);
    }
    else    // delta > 0
    {
        int moveStart = gb->bufferSize - gb->postSize;
        memcpy(gb->buffer + gb->preSize, gb->buffer + moveStart, delta);
        gb->preSize += delta;
        gb->postSize -= delta;
    }
}

void gbMoveGapUp(GapBuffer *gb)
{
    // read backwards, looking for newline
    if (gb->preSize == 0)
    {
        return;
    }

    for (int i = gb->preSize - 1; i > -1; i--)
    {
        if (gb->buffer[i] == '\n')
        {
            int delta = i - gb->preSize;
            gbMoveGapByDelta(gb, delta);
            break;
        }
    }
}

void gbMoveGapDown(GapBuffer *gb)
{
    // read forwards, looking for newline
    if (gb->postSize == 0)
    {
        return;
    }

    int postStart = gb->bufferSize - gb->postSize;
    for (int i = postStart; i < gb->bufferSize; i++)
    {
        if (gb->buffer[i] == '\n')
        {
            int delta = i - postStart + 1;
            gbMoveGapByDelta(gb, delta);
            break;
        }
    }
}

void gbBackspace(GapBuffer *gb)
{
    if (gb->preSize > 0)
    {
        // FOR TESTING
        gb->buffer[gb->preSize] = 'A';
        gb->preSize--;
    }
}

void gbGetCursor(GapBuffer *gb, int *cy, int *cx, int cols)
{
    int lineCount = 0;
    int colCount = 0;
    for (int i = 0; i < gb->preSize; i++)
    {
        if (gb->buffer[i] == '\n')
        {
            lineCount++;
            colCount = 0;
        }
        else
        {
            colCount++;
            if (colCount >= cols)
            {
                colCount = 0;
                lineCount++;
            }
        }
    }

    *cy = lineCount;
    *cx = colCount;
}

GapBuffer *gbCreateFromFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    int res = fseek(file, 0, SEEK_END);
    if (res != 0)
    {
        return NULL;
    }
    int fsize = ftell(file);    // TODO: Error check
    rewind(file);

    // Not +1 because we don't care about the null terminator
    char *string = malloc(fsize);
    int numRead = fread(string, 1, fsize, file);
    if (numRead != fsize)
    {
        return NULL;
    }

    GapBuffer *newgb = gbCreateFromString(string, fsize);
    gbMoveGapToIndex(newgb, 0);
    
    free(string);
    fclose(file);

    return newgb;
}

void gbWriteToFile(GapBuffer *gb, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        return;
    }

    // TODO: We should implement this in a more efficient way, i.e. shouldn't
    // be writing each character individually.
    char lastWritten = '\0';
    for (int i = 0; i < gb->preSize; i++) 
    {
        fprintf(file, "%c", gb->buffer[i]);
        lastWritten = gb->buffer[i];
    }

    // Print end buffer
    int endBufferStart = gb->bufferSize - gb->postSize;
    for (int i = endBufferStart; i < gb->bufferSize; i++) 
    {
        fprintf(file, "%c", gb->buffer[i]);
        lastWritten = gb->buffer[i];
    }

    // If last character was not a newline
    if (lastWritten != '\n')
    {
        fprintf(file, "%c", '\n');
    }
}

char *gbGetString(GapBuffer *gb)
{
    size_t newStringSize = gb->preSize + gb->postSize;
    char *string = malloc(sizeof(char) * newStringSize + 1);

    int endBufferStart = gb->bufferSize - gb->postSize;
    memcpy(string, gb->buffer, gb->preSize);
    memcpy(string + gb->preSize, gb->buffer + endBufferStart, gb->postSize);
    
    string[newStringSize] = '\0';
    
    return string;
}

void gbFree(GapBuffer *gb)
{
    free(gb->buffer);
    free(gb);
}