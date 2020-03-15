#include "graphics.hpp"
#include <vector>
#include <cmath>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>

using namespace genv;
using namespace std;

float g = 9.81F;

struct Labda
{
private:
    float v = 0;
    bool esik_e = true;
    float rugalmassag = 0.5;
    color c = color(255, 255, 255);

    int x, y, r;
    canvas k;

    bool modosul = true;

public:
    // Labda(int x, int y, int r, float rugalmassag, float g)
    Labda(int x, int y, int r, float rugalmassag)
        : x(x), y(y), r(r), k(2*r, 2*r), rugalmassag(0)
    {
        mod_rugalmassag(rugalmassag);
        // mod_grav(g);
    }

    void rajz()
    {
        gout << stamp(k, x - r, y - r);

        if (modosul)
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << rugalmassag;
            gout << color(255, 255, 255);
            gout << move_to(x + r + 10, y) << text("rug: " + ss.str());
        }
    }

    void mod_rugalmassag(float rug)
    {
        rugalmassag += rug;
        if (rugalmassag > 1) rugalmassag = 1;
        if (rugalmassag < 0) rugalmassag = 0;
        c = color(255, 255 - 255*rugalmassag, 255 - 255*rugalmassag);
        for (int x = -r; x < r; x++)
        {
            for (int y = -r; y < r; y++)
            {
                if (x*x + y*y <= r*r)
                {
                    k << c << move_to(x+r, y+r) << dot;
                }
            }
        }
    }

    // void mod_grav(float gr)
    // {
    //     g += gr;
    //     if (g < 0) g=0.01;
    //     c = color(255 - 255*g, 255, 255 - 255*g);
    //     for (int x = -r; x < r; x++)
    //     {
    //         for (int y = -r; y < r; y++)
    //         {
    //             if (x*x + y*y <= r*r)
    //             {
    //                 k << c << move_to(x+r, y+r) << dot;
    //             }
    //         }
    //     }
    // }

    void mozgat(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void szimulacio()
    {
        modosul = false;

        v += 0.01 * g;
        y += v;

        if (y+r >= 380 && esik_e)
        {
            esik_e = false;
            v = -rugalmassag * v + rugalmassag;
        }

        if (v >= 0 && !esik_e)
        {
            esik_e = true;
        }
    }

    void essen_le()
    {
        modosul = false;
        
        v += 0.01 * g;
        y += v;
    }
};

enum Status
{
    kezdkep = 0,
    szimu = 1,
};

int main()
{
    gout.open(400,400);

    vector<Labda*> labdak;
    // Labda* legujabb = new Labda(0, 0, 10, 0, 0);
    Labda* legujabb = new Labda(0, 0, 10, 0.5);
    Labda* utolso = nullptr;

    Status stat=kezdkep;
    gin.timer(10);
    event ev;

    int m_x, m_y;
    bool fut = false;

    bool flush = false;
    int flushing = 100;

    while(gin >> ev && ev.keycode!=key_escape)
    {
        if (stat == kezdkep)
        {
            gout << color(255,255,255) << move_to(125,50) << text("inditas - BACKSPACE") << move_to(80,100)
                 << text("labda rogzitese - LEFT CLICK") << move_to(130,150) << text("uj labda - SPACE")
                 << move_to(30,200) << text("rugalmassagi tenyezo valt. - UP, DOWN ARROW") << move_to(35,250)
                 << text("gravitacios ero valt. - RIGHT, LEFT ARROW") << move_to(160,300) << text("reset - R")
                 << move_to(100,350) << text("szimulacio start - ENTER") << refresh;

            if (ev.type==ev_key && ev.keycode==key_backspace)
            {
                stat=szimu;
            }
        }

        if (stat == szimu)
        {

            if (ev.type == ev_mouse)
            {
                m_x = ev.pos_x;
                m_y = ev.pos_y;

                if (ev.button == btn_left && legujabb)
                {
                    labdak.push_back(legujabb);
                    utolso = legujabb;
                    legujabb = nullptr;
                }
            }

            if (ev.type == ev_key)
            {
                if (ev.keycode == key_up && utolso)
                {
                    utolso->mod_rugalmassag(+0.05);
                }
                if (ev.keycode == key_down && utolso)
                {
                    utolso->mod_rugalmassag(-0.05);
                }
                // if (ev.keycode == key_right && utolso)
                if (ev.keycode == key_right)
                {
                    // utolso->mod_grav(+0.05);
                    g += 0.05;
                }
                // if (ev.keycode == key_left  && utolso)
                if (ev.keycode == key_left)
                {
                    g -= 0.05;
                    // utolso->mod_grav(-0.05);
                }
                if (ev.keycode == 114)
                {
                    fut = false;
                    flush = true;
                    flushing = 100;
                }
                if (ev.keycode == key_space)
                {
                    utolso = nullptr;
                    // legujabb = new Labda(0, 0, 10, 0, 0);
                    legujabb = new Labda(0, 0, 10, 0.5);
                }
                if (ev.keycode == key_enter)
                {
                    legujabb = nullptr;
                    fut = true;
                }
            }

            if (ev.type == ev_timer)
            {
                gout << color(0, 0, 0) << move_to(0, 0) << box(400, 400);
                if (legujabb != nullptr)
                {
                    legujabb->mozgat(m_x, m_y);
                    legujabb->rajz();
                }

                for (Labda*& l : labdak)
                {
                    if (fut)
                    {
                        l->szimulacio();
                    }
                    if (flush)
                    {
                        l->essen_le();
                    }
                    l->rajz();
                }


                if (!flush)
                {
                    gout << color(255, 0, 0) << move_to(0, 380) << line(400, 0);
                }
                else if (flushing)
                {
                    flushing--;
                }
                else
                {
                    labdak.clear();
                    flush = false;
                    // legujabb = new Labda(0, 0, 10, 0, 0);
                    legujabb = new Labda(0, 0, 10, 0.5);
                }
            }

            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << g;
            gout << color(255, 255, 255);
            gout << move_to(20, 20) << text("gravitacio: " + ss.str());
            gout << refresh;
        }

    }
    return 0;
}
