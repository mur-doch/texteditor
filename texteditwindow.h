#ifndef TEXT_EDIT_WINDOW_H
#define TEXT_EDIT_WINDOW_H

#include <ctype.h>
#include <math.h>

#include "ncurses.h"
#include "gapbuffer.h"
#include "linehandler.h"

typedef struct 
{
    WINDOW *win;
    int cx, cy;
    int lines, cols;
    int lineOffset;
    GapBuffer *gb;
    char *filename;

    // FLAGS
    int shouldExit, waitingForFilename;
    // Flags for handling new filenames
    int shouldOpen, shouldSave;
} TextWindow;

extern TextWindow tw;

void twInit(int lines, int cols, int cy, int cx);
void twLoadFile(char *);
void twDraw();
void twDrawLines();
void twUpdate();
void twReceiveFilename(char *filename);
void twDelete();

#endif