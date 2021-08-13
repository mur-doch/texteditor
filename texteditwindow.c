#include "texteditwindow.h"

TextWindow tw;

void twInit(int lines, int cols, int cy, int cx)
{
    tw.win = newwin(lines, cols, cy, cx);
    keypad(tw.win, TRUE);
    tw.wx = cx;
    tw.wy = cy;
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
        tw.lineOffset = 0;
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

    // TODO: DELETE; for debugging
    int gc, gl;
    gbGetCursor(tw.gb, &gl, &gc, tw.cols);
    // wprintw(tw.win, "%d", gl);
    wprintw(tw.win, "%d", tw.lineOffset);

    int i = lhInit(tw.gb, tw.lineOffset);
    int linesLeft = tw.lines;
    char *line; 
    size_t lineLen;
    while (linesLeft > 0 && (line = lhGetNextLine(&lineLen)) != 0)
    {
        int linesNeeded = lineLen / tw.cols + 1;
        if (linesNeeded > linesLeft)
        {
            waddstr(tw.win, "@");
            linesLeft--;
            while (linesLeft > 0)
            {
                waddstr(tw.win, "\n@");
                linesLeft--;
            }
        }
        linesLeft -= linesNeeded;

        waddstr(tw.win, line);
    }
}

void twDrawCursor()
{
    // TODO: The wx,wy might be unnecessary 
    tw.wx = tw.cx;
    tw.wy = tw.cy;
    tw.wy -= tw.lineOffset;
    if (tw.wy < 0 || tw.wy >= tw.lines)
    {
        curs_set(0);
    }
    else
    {
        curs_set(1);
    }

    wmove(tw.win, tw.wy, tw.wx);
}

void twUpdateCursor()
{
    int prevX = tw.cx;
    int prevY = tw.cy;
    gbGetCursor(tw.gb, &tw.cy, &tw.cx, tw.cols);

    // If the cursor is offscreen, shift the lineOffset to put it on screen
    int relY = tw.cy - tw.lineOffset;
    if (relY < 0)
    {
        tw.lineOffset += relY;
    }
    else if (relY > tw.lines - 1)
    {
        tw.lineOffset += relY - (tw.lines - 1);
    }
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
        else if (optChar == 'k')
        {
            if (tw.lineOffset <= 0)
                break;
            tw.lineOffset--;
            // If this has made the cursor offscreen, move the cursor up
            int relY = tw.cy - tw.lineOffset;
            // if (relY < 0)
            if (relY > tw.lines - 1)
            {
                gbMoveGapUp(tw.gb);
            }
        }
        else if (optChar == 'j')
        {
            // TODO: This shouldn't be based on the number of lines on the 
            // screen, but the number of lines in the file.
            // if (tw.lineOffset >= tw.lines - 1)
            //     break;
            tw.lineOffset++;
            // If this has made the cursor offscreen, move the cursor down
            int relY = tw.cy - tw.lineOffset;
            // if (relY > tw.lines - 1)
            if (relY < 0)
            {
                gbMoveGapDown(tw.gb);
            }
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
        else
        {
            shouldMoveCursor = 0;
        }
    }

    if (shouldMoveCursor)
    {
        twUpdateCursor();
        shouldRedraw = 1;
    }

    if (shouldRedraw)
    {
        // twDraw();
        twDrawLines();
        // Move cursor back to where it was before draw
        // gbGetCursor(gb, &tw.cy, &tw.cx, tw.cols);
        // wmove(tw.win, tw.cy, tw.cx);
        // twUpdateCursor();
        twDrawCursor();
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
