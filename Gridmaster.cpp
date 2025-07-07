#include "Gridmaster.h"
#include <iostream>
#include "raylib.h"
#include "Calendar.h"




Font Gridmaster::monthfont;
Font Gridmaster::dayfont;
Font Gridmaster::marker;

//********************************************************
//  ⁡⁣⁢⁣Constructor⁡
Gridmaster::Gridmaster()

{
    
    Gridmaster::monthfont=LoadFontEx("./resources/calfont.ttf",80,NULL,0);
    Gridmaster::dayfont=LoadFontEx("./resources/days.ttf",100,NULL,0);
    Gridmaster::marker=LoadFont("./resources/Marker Felt.ttf");

  
    monWidth=GetScreenWidth();    //initialize to monitor size and calculate everything else
    monHeight=GetScreenHeight();

    Hinterval=monWidth/5;   //(horizontal) width of month block
    Vinterval=monHeight/3;  //(vertical) height of month block

    //intialize dayGrid using placeholder

    gridData placeholder;       //create a temp gridData item and use it to fill in default values
    placeholder.activeBox=false;
    placeholder.dayRect=Rectangle{0,0,0,0};
    placeholder.dayValue=0;
    placeholder.dayofweek=0;
    placeholder.month=0;
    placeholder.year=0;
    placeholder.typeofday=0;

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

    Scoreboard();


     
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
        //Color grid square according to the type of PTO
        if(dayGrid[boxCounter].typeofday==1) //full time PTO
            DrawRectangle(currday.x+1,currday.y+1,cellWidth-2,cellHeight-2,Color{0,121,241,150});


        if(dayGrid[boxCounter].dayValue !=0)    //only display date in grid boxes that are not blank
        {
            std::string gridDate=std::to_string(dayGrid[boxCounter].dayValue);
            DrawTextEx(dayfont,gridDate.c_str(),Vector2{x+30,y+40},40,0,DARKGRAY);
            //DrawText(gridDate.c_str(),x+30,y+40,40,BLACK);
        }

        //create numbering for each cell....0 to 419
        std::string cellNum=std::to_string(boxCounter++);  //increment boxCounter
        //DrawText(cellNum.c_str(),x+20,y+10,20,BLACK);

        
    }    
   }
    return;
}
//*************************************************************************/
//          ⁡⁣⁢⁣​‌‌‍𝗦𝗰𝗼𝗿𝗲𝗯𝗼𝗮𝗿𝗱 𝗳𝗼𝗿 𝗱𝗶𝘀𝗽𝗹𝗮𝘆 𝗮𝗻𝗱 𝗧𝗮𝗹𝗹𝘆 𝗼𝗳 𝗗𝗮𝘆𝘀​⁡
// ***Needs re-write


void Gridmaster::Scoreboard(void)
{
    //right margin starts at Hinterval*4

    DrawRectangle(Hinterval*4,0,Hinterval,Vinterval*3,Color{131,197,203,100});

    DrawCircle(Hinterval*4+Hinterval/2,1350,108,WHITE);
    DrawCircle(Hinterval*4+Hinterval/2,1350,100,Color{0,121,241,180});


    Vector2 stringsizeyear=MeasureTextEx(monthfont,"2025",120,0);
    std::string yearstring="2025";
    DrawTextEx(monthfont,yearstring.c_str(),Vector2{Hinterval*4+Hinterval/2-stringsizeyear.x/2,0},120,0,BLACK);



    Rectangle scorerectfull{Hinterval*4+Hinterval/2-250,100,500,300};
    Rectangle scorerectReduced{Hinterval*4+Hinterval/2-250,500,500,300};
    Rectangle scorerecttotal{Hinterval*4+Hinterval/2-250,900,500,300};


    //std::string fullstring=std::to_string(fullTimeDays);

    char buffer[32];
    snprintf(buffer,sizeof(buffer),"%.2f",fullTimeDays);
    char buffer2[32];
    snprintf(buffer2,sizeof(buffer2),"%.2f",reducedTimeDays);
    char buffer3[32];
    snprintf(buffer3,sizeof(buffer3),"%.2f",totalVacation);

    

    Vector2 fullstringsize=MeasureTextEx(monthfont,buffer,200,0);
    Vector2 centerfull {Hinterval*4+Hinterval/2-fullstringsize.x/2, 150};
    Vector2 centerred {Hinterval*4+Hinterval/2-fullstringsize.x/2, 550};
    Vector2 centertotal {Hinterval*4+Hinterval/2-fullstringsize.x/2, 950};



    //draw rounded rectangles for the days
    DrawRectangleRounded(scorerectfull,0.4,8,BLUE);                 
    DrawRectangleRounded(scorerectReduced,0.4,8,Color{44,104,34,255});
    DrawRectangleRounded(scorerecttotal,0.4,8,Color{94,116,150,255});
    

    //fill in the days
    DrawTextEx(monthfont,buffer,centerfull,200,0,WHITE);
    DrawTextEx(monthfont,buffer2,centerred,200,0,WHITE);
    DrawTextEx(monthfont,buffer3,centertotal,200,0,WHITE);

    //labels
    Vector2 centerfulltext {Hinterval*4+Hinterval/2-MeasureTextEx(monthfont,"FULL TIME",50,0).x/2,120};
    Vector2 centerredtext {Hinterval*4+Hinterval/2-MeasureTextEx(monthfont,"REDUCED TIME",50,0).x/2, 530};
    Vector2 centertotaltext {Hinterval*4+Hinterval/2-MeasureTextEx(monthfont,"TOTAL DAYS",50,0).x/2, 930};

    DrawTextEx(monthfont,"FULL TIME",centerfulltext,50,0,WHITE);
    DrawTextEx(monthfont,"REDUCED TIME",centerredtext,50,0,WHITE);
    DrawTextEx(monthfont,"TOTAL DAYS",centertotaltext,50,0,WHITE);
    centertotaltext.y+=200;
    DrawTextEx(monthfont,"REMAINING",centertotaltext,50,0,WHITE);


    DrawTextEx(marker,"Percentage of FTE: 80%",Vector2{Hinterval*4+25,1600},100,0,BLACK);
    DrawTextEx(marker,"Total Days: 91 ",Vector2{Hinterval*4+25,1700},100,0,BLACK);
    DrawTextEx(marker,"Total Weeks: 18 ",Vector2{Hinterval*4+25,1800},100,0,BLACK);
    DrawTextEx(marker,"Carry-Over: 3 days ",Vector2{Hinterval*4+25,1900},100,0,BLACK);




    
    
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
    int currentsquare=999;

    mousepos=GetMousePosition(); //shows highligted square on current mouse pos

    currentsquare=Gridmaster::MouseCollision(mousepos); //get the current index for the square

    Gridmaster::mouseClickChoices(currentsquare);


}
//*******************************************************/
//      ⁡⁣⁢⁣​‌‌‍ℂ𝕙𝕖𝕔𝕜 𝕄𝕠𝕦𝕤𝕖 ℙ𝕠𝕤𝕚𝕥𝕚𝕠𝕟 𝔸𝕘𝕒𝕚𝕟𝕤𝕥 𝕥𝕙𝕖 𝔾𝕣𝕚𝕕 𝕍𝕖𝕔𝕥𝕠𝕣​⁡
//      Determine square/date the mouse is over

int Gridmaster::MouseCollision(Vector2 mousepos)
{
    int contactedSquare=0;
    std::string squarestr;
    bool flag=false;

    int index=0;

    //did you click on the color circle
    if(CheckCollisionPointCircle(mousepos,Vector2{(float)(Hinterval*4+Hinterval/2),1350},100)&&
                                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        std::cout<<"CIRCLE JERK!!!!!\n";
        //routine to change the active palette color
        return 999;
    }

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
    //squarestr="BOX NUM: "+std::to_string(contactedSquare);
    //DrawTextEx(monthfont,squarestr.c_str(),Vector2{4*(float)Hinterval+100,(float)Vinterval+100},80,0,BLUE);
    
    if(contactedSquare!=999)
        DrawRectangleRec(dayGrid[contactedSquare].dayRect,Color{0,20,200,50});
        

    return contactedSquare;




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

            //set all of the values for the day in the grid to match the appropriate calendar values
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].dayValue=cal->DAY[dayindex].day;
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].dayofweek=cal->DAY[dayindex].dayofWeek;
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].month=cal->DAY[dayindex].month;
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].year=cal->DAY[dayindex].year;
            //set type of day to 0: default unbooked (not present in cal)
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].typeofday=0;



            dayindex++;
        }
        std::cout<<"**********  increment month  *********"<<std::endl;
        monthindex++;
        dayindex++;
    }





}    
//***************************************************************************/
//              ⁡⁣⁢⁣​‌‌‍𝗠𝗼𝘂𝘀𝗲 𝗖𝗹𝗶𝗰𝗸 𝗖𝗵𝗼𝗶𝗰𝗲𝘀​⁡

void Gridmaster::mouseClickChoices(int gridIndex)
{
    //are we in the color circle-yes: change color, 
    //are we in an invlaid square or weekend yes:return
    //valid day: is it colored unchoose it...adjust numbers
    //valid day with shift down...add quarter day
    


    // Mouse Clicking for PTO Choice and not on a weekend???
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&& (dayGrid[gridIndex].dayofweek!=0)&&
                            (dayGrid[gridIndex].dayofweek!=1))
    {
        std::cout<<"LEFT MOUSE BUTTON CLICKED on "<<
            dayGrid[gridIndex].month<<"/"<<dayGrid[gridIndex].dayValue<<"/"<<
                dayGrid[gridIndex].year<<std::endl;
            dayGrid[gridIndex].typeofday=activePaint;
        fullTimeDays--;


    }







return;
}