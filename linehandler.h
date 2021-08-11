#ifndef GB_LINE_HANDLER_H
#define GB_LINE_HANDLER_H

#include "gapbuffer.h"

extern size_t lbIndex;
extern GapBuffer *lbGapBuffer;
extern int lbHasPastPreBuf;

size_t lbInit(GapBuffer *gb, int line);
size_t lbGetNextLineLength();

#endif