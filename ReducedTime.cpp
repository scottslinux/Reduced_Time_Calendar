#include <iostream>
#include "Gridmaster.h"
#include "Calendar.h"
#include "raylib.h";

using namespace std;

int main()
{
    
   

    InitWindow(GetMonitorWidth(0),GetMonitorHeight(0),"Reduced Time");
    InitAudioDevice();

    SetWindowPosition(300,300);
    SetTargetFPS(60);


    Gridmaster mainGrid;  //must be called after the window is initialized..Rookie error
    Calendar* CurrYear;
    int result=2026;


while (result!=999)     //terminate signal
{
    CurrYear=new Calendar();
    CurrYear->loadCalendar(result,0); //create a year

    //now pass the year to gridmaster just for a function to merge the squares with cal data
    //I dont think it needs to persist
    mainGrid.MergeGridwithCalendar(CurrYear);
    
    



    while(true) //stay in the loop until EXIT is chosen
    {
        BeginDrawing();


        result=mainGrid.DrawGrid();    //compare activeyear to desiredyear
                                //if different then exit and start again
                                //with new year
        if(result!=0)           //a new year get out
            {   
                
                break;
            }
        mainGrid.MouseTrap();
        

        EndDrawing();



    }

    
    delete CurrYear;
    CurrYear = nullptr;
    
}
    
    
    return 0;
    
} 