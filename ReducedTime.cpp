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




    while(!WindowShouldClose())
    {
        BeginDrawing();


        mainGrid.DrawGrid();
        mainGrid.MouseTrap();
        

        EndDrawing();



    }

    

    










    cout<<"Hello World ..."<<endl;
    cout<<"Looks like we are ready to begin bringing it all together"<<"\n\n";
    

    return 0;
    
} 