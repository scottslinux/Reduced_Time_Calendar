#include "Gridmaster.h"
#include <iostream>
#include "raylib.h"
#include "Calendar.h"




Font Gridmaster::monthfont;
Font Gridmaster::dayfont;

//********************************************************
//  ⁡⁣⁢⁣Constructor⁡
Gridmaster::Gridmaster()
:weekdaynames{"SU","MO","TU","WE","TH","FR","SA"},
 monthNames{"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY",
    "AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"} 

{
    
    Gridmaster::monthfont=LoadFontEx("./resources/calfont.ttf",80,NULL,0);
    Gridmaster::dayfont=LoadFontEx("./resources/days.ttf",100,NULL,0);


    monWidth=GetScreenWidth();    //initialize to monitor size and calculate everything else
    monHeight=GetScreenHeight();

    Hinterval=monWidth/5;   //(horizontal) width of month block
    Vinterval=monHeight/3;  //(vertical) height of month block

    boxCounter= 0;  //count all of the grid squares
 

    monthPosxy.x=0;
    monthPosxy.y=0;

    
    //intialize dayGrid using placeholder

    gridData placeholder;       //create a temp gridData item and use it to fill in default values
    placeholder.activeBox=false;
    placeholder.dayRect=Rectangle{0,0,0,0};
    placeholder.dayValue=0;
    placeholder.dayofweek=0;
    placeholder.month=0;
    placeholder.year=0;

    dayGrid.resize(600, placeholder);  //this may have fixed the stack slamming exception

    
        //debugging
        std::cout<<"Days in dayGrid vector: "<<dayGrid.size()<<"\n";

    
 
}
//********************************************************
Gridmaster::~Gridmaster()   //destructor Called Automatically by C++
{
    //delete calendarYear;  // Clean up the dynamically allocated Calendar
    //calendarYear = nullptr; // Good practice to set to nullptr
}

//********************************************************


//      ⁡⁣⁢⁣Create the screen Grid⁡
void Gridmaster::DrawGrid()
{
    ClearBackground(RAYWHITE);
    //Large Outline
    DrawRectangleLines(1,1,monWidth-1,monHeight-1,BLACK);

    //Columns
    for(int i=1;i<=4;i++)
        DrawLineEx(Vector2{(float)i*Hinterval,1},Vector2{(float)i*Hinterval,(float)monHeight-1},15,BLACK);
    
    //weekend shading
    
    for(int i=0;i<4;i++)
    {
        DrawRectangle(0+i*Hinterval,0,Hinterval/7,Vinterval*3,Color{177,225,175,255});
        DrawRectangle((Hinterval/7)*6+i*Hinterval,0,Hinterval/7,Vinterval*3,Color{177,225,175,255});
    }


    //Month Bars
    for(int num=0;num<=2;num++)
    {
        Rectangle monthbar{0,Vinterval*num,Hinterval*4,Vinterval/7};
        DrawRectangleRec(monthbar,Color{13,18,100,255});  //blue solid bars
    }
    
    //reset the boxcounter
    boxCounter=0;
    //draw all months
    for(int mnth=1;mnth<=12;mnth++)
        DrawdayGrid(mnth);  //call to draw all day rectangles


     
    return;

}
//*******************************************************/
//      ⁡⁣⁢⁣𝗗𝗿𝗮𝘄 𝘁𝗵𝗲 𝗱𝗮𝘆 𝗴𝗿𝗶𝗱 𝗶𝗻𝘀𝗶𝗱𝗲 𝗼𝗳 𝘁𝗵𝗲 𝗿𝗲𝗾𝘂𝗲𝘀𝘁𝗲𝗱 𝗺𝗼𝗻𝘁𝗵⁡

void Gridmaster::DrawdayGrid(int month)
{
    
    int cellWidth=Hinterval/7;  //width of day block
    int cellHeight=Vinterval/7; //height of day block--make it more narrow to fit 6th week

    monthPosxy=FindMonthxy(month);  //find the offset based on the month for where to draw

    Vector2 monthPixeldistance=MeasureTextEx(monthfont,monthNames[month-1].c_str(),60,0);
    int centerPos=(0.5*Hinterval)-(0.5*monthPixeldistance.x);

    DrawTextEx(monthfont,monthNames[month-1].c_str(),Vector2{monthPosxy.x * Hinterval +centerPos,
                 monthPosxy.y * Vinterval},60,0,WHITE);
    
    //write the day names to the grids
    Vector2 daypixeldist;
    for(int day=0;day<7;day++)
    {
        daypixeldist=MeasureTextEx(dayfont,weekdaynames[day].c_str(),40,0);
        centerPos=(0.5*Hinterval/7)-(0.5*daypixeldist.x);

        Vector2 daypos;
        daypos.x=day*(Hinterval/7)+centerPos+(monthPosxy.x*Hinterval);
        daypos.y=monthPosxy.y*Vinterval+55;

        DrawTextEx(dayfont,weekdaynames[day].c_str(),daypos,40,0,WHITE);

        

    }

  
  for(int week=1;week<=6;week++)     //for each week of the 5 week grid
  {
    for(int day=0;day<=6;day++)     //Create 7 days
    {
        int x=(day*cellWidth)+(monthPosxy.x*Hinterval);      //find the x,y of upper left corner of the day
        int y=(week*cellHeight)+(monthPosxy.y*Vinterval);




        

        Rectangle currday={x,y,cellWidth,cellHeight};  //​‌‍‌⁡⁣⁢⁣𝘁𝗵𝗶𝘀 𝗶𝘀 𝘄𝗵𝗲𝗿𝗲 𝘄𝗲 𝘀𝘁𝗼𝗿𝗲 𝘁𝗵𝗲 𝗿𝗲𝗰𝘁 𝗶𝗻 𝘃𝗲𝗰𝘁𝗼𝗿⁡​
        dayGrid[boxCounter].dayRect=currday;           //​‌‍‌⁡⁣⁢⁣𝗠𝗼𝗻𝗲𝘆!!!!⁡​
        

        DrawRectangleLinesEx(currday,1,BLACK);         //draw grid squares for each day

        if(dayGrid[boxCounter].dayValue !=0)    //only display date in grid boxes that are not blank
        {
            std::string gridDate=std::to_string(dayGrid[boxCounter].dayValue);
            DrawTextEx(dayfont,gridDate.c_str(),Vector2{x+30,y+40},40,0,DARKGRAY);
            //DrawText(gridDate.c_str(),x+30,y+40,40,BLACK);
        }

        //create numbering for each cell....0 to 419
        std::string cellNum=std::to_string(boxCounter++);  //increment boxCounter
        DrawText(cellNum.c_str(),x+20,y+10,20,BLACK);

        
    }    
   }
    return;
}
//*******************************************************/
//  ⁡⁣⁢⁣Function to determine grid position for a given month-returns Vec2d x,y⁡

Vector2 Gridmaster::FindMonthxy(int month)
{
    Vector2 temp;
    if(month%4==0)  //if in the far right cell, make x=3
        temp.x=3;
            else
            temp.x=((month%4)-1);
    
    if(month<=4) temp.y=0;          //first row
        else if(month<=8) temp.y=1; //second row
            else temp.y=2;          //third row  


    return(temp);


}
//*******************************************************/
//            ⁡⁣⁢⁣​‌‌‍  𝕄𝕠𝕦𝕤𝕖 𝕋𝕣𝕒𝕡​⁡

void Gridmaster::MouseTrap()
{
    Vector2 mousepos;
    std::string positionReadout;

    mousepos=GetMousePosition();
    positionReadout="X= "+std::to_string((int)mousepos.x)+
                   "\nY= "+std::to_string((int)mousepos.y);

    DrawText(positionReadout.c_str(),4*Hinterval+100,Vinterval,40,DARKGREEN);

    
    Gridmaster::MouseCollision(mousepos);



}
//*******************************************************/
int Gridmaster::MouseCollision(Vector2 mousepos)
{
    int contactedSquare=0;
    std::string squarestr;
    bool flag=false;

    int index=0;

    for(gridData& box : dayGrid)    //for each gridData element in the vector DayGrid
    {
       if(CheckCollisionPointRec(mousepos,box.dayRect))
        {       
                flag=true;
                contactedSquare=index;
                break;                  //⁡⁣⁣⁢found the square...get out of the loop for efficiency⁡
        }
        index++;    //move to the next grid square to test
        
    }
    if(!flag)
        contactedSquare=999;
    squarestr="BOX NUM: "+std::to_string(contactedSquare);
    DrawTextEx(monthfont,squarestr.c_str(),Vector2{4*(float)Hinterval+100,(float)Vinterval+100},80,0,BLUE);
    
    if(contactedSquare!=999)
        DrawRectangleRec(dayGrid[contactedSquare].dayRect,Color{0,20,200,50});
        

    return 1;




}

//*******************************************************/
//      ⁡⁣⁢⁣​‌‌‍𝕄𝔼ℝ𝔾𝔼 𝔾ℝ𝕀𝔻 𝔸ℕ𝔻 ℂ𝔸𝕃𝔼ℕ𝔻𝔸ℝ​⁡


void Gridmaster::MergeGridwithCalendar(Calendar* cal)  //Generate Desired Year and Merge it
{
    //use example 2025 -created in the constructor

    
    std::cout<<"In the merge method within Gridmaster...pointer passed\n";

    int monthindex=0; //start at first month
    int dayindex=0;
    int firstdayoffset=0;

    while(monthindex<=11)
    {
        if (cal->DAY[dayindex].dayofWeek ==0) //if month starts on Sat. push to end of week
            firstdayoffset=5;
                else
                    firstdayoffset=cal->DAY[dayindex].dayofWeek -2; //correct for week starting Sunday


        while( cal->DAY[dayindex].year !=0) //iterate a monthuntil hit divider
        {
            std::cout<<cal->DAY[dayindex].month<<"/"<<cal->DAY[dayindex].day<<
            "/"<< cal->DAY[dayindex].year<<"\t\t"<<dayindex<<std::endl;

            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].dayValue=cal->DAY[dayindex].day;



            dayindex++;
        }
        std::cout<<"**********  increment month  *********"<<std::endl;
        monthindex++;
        dayindex++;
    }





}    
//*******************************************************/
