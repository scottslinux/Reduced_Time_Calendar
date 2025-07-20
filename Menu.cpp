#include "Menu.h"
#include <iostream>
#include <raylib.h>
#include <vector>
#include <string>

Font Menu::digital7{0};
Font Menu::Bold{0};
Sound Menu::menuchange{0};
Sound Menu::Chosen{0};


Menu::Menu()
{
    digital7=LoadFontEx("./resources/liberation.ttf",100,0,0);
    Bold=LoadFontEx("./resources/liberationBold.ttf",100,0,0);
    menuchange=LoadSound("./resources/click.wav");
    Chosen=LoadSound("./resources/selected.wav");


    Rectangle placeholder{0,0,0,0};
    menurecs.resize(20,placeholder);
    



}
//********************************************************************* */
//  Accept vector list of menu items and vect2 position of box
//  need to establish rectangles for the choices

int Menu::displayMenu(std::vector<std::string>& menuitem, Vector2 xy, int size)
{
    int index=0;
    size_t rows=menuitem.size();
    fontsize=size; //adjust font according to user choice
    int clicked=0;  //used to return the last menu item chosen



    
    Vector2 maxchoicesize;
    Vector2 tempsize;

    maxchoicesize=MeasureTextEx(digital7,menuitem[0].c_str(),fontsize,0); //start with size of first item
                                                                //then compare looking for largest
    
    for(auto choice:menuitem)       //reads thru the list and finds the longest item to size box
        {           
            tempsize=MeasureTextEx(digital7,choice.c_str(),fontsize,0);
            if(tempsize.x>maxchoicesize.x)
                maxchoicesize=tempsize;

        }

    float menuwidth=maxchoicesize.x*1.25; //add some buffer around the items
    float rowheigt=maxchoicesize.y;

    float edge=0.25;
    float pad=edge*rowheigt;
    
    DrawRectangle(xy.x-(pad)-4,xy.y-(pad)-4,menuwidth+pad*2+8,rows*rowheigt+pad*2+8, //outer black edge
                    BLACK);
    DrawRectangle(xy.x-(pad),xy.y-(pad),menuwidth+pad*2,rows*rowheigt+pad*2, //gray Frame
                    Color{163,163,163,255});


    DrawRectangleV(xy,Vector2{menuwidth,rowheigt*rows},Color{12,29,12,255});


    
    Rectangle placeholder{0,0,0,0};
    menurecs.resize(rows,placeholder);  //resize the main vector to current size. avoid exceptions

    size_t rowindex=0;

    for(auto choice:menuitem)
    {   float center=xy.x+menuwidth/2-MeasureTextEx(digital7,choice.c_str(),fontsize,0).x/2;

        
        if(hovered==rowindex)
        {
            DrawTextEx(digital7,choice.c_str(),
                Vector2{center,xy.y+(rowheigt*rowindex)},fontsize,0,activegreen2);
        }
            else
            {
                DrawTextEx(digital7,choice.c_str(),
                            Vector2{center,xy.y+(rowheigt*rowindex)},fontsize,0,activegreen);
            }

        //store the collision rectangles
        menurecs[rowindex].height=rowheigt;
        menurecs[rowindex].width=menuwidth;
        menurecs[rowindex].x=xy.x;
        menurecs[rowindex].y=xy.y+rowindex*rowheigt;

        rowindex++;

    }

    hovered=Menu::mousewatch(); //return the row

    if(hovered<100)
    {
        //std::cout<<"hovering on "<<hovered+1<<std::endl;
        //std::cout<<"....................."<<std::endl;
    }

            else
                if(hovered<150)
                {
                    
                    clicked=hovered-100;
                }



    return clicked;
}
//********************************************************************* */
//  Polymorphism!!!! Version of the displaymenu method that also displays a title.
//  Accept vector list of menu items and vect2 position of box
//  need to establish rectangles for the choices

int Menu::displayMenu(std::string wintitle,std::vector<std::string>& menuitem, Vector2 xy, int size)
{
    int index=0;
    size_t rows=menuitem.size();
    fontsize=size; //adjust font according to user choice
    int clicked=0;  //used to return the last menu item chosen



    
    Vector2 maxchoicesize;
    Vector2 tempsize;

    maxchoicesize=MeasureTextEx(Bold,wintitle.c_str(),fontsize*1.25,0); //start with size of the title
                                                                //then compare looking for largest
    
    for(auto choice:menuitem)       //reads thru the list and finds the longest item to size box
        {           
            tempsize=MeasureTextEx(digital7,choice.c_str(),fontsize,0);
            if(tempsize.x>maxchoicesize.x)
                maxchoicesize=tempsize;

        }

    float menuwidth=maxchoicesize.x*1.25; //add some buffer around the items
    float rowheigt=maxchoicesize.y;

    float edge=0.25;
    float pad=edge*rowheigt;

    //Menu Title Section
    //titlebar frame

    

    DrawRectangle(xy.x-(pad)-4,xy.y-rowheigt*2-12,menuwidth+pad*2+8,rowheigt*2+24,BLACK);
    DrawRectangle(xy.x-(pad),xy.y-rowheigt*2-8,menuwidth+pad*2+2,rowheigt*2+16,Color{163,163,163,255});



    //titlebar Text Section
    float cntrx=xy.x+menuwidth/2-MeasureTextEx(Bold,wintitle.c_str(),fontsize,0).x/2;
    float cntry=(xy.y-(rowheigt*2)+MeasureTextEx(Bold,wintitle.c_str(),fontsize,0).y/2);

    DrawTextEx(Bold,wintitle.c_str(),{cntrx-3,cntry-3},fontsize,0,WHITE);

    DrawTextEx(Bold,wintitle.c_str(),{cntrx,cntry},fontsize,0,BLACK);


    //draw main menu rectangles
    DrawRectangle(xy.x-(pad)-4,xy.y-(pad)-4,menuwidth+pad*2+8,rows*rowheigt+pad*2+8, //outer black edge
                    BLACK);
    DrawRectangle(xy.x-(pad),xy.y-(pad),menuwidth+pad*2,rows*rowheigt+pad*2, //gray Frame
                    Color{163,163,163,255});
    DrawRectangleV(xy,Vector2{menuwidth,rowheigt*rows},Color{36,49,30,255});


    
    Rectangle placeholder{0,0,0,0};
    menurecs.resize(rows,placeholder);  //resize the main vector to current size. avoid exceptions

    size_t rowindex=0;

    for(auto choice:menuitem)
    {   float center=xy.x+menuwidth/2-MeasureTextEx(digital7,choice.c_str(),fontsize,0).x/2;

        
        if(hovered==rowindex)
        {
            DrawTextEx(digital7,choice.c_str(),
                Vector2{center,xy.y+(rowheigt*rowindex)},fontsize,0,activegreen);
        }
            else
            {
                DrawTextEx(digital7,choice.c_str(),
                            Vector2{center,xy.y+(rowheigt*rowindex)},fontsize,0,WHITE);
            }

        //store the collision rectangles
        menurecs[rowindex].height=rowheigt;
        menurecs[rowindex].width=menuwidth;
        menurecs[rowindex].x=xy.x;
        menurecs[rowindex].y=xy.y+rowindex*rowheigt;

        rowindex++;

    }

    hovered=Menu::mousewatch(); //return the row

    if(hovered<100)
    {
        //std::cout<<"hovering on "<<hovered+1<<std::endl;
        //std::cout<<"....................."<<std::endl;
    }

            else
                if(hovered<150)
                {
                    
                    clicked=hovered-100;
                }



    return clicked;
}
//*********************************************************************
//          Mouse Watch

int Menu::mousewatch()
{
    size_t i=0;

    for(Rectangle& rec: menurecs)
    {
        if(CheckCollisionPointRec(GetMousePosition(),rec))
        {
            DrawRectangleLinesEx(rec,4,activegreen);
            //DrawRectangle(rec.x,rec.y,rec.width,rec.height,BLACK);

            if(!IsSoundPlaying(menuchange)&&(soundindexer!=i))
                {
                    PlaySound(menuchange);
                    soundindexer=i;

                    //std::cout<<"soundflag: "<<soundflag<<std::endl;
                    //std::cout<<"i="<<i<<"\n";
                    

                }

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    //std::cout<<"Option "<<i+1<<" Selected"<<std::endl;
                    if(!IsSoundPlaying(Chosen))
                        {
                            PlaySound(Chosen);
                        }

                    return i+101;   //return the choice (add 101 to indicate clicked)

                }
            return i;   //return the row you are hovering on

        }

        i++;

    }

return 999; //signal that no valid box is hovering

}


//********************************************************************* */
void Menu::unloadresources(void)
{   
    UnloadFont(digital7);
    UnloadFont(Bold);
    UnloadSound(menuchange);
    UnloadSound(Chosen);

    CloseAudioDevice();

    return;
}
