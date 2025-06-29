#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>



struct CalendarData     //keeping it simple for now
{
    int month;
    int day;
    int dayofWeek;  //0..Sat,1..Sun.......6.Fri
    int year;
};

//============================================================================== */
class Calendar
{
    private:
    


    


    public:
    std::vector<CalendarData> DAY;
    
    std::vector<std::string> monthnames;
    std::vector<std::string> daynames;
    std::vector<int> month_limits;
    
    
    bool leapyear=false;

    Calendar();  //constructor
    int DayfinderJan1(int day,int month,int year);        //execute Zeller's Congruence-return 0-6 (sat-fri)
    int CreateCalendar(int year);   //function to create a calendar for a given year
    bool leapcheck(int year);   //return true only for leap years
    void loadCalendar(int startingyear,int duration);   //load it up 
    void CalendarGrid(Calendar &myCal);
    void MultiCalendarGrid(Calendar &myCal);
    
    };
    