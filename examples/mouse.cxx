// More built-in functions, including 
// graphics, keyboard input, and mouse functions
//
// CSCI 1300
// September 12, 2005
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
            cout << "Key hit: " << (char) getch () << endl;

        if (ismouseclick (WM_LBUTTONDOWN))
        {
            cout << "Down at " << mousex () << ", " << mousey () << endl;
            clearmouseclick (WM_LBUTTONDOWN);
        }

        if (ismouseclick (WM_MOUSEMOVE))
        {
            cout << "Moving at " << mousex () << ", " << mousey () << endl;
            clearmouseclick (WM_MOUSEMOVE);
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
