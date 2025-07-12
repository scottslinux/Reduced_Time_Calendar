#include "Gridmaster.h"
#include <iostream>
#include <fstream>
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
    Gridmaster::marker=LoadFont("./resources/mono.ttf");

  
    monWidth=GetScreenWidth();    //initialize to monitor size and calculate everything else
    monHeight=GetScreenHeight();

    Hinterval=monWidth/5;   //(horizontal) width of month block
    Vinterval=monHeight/3;  //(vertical) height of month block

    //intialize dayGrid using placeholder

    gridData placeholder;       //create a temp gridData item and use it to fill in default values
    placeholder.activeBox=false;
    placeholder.blackout=false;
    placeholder.dayRect=Rectangle{0,0,0,0};
    placeholder.dayValue=0;
    placeholder.dayofweek=0;
    placeholder.month=0;
    placeholder.year=0;
    placeholder.designation=0;
    placeholder.value=0;

    //⁡⁣⁢⁣​‌‍‌ℂℝ𝔼𝔸𝕋𝔼 𝕋ℍ𝔼 𝔻𝔸𝕐𝔾ℝ𝕀𝔻 𝕍𝔼ℂ𝕋𝕆ℝ 𝕀ℕ𝕀𝕋𝕀𝔸𝕃𝕀ℤ𝔼𝔻 𝕎𝕀𝕋ℍ 𝔻𝔼𝔽𝔸𝕌𝕃𝕋​⁡
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
        

        DrawRectangleLinesEx(currday,1,BLACK);         //draw grid square outlines for each day

        //⁡⁢⁣⁣​‌‍‌𝗖𝗼𝗹𝗼𝗿 𝗴𝗿𝗶𝗱 𝘀𝗾𝘂𝗮𝗿𝗲 𝗮𝗰𝗰𝗼𝗿𝗱𝗶𝗻𝗴 𝘁𝗼 𝘁𝗵𝗲 𝘁𝘆𝗽𝗲 𝗼𝗳 𝗣𝗧𝗢​⁡

        if(dayGrid[boxCounter].designation==1) //full time PTO
        {
            float currdayadjusted=currday.y+(cellHeight-(cellHeight*dayGrid[boxCounter].value));
            DrawRectangle(currday.x+1,currdayadjusted,cellWidth-2,cellHeight*dayGrid[boxCounter].value
                            ,paletteColor[0]); // 0: color fulltime 
        
        }

        if(dayGrid[boxCounter].designation==2) //reduced time
        {
            float currdayadjusted=currday.y+(cellHeight-(cellHeight*dayGrid[boxCounter].value));
            DrawRectangle(currday.x+1,currdayadjusted,cellWidth-2,cellHeight*dayGrid[boxCounter].value
                            ,paletteColor[1]); // 0: color fulltime 

        }


        if(dayGrid[boxCounter].dayValue !=0)    //only display ⁡⁢⁣⁣date⁡ in grid boxes that are not blank
        {
            std::string gridDate=std::to_string(dayGrid[boxCounter].dayValue);
            DrawTextEx(dayfont,gridDate.c_str(),Vector2{x+30,y+40},40,0,Color{0,0,0,255});
            
  
            
        }
        if(dayGrid[boxCounter].blackout)
        {
            
            DrawRectangleLinesEx(Rectangle{currday.x,currday.y,(float)cellWidth,(float)cellHeight},
                                    10,Color{205,155,255,255});
        }
        if(dayGrid[boxCounter].activeBox)
        {
            DrawRectangle(currday.x,currday.y,10,10,Color{55,74,45,255});
        }




        //increment the gridbox counter

        boxCounter++;
        
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

    DrawRectangle(Hinterval*4,0,Hinterval,Vinterval*3,Color{131,197,203,100});  //color the entire right margin



    DrawCircle(Hinterval*4+Hinterval/2,1350,108,WHITE);
    DrawCircle(Hinterval*4+Hinterval/2,1350,100,paletteColor[colorindex]);

    std::string titleyear=std::to_string(dayGrid[20].year); //the 20th element will definitely be a real day and not blank
    Vector2 stringsizeyear=MeasureTextEx(monthfont,titleyear.c_str(),120,0);
    
    DrawTextEx(monthfont,titleyear.c_str(),Vector2{Hinterval*4+Hinterval/2-stringsizeyear.x/2,0},120,0,BLACK);



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

    Rectangle menu1{Hinterval*4+10,1600,700,100};
    Rectangle menu2{Hinterval*4+10,1710,700,100};
    Rectangle menu3{Hinterval*4+10,1820,700,100};
    Color buttoncolor=Color{241,198,71,255};
    Color buttonshadow=Color{148,120,44,255};


if(!menuflag)   //display the main choices unless the dialogue box is up
{

    DrawRectangle(menu1.x+12,menu1.y+12,700,100,DARKGRAY);
    DrawRectangle(menu1.x,menu1.y,700,100,buttoncolor);
    DrawRectangleLinesEx(menu1,10,buttonshadow);

    DrawRectangle(menu2.x+12,menu2.y+12,700,100,DARKGRAY);
    DrawRectangle(menu2.x,menu2.y,700,100,buttoncolor);
    DrawRectangleLinesEx(menu2,10,buttonshadow);

    DrawRectangle(menu3.x+12,menu3.y+12,700,100,DARKGRAY);
    DrawRectangle(menu3.x,menu3.y,700,100,buttoncolor);
    DrawRectangleLinesEx(menu3,10,buttonshadow);


    DrawTextEx(marker,"Save Current Calendar",Vector2{Hinterval*4+90,1630},50,0,BLACK);
    DrawTextEx(marker,"Load Existing Calendar ",Vector2{Hinterval*4+95,1730},50,0,BLACK);
    DrawTextEx(marker,"Create New Calendar",Vector2{Hinterval*4+100,1850},50,0,BLACK);
}
    else
        {
            DrawRectangle(Hinterval*4+10,1600,700,400,BLACK);
            DrawRectangleLinesEx(Rectangle{Hinterval*4+10,1600,700,400},10,LIGHTGRAY);

            DrawTextEx(marker,"File Exists....Overwrite?",Vector2{Hinterval*4+25,1650},57,0,Color{71,176,26,255});
            DrawTextEx(marker,"   OKAY           CANCEL",Vector2{Hinterval*4+25,1750},50,0,Color{71,176,26,255});
         
            Rectangle okayrect=Rectangle{Hinterval*4+25,1745,350,70};
            Rectangle cancelrect=Rectangle{Hinterval*4+375,1745,300,70};

            int choice=Gridmaster::submenuCheck(okayrect,cancelrect);
                
            

            



        }
    if (loadgraphflag)
    {
        DrawRectangle(Hinterval*4+50,2100,600,15,WHITE);
        graphtimer+=GetFrameTime()*300;

        if(graphtimer<600)
        {
            DrawRectangle(Hinterval*4+50,2100,600-(600-graphtimer),15,RED);
            DrawTextEx(marker,"LOADING CALENDAR",Vector2{Hinterval*4+150,2000},
                60,0,Color{39,75,43,255});

            std::cout<<"Graphtimer: "<<graphtimer<<std::endl;


        }
                else
                {
                    loadgraphflag=false;
                    Gridmaster::loadCalendarfromFile("Reduced_time_2026.txt");
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
    int currentsquare=0;

    mousepos=GetMousePosition(); //shows highligted square on current mouse pos

    currentsquare=Gridmaster::MouseCollision(mousepos); //get the current index for the square

    Gridmaster::mouseClickChoices(currentsquare, mousepos);

    Gridmaster::menuchecking(mousepos);

    


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
        contactedSquare=550; //make this the signal that no valid square exists
                             //still in the vector--no overflow was 999 :(
    
    
    if(contactedSquare!=550)
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
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].designation=0;



            dayindex++;
        }
        std::cout<<"**********  increment month  *********"<<std::endl;
        monthindex++;
        dayindex++;
    }





}    
//***************************************************************************/
//              ⁡⁣⁢⁣​‌‌‍𝗠𝗼𝘂𝘀𝗲 𝗖𝗹𝗶𝗰𝗸 𝗖𝗵𝗼𝗶𝗰𝗲𝘀​⁡

void Gridmaster::mouseClickChoices(int gridIndex, Vector2 mousepos)
{
    //are we in the color circle-yes: change color, 
    //are we in an invlaid square or weekend yes:return
    //valid day: is it colored unchoose it...adjust numbers
    //valid day with shift down...add quarter day
    //**⁡⁣⁢⁣𝘊𝘳𝘦𝘢𝘵𝘪𝘯𝘨 𝘧𝘭𝘢𝘨𝘴 𝘧𝘰𝘳 𝘤𝘭𝘢𝘳𝘪𝘵𝘺 𝘪𝘯 𝘥𝘦𝘤𝘪𝘴𝘪𝘰𝘯 𝘵𝘳𝘦𝘦⁡ */

    

    

    bool shiftFlag=(IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown((KEY_RIGHT_SHIFT)));  //for quarter days
    bool ctrlFlag=(IsKeyDown(KEY_LEFT_CONTROL)||(IsKeyDown(KEY_RIGHT_CONTROL)));//confirmed with VAC
    bool mouseInCircle=CheckCollisionPointCircle(mousepos,Vector2{(float)(Hinterval*4+Hinterval/2),1350},100);
    bool leftClick=IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool rightClick=IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    bool weekendFlag=((dayGrid[gridIndex].dayofweek==0)||(dayGrid[gridIndex].dayofweek==1));
    bool activeDay=(dayGrid[gridIndex].designation);

  
    
    // ⁡⁢⁢⁣⁡⁣⁢⁣𝗖𝗵𝗮𝗻𝗴𝗶𝗻𝗴 𝘁𝗵𝗲 𝗽𝗮𝗶𝗻𝘁 𝗰𝗼𝗹𝗼𝗿 ⁡
    if(mouseInCircle && leftClick)
    {
        colorindex++;                   //routine to change the active palette color
        if(colorindex>1) colorindex=0;
        return;
    }
    

 


    //⁡⁣⁢⁣𝗥𝗶𝗴𝗵𝘁 𝗰𝗹𝗶𝗰𝗸--𝗮𝗱𝗱𝗶𝗻𝗴 𝗯𝗹𝗮𝗰𝗸𝗼𝘂𝘁 𝗱𝗮𝘁𝗲𝘀⁡
    if(rightClick)  //mark blackout dates
        {
            if(dayGrid[gridIndex].blackout)
                dayGrid[gridIndex].blackout=false; //toggle to off if it was already on
                else    
                    dayGrid[gridIndex].blackout=true;


        }


    //-------------------------------------------
    // ⁡⁣⁢⁣Mouse Clicking on a VALID Day that is not a weekend???⁡

    if(leftClick && !weekendFlag && (gridIndex !=550))  //click, non-weekend, in the grid
    {
        if(dayGrid[gridIndex].value<1 && shiftFlag)  //⁡⁣⁣⁢𝘀𝗵𝗶𝗳𝘁= 𝘄𝗼𝗿𝗸𝗶𝗻𝗴 𝘄𝗶𝘁𝗵 𝗾𝘂𝗮𝗿𝘁𝗲𝗿 𝗱𝗮𝘆𝘀⁡ val<1 either 0 or fractional
        {   
            dayGrid[gridIndex].designation=colorindex+1;
            Gridmaster::adjustTotals(dayGrid[gridIndex].designation,0.25);
            dayGrid[gridIndex].value+=0.25;               //attribute full day on that date
            std::cout<<"Value: "<<dayGrid[gridIndex].value<<std::endl;

            return; //gotta get out of the routine after a choice is executed
        }

        if(leftClick && ctrlFlag)   //note the day as confirmed with Valley (can toggle on and off
        {
            if(dayGrid[gridIndex].activeBox==0)
                dayGrid[gridIndex].activeBox=1;
                    else
                    dayGrid[gridIndex].activeBox=0;

            return; //exit before the click triggers anything else


        }

        //if the day is clear -mark it as a full day in whichever category and adjust totals
        if(dayGrid[gridIndex].designation==0)
        {   
            dayGrid[gridIndex].designation=colorindex+1;
            Gridmaster::adjustTotals(dayGrid[gridIndex].designation,1);
            dayGrid[gridIndex].value=1.0;               //attribute full day on that date

            return; //gotta get out of the routine after a choice is executed
        }



        //erase a day
        if((dayGrid[gridIndex].designation==1)||(dayGrid[gridIndex].designation==2))   
        {

            Gridmaster::adjustTotals(dayGrid[gridIndex].designation,dayGrid[gridIndex].value* -1);   //return the time (subtract)
            
            dayGrid[gridIndex].value-=dayGrid[gridIndex].value;          //take it away from that day

            if(dayGrid[gridIndex].value==0)         //only kill designation if all value gone <1/4
                dayGrid[gridIndex].designation=0;

            return;
        }
        
    return;
    
}
}
//****************************************************************************/

void Gridmaster::adjustTotals(int designation,float val) //1=FullTime 2=Reduced
{
    std::cout<<"entering adjusttotals: designation:"<<designation<<" Value:"<<val<<std::endl;

    if(designation==1)  //adjustment to full time when passing negative one it adds a day that is being returned
        fullTimeDays-=val;
        else    
            reducedTimeDays-=val; //adjust reduced

    totalVacation=fullTimeDays+reducedTimeDays;


    return;
}
//****************************************************************************/

void Gridmaster::menuchecking(Vector2 mousepos)
{
    //load the rectangels from the menu drawing area
    std::vector<Rectangle>  menurects{
    {Hinterval*4+10,1600,700,100},{Hinterval*4+10,1710,700,100},{Hinterval*4+10,1820,700,100}};

    for(int i=0;i<3;i++)        //0: save   1: load    2: create new
    {

        if (CheckCollisionPointRec(mousepos,menurects[i]))
        {
            if(!menuflag)   //if dialoguing do not display choice bars
                DrawRectangleLinesEx(menurects[i],15,YELLOW);

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&& i==0)
            {
                menuflag=true;  //trigger the dialogue box
                //this opens the dialogue for the save button
               

            }
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&& i==1)
            {
                loadgraphflag=true;
                graphtimer=10.0;


            }
        }




    }





        return;
}
//******************************************************************************/
//A little chat GPT help. Takes a filename string and a vector(dayGrid)
void Gridmaster::SaveCalendarToFile(const std::string& filename, const std::vector<gridData>& calendar)
{
    std::ofstream outFile(filename);   //create an object of type ofstream attached to filename
    
    if (!outFile.is_open()) {
    std::cerr << "Failed to open file for writing!\n";
    return;
}


    for (const auto& day : calendar)
    {
        outFile<<day.activeBox<<" "<<day.blackout<<" "<<day.dayofweek<<" "<<day.dayRect.x<<" "
            <<day.dayRect.y<<" "<<day.dayRect.height<<" "" "<<day.dayRect.width<<" "<<day.dayValue
            <<" "<<day.designation<<" "<<day.month<<" "<<day.value<<" "<<day.year<<"\n";


    }

    outFile.close();

    return;



}
//******************************************************************************/
int Gridmaster::submenuCheck(Rectangle choice1, Rectangle choice2)
{
    float timedelay=0;

    if(CheckCollisionPointRec(GetMousePosition(),choice1))
            {
                DrawRectangleLinesEx(choice1,5,RED);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    menuflag=false;
                    SetMousePosition(Hinterval*4+50,2000);
                    std::string filename="Reduced_time_"+std::to_string(2026)+".txt";
                    std::cout<<"Saving the current calendar as "<<filename<<std::endl;
                    Gridmaster::SaveCalendarToFile(filename,dayGrid);

                    std::cout<<"File Saved Successfully.....\n";




                    return 1;   //end the dialogue and return choice 1. OKAY
                }
            }
    if(CheckCollisionPointRec(GetMousePosition(),choice2))
            {
                DrawRectangleLinesEx(choice2,5,RED);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    menuflag=false;
                    SetMousePosition(Hinterval*4+50,2000);
                    std::cout<<"File Operation Cancelled\n";


                    return 2;   //end the dialogue and return choice 2. NOPE
                }
            }
            

    return 0;



}
//******************************************************************************/
//Loads from a saved Calendar...need to scan day totals

int Gridmaster::loadCalendarfromFile(std::string filename)
{
        std::cout << "Starting loadCalendarfromFile" << std::endl;

    std::ifstream inputfile(filename);

    if(!inputfile.is_open())
    {
        std::cout<<"*** Unable to open "<<filename<<std::endl;
        return 0;
    }
    for(auto& day: dayGrid)
    {
        inputfile>>day.activeBox;
        inputfile>>day.blackout;
        inputfile>>day.dayofweek;
        inputfile>>day.dayRect.x;
        inputfile>>day.dayRect.y;
        inputfile>>day.dayRect.height;
        inputfile>>day.dayRect.width;
        inputfile>>day.dayValue;
        inputfile>>day.designation;
        inputfile>>day.month;
        inputfile>>day.value;
        inputfile>>day.year;




    }

    inputfile.close();

    //reset the days before calculating how they were reduced
    fullTimeDays=fulltimeallotment;
    reducedTimeDays=reducedtimeallotment;

    for( auto& day: dayGrid)
    {
        if(day.designation==1)
            fullTimeDays-=day.value;
        if(day.designation==2)
            reducedTimeDays-=day.value;


    }
    totalVacation=fullTimeDays+reducedTimeDays;



return 1;

}