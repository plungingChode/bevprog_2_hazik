#include "graphics.hpp"
#include <vector>
#include <fstream>
#include <iostream>

using namespace genv;

static int image_width = 0;
static int image_height = 0;

struct Pixel
{
    int x, y;
    int r, g, b;

    void render() const
    {
        gout << color(r, g, b)
             << move_to(x, y)
             << dot;
    }

    void render(int offset_x, int offset_y) const
    {
        if (x + offset_x > image_width ||
            y + offset_y > image_height)
        {
            return;
        }
        gout << color(r, g, b)
             << move_to(x + offset_x, y + offset_y)
             << dot;
    }
};

void read_img(std::ifstream& f, std::vector<Pixel>& v, int& width, int& height)
{
    f >> width >> std::ws;
    f >> height >> std::ws;
    v.resize(width * height);  

    int x, y;
    for (int i = 0; i < v.size(); i++)
    {
        y = i / 600;
        x = i - y * 600;

        Pixel& p = v[i];
        p.x = x;
        p.y = y;

        f >> p.r >> std::ws;
        f >> p.g >> std::ws;
        f >> p.b >> std::ws;
    }
}

void clear_screen()
{
    gout << color(0, 0, 0)
         << move_to(0, 0)
         << box_to(image_width - 1, image_height - 1);
}

int main()
{
    std::ifstream f("a.kep");
    std::vector<Pixel> pixels(0);
    // int width, height;
    read_img(f, pixels, image_width, image_height);

    gout.open(image_width, image_height);

    for (const Pixel& p : pixels)
    {
        p.render();
    }

    gout << refresh;

    int mid_x = image_width / 2;
    int mid_y = image_height / 2;

    event ev;
    while (gin >> ev)
    {
        // if (ev.type == ev_mouse)
        // {
        //     clear_screen();
        //     for (const Pixel& p : pixels)
        //     {
        //         p.render(ev.pos_x - mid_x, ev.pos_y - mid_y);
        //     }
        //     gout << refresh;
        // }
    }
}