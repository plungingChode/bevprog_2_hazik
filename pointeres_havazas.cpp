#include "graphics.hpp"
#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>

using namespace genv;

static const int SIZE_X = 400;
static const int SIZE_Y = 400;
static const color WHITE = color(255, 255, 255);

struct Ho
{
    int x, y;
    int x_irany, y_irany;
    Ho* masik;
    color c;

    Ho(int x, int y, color c) 
        : x(x), y(y), c(c), 
          x_irany(0), y_irany(0)
    {
    }

    void move()
    {
        // ok...
        x_irany = masik->x - this->x + x_irany * 0.55;
        y_irany = masik->y - this->y + y_irany * 0.55;

        this->x += x_irany * 0.025;
        this->y += y_irany * 0.025;
    }

    void render(canvas& c) const
    {
        c << move_to(x - 2, y - 2) << this->c << box(5, 5);
        // c << move_to(x, y) << line_to(x + x_irany, y + y_irany);
    }

    static Ho* get_random()
    {
        unsigned char r = 100 + std::rand() % 156;
        unsigned char g = 100 + std::rand() % 156;
        unsigned char b = 100 + std::rand() % 156;

        return new Ho(
            std::rand() % SIZE_X, 
            std::rand() % SIZE_Y,
            color(r, g, b));
    }
};

void restart(std::vector<Ho*>& ho)
{
    for (int i = 0; i < ho.size(); i++)
    {
        delete ho[i];
        ho[i] = nullptr;
    }

    ho.resize(3);

    ho[0] = Ho::get_random();
    for (int i = 1; i < ho.size(); i++)
    {
        ho[i] = Ho::get_random();
        ho[i - 1]->masik = ho[i];
    }
    ho[ho.size() - 1]->masik = ho[0];
}

int main()
{
    time_t seed = time(nullptr);
    std::srand(seed);
    std::cout << seed << '\n';

    canvas ures(SIZE_X, SIZE_Y);
    gout.open(SIZE_X, SIZE_Y);

    std::vector<Ho*> ho;
    restart(ho);

    int refresh_rate = 40;
    bool paused = false;

    gin.timer(refresh_rate);

    event ev;
    while(gin >> ev)
    {
        if (ev.type == ev_key)
        {
            if (ev.keycode == key_space)
            {
                if (!paused)
                {
                    gin.timer(-1);
                    paused = true;
                }
                else
                {
                    gin.timer(refresh_rate);
                    paused = false;
                }
            }

            if (ev.keycode == key_enter)
            {
                restart(ho);
            }

            if (ev.keycode == key_up)
            {
                Ho* utolso = Ho::get_random();
                ho.push_back(utolso);

                ho[ho.size() - 2]->masik = utolso;
                utolso->masik = ho[0];
            }
        }
        
        if (ev.type == ev_timer)
        {
            gout << stamp(ures, 0, 0);
            for (Ho *h : ho)
            {
                h->move();
                h->render(gout);
            }
            gout << refresh;
        }
    }

    return 0;
}