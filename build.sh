#!/bin/bash

gcc -g main.c gapbuffer.c texteditwindow.c linehandler.c infowindow.c -o main.out -lncurses
# gcc -g main.c -o main.out -lncurses

# gcc -dynamiclib -o gapbuffer.dylib gapbuffer.c
# gcc -dynamiclib -o texteditwindow.dylib texteditwindow.c
# gcc main.c -L. -lgapbuffer -ltexteditwindow -lncurses -o main.out
