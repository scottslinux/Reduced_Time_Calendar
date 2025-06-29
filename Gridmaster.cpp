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

    //Create a  pointer on the stack with the actual object on the heap
    //stack is small..heap is all of memory
    //pointers use heap for object creation otherwise stack smashing
    
  
    //calendarYear=new Calendar();  //⁡⁣⁢⁣𝗰𝗿𝗲𝗮𝘁𝗲 𝘁𝗵𝗲 𝗰𝗮𝗹𝗲𝗻𝗱𝗮𝗿 𝗼𝗯𝗷𝗲𝗰𝘁 𝗼𝗻 𝘁𝗵𝗲 𝗵𝗲𝗮𝗽⁡ 
    //calendarYear->loadCalendar(2025,0); //⁡⁣⁢⁣𝗹𝗼𝗮𝗱 𝘄𝗶𝘁𝗵 𝟮𝟬𝟮𝟱⁡

    
    //intialize dayGrid using placeholder

    gridData placeholder;       //create a temp gridData item and use it to fill in default values
    placeholder.activeBox=false;
    placeholder.dayRect=Rectangle{0,0,0,0};
    placeholder.dayValue=0;
    placeholder.dayofweek=0;
    placeholder.month=0;
    placeholder.year=0;

    dayGrid.resize(420, placeholder);  //this may have fixed the stack slamming exception

    
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
        DrawLineEx(Vector2{(float)i*Hinterval,1},Vector2{(float)i*Hinterval,(float)monHeight-1},8,BLACK);
    
    //weekend shading
    
    for(int i=0;i<4;i++)
    {
        DrawRectangle(0+i*Hinterval,0,Hinterval/7,Vinterval*3,Color{177,225,175,255});
        DrawRectangle((Hinterval/7)*6+i*Hinterval,0,Hinterval/7,Vinterval*3,Color{177,225,175,255});
    }


    //Month Bars
    for(int num=0;num<=2;num++)
    {
        Rectangle monthbar{0,Vinterval*num,Hinterval*4,Vinterval/6};
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
    int cellHeight=Vinterval/6; //height of day block

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
        daypos.y=monthPosxy.y*Vinterval+70;

        DrawTextEx(dayfont,weekdaynames[day].c_str(),daypos,40,0,WHITE);

        

    }

  
  for(int week=1;week<=5;week++)     //for each week of the 5 week grid
  {
    for(int day=0;day<=6;day++)     //Create 7 days
    {
        int x=(day*cellWidth)+(monthPosxy.x*Hinterval);      //find the x,y of upper left corner of the day
        int y=(week*cellHeight)+(monthPosxy.y*Vinterval);




        

        Rectangle currday={x,y,cellWidth,cellHeight};  //​‌‍‌⁡⁣⁢⁣𝘁𝗵𝗶𝘀 𝗶𝘀 𝘄𝗵𝗲𝗿𝗲 𝘄𝗲 𝘀𝘁𝗼𝗿𝗲 𝘁𝗵𝗲 𝗿𝗲𝗰𝘁 𝗶𝗻 𝘃𝗲𝗰𝘁𝗼𝗿⁡​
        dayGrid[boxCounter].dayRect=currday;           //​‌‍‌⁡⁣⁢⁣𝗠𝗼𝗻𝗲𝘆!!!!⁡​
        

        DrawRectangleLinesEx(currday,1,BLACK);         //draw grid squares for each day

        std::string gridDate=std::to_string(dayGrid[boxCounter].dayValue);
        DrawText(gridDate.c_str(),x+30,y+40,40,BLACK);

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




    for(int i=0;i<=30;i++)
    {
        //Mullers has Sat as 0...we need Sun as 0 and Sat as 6 far right
        //not altering actual Calendar object

        int correctedDay=cal->DAY[i].dayofWeek;
        if (correctedDay==0)
            correctedDay=6;
            else
                correctedDay=correctedDay-1;

        int monthoffset=(cal->DAY[i].month-1)*35;   //first square of every box
                                                    //is mult of 35.  o, 35, 70...
        
        
        int gridbox=monthoffset+ cal->DAY[i].day+3-1;
        
        dayGrid[gridbox].dayValue=cal->DAY[i].day;

        std::cout<<cal->DAY[i].month<<" "<<cal->DAY[i].day<<" "<<cal->DAY[i].dayofWeek<<
            " "<<cal->DAY[i].year<<"\t";

        std::cout<<"corr day: "<<correctedDay<<"  gridbox: "<<gridbox<<std::endl;



        




    }





/*

    for(size_t i=0;i<cal->DAY.size();i++)
    {
        std::cout<<cal->DAY[i].month<<" "<<cal->DAY[i].day<<" "<<cal->DAY[i].dayofWeek<<
            " "<<cal->DAY[i].year<<std::endl;

    }

  */



}    
//*******************************************************/
