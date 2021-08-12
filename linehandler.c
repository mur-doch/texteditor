#include "linehandler.h"

size_t lhIndex;
GapBuffer *lhGapBuffer;
int lhHasPastPreBuf;
char *lhStr = NULL;
size_t lhStrLen = 0;

size_t lhInit(GapBuffer *gb, int line)
{
    // TODO: Handle lines other than 0
    lhIndex = 0;
    lhGapBuffer = gb;
    lhHasPastPreBuf = 0;

    // TODO:  This is new
    if (lhStrLen < gb->bufferSize + 1)
    {
        if (lhStr != NULL)
            free(lhStr);
        lhStrLen = gb->bufferSize + 1;
        lhStr = malloc(sizeof(char) * lhStrLen);
    }

    return lhIndex;
}

size_t lhGetNextLineLength()
{
    int tempPastPreBuf = lhHasPastPreBuf;
    size_t startIndex = lhIndex;
    size_t gapSize = lhGapBuffer->bufferSize - (lhGapBuffer->preSize + lhGapBuffer->postSize);

    // Check to make sure that we're not at the end
    // If we're in the preBuf, we'll just make sure we're < preSize, and if 
    // not, that we have some room in postSize
    if (!lhHasPastPreBuf && (startIndex >= lhGapBuffer->preSize && lhGapBuffer->postSize <= 0))
    {
        return 0;
    }
    else if (lhHasPastPreBuf && startIndex - gapSize >= lhGapBuffer->postSize)
    {
        return 0;
    }

    while (1)
    {
        if (lhIndex >= lhGapBuffer->bufferSize)
        {
            break;
        }
        
        if (lhGapBuffer->buffer[lhIndex] == '\n')
        {
            lhIndex++;
            break;
        }

        lhIndex++;
        if (!lhHasPastPreBuf && lhIndex >= lhGapBuffer->preSize)
        {
            lhHasPastPreBuf = 1;
            // size_t gapSize = lhGapBuffer->bufferSize - (lhGapBuffer->preSize + lhGapBuffer->postSize);
            lhIndex += gapSize;
        }
    }

    int length = lhIndex - startIndex;
    if (lhHasPastPreBuf && !tempPastPreBuf)
    {
        length = (lhIndex - gapSize) - startIndex;
    }
    return length;
}

char* lhGetNextLine(size_t *lineLen)
{
    int tempPastPreBuf = lhHasPastPreBuf;
    size_t startIndex = lhIndex;
    size_t gapSize = lhGapBuffer->bufferSize - (lhGapBuffer->preSize + lhGapBuffer->postSize);
    int postBufferStart = lhGapBuffer->bufferSize - lhGapBuffer->postSize;

    if (!lhHasPastPreBuf && (startIndex >= lhGapBuffer->preSize && lhGapBuffer->postSize <= 0))
    {
        return 0;
    }
    else if (lhHasPastPreBuf && (startIndex - postBufferStart) >= lhGapBuffer->postSize)
    {
        return 0;
    }

    size_t strIndex = 0;
    while (1)
    {
        if (lhIndex >= lhGapBuffer->bufferSize)
        {
            break;
        }

        // We need to check this before accessing any characters. 
        if (!lhHasPastPreBuf && lhIndex >= lhGapBuffer->preSize)
        {
            lhHasPastPreBuf = 1;
            // size_t gapSize = lhGapBuffer->bufferSize - (lhGapBuffer->preSize + lhGapBuffer->postSize);
            lhIndex += gapSize;
        }

        if (lhGapBuffer->buffer[lhIndex] == '\n')
        {
            lhStr[strIndex++] = '\n';
            lhIndex++;
            break;
        }

        lhStr[strIndex++] = lhGapBuffer->buffer[lhIndex];
        lhIndex++;
    }

    lhStr[strIndex] = '\0';
    // Don't want to count the null byte
    *lineLen = strIndex - 1;
    return lhStr;
}