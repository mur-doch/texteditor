#ifndef WINDOW_STATE_MACHINE_H
#define WINDOW_STATE_MACHINE_H

#include "texteditwindow.h"
#include "infowindow.h"

enum State {
    TEXT_WINDOW_STATE,
    INFO_WINDOW_STATE
};

typedef struct
{
    enum State state;
    int shouldExit;
} WindowsStateMachine;

WindowsStateMachine wsm;

// void wsmInit();
void wsmUpdate();
void wsmChangeState();

#endif