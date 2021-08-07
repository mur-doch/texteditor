#ifndef TEXT_EDIT_WINDOW_H
#define TEXT_EDIT_WINDOW_H

#include <ctype.h>

#include "ncurses.h"
#include "gapbuffer.h"

typedef struct 
{
    WINDOW *win;
    int cx, cy;
    int lines, cols;
    int shouldExit;
    GapBuffer *gb;
} TextWindow;

TextWindow tw;

void twInit(int lines, int cols, int cy, int cx)
{
    tw.win = newwin(lines, cols, cy, cx);
    tw.cx = cx;
    tw.cy = cy;
    tw.lines = lines;
    tw.cols = cols;
    tw.shouldExit = 0;
    tw.gb = gbCreateEmpty();
}

void twLoadFile(char *filename)
{
    GapBuffer *newgb = gbCreateFromFile(filename);
    if (newgb != NULL)
    {
        if (tw.gb == NULL)
            gbFree(tw.gb);
        tw.gb = newgb;
        gbGetCursor(tw.gb, &tw.cy, &tw.cx, tw.cols);
    }
}

void twDraw()
{
    GapBuffer *gb = tw.gb;
    WINDOW *win = tw.win;

    wclear(win);
    wmove(win, 0, 0);
    for (int i = 0; i < gb->preSize; i++) 
    {
        waddch(win, gb->buffer[i]);
    }

    int endBufferStart = gb->bufferSize - gb->postSize;
    for (int i = endBufferStart; i < gb->bufferSize; i++) 
    {
        waddch(win, gb->buffer[i]);
    }
    wrefresh(win);
}

void twUpdate()
{
    GapBuffer *gb = tw.gb;

    int ch = wgetch(tw.win);
    int shouldRedraw = 0;
    int shouldMoveCursor = 1;
    int optChar;
    switch (ch)
    {
    case KEY_LEFT:
        gbMoveGapLeft(gb);
        break;
    case KEY_RIGHT:
        gbMoveGapRight(gb);
        break;
    case KEY_UP:
        gbMoveGapUp(gb);
        break;
    case KEY_DOWN:
        gbMoveGapDown(gb);
        break;
    case KEY_BACKSPACE:
        gbBackspace(gb);
        shouldRedraw = 1;
        break;
    case 127:
        gbBackspace(gb);
        shouldRedraw = 1;
        break;
    case '\b':
        gbBackspace(gb);
        shouldRedraw = 1;
        break;
    case KEY_ENTER:
        gbInsertCharacter(gb, '\n');
        shouldRedraw = 1;
        break;
    case 10:
        gbInsertCharacter(gb, '\n');
        shouldRedraw = 1;
        break;
    case 27:        // Try to handle opt key
        optChar = wgetch(tw.win);
        if (optChar == 120)
        {
            tw.shouldExit = 1;
        }
        else if (optChar == 115)
        {
            gbWriteToFile(gb, "test2.txt");
        }
        break;
    case '\t':
        gbInsertString(gb, "    ", 4);
        shouldRedraw = 1;
        break;
    default:
        if (isprint(ch))
        {
            gbInsertCharacter(gb, ch);
            shouldRedraw = 1;
        }
        shouldMoveCursor = 0;
    }

    if (shouldMoveCursor)
    {
        gbGetCursor(gb, &tw.cy, &tw.cx, tw.cols);
        wmove(tw.win, tw.cy, tw.cx);
    }

    if (shouldRedraw)
    {
        twDraw();
        // Move cursor back to where it was before draw
        gbGetCursor(gb, &tw.cy, &tw.cx, tw.cols);
        wmove(tw.win, tw.cy, tw.cx);
    }
}

void twDelete()
{
    if (tw.gb != NULL)
    {
        gbFree(tw.gb);
        tw.gb = NULL;
    } 

    if (tw.win != NULL)
    {
        delwin(tw.win);
        tw.win = NULL;
    }
}

#endif