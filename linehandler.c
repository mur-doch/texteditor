#include "linehandler.h"

size_t lbIndex;
GapBuffer *lbGapBuffer;
int lbHasPastPreBuf;

size_t lbInit(GapBuffer *gb, int line)
{
    // TODO: Handle lines other than 0
    lbIndex = 0;
    lbGapBuffer = gb;
    lbHasPastPreBuf = 0;
    return lbIndex;
}

size_t lbGetNextLineLength()
{
    int tempPastPreBuf = lbHasPastPreBuf;
    size_t startIndex = lbIndex;
    size_t gapSize = lbGapBuffer->bufferSize - (lbGapBuffer->preSize + lbGapBuffer->postSize);

    // Check to make sure that we're not at the end
    // If we're in the preBuf, we'll just make sure we're < preSize, and if 
    // not, that we have some room in postSize
    if (!lbHasPastPreBuf && (startIndex >= lbGapBuffer->preSize && lbGapBuffer->postSize <= 0))
    {
        return 0;
    }
    else if (lbHasPastPreBuf && startIndex - gapSize >= lbGapBuffer->postSize)
    {
        return 0;
    }

    while (1)
    {
        if (lbIndex >= lbGapBuffer->bufferSize)
        {
            break;
        }
        
        if (lbGapBuffer->buffer[lbIndex] == '\n')
        {
            lbIndex++;
            break;
        }

        lbIndex++;
        if (!lbHasPastPreBuf && lbIndex >= lbGapBuffer->preSize)
        {
            lbHasPastPreBuf = 1;
            // size_t gapSize = lbGapBuffer->bufferSize - (lbGapBuffer->preSize + lbGapBuffer->postSize);
            lbIndex += gapSize;
        }
    }

    int length = lbIndex - startIndex;
    if (lbHasPastPreBuf && !tempPastPreBuf)
    {
        length = (lbIndex - gapSize) - startIndex;
    }
    return length;
}
