#include "texteditwindow.h"

TextWindow tw;

void twInit(int lines, int cols, int cy, int cx)
{
    tw.win = newwin(lines, cols, cy, cx);
    keypad(tw.win, TRUE);
    tw.cx = cx;
    tw.cy = cy;
    tw.lines = lines;
    tw.cols = cols;
    tw.lineOffset = 0;
    tw.shouldExit = 0;
    tw.waitingForFilename = 0;
    tw.gb = gbCreateEmpty();
    tw.filename = NULL;
}

void twLoadFile(char *filename)
{
    GapBuffer *newgb = gbCreateFromFile(filename);
    if (newgb != NULL)
    {
        if (tw.filename != NULL)
            free(tw.filename);
        tw.filename = filename;

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

void twDrawLines()
{
    wclear(tw.win);
    wmove(tw.win, 0, 0);

    int i = lhInit(tw.gb, tw.lineOffset);
    int hasPastPreBuf = lhHasPastPreBuf;
    size_t length = lhGetNextLineLength();
    int numLinesLeft = tw.lines;
    while (numLinesLeft > 0 && length != 0)
    {
        // linesUsed THIS line
        // int linesUsed = 1;
        int linesNeeded = length / tw.cols + 1;     // floor result
        if (numLinesLeft - linesNeeded < 0)
        {
            waddstr(tw.win, "@");
            numLinesLeft--;
            while (numLinesLeft > 0)
            {
                waddstr(tw.win, "\n@");
                numLinesLeft--;
            }
            continue;
        }
        numLinesLeft -= linesNeeded;

        int endIndex = i + length;
        for (; i < endIndex; i++)
        {
            if (!hasPastPreBuf && i >= tw.gb->preSize)
            {
                hasPastPreBuf = 1;
                size_t gapSize = tw.gb->bufferSize - (tw.gb->preSize + tw.gb->postSize);
                i += gapSize;
            }
            waddch(tw.win, tw.gb->buffer[i]);
            // if (i > tw.cols * linesUsed)
            // {
            //     linesUsed++;
            //     numLinesLeft--;
            // }
        }

        // numLinesLeft--;
        length = lhGetNextLineLength();
    }
    // wrefresh(tw.win);
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
            if (tw.filename != NULL)
            {
                // gbWriteToFile(gb, "test2.txt");
                gbWriteToFile(gb, tw.filename);
            }
            else
            {
                tw.waitingForFilename = 1;
                tw.shouldSave = 1;
            }
        }
        else if (optChar == 83)
        {
            tw.waitingForFilename = 1;
            tw.shouldSave = 1;
        }
        else if (optChar == 111)
        {
            tw.waitingForFilename = 1;
            tw.shouldOpen = 1;
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
        // twDraw();
        twDrawLines();
        // Move cursor back to where it was before draw
        gbGetCursor(gb, &tw.cy, &tw.cx, tw.cols);
        wmove(tw.win, tw.cy, tw.cx);
        wrefresh(tw.win);
    }
}

void twReceiveFilename(char *filename)
{
    if (tw.shouldSave)
    {
        if (tw.filename != NULL)
            free(tw.filename);
        tw.filename = filename;
        gbWriteToFile(tw.gb, tw.filename);
        tw.shouldSave = 0;
    }
    else if (tw.shouldOpen)
    {
        twLoadFile(filename);
        tw.shouldOpen = 0;
        // twDraw();
        twDrawLines();
        wmove(tw.win, tw.cy, tw.cx);
        wrefresh(tw.win);
    }

    tw.waitingForFilename = 0;
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
