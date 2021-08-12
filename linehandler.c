#include "linehandler.h"

size_t lhIndex;
GapBuffer *lhGapBuffer;
int lhHasPastPreBuf;

size_t lhInit(GapBuffer *gb, int line)
{
    // TODO: Handle lines other than 0
    lhIndex = 0;
    lhGapBuffer = gb;
    lhHasPastPreBuf = 0;
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
