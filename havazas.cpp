#include "graphics.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

using namespace genv;

const int SCREEN_X = 640;
const int SCREEN_Y = 480;

struct Hopihe
{
    unsigned int x, y;

    Hopihe(unsigned int x, unsigned int y)
    {
        this->x = x;
        this->y = y;
    }

    Hopihe(unsigned int x)
    {
        this->x = x;
        this->y = 0;
    }

    void move(int x, int y)
    {
        this->x += x;
        this->y += y;

        if (this->x > SCREEN_X)
        {
            this->x = SCREEN_X - this->x;
        }

        if (this->y > SCREEN_Y)
        {
            this->y = SCREEN_Y - this->y;
        }
    }

    void render()
    {
        gout << move_to(x, y) << dot;
    }
};

void clear_screen()
{
    gout << color(0, 0, 0)
         << move_to(0, 0)
         << box_to(SCREEN_X - 1, SCREEN_Y - 1);
}


int main()
{
    std::srand(std::time(nullptr));

    gout.open(SCREEN_X, SCREEN_Y);

    std::vector<Hopihe> v;
    for (size_t i = 0; i < 600; i++)
    {
        v.emplace_back(std::rand() % SCREEN_X, std::rand() % SCREEN_Y);
    }

    gin.timer(30);

    int x_mod = 0;

    event ev;
    while (gin >> ev)
    {
        if (ev.type == ev_mouse)
        {
            x_mod = (SCREEN_X / 2 - ev.pos_x) * -1;
            x_mod = (x_mod * 10) / (SCREEN_X / 2);
            x_mod = (x_mod / 3);

            // std::cout << x_mod << '\n';
        }

        if (ev.type == ev_timer)
        {
            clear_screen();
            gout << color(255, 255, 255);

            for (Hopihe& h : v)
            {
                int rand_x = (std::rand() % 4);
                int rand_y = (std::rand() % 4);
                
                if (std::rand() % 2)
                {
                    rand_x *= -1;
                }

                h.move(rand_x + x_mod, rand_y);
                h.render();
            }

            gout << refresh;
        }
    }
    
    return 0;
}
