#include "graphics.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <sstream>
#include <time.h>

using namespace std;
using namespace genv;

const int szelesseg = 600, magassag = 400;

struct block
{
private:
    int x, y, szel, mag, dx, dy;
    bool kijelolt = false;

public:
    bool valtoztam = false;

    block()
        : szel(rand()% 30 + 20),
          mag(rand()% 30 + 20)
    {
        x = (rand()%(szelesseg - szel));
        y = (rand()%(magassag - mag));
    }

    void draw()
    {
        int keret = 2;
        if(kijelolt)
        {
            keret = 4;
        }
        // gout << color(200, 0, 0) << move_to(x, y) << box(szel, mag)
        //      << color(255, 255, 40) << move_to(x, y) << box(szel, keret)
        //      << box(-keret, mag-keret+1) << box(-szel+keret-1, -keret) 
        //      << box(keret, -mag+keret) << refresh;

        gout << color(255, 255, 40) << move_to(x, y) << box(szel, mag);
        gout << color(200, 0, 0) << move_to(x+keret, y+keret) << box(szel-2*keret, mag-2*keret);
        valtoztam = false;
        // gout << refresh;
    }

    void mozgat(int x, int y)
    {
        this->x = x - szel/2;
        this->y = y - mag/2;
        valtoztam = true;
        // draw();
    }

    // ez annyira nem kell am
    // void elkap(int ex, int ey)
    // {
    //     // dx = ex-x;
    //     // dy = ey-y;
    //     x += ex - (x + szel/2);
    //     y += ey - (y + mag/2);
    // }

    void kijelol()   //Setter
    {
        kijelolt = true;
        valtoztam = true;
    }
    void elenged()   
    {
        kijelolt = false;
        valtoztam = true;
    }
    bool kijelolte() //Getter
    {
        return kijelolt;
    }

    bool folotte(int ex, int ey)
    {
        return (x < ex and ex < x + szel and
                y < ey and ey < y + mag);
    }
};

void torol()
{
    gout << color(0, 0, 0) << move_to(0, 0) << box(szelesseg, magassag);
}

int main()
{
    srand(time(0));
    gout.open(600, 400);

    vector<block*> blocks;
    block *elkapott = nullptr;
//    int elkapott;

    for(size_t i = 0; i<10; i++)
    {
        blocks.push_back(new block());
    }

    for(block *egyik: blocks)
    {
        egyik->draw();
    }

    bool lenyomva = false;

    gout << refresh;
    // gin.timer(20);

    event ev;
    while(gin >> ev)
    {
        if(ev.type == ev_mouse)
        {
            if(ev.button == btn_left)
            {
                lenyomva = true;
                elkapott = nullptr;
                for(block *egyik : blocks)
                {
                    egyik->elenged();
                    if(egyik->folotte(ev.pos_x, ev.pos_y))
                    {
                        egyik->kijelol();
                        // egyik->elkap(ev.pos_x, ev.pos_y);
                        elkapott = egyik;
                    }
                }
            }
            else if(ev.button == -btn_left)
            {
                lenyomva = false;
            }
            else if(ev.button == 0 and lenyomva)
            {
                if (elkapott != nullptr)
                {
                    elkapott->mozgat(ev.pos_x, ev.pos_y);
                }
                // for(block *egyik: blocks)
                // {
                //     if(egyik->kijelolte())
                //     {
                //         egyik->mozgat(ev.pos_x, ev.pos_y);
                //         break;
                //     }
                // }                
            }

            bool kell_frissiteni = false;
            for (block *b : blocks)
            {
                if (b->valtoztam)
                {
                    kell_frissiteni = true;
                }
            }

            if (kell_frissiteni)
            {
                torol();
                for (block *b : blocks)
                {
                    b->draw();
                }
                gout << refresh;
            }
        }
    }

    return 0;
}