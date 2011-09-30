 /*
 * Chris' implementation of Graham's Scan
 * Copyright (C) 2003 Chris Harrison
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <time.h>
#include "SDL.h"
#include "SDL_gfxPrimitives.h"

//--------------------POINT DATA STRUCTURE---------------------------
struct point
{
    double x; //X POSITION
    double y; //Y POSITION
};

//--------------------GLOBAL VARIABLES---------------------------
int NumPoints = 15; // n<1000
point points[1000]; //GLOBAL POINT ARRAY
int usedPoints[1000]; //GLOBAL LIST OF POINTS IN CONVEX HULL
int minPoint, maxPoint, currPoint;  //GLOBAL VARIABLES
SDL_Surface *screen; //GLOBAL SDL GRAPHICS SURFACE POINTER

//--------------------JARVIS' MARCH FUNCTIONS---------------------------
void jarvisMain(); //RANDOMLY GENERATES POINTS, RUNS JARVIS' MARCH, AND DISPLAYS RESULT
double findAngle(double x1, double y1, double x2, double y2); //FIND ANGLE GIVEN TWO POINTS
bool notUsed(int y); //CHECK IF POINT INDEX HAS ALREADY BEEN ADDED TO THE CONVEX HULL
void addUsedPoint(int index); //ADD POINT TO CONVEX HULL
void jarvis(); //FINDS MIN POINT, AND USES GLOBAL POINT ARRAY TO COMPUTER CONVEX HULL

//--------------------AUXILARY GRAPHICS FUNCTIONS---------------------------
void initScreen(); //SETUP THE GRAPHICS SURFACE AND WINDOW
void drawPoints(); //DRAW POINTS FROM GLOBAL DOUBLELY LINKED LIST
void drawInnerLine(point A, point B); //DRAW A LINE BETWEEN TWO POINTS
void drawPermeter(); //DRAWS COMPLETED PERIMETER (CONVEX HULL)
void graphicsLoop(); //MAIN GRAPHICS LOOP


//--------------------MAIN---------------------------
int main(int argc, char *argv[])
{
    initScreen(); //INITIALIZE THE GRAPHICS WINDOW
    srand(time(NULL)); //SEED THE RANDOM NUMBER GENERATER WITH THE TIME
    jarvisMain(); //EXECUTE JARVIS' MARCH
    graphicsLoop(); //WAIT UNTIL USER EXITS
    return 0; //EXIT
}


void jarvisMain()
{
    for (int i=0;i<NumPoints;i++) //GENERATE RANDOM POINTS
    {
        usedPoints[i]=-1;
        points[i].x=rand()%400+50;
        points[i].y=rand()%400+50;
        //cout<<points[i].x<<" "<<points[i].y<<endl;
    }
    jarvis(); //EXECUTE JARVIS' MARCH
    drawPermeter(); //DRAW OUTER EDGES FOUND BY JARVIS' MARCH
    drawPoints(); //DRAW THE POINTS
}


void jarvis()
{
    int maxAngle=0, minAngle=0;
    maxPoint=0;
    
    for (int k=1;k<NumPoints;k++)
        if (points[k].y>points[maxPoint].y)
            maxPoint=k;
    
    //cout<<"Max: "<<points[maxPoint].x<<" "<<points[maxPoint].y<<endl;
    minPoint=0;
    
    for (int j=1;j<NumPoints;j++) //FOR ALL POINTS IN THE SET, FIND MIN POINT
        if (points[j].y<points[minPoint].y)
            minPoint=j;
    
    //cout<<"Min: "<<points[minPoint].x<<" "<<points[minPoint].y<<endl;
    addUsedPoint(minPoint); //ADD MIN POINT TO LIST
    currPoint=minPoint;

    while(currPoint!=maxPoint) //BUILD LEFT-HAND SIDE OF CONVEX HULL
    {
        maxAngle=currPoint;
        
        for (int y=0;y<NumPoints;y++) //LOOP FOR ALL POINTS IN THE SET, FIND POINT WITH LOWEST RELATIVE ANGLE
        {
            //cout<<"Angles: "<<findAngle(points[currPoint].x,points[currPoint].y,points[maxAngle].x,points[maxAngle].y)<<" "<<findAngle(points[currPoint].x,points[currPoint].y,points[y].x,points[y].y)<<endl;
            drawInnerLine(points[currPoint],points[y]); //DRAW A LINE TO VISUALLY SHOW COMPARISON
             
            if (findAngle(points[currPoint].x,points[currPoint].y,points[maxAngle].x,points[maxAngle].y)<findAngle(points[currPoint].x,points[currPoint].y,points[y].x,points[y].y) && (notUsed(y) || y==maxPoint) && findAngle(points[currPoint].x,points[currPoint].y,points[y].x,points[y].y)<=270)
            {
                maxAngle=y;
            }
        }
        
        currPoint=maxAngle;
        addUsedPoint(currPoint); //ADD NEW POINT TO FINAL PERIMETER LIST

    } //END WHILE LOOP


    currPoint=minPoint;
    
    while(currPoint!=maxPoint) //BUILD RIGHT-HAND SIDE OF CONVEX HULL
    {
        minAngle=maxPoint;
        
        for (int y=0;y<NumPoints;y++) //LOOP FOR ALL POINTS IN THE SET, FIND POINT WITH GREATEST RELATIVE ANGLE
        {
            //cout<<"Angles: "<<findAngle(points[currPoint].x,points[currPoint].y,points[minAngle].x,points[minAngle].y)<<" "<<findAngle(points[currPoint].x,points[currPoint].y,points[y].x,points[y].y)<<endl;
            drawInnerLine(points[currPoint],points[y]); //DRAW A LINE TO VISUALLY SHOW COMPARISON

            if (findAngle(points[currPoint].x,points[currPoint].y,points[minAngle].x,points[minAngle].y)>findAngle(points[currPoint].x,points[currPoint].y,points[y].x,points[y].y) && (notUsed(y) || y==maxPoint) && findAngle(points[currPoint].x,points[currPoint].y,points[y].x,points[y].y)>=90)
            {
                minAngle=y;
            }
        }

        currPoint=minAngle;
        //cout<<"Selected Point: "<<currPoint<<endl<<endl;
        addUsedPoint(currPoint); //ADD NEW POINT TO FINAL PERIMETER LIST
    } //END WHILE LOOP
} //END JARVIS()


double findAngle(double x1, double y1, double x2, double y2)
{
    double deltaX=x2-x1;
    double deltaY=y2-y1;
    
    if (deltaX==0 && deltaY==0)
        return 0;
    
    double angle=atan(deltaY/deltaX)*(180.0/3.141592);

    //TAKE INTO ACCOUNT QUADRANTS, VALUE: 0°-360°
    if (deltaX>=0 && deltaY>=0)
        angle=90+angle;
    else if (deltaX>=0 && deltaY<0)
        angle=90+angle;
    else if (deltaX<0 && deltaY>0)
        angle=270+angle;
    else if (deltaX<0 && deltaY<=0)
        angle=270+angle;
    
    return angle;
}


bool notUsed(int y)
{
    for (int i=0;i<NumPoints;i++) //FOR ALL POINTS IN THE SET, CHECK IF INDEX HAS ALREADY BEEN ADDED
        if (y==usedPoints[i])
            return false;
    
    return true;
}


void addUsedPoint(int index)
{
    int i=0;
    while (usedPoints[i]!=-1) //FIND NEXT FREE SPOT IN USEDPOINTS[], AND STORE INDEX
        i++;
    
    usedPoints[i]=index;
    return;
}


//--------------------------PERIPHERAL FUNCTIONS-------------------------------

void drawPoints() 
{
    for (int i=0;i<NumPoints;i++) //FOR ALL POINTS IN THE SET
        filledCircleRGBA (screen, points[i].x, points[i].y, 3, 255, 0, 0, 255); //DRAW RED CIRCLE OF RADIUS 3
    
    return;
}


void graphicsLoop()
{
    SDL_Event event;
    Uint8 *keys;
    
    SDL_Flip(screen); //FLIP GRAPHICS BUFFER
    
    while (true)
    {
        SDL_PollEvent(&event);
        keys=SDL_GetKeyState(NULL);
        
        if (keys[SDLK_ESCAPE]) //TEST FOR USER INPUT (ESCAPE KEY)
        {
            exit(1); //EXIT PROGRAM
        }
        
        if(keys[SDLK_SPACE]) //TEST FOR USER INPUT (SPACE BAR)
        {
            boxRGBA(screen, 0, 0, 500, 500, 0, 0, 0, 255); //CLEAR SCREEN WITH BLACK BOX
            jarvisMain(); //GENERATE BRAND NEW CONVEX HULL
            SDL_Flip(screen); //FLIP GRAPHICS BUFFER
        }
        
        //SDL_Delay(100);
    }        
}


void initScreen()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) //CHECK SDL STARTED PROPERLY
        exit(1); //ON ERROR, EXIT
    
    atexit(SDL_Quit); //CLEAN UP SDL BEFORE EXITING
    screen=SDL_SetVideoMode(500, 500, 16, SDL_SWSURFACE); //SET SCREEN MODE
}

void drawInnerLine(point A, point B)
{
    lineRGBA (screen, A.x, A.y, B.x, B.y, 0, 150, 220, 255); //DRAW LIGHT BLUE LINE
}

void drawPermeter()
{
    int k=0;
    while (usedPoints[k]!=maxPoint) //RENDER LEFT-HAND SIDE OF CONVEX HULL WITH GREEN LINES
    {
        lineRGBA (screen, points[usedPoints[k]].x, points[usedPoints[k]].y, points[usedPoints[k+1]].x, points[usedPoints[k+1]].y, 0, 255, 0, 255);
        k++;
    }
    
    k++;
    lineRGBA (screen, points[usedPoints[0]].x, points[usedPoints[0]].y, points[usedPoints[k]].x, points[usedPoints[k]].y, 0, 255, 0, 255);
    
    while (usedPoints[k+1]!=-1) //RENDER RIGHT-HAND SIDE OF CONVEX HULL WITH GREEN LINES
    {
        lineRGBA (screen, points[usedPoints[k]].x, points[usedPoints[k]].y, points[usedPoints[k+1]].x, points[usedPoints[k+1]].y, 0, 255, 0, 255);
        k++;
    }
}
