#include "Gridmaster.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <string>
#include "raylib.h"
#include "Calendar.h"
#include "Menu.h"

namespace fs = std::filesystem;


Font Gridmaster::monthfont;
Font Gridmaster::dayfont;
Font Gridmaster::marker;
Font Gridmaster::dot;
Sound Gridmaster::click;

//********************************************************
//  ⁡⁣⁢⁣Constructor⁡
Gridmaster::Gridmaster()

{
    
    Gridmaster::monthfont=LoadFontEx("./resources/calfont.ttf",80,NULL,0);
    Gridmaster::dayfont=LoadFontEx("./resources/days.ttf",100,NULL,0);
    Gridmaster::marker=LoadFont("./resources/mono.ttf");
    Gridmaster::dot=LoadFontEx("./resources/digital-7.ttf",50,0,0);

    Gridmaster::click=LoadSound("./resources/softclick.wav");

    Gridmaster::buttondownoff=LoadTexture("./resources/button_down_off.png");
    Gridmaster::buttondownon=LoadTexture("./resources/button_down_on.png");
    Gridmaster::buttonupoff=LoadTexture("./resources/button_add_off.png");
    Gridmaster::buttonupon=LoadTexture("./resources/button_add_on.png");
    Gridmaster::press_shadow=LoadTexture("./resources/press_shadow.png");


  
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

    //⁡⁣⁢⁣‍ℂℝ𝔼𝔸𝕋𝔼 𝕋ℍ𝔼 𝔻𝔸𝕐𝔾ℝ𝕀𝔻 𝕍𝔼ℂ𝕋𝕆ℝ 𝕀ℕ𝕀𝕋𝕀𝔸𝕃𝕀ℤ𝔼𝔻 𝕎𝕀𝕋ℍ 𝔻𝔼𝔽𝔸𝕌𝕃𝕋⁡
    dayGrid.resize(600, placeholder);  //this may have fixed the stack slamming exception

    
        //debugging
        std::cout<<"Days in dayGrid vector: "<<dayGrid.size()<<"\n";

    
 
}
//********************************************************
Gridmaster::~Gridmaster()   //destructor Called Automatically by C++
{
    //delete calendarYear;  // Clean up the dynamically allocated Calendar
    //calendarYear = nullptr; // Good practice to set to nullptr

    std::cout<<"Unloading resources in Gridmaster Destructor....."<<std::endl;

    userMenu.unloadresources();
    UnloadTexture(buttondownoff);
    UnloadTexture(buttondownon);
    UnloadTexture(buttonupoff);
    UnloadTexture(buttonupon);
}

//********************************************************


//      ⁡⁣⁢⁣Create the big screen Grid⁡
int Gridmaster::DrawGrid()
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

    Scoreboard();  // main drawing routine and menu link

    //   ⁡⁣⁢⁣A new year has been chosen...fall out to ReducedTime.cpp to start again⁡
    //   ⁡⁣⁢⁣with the new year.⁡

    if(desiredyear!=activeYear)
    {
        return(desiredyear);


    }


     
    return 0;

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




        

        Rectangle currday={x,y,cellWidth,cellHeight};  //‍⁡⁣⁢⁣𝘁𝗵𝗶𝘀 𝗶𝘀 𝘄𝗵𝗲𝗿𝗲 𝘄𝗲 𝘀𝘁𝗼𝗿𝗲 𝘁𝗵𝗲 𝗿𝗲𝗰𝘁 𝗶𝗻 𝘃𝗲𝗰𝘁𝗼𝗿⁡
        dayGrid[boxCounter].dayRect=currday;           //‍⁡⁣⁢⁣𝗠𝗼𝗻𝗲𝘆!!!!⁡
        

        DrawRectangleLinesEx(currday,1,BLACK);         //draw grid square outlines for each day

        //⁡⁢⁣⁣‍𝗖𝗼𝗹𝗼𝗿 𝗴𝗿𝗶𝗱 𝘀𝗾𝘂𝗮𝗿𝗲 𝗮𝗰𝗰𝗼𝗿𝗱𝗶𝗻𝗴 𝘁𝗼 𝘁𝗵𝗲 𝘁𝘆𝗽𝗲 𝗼𝗳 𝗣𝗧𝗢⁡

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
//          ⁡⁣⁢⁣‍𝗦𝗰𝗼𝗿𝗲𝗯𝗼𝗮𝗿𝗱 𝗳𝗼𝗿 𝗱𝗶𝘀𝗽𝗹𝗮𝘆 𝗮𝗻𝗱 𝗧𝗮𝗹𝗹𝘆 𝗼𝗳 𝗗𝗮𝘆𝘀⁡
// ***Needs re-write


void Gridmaster::Scoreboard(void)
{
    //right margin starts at Hinterval*4

    DrawRectangle(Hinterval*4,0,Hinterval,Vinterval*3,Color{131,197,203,100});  //color the entire right margin



    DrawCircle(Hinterval*4+Hinterval/2,1350,108,WHITE);
    DrawCircle(Hinterval*4+Hinterval/2,1350,100,paletteColor[colorindex]);

    std::string titleyear=std::to_string(activeYear); // the year currently being operated on
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

    DrawTextureEx(buttonupoff,{Hinterval*5-160,80},2,.1,WHITE);
    DrawTextureEx(buttondownoff,{Hinterval*5-160,230},2,.1,WHITE);

    DrawTextureEx(buttonupoff,{Hinterval*5-160,470},2,.1,WHITE);
    DrawTextureEx(buttondownoff,{Hinterval*5-160,620},2,.1,WHITE);

    dashBoard();    //draws the FTE DAY Tally dashboard



    menuserver();   //service the menus

    eventTimer();   //service the event clock
    
  

    
    
    
    
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
//            ⁡⁣⁢⁣‍  𝕄𝕠𝕦𝕤𝕖 𝕋𝕣𝕒𝕡⁡

void Gridmaster::MouseTrap()
{
    Vector2 mousepos;
    std::string positionReadout;
    int currentsquare=0;

    mousepos=GetMousePosition(); //shows highligted square on current mouse pos

    currentsquare=Gridmaster::MouseCollision(mousepos); //get the current index for the square

    Gridmaster::mouseClickChoices(currentsquare, mousepos);

    


}
//*******************************************************/
//      ⁡⁣⁢⁣‍ℂ𝕙𝕖𝕔𝕜 𝕄𝕠𝕦𝕤𝕖 ℙ𝕠𝕤𝕚𝕥𝕚𝕠𝕟 𝔸𝕘𝕒𝕚𝕟𝕤𝕥 𝕥𝕙𝕖 𝔾𝕣𝕚𝕕 𝕍𝕖𝕔𝕥𝕠𝕣⁡
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
        

    //std::cout<<"Mouse x: "<<GetMousePosition().x<<"  y: "<<GetMousePosition().y<<std::endl;

    return contactedSquare;




}

//*******************************************************/
//      ⁡⁣⁢⁣‍𝕄𝔼ℝ𝔾𝔼 𝔾ℝ𝕀𝔻 𝔸ℕ𝔻 ℂ𝔸𝕃𝔼ℕ𝔻𝔸ℝ⁡


void Gridmaster::MergeGridwithCalendar(Calendar* cal)  //Generate Desired Year and Merge it
{
    
    //  ⁡⁣⁢⁣𝗦𝗲𝘁 𝗯𝗼𝘁𝗵 𝗮𝗰𝘁𝗶𝘃𝗲𝗬𝗲𝗮𝗿 𝗮𝗻𝗱 𝗱𝗲𝘀𝗶𝗿𝗲𝗱𝗬𝗲𝗮𝗿 𝗲𝗾𝘂𝗮𝗹 𝘁𝗼 𝗰𝘂𝗿𝗿𝗲𝗻𝘁 𝘆𝗲𝗮𝗿 𝘁𝗵𝗮𝘁 𝗶𝘀 𝗯𝗲𝗶𝗻𝗴 𝗺𝗲𝗿𝗴𝗲𝗱⁡
    activeYear=cal->DAY[10].year;      // take 10th grid spot for January..pick the year up there
    desiredyear=activeYear;
    

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
            //std::cout<<cal->DAY[dayindex].month<<"/"<<cal->DAY[dayindex].day<<
            //"/"<< cal->DAY[dayindex].year<<"\t\t"<<dayindex<<std::endl;

            //set all of the values for the day in the grid to match the appropriate calendar values
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].dayValue=cal->DAY[dayindex].day;
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].dayofweek=cal->DAY[dayindex].dayofWeek;
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].month=cal->DAY[dayindex].month;
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].year=cal->DAY[dayindex].year;
            //set type of day to 0: default unbooked (not present in cal)
            dayGrid[monthindex*42 + firstdayoffset + cal->DAY[dayindex].day].designation=0;



            dayindex++;
        }
        //std::cout<<"**********  increment month  *********"<<std::endl;
        monthindex++;
        dayindex++;
    }





}    
//***************************************************************************/
//              ⁡⁣⁢⁣‍𝗠𝗼𝘂𝘀𝗲 𝗖𝗹𝗶𝗰𝗸 𝗖𝗵𝗼𝗶𝗰𝗲𝘀⁡

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
    
    //--------------------------------------------------
    // ⁡⁣⁣⁢‍𝗨𝗣 𝗕𝗨𝗧𝗧𝗢𝗡⁡ FULL TIME
    
    if(CheckCollisionPointCircle(mousepos,{3774,167},50)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && !eventTimerFlag)
    {   
        fullTimeDays+=0.25;     //add a quarter day
        fulltimeallotment+=0.25;
        totalVacation+=0.25;
        if(!IsSoundPlaying(click))
            PlaySound(click);
        eventTimerFlag=true;
        

    }
    if ((eventTimerFlag)&&(CheckCollisionPointCircle(mousepos,{3774,167},50)))
    {
        DrawTextureEx(buttonupon,{Hinterval*5-160,80},2,.1,WHITE);
        DrawTextureEx(press_shadow,{Hinterval*5-160,80},2,.1,WHITE);

    }
    //---------------------------------------------------------
    //⁡⁣⁣⁢‍𝗗𝗢𝗪𝗡 𝗕𝗨𝗧𝗧𝗢𝗡⁡ FULL TIME
    
 
    if(CheckCollisionPointCircle(mousepos,{3774,319},50)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && !eventTimerFlag)
    {   
        fullTimeDays-=0.25;     //add a quarter day
        fulltimeallotment-=0.25;
        totalVacation-=0.25;

        if(!IsSoundPlaying(click))
            PlaySound(click);
        eventTimerFlag=true;
        

    }
    if ((eventTimerFlag)&&(CheckCollisionPointCircle(mousepos,{3774,319},50)))
    {
        DrawTextureEx(buttondownon,{Hinterval*5-160,230},2,.1,WHITE);
        DrawTextureEx(press_shadow,{Hinterval*5-160,230},2,.1,WHITE);

    }
    //---------------------------------------------------------
    //⁡⁣⁣⁢𝗨𝗣 𝗕𝗨𝗧𝗧𝗢𝗡 𝗥𝗘𝗗𝗨𝗖𝗘𝗗 𝗧𝗜𝗠𝗘⁡
if(CheckCollisionPointCircle(mousepos,{3774,560},50)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && !eventTimerFlag)
    {   
        reducedTimeDays+=0.25;     //add a quarter day
        reducedtimeallotment+=0.25;
        totalVacation+=0.25;

        if(!IsSoundPlaying(click))
            PlaySound(click);
        eventTimerFlag=true;
        

    }
    if ((eventTimerFlag)&&(CheckCollisionPointCircle(mousepos,{3774,560},50)))
    {
        DrawTextureEx(buttonupon,{Hinterval*5-160,470},2,.1,WHITE);
        DrawTextureEx(press_shadow,{Hinterval*5-160,470},2,.1,WHITE);

    }
    //---------------------------------------------------------
    //⁡⁣⁣⁢⁡⁣⁣⁢𝗗𝗢𝗪𝗡 𝗕𝗨𝗧𝗧𝗢𝗡 𝗥𝗘𝗗𝗨𝗖𝗘𝗗 𝗧𝗜𝗠𝗘⁡
    if(CheckCollisionPointCircle(mousepos,{3774,710},50)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && !eventTimerFlag)
    {   
        reducedTimeDays-=0.25;     //add a quarter day
        reducedtimeallotment-=0.25;
        totalVacation-=0.25;

        if(!IsSoundPlaying(click))
            PlaySound(click);
        eventTimerFlag=true;
        

    }
    if ((eventTimerFlag)&&(CheckCollisionPointCircle(mousepos,{3774,710},50)))
    {
        DrawTextureEx(buttondownon,{Hinterval*5-160,620},2,.1,WHITE);
        DrawTextureEx(press_shadow,{Hinterval*5-160,620},2,.1,WHITE);

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
            //std::cout<<"Value: "<<dayGrid[gridIndex].value<<std::endl;

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
    //std::cout<<"entering adjusttotals: designation:"<<designation<<" Value:"<<val<<std::endl;

    if(designation==1)  //adjustment to full time when passing negative one it adds a day that is being returned
        fullTimeDays-=val;
        else    
            reducedTimeDays-=val; //adjust reduced

    totalVacation=fullTimeDays+reducedTimeDays;


    return;
}
//****************************************************************************/

//******************************************************************************/
//A little chat GPT help. Takes a filename string and a vector(dayGrid)
void Gridmaster::SaveCalendarToFile(const std::string& filename, const std::vector<gridData>& calendar)
{
    std::ofstream outFile(filename);   //create an object of type ofstream attached to filename
    
    if (!outFile.is_open()) {
    std::cerr << "Failed to open file for writing!\n";
    return;
}
    //output the header with data
    outFile<<ftePercentage<<" "<<fulltimeallotment<<" "<<reducedtimeallotment<<"\n";

    for (const auto& day : calendar)
    {
        outFile<<day.activeBox<<" "<<day.blackout<<" "<<day.dayofweek<<" "<<day.dayRect.x<<" "
            <<day.dayRect.y<<" "<<day.dayRect.height<<" "<<day.dayRect.width<<" "<<day.dayValue
            <<" "<<day.designation<<" "<<day.month<<" "<<day.value<<" "<<day.year<<"\n";


    }

    outFile.close();

    return;



}

//******************************************************************************/

//******************************************************************************/
//                           ‍⁡⁣⁢⁣‍𝕄𝕖𝕟𝕦 𝕊𝕖𝕣𝕧𝕖𝕣⁡

void Gridmaster::menuserver(void)
{


if (!menuTimerFlag)
{

    if(mainMenuflag)   //display the main choices unless the dialogue box is up
    {
        std::vector<std::string> options1={"Save Current Calendar","Load Existing Calendar ","Create New Calendar","Set % FTE","EXIT"};

        int selection=userMenu.displayMenu(options1,{3100,1800},60);
        
        switch (selection)  //int returned from the menu selection
        {
        case 1:                     //Save Case -->opens confirmation menu for (exists...save/cancel
            {
            replaceMenuflag=true;
            mainMenuflag=false;
            menuDelay();

            }
            break;
        //------------------------------------------
        case 2:                     //load menu....will open choices of which file
            {
                loadMenuflag=true;
                mainMenuflag=false;
                menuDelay();
                

            }
            break;
        //------------------------------------------
        case 3:                     //will eventually allow creation of new years schedule
            {
                mainMenuflag=false; //turn off main menu and bring up new year selection
                createCalflag=true; //turn on selection menu
                menuDelay();
                //SetMousePosition(Hinterval*4.3,1600);   //move the mouse from the main menu buttons


            }
            break;
        //------------------------------------------
        case 4:                     //EXIT the program
            {
                mainMenuflag=false;
                FTEflag=true;    //signal to return to main and exit completely
                SetMousePosition(Hinterval*4.3,1600);   //move the mouse from the main menu buttons


                
            }
            break;
        //------------------------------------------
        case 5:                     //EXIT the program
            {
                desiredyear=999;    //signal to return to main and exit the program
            }
            break;
        //------------------------------------------
      
        default:
            break;
        }
    }  
}
if(!menuTimerFlag)
{   std::string filenametest="./SavedCalendars/Reduced_time_"+(std::to_string(dayGrid[20].year))+".txt";

    if (replaceMenuflag && std::filesystem::exists(filenametest))

         {
            //  ⁡⁣⁣⁢𝗨𝘀𝗶𝗻𝗴 𝘁𝗵𝗲 𝗠𝗲𝗻𝘂 𝗖𝗹𝗮𝘀𝘀 𝘁𝗼 𝗱𝗶𝘀𝗽𝗹𝗮𝘆 𝘀𝘂𝗯𝗺𝗲𝗻𝘂 𝗮𝗻𝗱 𝗳𝗶𝗲𝗹𝗱 𝘁𝗵𝗲 𝗿𝗲𝘀𝗽𝗼𝗻𝘀𝗲𝘀. 𝗘𝗹𝗶𝗺𝗶𝗻𝗮𝘁𝗲𝘀 𝗦𝘂𝗯𝗺𝗲𝗻𝘂𝗰𝗵𝗲𝗰𝗸𝗶𝗻𝗴 𝗺𝗲𝘁𝗵𝗼𝗱⁡
            //  ⁡⁣⁣⁢Appears and disappears with mainMenuflag boolean⁡

            std::vector<std::string> menuitems{"Replace File","Cancel"};
            int choice=userMenu.displayMenu("File Already Exists",menuitems,Vector2{Hinterval*4+50,1800},50);
            if(choice==1)
            {
                    //mainMenuflag=true;  //turn main menu back on
                    menuDelay();
                    replaceMenuflag=false; //finished with replacemenu..turn it off
                    mainMenuflag=true;

                    std::string filename="./SavedCalendars/Reduced_time_"+(std::to_string(dayGrid[20].year))+".txt";
                    std::cout<<"Saving the current calendar as "<<filename<<std::endl;
                    Gridmaster::SaveCalendarToFile(filename,dayGrid);

                    std::cout<<"File Saved Successfully.....\n";

       
            }
            if(choice==2)
            {       menuDelay();
                    mainMenuflag=true;  //turn main menu back on
                    replaceMenuflag=false; //finished with replacemenu..turn it off
                    std::cout<<"exiting without change..."<<std::endl;
                    mainMenuflag=true;

            }

        }
        else  
        if (replaceMenuflag && !std::filesystem::exists(filenametest))  //file does not exist....just save it
        {           
                    menuDelay();
                    mainMenuflag=true;  //turn main menu back on
                    replaceMenuflag=false; //finished with replacemenu..turn it off
                    std::cout<<"File did not exist...now it does!!!!!"<<std::endl;
                    Gridmaster::SaveCalendarToFile(filenametest,dayGrid);
        }


}
if (!menuTimerFlag)  //while the timer is running wait before new menu pops
{
    if(loadMenuflag)
    {
        std::string temp=chooseLoadFile();   //  ⁡⁣⁢⁣𝙘𝙝𝙤𝙤𝙨𝙚𝙇𝙤𝙖𝙙𝙁𝙞𝙡𝙚() 𝙘𝙖𝙡𝙡⁡
        if (temp !="NONE")
        {
            //at this point a choice of a file has been made
            activeFileName=temp;
            loadgraphflag=true;     //start the graphic running
            graphtimer=10.0;



        }
        
    }
}
    //  ⁡⁣⁢⁣Flag is active to signal create new calendar operation⁡
if (!menuTimerFlag) //while the timer is running wait before new menu pops
{
    if (createCalflag)
    {
        std::vector<std::string> futureyrs={"2025","2026","2027","2028","2029","2030","2031","2032","2033","2034","2035"};
        int yearchosen=userMenu.displayMenu("Select Year",futureyrs,Vector2{Hinterval*4.3,1600},42);

        if(yearchosen !=0)  //user has chosen a year
        {
            createCalflag=false;
            Gridmaster::menuDelay();

            desiredyear=yearchosen+2024;
            menuDelay();
            mainMenuflag=true;

            

        }


    }
}  
    //                              load graph bar graphic
    if (loadgraphflag)
    {
        DrawRectangle(Hinterval*4+50,2100,600,15,WHITE);
        graphtimer+=GetFrameTime()*300;
        mainMenuflag=false;

        if(graphtimer<600)
        {
            DrawRectangle(Hinterval*4+50,2100,600-(600-graphtimer),15,RED);
            DrawTextEx(marker,"LOADING CALENDAR",Vector2{Hinterval*4+150,2000},
                60,0,Color{39,75,43,255});
        }
                else
                {
                    loadgraphflag=false;
                    mainMenuflag=true;
                    Gridmaster::loadCalendarfromFile(activeFileName);  //actually perform the load
                    activeYear=dayGrid[20].year;
                    desiredyear=activeYear;
                }



    }

    if (menuTimerFlag)  //slowly phase in main menu to prevent extra click sensing
            menuDelay();

    
    if (FTEflag)    //in the FTE selection menu
    {   
        
        std::vector<std::string> choices={"100","90","80","70","60","50"};
        int percent=userMenu.displayMenu("% FTE",choices,Vector2{Hinterval*4+200,1650},50);
        


        if (percent !=0)    //percentage has been chosen
        {
            /*   100% fte awarded 50 days PTO. Approx workdays in the year is 261 minus
                 Holidays is 250. Which means that 100% fte works 250-50days = 200 days
                 per year.  Every 10% reduction is 20 days less than 200*/
            percent--;

        
            
            fullTimeDays=50;    //5 weeks of standard vacation
            reducedTimeDays= (200*((percent*.10)));
            ftePercentage= std::stof(choices[percent].c_str())/100;





            std::cout<<"fulltime: "<<fullTimeDays<<std::endl;
            std::cout<<"reduced: "<<reducedTimeDays<<std::endl;
            std::cout<<"total: "<<totalVacation<<std::endl;
           


            totalVacation=fullTimeDays+reducedTimeDays;

            Gridmaster::SetPercentFTE();

            std::cout<<"******after adjustment*********\n";
            std::cout<<"fulltime: "<<fullTimeDays<<std::endl;
            std::cout<<"reduced: "<<reducedTimeDays<<std::endl;
            std::cout<<"total: "<<totalVacation<<std::endl;
            std::cout<<"Percentage fte: "<<ftePercentage<<std::endl;

            FTEflag=false;  //turn off FTE submenu

            menuDelay();
            mainMenuflag=true;

            
            



        }

    }

    


}
//**************************************************************************************** */
//              ‍⁡⁣⁢⁣𝗰𝗿𝗲𝗮𝘁𝗲 𝗮 𝗺𝗲𝗻𝘂 𝗽𝗼𝗽𝘂𝗹𝗮𝘁𝗲𝗱 𝘄𝗶𝘁𝗵 𝗳𝗶𝗹𝗲𝘀 𝗶𝗻 𝗱𝗶𝗿𝗲𝗰𝘁𝗼𝗿𝘆⁡

std::string Gridmaster::chooseLoadFile(void)    //return the string of selected filename
{
    std::string tempfile;
    std::vector<std::string> files;
    

    std::string path = "./SavedCalendars"; // Change this to your directory
    int index=0;
    int choice=0;

    for (const auto& entry : fs::directory_iterator(path)) 
    {
        if (entry.is_regular_file()) //only include valid files
        {
            if(entry.path().filename().string().find("txt")!=std::string::npos)
                    files.emplace_back(entry.path().filename().string());
        }
    
    }

   

    loadMenuflag=true;
    mainMenuflag=false;

    std::sort(files.begin(),files.end());  //sort the files
    choice=userMenu.displayMenu(files,{3150,1600},50);

    if(choice>0)    //if display returns a menu choice then end the submenu
    {
        mainMenuflag=true;
        loadMenuflag=false;
        std::string selected=path+"/"+files[choice-1];  //formats to "./SavedCalendars/filename.txt"

        std::cout<<"Loading "<<selected<<std::endl;
        
        return(selected); //return the filename to load formatted with path.

    }
    


    return "NONE";  //no choice made
}
//**************************************************************************************** */

//          ‍⁡⁣⁢⁣Loads from a saved Calendar...need to scan day totals⁡

int Gridmaster::loadCalendarfromFile(std::string filename)
{
    std::cout << "Starting loadCalendarfromFile" << std::endl;

    std::ifstream inputfile(filename);

    if(!inputfile.is_open())
    {
        std::cout<<"*** Unable to open "<<filename<<std::endl;
        return 0;
    }
    //Bring in the header
    inputfile>>ftePercentage;
    inputfile>>fulltimeallotment;
    inputfile>>reducedtimeallotment;

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
    totalVacation=fulltimeallotment+reducedtimeallotment;

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
//********************************************************************** */
void Gridmaster::menuDelay() //pass the flag to wait on
{
    menuTimerFlag=true; //  assure revisits out of menu server
    IsMouseButtonPressed(MOUSE_LEFT_BUTTON); //flush mouse
    menutimer+=GetFrameTime();
    if(menutimer>0.50)
    {
        
        menuTimerFlag=false;
        menutimer=0;        //reset timer and flag. Start main menu again

        
    }


    return;

}

//*************************************************************** */

//              Re-intialize dayGrid using placeholder
void Gridmaster::reInitializeGrid()
{
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

    //⁡⁣⁢⁣‍ℂℝ𝔼𝔸𝕋𝔼 𝕋ℍ𝔼 𝔻𝔸𝕐𝔾ℝ𝕀𝔻 𝕍𝔼ℂ𝕋𝕆ℝ 𝕀ℕ𝕀𝕋𝕀𝔸𝕃𝕀ℤ𝔼𝔻 𝕎𝕀𝕋ℍ 𝔻𝔼𝔽𝔸𝕌𝕃𝕋⁡
    dayGrid.clear();    //had to delete the items in the vector and rebuild
    dayGrid.resize(600, placeholder);  //this may have fixed the stack slamming exception

return;
}  
//***************************************************************** */
void Gridmaster::SetPercentFTE(void)
// Retally the days according to the current calendar allotment.
{
    //prior to adjustment set allotments
    fulltimeallotment=fullTimeDays;
    reducedtimeallotment=reducedTimeDays;
    initialVacation=fulltimeallotment+reducedtimeallotment;
    

    for( auto& day: dayGrid)
    {
        if(day.designation==1)
            fullTimeDays-=day.value;
        if(day.designation==2)
            reducedTimeDays-=day.value;


    }
    totalVacation=fullTimeDays+reducedTimeDays;

    return;
}
//******************************************************************* */
//          Dashboard Graphic Display Allotments
void Gridmaster::dashBoard(void)
{
  //  Hinterval*4+Hinterval/2,1350,100
  initialVacation=fulltimeallotment+reducedtimeallotment;
  Vector2 textpos{Hinterval*4+45,1470};
    DrawRectangle(Hinterval*4+3,1465,Hinterval-8,90,Color{0,0,20,30});

    char buffer[100];  // adjust size as needed
    snprintf(buffer, sizeof(buffer), "         percent FTE: %.0f%%  PTO days: %.2f\nfulltime days: %.2f  reduced time: %.2f", 
         ftePercentage*100,initialVacation,fulltimeallotment,reducedtimeallotment);

    std::string message = buffer;

    DrawTextEx(dot,buffer,textpos,40,0,BLACK);



}
//******************************************************************* */

void Gridmaster::eventTimer()
{
    if (!eventTimerFlag)    //if no delay timer is going return
        return;

    stopwatch+=GetFrameTime();  //otherwise increment counter

    if (stopwatch<0.4)     //check for the end and reset stopwatch if needed
        return;
    
        else
        {
            stopwatch=0;
            eventTimerFlag=false;
        }

    return;
}