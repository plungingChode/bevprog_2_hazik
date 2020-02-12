#include "graphics.hpp"
using namespace genv;

const int WINDOW_X = 600;
const int WINDOW_Y = 600;

void clear_screen()
{
    gout << color(0,0,0);
    gout << move_to(0,0) << box_to(WINDOW_X - 1, WINDOW_Y - 1);
    gout << refresh;
}

void vasarely(unsigned vonalkoz)
{
    gout<<move_to(300,300);
    unsigned lepesek = 300 / vonalkoz;

    for(unsigned i=0; i < lepesek; i++)
    {
        unsigned char diff = (i * 255 / lepesek);
        gout << color(0, 255 - diff, diff);
        
        // jobb felso
        gout << move_to(300 + i * vonalkoz, 300);
        gout << line_to(300 + i / vonalkoz, 0 + i * vonalkoz);

        // jobb also
        gout << move_to(600 - i * vonalkoz, 300);
        gout << line_to(300 + i / vonalkoz, 300 + i * vonalkoz);

        // bal felso
        gout << move_to(300 - i * vonalkoz, 300);
        gout << line_to(300 - i / vonalkoz, 0 + i * vonalkoz);

        // bal also
        gout << move_to(0   + i * vonalkoz, 300);
        gout << line_to(300 + i / vonalkoz, 300 + i * vonalkoz);
    }

    gout << color(0, 255, 0);
    gout << move_to(300, 0) << line_to(300,600);
    gout << move_to(0, 300) << line_to(600,300);
    gout << refresh;
}

int main()
{
    gout.open(WINDOW_X, WINDOW_Y);
    // unsigned vonalkoz = 10;
    vasarely(10);

    event ev;
    while(gin >> ev)
    {
        // if (ev.type == ev_key)
        // {
        //     if (ev.keycode == key_up)
        //     {
        //         vonalkoz += 1;
        //         clear_screen();
        //         vasarely(vonalkoz);
        //     }

        //     if (ev.keycode == key_down && vonalkoz > 0)
        //     {
        //         vonalkoz -= 1;
        //         clear_screen();
        //         vasarely(vonalkoz);
        //     }
        // }
    }
    return 0;
}
