#include "windowstatemachine.h"

void wsmUpdate()
{
    switch (wsm.state)
    {
        case TEXT_WINDOW_STATE: 
        {
            if (tw.waitingForFilename)
            {
                wsmChangeState(INFO_WINDOW_STATE);
            }
            else
            {
                twUpdate();
            }
            break;
        }
        case INFO_WINDOW_STATE: {
            iwUpdate();
            wsmChangeState(TEXT_WINDOW_STATE);
            break;
        }
    }
}

void wsmChangeState(enum State newState)
{
    if (wsm.state == newState)
    {
        return;
    }

    switch (newState)
    {
        case TEXT_WINDOW_STATE: {
            if (iw.shouldExit)
            {
                tw.waitingForFilename = 0;
                iw.shouldExit = 0;
            }
            else if (iw.filenameReady)
            {
                char *filename = gbGetString(iw.gb);
                twReceiveFilename(filename);
                iw.filenameReady = 0;
            }
            wsm.state = TEXT_WINDOW_STATE;
            twDraw();
            break;
        }
        case INFO_WINDOW_STATE: {
            if (tw.shouldOpen)
            {
                iw.prompt = "File to open: ";
                iw.promptLen = 14;
            }
            else if (tw.shouldSave)
            {
                iw.prompt = "File to save to: ";
                iw.promptLen = 17;
            }
            wsm.state = INFO_WINDOW_STATE;
            iwDraw();
            break;
        }
    }
}