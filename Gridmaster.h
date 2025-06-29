#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"
#include "Calendar.h"


// Structure of each grid data box
struct gridData
{
    Rectangle dayRect;  // coordinates of rectangle for each box
    bool activeBox;     // is the grid box a blank(false)
    int dayValue;       // day of month
    int dayofweek;
    int month;
    int year;
                           



};
//*****************************************************************************/
//⁡⁣⁢⁣​‌‍‌𝗠𝗮𝗶𝗻 𝗖𝗹𝗮𝘀𝘀 𝘁𝗼 𝗰𝗿𝗲𝗮𝘁𝗲 𝘁𝗵𝗲 𝗚𝗿𝗶𝗱 𝗮𝗻𝗱 𝗠𝗼𝗻𝗶𝘁𝗼𝗿 𝗺𝗼𝘂𝘀𝗲 𝗺𝗼𝘃𝗲𝗺𝗲𝗻𝘁​⁡
class Gridmaster
{

    private:


    std::vector<gridData> dayGrid;  //the master list of all grid squares..active or not
    std::vector<std::string> weekdaynames;
    std::vector<std::string> monthNames;
    static Font monthfont;  //staying with a static just because
    static Font dayfont;


    //  Monitor specific information 
    int monWidth; 
    int monHeight;
    int Hinterval;
    int Vinterval;
    int boxCounter;   //⁡⁣⁣⁢total number of boxes on grid. 7days*5 weeks * 12 months= 420⁡
    Vector2 monthPosxy; //used to specify position on the grid of a particular month



    public:
//  ⁡⁣⁣⁢𝘋𝘦𝘤𝘭𝘢𝘳𝘦 𝘢𝘭𝘭 𝘰𝘧 𝘵𝘩𝘦 𝘧𝘶𝘯𝘤𝘵𝘪𝘰𝘯𝘴 𝘧𝘰𝘳 𝘵𝘩𝘦 𝘤𝘭𝘢𝘴𝘴...𝘳𝘦𝘮𝘦𝘮𝘣𝘦𝘳⁡

    Gridmaster();   //You need to declare all functions..even the ⁡⁢⁣⁣𝗰𝗼𝗻𝘀𝘁𝗿𝘂𝗰𝘁𝗼𝗿
    ~Gridmaster();
    void DrawGrid();    //may enhance this definition later
    void DrawdayGrid(int month); //routine to draw monthly calander grid
    void SenseMouse(Rectangle); //return the rectangle that the mouse hovered/clicked on
    Vector2 FindMonthxy(int month); //function to determine grid position for a given month-returns Vec2d x,y
    void MouseTrap(void); //mouse tracking routine
    int MouseCollision(Vector2); //check the mouse against the grid vector. Return the # of contact rectangle
    void MergeGridwithCalendar(Calendar*); //Merge the calendar of year(int) with the grid
};

