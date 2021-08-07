#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gapbuffer.h"
// #include "display.h"
#include "ncurses.h"

int NUM_LINES, NUM_COLS;

void initncurses()
{
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    // curs_set(0);
    keypad(stdscr, TRUE);
    // initColours();
}

void drawTextWindow(WINDOW *tw, GapBuffer *gb)
{
    wclear(tw);
    wmove(tw, 0, 0);
    for (int i = 0; i < gb->preSize; i++) 
    {
        waddch(tw, gb->buffer[i]);
    }

    int endBufferStart = gb->bufferSize - gb->postSize;
    for (int i = endBufferStart; i < gb->bufferSize; i++) 
    {
        waddch(tw, gb->buffer[i]);
    }
    wrefresh(tw);
}

int main()
{
    initncurses();

    GapBuffer *gb = gbCreateFromFile("test.txt");
    if (gb == NULL)
    {
        endwin();
        printf("Failed to read file!\n");
        return 1;
    }

    getmaxyx(stdscr, NUM_LINES, NUM_COLS);
    int cx, cy;
    gbGetCursor(gb, &cy, &cx, NUM_COLS);

    WINDOW *textWindow = newwin(NUM_LINES - 1, NUM_COLS, 0, 0);
    WINDOW *metaWindow = newwin(1, NUM_COLS, NUM_LINES - 1, 0);
    drawTextWindow(textWindow, gb);

    int shouldExit = 0;
    while (!shouldExit)
    {
        int ch = wgetch(textWindow);
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
                optChar = wgetch(textWindow);
                if (optChar == 120)
                {
                    shouldExit = 1;
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
            gbGetCursor(gb, &cy, &cx, NUM_COLS);
            wmove(textWindow, cy, cx);
        }

        if (shouldRedraw)
        {
            drawTextWindow(textWindow, gb);
            // Move cursor back to where it was before draw
            gbGetCursor(gb, &cy, &cx, NUM_COLS);
            wmove(textWindow, cy, cx);
        }
    }

    delwin(textWindow);
    delwin(metaWindow);
    gbFree(gb);

    endwin();
}