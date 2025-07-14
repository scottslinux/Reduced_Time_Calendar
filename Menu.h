#pragma once

#include <iostream>
#include <raylib.h>
#include <vector>
#include <string>



class Menu
{
    private:

    static Font digital7;
    static Font Bold;
    static Sound menuchange;
    static Sound Chosen;



    std::vector<Rectangle> menurecs;






    public:
        Color activegreen{10,255,100,255};
        Color activegreen2{100,255,162,255};
        Color BlueHighlight{0,255,255,255};

        bool soundflag=false;
        size_t soundindexer=99; //helping to play sound only once per event
        int hovered=99;
        int fontsize=80;






        Menu();
        int displayMenu(std::vector<std::string>&, Vector2,int size);
        int displayMenu(std::string title,std::vector<std::string>&, Vector2,int size);

        int mousewatch(); //check mouse contact

        void unloadresources();





};
