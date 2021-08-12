#ifndef GB_LINE_HANDLER_H
#define GB_LINE_HANDLER_H

#include "gapbuffer.h"

extern size_t lhIndex;
extern GapBuffer *lhGapBuffer;
extern int lhHasPastPreBuf;

size_t lhInit(GapBuffer *gb, int line);
size_t lhGetNextLineLength();

#endif