#include "graphics.hpp"
using namespace genv;

const int SIZE_X = 600;
const int SIZE_Y = 600;

void vasarely(int hezag, int margo)
{
    const int mid_x = SIZE_X / 2;
    const int mid_y = SIZE_Y / 2;

    const int max_x = SIZE_X - margo;
    const int max_y = SIZE_Y - margo;

    const int min_xy = margo;

    const int cycles = (SIZE_X - margo) / (2 * hezag);

    for (int i = 0; i < cycles; i++)
    {
        int color_diff = i * 255 / cycles;
        gout << color(255 - color_diff, color_diff, 255);
        gout << move_to(mid_x + i * hezag, mid_y)
             << line_to(mid_x + i / hezag, min_xy + i * hezag);        

        gout << move_to(mid_x - i * hezag, mid_y)
             << line_to(mid_x - i / hezag, max_y - i * hezag);

        gout << color(color_diff, 255 - color_diff, 255);
        gout << move_to(mid_x + i * hezag, mid_y)
             << line_to(mid_x + i / hezag, max_y - i * hezag);

        gout << move_to(mid_x - i * hezag, mid_y)
             << line_to(mid_x - i / hezag, min_xy + i * hezag);
    }

    gout << color(255, 0, 255)
         << move_to(mid_x , min_xy)
         << line_to(mid_x , max_y)
         << move_to(min_xy, mid_y)
         << line_to(max_x , mid_y);
}

void clear_screen()
{
    gout << color(0, 0, 0)
         << move_to(0, 0)
         << box_to(SIZE_X - 1, SIZE_Y - 1);
}

int main()
{
    gout.open(SIZE_X, SIZE_Y);

    int hezag = 9;
    int margo = 25;
    vasarely(hezag, margo);
    gout << refresh;

    event ev;
    while(gin >> ev) 
    {        
        if (ev.type == ev_key)
        {
            if (ev.keycode == key_up)
            {
                hezag++;
                clear_screen();
                vasarely(hezag, margo);
            }
            else if (ev.keycode == key_down && hezag > 1)
            {
                hezag--;
                clear_screen();
                vasarely(hezag, margo);
            }
            else if (ev.keycode == 43) // numpad +
            {
                margo += 5;
                clear_screen();
                vasarely(hezag, margo);
            }
            else if (ev.keycode == 45 && margo >= 5) // numpad -
            {
                margo -= 5;
                clear_screen();
                vasarely(hezag, margo);
            }
            gout << refresh;
        }
    }
    return 0;
}
