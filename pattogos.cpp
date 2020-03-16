#include "graphics.hpp"
#include <vector>
#include <cmath>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>

using namespace genv;
using namespace std;

const int SCREEN_X = 400;
const int SCREEN_Y = 700;

float g = 9.81f;

template<typename T>
int clean_vector(std::vector<T*>& v, size_t lbound = -1)
{
    size_t ubound = v.size();
    if (lbound == -1)
    {
        lbound = 0;
    }
    for (size_t i = lbound; i < v.size(); i++)
    {
        if (v[i] == nullptr)
        {
            for (size_t j = ubound - 1; j > i; j--)
            {
                if (v[j] != nullptr)
                {
                    ubound = j;
                    v[i] = v[j];
                    v[j] = nullptr;
                    break;
                }
            }
        }
        if (i >= ubound) break;
    }

    size_t remove = 0;
    for (size_t b = v.size() - 1; b >= 0; b--)
    {
        if (v[b] != nullptr) break;
        remove++;
    }
    v.resize(v.size() - remove);
    return remove;
}

struct Labda
{
private:
    float v = 0;
    bool esik_e = true;
    float rugalmassag = 0.5f;
    color c = color(255, 255, 255);

    int x, y, r;
    canvas k;

    bool simulating = false;

public:
    bool show_stats = true;

    Labda(int x, int y, int r, float rugalmassag)
        : x(x), y(y), r(r), k(2*r, 2*r), rugalmassag(0)
    {
        mod_rugalmassag(rugalmassag);
    }

    void rajz()
    {
        gout << stamp(k, x - r, y - r);

        if (y+r < 0)
        {
            gout << color(255, 255, 255);
            for (int i = 0; i < 7; i++)
            {
                gout << move_to(x - i/1.5, i) << dot;
                gout << move_to(x + i/1.5, i) << dot;
            }
            gout << move_to(x - 7, 20) << text(std::to_string(-y));
        }

        int text_x = x+r+10;
        if (text_x + 80 > SCREEN_X) text_x = x-r-90;
        if (show_stats)
        {
            gout << color(255, 255, 255);

            std::stringstream ss;
            ss << v;
            gout << move_to(text_x, y + 15) << text(" v0 = " + ss.str());

            ss.str(std::string());
            ss << std::fixed << std::setprecision(2) << rugalmassag;
            gout << move_to(text_x, y - 5) << text("rug = " + ss.str());
        }

        if (!simulating)
        {
            gout << color(255, 255, 255);
            if (v > 0)
            {
                gout << move_to(x, y + r) << line(0, v*5);
                for (int i = 0; i < 7; i++)
                {
                    gout << move_to(x - i/1.5, y + r + 5*v - i) << dot;
                    gout << move_to(x + i/1.5, y + r + 5*v - i) << dot;
                }
            }
            else if (v < 0)
            {
                gout << move_to(x, y - r) << line(0, v*5);
                for (int i = 0; i < 7; i++)
                {
                    gout << move_to(x - i/1.5, y - r + 5*v + i) << dot;
                    gout << move_to(x + i/1.5, y - r + 5*v + i) << dot;
                }
            }
        }
    }

    void mod_rugalmassag(float rug)
    {
        rugalmassag += rug;
        rugalmassag = std::max(std::min(rugalmassag, 1.0f), 0.0f);
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

    void mod_v0(float v0)
    {
        v += v0;
        v = std::max(std::min(v, 25.0f), -25.0f);
    }

    void mozgat(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void szimulacio()
    {
        k.transparent(true);
        show_stats = false;
        simulating = true;

        v += 0.01 * g;
        y += v;

        if (y+r >= SCREEN_Y - 20 && esik_e)
        {
            esik_e = false;
            v = -rugalmassag * v + rugalmassag;
        }

        if (v >= 0 && !esik_e)
        {
            esik_e = true;
        }
    }

    bool off_screen()
    {
        return y + r < 0 || y - r > SCREEN_Y; 
    }

    bool is_mouse_over(int m_x, int m_y)
    {
        return x-r <= m_x && x+r >= m_x &&
               y-r <= m_y && y+r >= m_y;
    }

    void essen_le()
    {
        show_stats = false;
        simulating = true;
        
        v += 0.01 * g;
        y += v;
    }
};

enum Status
{
    kezdkep = 0,
    szimu = 1
};

int main()
{
    gout.open(SCREEN_X, SCREEN_Y);

    vector<Labda*> labdak;
    Labda* legujabb = new Labda(0, 0, 10, 0.5);
    Labda* utolso = nullptr;

    Status stat=kezdkep;
    gin.timer(10);
    event ev;

    int m_x, m_y;
    bool fut = false;

    bool flush = false;
    int flushing = -1;

    int key_buf = 0;
    int key_limiter = 0;
    while(gin >> ev && ev.keycode!=key_escape)
    {
        if (stat == kezdkep)
        {
            gout << color(255,255,255);
            gout << move_to(30,50) << text("inditas - BACKSPACE") 
                 << move_to(30,75) << text("labda rogzitese - LEFT CLICK") 
                 << move_to(30,100) << text("uj labda - SPACE")
                 << move_to(30,125) << text("rugalmassagi tenyezo valt. - UP, DOWN ARROW") 
                 << move_to(30,150) << text("gravitacios ero valt. - RIGHT, LEFT ARROW") 
                 << move_to(30,175) << text("kezdeti sebesseg valt. - PG UP/PG DOWN") 
                 << move_to(30,200) << text("reset - R")
                 << move_to(30,225) << text("szimulacio start - ENTER") 
                 << refresh;

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
                switch (std::abs(ev.keycode))
                {
                case key_up:
                case key_down:
                case key_right:
                case key_left:
                case key_pgup:
                case key_pgdn:
                    key_buf += ev.keycode;
                    break;

                default:
                    break;
                }

                if (ev.keycode == 114)
                {
                    fut = false;
                    flush = true;
                }
                if (ev.keycode == key_space && utolso)
                {
                    legujabb = new Labda(0, 0, 10, 0.5);

                    utolso->show_stats = false;
                    utolso = nullptr;
                }
                if (ev.keycode == key_enter)
                {
                    fut = true;
                    legujabb = nullptr;
                }                
            }

            if (ev.type == ev_timer)
            {
                key_limiter++;
                if (key_limiter % 11 == 0)
                {
                    if (utolso)     
                    {
                        switch (key_buf)
                        {
                        case key_up:   utolso->mod_rugalmassag(+0.05); break;
                        case key_down: utolso->mod_rugalmassag(-0.05); break;
                        case key_pgup: utolso->mod_v0(-1); break;
                        case key_pgdn: utolso->mod_v0(+1); break;
                        default: break;
                        }
                    }

                    if (legujabb)
                    {
                        switch (key_buf)
                        {
                        case key_up:   legujabb->mod_rugalmassag(+0.05); break;
                        case key_down: legujabb->mod_rugalmassag(-0.05); break;
                        case key_pgup: legujabb->mod_v0(-1); break;
                        case key_pgdn: legujabb->mod_v0(+1); break;
                        default: break;
                        }
                    }

                    if (key_buf == key_left)  g += 0.5;
                    if (key_buf == key_right) g -= 0.5;
                }

                gout << color(0, 0, 0) << move_to(0, 0) << box(SCREEN_X, SCREEN_Y);
                if (legujabb != nullptr)
                {
                    legujabb->mozgat(m_x, m_y);
                    legujabb->rajz();
                }

                // for (Labda*& l : labdak)
                for (auto it = labdak.rbegin(); it != labdak.rend(); it++)
                {
                    Labda*& l = *it;
                    if (l != utolso)
                    {
                        l->show_stats = false;
                    }
                    if (!fut && l->is_mouse_over(m_x, m_y))
                    {
                        l->show_stats = true;
                    }
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
                    gout << color(255, 150, 0) << move_to(0, SCREEN_Y - 20) << line(SCREEN_X, 0);
                    gout << color(255, 150, 0) << move_to(0, SCREEN_Y - 19) << box(SCREEN_X, 19);
                }
                else if (flushing == -1)
                {
                    int lbound = -1;
                    for (int i = 0; i < labdak.size(); i++)
                    {
                        if (labdak[i]->off_screen())
                        {
                            if (lbound == -1) lbound = i;

                            delete labdak[i];
                            labdak[i] = nullptr;
                        }
                    }
                    if (lbound > -1)
                    {
                        clean_vector(labdak, lbound);
                    }

                    if (labdak.size() == 0)
                    {
                        flushing = 100;
                    }
                }

                // std::cout << flushing << '\n';
                if (flushing > 0)
                {
                    flushing--;
                }
                else if (flushing == 0)
                {
                    flushing = -1;
                    flush = false;
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
