#include "graphics.hpp"
#include <vector>
#include <fstream>
#include <iostream>

using namespace genv;

void read_img(std::ifstream& f, canvas& c, int& width, int& height)
{
    f >> width >> std::ws;
    f >> height >> std::ws;
    c.open(width, height); 

    int x, y, r, g, b;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            f >> r >> std::ws;
            f >> g >> std::ws;
            f >> b >> std::ws;

            c << move_to(x, y) 
              << color(r, g, b) 
              << dot;
        }
    }
}

void clear_screen(int width, int height)
{
    gout << color(0, 0, 0)
         << move_to(0, 0)
         << box_to(width - 1, height - 1);
}

int main()
{
    std::ifstream f("a.kep");

    int image_width, image_height;
    canvas c;

    read_img(f, c, image_width, image_height);
    gout.open(image_width, image_height);
    gout << refresh;

    int mid_x = image_width / 2;
    int mid_y = image_height / 2;

    event ev;
    while (gin >> ev)
    {
        if (ev.type == ev_mouse)
        {
            clear_screen(image_width, image_height);
            gout << stamp(c, ev.pos_x -  mid_x, ev.pos_y - mid_y);
            gout << refresh;
        }
    }
}