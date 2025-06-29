#include <iostream>
#include "Gridmaster.h"
#include "Calendar.h"
#include "raylib.h";

using namespace std;

int main()
{
    
   

    InitWindow(GetMonitorWidth(0),GetMonitorHeight(0),"Reduced Time");
    SetWindowPosition(300,300);
    SetTargetFPS(60);


    Gridmaster mainGrid;  //must be called after the window is initialized..Rookie error
    Calendar* CurrYear;

    CurrYear=new Calendar();
    CurrYear->loadCalendar(2025,0); //create a year

    //now pass the year to gridmaster just for a function to merge the squares with cal data
    //I dont think it needs to persist
    mainGrid.MergeGridwithCalendar(CurrYear);
    



    while(!WindowShouldClose())
    {
        BeginDrawing();


        mainGrid.DrawGrid();
        mainGrid.MouseTrap();
        

        EndDrawing();



    }

    
    delete CurrYear;
    CurrYear = nullptr;
    

    

    return 0;
    
} 