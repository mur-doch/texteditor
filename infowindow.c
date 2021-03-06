#include "infowindow.h"

InfoWindow iw;

void iwInit(int lines, int cols, int cy, int cx)
{
    iw.win = newwin(lines, cols, cy, cx);
    keypad(iw.win, TRUE);
    iw.cx = cx;
    iw.cy = cy;
    iw.lines = lines;
    iw.cols = cols;
    iw.prompt = "Filename: ";
    iw.promptLen = 10;
    iw.shouldExit = 0;
    iw.filenameReady = 0;
    iw.gb = gbCreateEmpty();
}

void iwDraw()
{
    GapBuffer *gb = iw.gb;
    WINDOW *win = iw.win;

    wclear(win);
    wmove(win, 0, 0);
    waddstr(win, iw.prompt);

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

void iwUpdate()
{
    GapBuffer *gb = iw.gb;
    WINDOW *win = iw.win;

    int ch = wgetch(win);
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
        iw.filenameReady = 1;
        break;
    case 10:
        iw.filenameReady = 1;
        break;
    case 27:
        iw.shouldExit = 1;
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
        gbGetCursor(gb, &iw.cy, &iw.cx, iw.cols);
        wmove(iw.win, iw.cy, iw.cx + iw.promptLen);
    }

    if (shouldRedraw)
    {
        iwDraw();
        // Move cursor back to where it was before draw
        gbGetCursor(gb, &iw.cy, &iw.cx, iw.cols);
        wmove(iw.win, iw.cy, iw.cx + iw.promptLen);
    }
}

void iwDelete()
{
    if (iw.gb != NULL)
    {
        gbFree(iw.gb);
        iw.gb = NULL;
    } 

    if (iw.win != NULL)
    {
        delwin(iw.win);
        iw.win = NULL;
    }
}