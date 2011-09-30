// More built-in functions, including 
// graphics, keyboard input, and mouse functions
//
// CSCI 1300
// September 14, 2005 (as changed in class)
//
// Karl Winklmann

#include <winbgim.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void drawCircle (int x, int y, int color);
bool nearby (int x1, int y1, int x2, int y2);

const int WINDOW_SIZE = 512;

int main ()
{
    bool mouseButtonDown = false;
    bool dragging = false;
    char inputChar = ' ';

    int screenButtonX = 20;
    int screenButtonY = 20;

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
            mouseButtonDown = true;
            if (nearby (screenButtonX, screenButtonY, mousex (), mousey ()))
            {
                dragging = true;
                cleardevice ();
            }
            else
                drawCircle (mousex (), mousey (), YELLOW);
            clearmouseclick (WM_LBUTTONDOWN);
        }

        if (ismouseclick (WM_LBUTTONUP))
        {
            mouseButtonDown = false;
            dragging = false;
            // drawCircle (mousex (), mousey (), GREEN);
            clearmouseclick (WM_LBUTTONUP);
        }

        if (ismouseclick (WM_MOUSEMOVE))
        {
            if (dragging)
            {
                 cleardevice ();
                 screenButtonX = mousex ();
                 screenButtonY = mousey ();
            }
            if (mouseButtonDown 
                  && !nearby (screenButtonX, screenButtonY, mousex (), mousey ()))
                drawCircle (mousex (), mousey (), WHITE);
            clearmouseclick (WM_MOUSEMOVE);
        }
     
        switch (inputChar)
        {
             case 'c': cleardevice ();
                       inputChar = ' ';
                       break;
        }

        drawCircle (screenButtonX, screenButtonY, RED);
    } 
    while (inputChar != 'x');

    return EXIT_SUCCESS;
}

void drawCircle (int x, int y, int color)
{
    setfillstyle (SOLID_FILL, color);
    fillellipse (x, y, 10, 10);
    setcolor (BLACK);
    circle (x, y, 10);
}

bool nearby (int x1, int y1, int x2, int y2)
{
     return ((x1 - x2)*(x1 - x2) + (y1 - y2) * (y1 - y2)) < 100;
}
