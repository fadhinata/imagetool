// More built-in functions, including 
// graphics, keyboard input, and mouse functions
//
// CSCI 1300
// September 12, 2005 (as changed in class)
//
// Karl Winklmann

#include <winbgim.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void drawCircle (int x, int y, int color);

const int WINDOW_SIZE = 512;

int main ()
{
    bool buttonDown = false;
    char inputChar = ' ';

    // setting up graphics window
    initwindow (
        WINDOW_SIZE, 
        WINDOW_SIZE, 
        "More functions, including graphics and mouse function"
    );
    setbkcolor (LIGHTCYAN);
    cleardevice ();

    // main loop, "event-driven"
    do
    {
        if (kbhit ())    
            inputChar = (char) getch ();

        if (ismouseclick (WM_LBUTTONDOWN))
        {
            buttonDown = true;
            drawCircle (mousex (), mousey (), YELLOW);
            clearmouseclick (WM_LBUTTONDOWN);
        }

        if (ismouseclick (WM_LBUTTONUP))
        {
            buttonDown = false;
            drawCircle (mousex (), mousey (), GREEN);
            clearmouseclick (WM_LBUTTONUP);
        }

        if (ismouseclick (WM_MOUSEMOVE))
        {
            if (buttonDown)
                drawCircle (mousex (), mousey (), WHITE);
            clearmouseclick (WM_MOUSEMOVE);
        }
     
        switch (inputChar)
        {
             case 'c': cleardevice ();
                       inputChar = ' ';
                       break;
        }
    } 
    while (true);

    return EXIT_SUCCESS;
}

void drawCircle (int x, int y, int color)
{
    setfillstyle (SOLID_FILL, color);
    fillellipse (x, y, 10, 10);
    setcolor (BLACK);
    circle (x, y, 10);
}
