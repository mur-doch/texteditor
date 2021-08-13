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
    // The wx,wy might be unnecessary 
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

    int relY = tw.cy - tw.lineOffset;
    if (relY < 0)
    {
        tw.lineOffset += relY;
    }
    else if (relY > tw.lines - 1)
    {
        tw.lineOffset += relY - (tw.lines - 1);
    }

    // See if we moved up down
    // and if the new spot is offscreen
    // if (prevY != tw.cy && (tw.cy - tw.lineOffset < 0 || tw.cy - tw.lineOffset >= tw.lines))
    // {
    //     tw.lineOffset += tw.cy - prevY;
    // }

    // NOTE: We don't need to draw the cursor since we trigger a redraw when 
    // the cursor is updated
    // twDrawCursor();

    // If the cursor has moved with the gap offscreen, we'll check and adjust offset.
    // First let's check if the new y is too low on the screen; >tw.lines.
    // if (tw.cy > tw.lines + tw.lineOffset)
    // if (tw.cy - tw.lineOffset > tw.lines)
    // {
    //     // tw.cy is the actual line number in the gap buffer
    //     // tw.cy - tw.lineOffset gives us the relative position of that line
    //     // on the screen
    //     // If that relative position is greater than the number of lines we can
    //     // display on, then we need to move the screen down the number of lines
    //     // to make it displayable
    //     // This should be the relative line - the last displayble line
    //     // tw.lineOffset += tw.cy - tw.lines - 1;
    //     tw.lineOffset += (tw.cy - tw.lineOffset) - (tw.lines - 1);
    //     tw.cy = tw.lines - 1;
    // }
    // Check if the new y is too high on the screen
    // else if (tw.cy - tw.lineOffset < 0)
    // {
    //     // tw.cy is the actual line in the gap buffer
    //     // Subtracting tw.lineOffset should give us the relative position of 
    //     // that line on the screen
    //     // For example, if we've scrolled down two lines, and the gap buffer is on line 0
    //     // Then the relative pos of that line is -2
    //     // In that case, we need to move tw.lineOffset two up 
    //     tw.lineOffset += tw.cy - tw.lineOffset;
    //     tw.cy = 0;
    // } 
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
            tw.lineOffset--;
        }
        else if (optChar == 'j')
        {
            tw.lineOffset++;
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
