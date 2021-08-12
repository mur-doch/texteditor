#ifndef GB_LINE_HANDLER_H
#define GB_LINE_HANDLER_H

#include <stdlib.h>

#include "gapbuffer.h"

extern size_t lhIndex;
extern GapBuffer *lhGapBuffer;
extern int lhHasPastPreBuf;
// TODO: This will end up being unused memory when we're not drawing.
extern char *lhStr;
extern size_t lhStrLen;

size_t lhInit(GapBuffer *gb, int line);
size_t lhGetNextLineLength();
char* lhGetNextLine(size_t *lineLen);

#endif