#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "ctype.h"

#include "ncurses.h"
#include "gapbuffer.h"

typedef struct 
{
    WINDOW *win;
    int cx, cy;
    int lines, cols;
    GapBuffer *gb;
    
    // FLAGS
    int shouldExit, filenameReady;
} InfoWindow;

extern InfoWindow iw;

void iwInit(int lines, int cols, int cy, int cx);
void iwDraw();
void iwUpdate();
void iwDelete();

#endif