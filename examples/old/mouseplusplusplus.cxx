// More built-in functions, including 
// graphics, keyboard input, and mouse functions
//
// CSCI 1300
// September 16, 2005 (as at the end of class)
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
bool onCard (int cardX, int cardY, int x, int y);
void wait (int msecs);
void drawCard (int x, int y);

const int WINDOW_SIZE = 512;

int main ()
{
    bool mouseButtonDown = false;
    bool buttonDragging = false;
    bool cardDragging = false;
    char inputChar = ' ';

    int screenButtonX = 20;
    int screenButtonY = 20;

    int cardX = 200;
    int cardY = 200;

    // int i = 0;
    int activepage = 0;

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
        // cout << i++ << endl;
        // wait (1000);

        if (kbhit ())    
            inputChar = (char) getch ();

        if (ismouseclick (WM_LBUTTONDOWN))
        {
            mouseButtonDown = true;
            if (nearby (screenButtonX, screenButtonY, mousex (), mousey ()))
            {
                buttonDragging = true;
            }
            else if (onCard (cardX, cardY, mousex (), mousey ()))
            {
                cardDragging = true;
            }
            else
                drawCircle (mousex (), mousey (), YELLOW);
            clearmouseclick (WM_LBUTTONDOWN);
        }

        if (ismouseclick (WM_LBUTTONUP))
        {
            mouseButtonDown = false;
            buttonDragging = false;
            cardDragging = false;
            // drawCircle (mousex (), mousey (), GREEN);
            clearmouseclick (WM_LBUTTONUP);
        }

        if (ismouseclick (WM_MOUSEMOVE))
        {
            if (buttonDragging)
            {
                 screenButtonX = mousex ();
                 screenButtonY = mousey ();
            }
            else if (cardDragging)
            {
                 cardX = mousex ();
                 cardY = mousey ();
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

        // re-draw whole window
        activepage = 1 - activepage;
        setactivepage (activepage);

        cleardevice ();
        drawCard (cardX, cardY);
        drawCircle (screenButtonX, screenButtonY, RED);
   
        setvisualpage (activepage);
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

bool onCard (int cardX, int cardY, int x, int y)
{
    if (x < cardX) return false;
    if (y < cardY) return false;
    if (x > cardX + 71) return false;
    if (y > cardY + 93) return false;
    return true;
}

void wait (int msecs)
{
     int oldtime = clock ();

     while (clock () < oldtime + msecs);
}

void drawCard (int x, int y)
{
     readimagefile ("QueenDiamonds.gif", x, y, x + 71, y + 93);
}
