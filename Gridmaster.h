#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "raylib.h"
#include "Calendar.h"
#include "Menu.h"


// Structure of each grid data box
struct gridData
{
    Rectangle dayRect;  // coordinates of rectangle for each box
    bool activeBox;     // Registered Vacation Day with Valley ?
    bool blackout;      //is this a blackout date?
    int dayValue;       // day of month should be day
    int dayofweek;
    int month;
    int year; 
    int designation;    // 1 fulltime 2 reduced
    float value;       //unused  
                           



};

//*****************************************************************************/
//⁡⁣⁢⁣​‌‍‌𝗠𝗮𝗶𝗻 𝗖𝗹𝗮𝘀𝘀 𝘁𝗼 𝗰𝗿𝗲𝗮𝘁𝗲 𝘁𝗵𝗲 𝗚𝗿𝗶𝗱 𝗮𝗻𝗱 𝗠𝗼𝗻𝗶𝘁𝗼𝗿 𝗺𝗼𝘂𝘀𝗲 𝗺𝗼𝘃𝗲𝗺𝗲𝗻𝘁​⁡
class Gridmaster
{

    private:


    std::vector<gridData> dayGrid;  //the master list of all grid squares..active or not

    std::vector<std::string> weekdaynames{"SU","MO","TU","WE","TH","FR","SA"};
    std::vector<std::string> monthNames{"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY",
    "AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};

    static Font monthfont;  //staying with a static just because
    static Font dayfont;
    static Font marker;

    float fulltimeallotment=45.0;
    float reducedtimeallotment=46.0;

    float totalVacation=91.0;
    float fullTimeDays=40.0;
    float reducedTimeDays=51.0;
    float initialVacation=totalVacation;

    std::string activeFileName="";
    int activeYear=2026;            //the default year for the calendar..functions to hold the year for the entire program
    int desiredyear=activeYear;     //changeable in create new calendar. Will reset both in merge

    bool mainMenuflag=true; //file menu choice display flag -starts in on position.  False is off/ True is displayed
    bool replaceMenuflag=false; //file exists menu
    bool loadMenuflag=false; //choosing which file to load
    bool loadgraphflag=false; //flag to keep loading graph alive
    bool createCalflag=false;

    float graphtimer=0;
    float menutimer=0;
    bool menuTimerFlag=false;
    
    
    int colorindex=0;  //keep track of current circle color
    

    Color paletteColor[2]={{0,121,241,200},{44,104,34,200}};
    //                        0  blue          1 green



    //  Monitor specific information needs to execute and define in constructor
    int monWidth;
    int monHeight;
    int Hinterval;
    int Vinterval;


    int boxCounter=0;   //⁡⁣⁣⁢total number of boxes on grid. 7days*5 weeks * 12 months= 420⁡
    Vector2 monthPosxy{0,0}; //used to specify position on the grid of a particular month
    Menu userMenu;



    public:
//  ⁡⁣⁣⁢𝘋𝘦𝘤𝘭𝘢𝘳𝘦 𝘢𝘭𝘭 𝘰𝘧 𝘵𝘩𝘦 𝘧𝘶𝘯𝘤𝘵𝘪𝘰𝘯𝘴 𝘧𝘰𝘳 𝘵𝘩𝘦 𝘤𝘭𝘢𝘴𝘴...𝘳𝘦𝘮𝘦𝘮𝘣𝘦𝘳⁡

    Gridmaster();   //You need to declare all functions..even the ⁡⁢⁣⁣𝗰𝗼𝗻𝘀𝘁𝗿𝘂𝗰𝘁𝗼𝗿
    ~Gridmaster();
    int DrawGrid();    //return desired new year
    void DrawdayGrid(int month); //routine to draw monthly calander grid
    void Scoreboard(void); //tally the vacation days and display on the right
    void SenseMouse(Rectangle); //return the rectangle that the mouse hovered/clicked on
    Vector2 FindMonthxy(int month); //function to determine grid position for a given month-returns Vec2d x,y
    void MouseTrap(void); //mouse tracking routine
    int MouseCollision(Vector2); //check the mouse against the grid vector. Return the # of contact rectangle
    void MergeGridwithCalendar(Calendar*); //Merge the calendar of year(int) with the grid
    void mouseClickChoices(int, Vector2); //evaluate mouse clicks, make day changes, change paint brush(red v fulltime)
    void adjustTotals(int designation,float val);//call routine to adjust full/reduced/total
    void menuchecking(Vector2 mousepos); //check the hover and menu choices
    void SaveCalendarToFile(const std::string& filename, const std::vector<gridData>& calendar);
    int loadCalendarfromFile(std::string filename); //most important..overwrite existing calendar with saved
    void menuserver(void); //all menu functions in one convenient sexy place
    std::string chooseLoadFile(void); //create a menu with the files/filtered in the directory
    void menuDelay(void); //  activates the main menu after a brief delay
    void reInitializeGrid(void);  //clear out the grid when creating a new year

};

