#include "graphics.hpp"
#include <vector>
#include <algorithm>
#include <ctime>

using namespace genv;

const int SCREEN_X = 800;
const int SCREEN_Y = 600;

struct vec2
{
    int x, y;
    vec2(int x, int y) : x(x), y(y) {}
    vec2() : vec2(0, 0) {}

    vec2& operator+=(const vec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
};

struct Kocka
{
protected:
    vec2 p;
    int a;
    color c;
    canvas k;

    void redraw()
    {
        k.open(a, a);
        k << genv::move_to(0, 0) << c << box(a, a);
    }

public:
    Kocka(vec2 pos, int sides, color color) : p(pos), a(sides), c(color)
    {
       redraw();
    }

    void move_to(vec2 pos)
    {
        p.x = pos.x - a/2;
        p.y = pos.y - a/2;
    }

    void move(vec2 diff)
    {
        p += diff;
    }

    void change_size(int diff)
    {
        a += diff;
        redraw();
    }

    bool collides(const Kocka& k) const
    {
        return p.x+a >= k.p.x && k.p.x+k.a >= p.x
            && p.y+a >= k.p.y && k.p.y+k.a >= p.y;
    }

    void render(canvas& c)
    {
        c << stamp(k, p.x, p.y);
    }
};

struct AutoKocka : public Kocka
{
private:
    vec2 dir;
    bool friendly;

public:
    AutoKocka(vec2 pos, int sides, color color, vec2 direction, bool friendly)
        : Kocka(pos, sides, color), dir(direction), friendly(friendly)
    {
    }

    bool out()
    {
        if (dir.x > 0)
        {
            return p.x > SCREEN_X;
        }
        else if (dir.x < 0)
        {
            return p.x + a < 0;
        }
        else if (dir.y > 0)
        {
            return p.y > SCREEN_Y;
        }
        else if (dir.y < 0)
        {
            return p.y + a < 0;
        }
        else
        {
            return true;
        }
    }

    bool is_friend() const
    {
        return friendly;
    }

    void move()
    {
        Kocka::move(dir);
    }
};

AutoKocka* spawn_auto(bool friendly)
{
    int size = 15 + std::rand() % 20;

    int spd = 1 + std::rand() % 4;

    vec2 direction, pos;
    switch (std::rand() % 4)
    {
    case 0: 
        direction.x = +spd;
        direction.y =  0;
        pos.x = 0 - std::rand() % 200 - size;
        pos.y = 0 + std::rand() % (SCREEN_Y - size);
        break;
    
     case 1: 
        direction.x =  0;
        direction.y = +spd;
        pos.x = 0 + std::rand() % (SCREEN_X - size);
        pos.y = 0 - std::rand() % 200 - size;
        break;

    case 2: 
        direction.x = -spd;
        direction.y =  0;
        pos.x = SCREEN_X + std::rand() % 200 + size;
        pos.y = 0 + std::rand() % (SCREEN_Y - size);
        break;

    case 3: 
        direction.x =  0;
        direction.y = -spd;
        pos.x = 0 + std::rand() % (SCREEN_X - size);
        pos.y = SCREEN_Y + std::rand() % 200 + size;
        break;

    default:
        break;
    }

    if (friendly)
    {
        color c(50, 100 + std::rand() % 155, 100 + std::rand() % 155);
        return new AutoKocka(pos, size, c, direction, friendly);
    }
    else
    {
        color c(240, 50, 50);
        return new AutoKocka(pos, size, c, direction, friendly);
    }
    
}

int main(int argc, char const *argv[])
{
    std::srand(time(NULL));

    Kocka* k = new Kocka(vec2(0, 0), 15, color(255, 255, 255));
    std::vector<AutoKocka*> others;

    canvas cls(SCREEN_X, SCREEN_Y);
    gout.open(SCREEN_X, SCREEN_Y);
    gin.timer(15);

    vec2 mpos;
    int timer = 0;
    int timer_tick = 15;

    event ev;
    while (gin >> ev)
    {
        if (ev.type == ev_key)
        {
            if (ev.keycode == key_escape)
            {
                exit(0);
            }
        }

        if (ev.type == ev_mouse)
        {
            mpos.x = ev.pos_x;
            mpos.y = ev.pos_y;
        }

        if (ev.type == ev_timer)
        {
            gout << stamp(cls, 0, 0);

            timer++;
            if (timer % timer_tick == 0)
            {
                others.push_back(spawn_auto(std::rand() % 2));
            }

            k->move_to(mpos);
            k->render(gout);
            for (int i = 0; i < others.size(); i++)
            {
                AutoKocka*& a = others[i];
                a->move();
                a->render(gout);
                if (a->out())
                {
                    delete a;
                    others[i] = nullptr;
                }
                else if (k->collides(*a))
                {
                    if (a->is_friend())
                    {
                        k->change_size(+2);
                        delete a;
                        others[i] = nullptr;
                    }
                    else
                    {
                        // delete k;
                        printf("Game over!\n");
                        gin.timer(-1);
                        // return 0;
                    }
                }
            }

            // https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
            others.erase(std::remove(others.begin(), others.end(), nullptr), others.end());
            // printf("'on screen'=%d\n", others.size());

            gout << refresh;
        }
    }

    return 0;
}
