#include "graphics.hpp"
#include "basic_types.cpp"
#include <vector>

using namespace genv;

const int SCREEN_X = 640;
const int SCREEN_Y = 480;

const color white = color(255, 255, 255);
const color pink = color(255, 20, 147);
canvas cls(SCREEN_X, SCREEN_Y);

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

struct Paddle
{
private:
    color col = pink;
    rect hb;
    canvas c;

public:
    Paddle(vec2 pos, int h, int w) : hb(pos, {h, w}), c(SCREEN_X, h) 
    {
        c << move_to(0, 0) << col << box(SCREEN_X, h);
    }

    const vec2 ball_pos(int ball_r) const
    {
        return vec2(hb.p.x+hb.sz.x/2-ball_r, hb.p.y-2*ball_r);
    }

    void move(int x)
    {
        hb.p.x += x;
        if (hb.p.x < 0) hb.p.x = 0;
        if (hb.p.x + hb.sz.x > SCREEN_X) hb.p.x = SCREEN_X - hb.sz.x;
    }

    const rect& hitbox() const
    {
        return hb;
    }

    void render(canvas& c)
    {
        c << stamp(this->c, 0, 0, hb.sz.x, hb.sz.y, hb.p.x, hb.p.y);
        // c << col << move_to(hb.tl.x+hb.sz.x/2, 0) << line(0, SCREEN_Y);
    }
};

struct Block
{
private:
    rect hb;
    canvas c;
    bool destroyed = false;

public:
    Block(rect hitbox, color color) : hb(hitbox)
    {
        c.open(hb.sz.x, hb.sz.y);
        c << move_to(0, 0) << color << box(hb.sz.x, hb.sz.y);
    }

    void render(canvas& c)
    {   
        c << stamp(this->c, hb.p.x, hb.p.y);
    }

    const rect& hitbox() const
    {
        return hb;
    }

    void destroy()
    {
        destroyed = true;
    }

    bool is_destroyed() const
    {
        return destroyed;
    }
}; 

struct Ball 
{
private:
    color col = white;
    rect hb;
    vec2 vel;
    canvas c;
    Paddle* p;

    int total_spd = 6;
    int r;

    bool fixed = true;
    bool bounced = false;

public:
    Ball(vec2 pos, int r, Paddle* p) 
        : hb(pos, {2*r, 2*r}), r(r), c(2*r, 2*r), p(p)
    {
        for (int x = -r; x < r; x++)
        {
            for (int y = -r; y < r; y++)
            {
                if (x*x + y*y <= r*r)
                {
                    c << col << move_to(x+r, y+r) << dot;
                }
            }
        }
    }

    void launch()
    {
        if (fixed)
        {
            fixed = false;
            vel.y = 0;
            vel.x = std::rand() % 10 - 5;
            vel.y = -std::sqrt(std::pow(total_spd, 2) - std::pow(vel.x, 2));
        }
    }

    void reset()
    {
        fixed = true;
    }

    bool out()
    {
        return hb.p.y > SCREEN_Y + 2*hb.sz.y;
    }

    void move()
    {
        bounced = false;

        if (fixed)
        {
            hb.p = p->ball_pos(r);
        }
        else
        {
            hb.p += vel;

            // check bounding
            if (hb.p.x <= 0 || hb.p.x+hb.sz.x >= SCREEN_X)
            {
                vel.x *= -1;
            }

            if (hb.p.y <= 0)
            {
                vel.y *= -1;
            }
        }
    }

    bool do_bounce(rect r, bool is_paddle = false)
    {
        int r_center = r.p.x+r.sz.x/2;
        int ball_center = hb.p.x+this->r;
        if (r.intersects(hb))
        {
            vel.x = (r_center - ball_center) / -(r.sz.x/12);
            if (vel.x == 0)
            {
                vel.x = 1 + std::rand() % 2;
                if (r_center > ball_center)
                {
                    vel.x *= -1;
                }
            }

            int new_vel_y = std::sqrt(std::pow(total_spd, 2) - std::pow(vel.x, 2));
            if (new_vel_y == 0)
            {
                new_vel_y = 1;
            }
            if (is_paddle)
            {
                vel.y = -new_vel_y;
                hb.p.y = r.p.y - r.sz.y;
            }
            else
            {
                if (vel.y < 0)
                {
                    vel.y = new_vel_y;
                }
                else
                {
                    vel.y = -new_vel_y;
                }
            }
            
            return true;
        }
        return false;
    }

    bool do_bounce(Block*& b)
    {
        // if (do_bounce(b->hitbox()))
        if (hb.intersects(b->hitbox()))
        {
            if (!bounced) 
            {
                vel.y *= -1;
                bounced = true;
            }
            b->destroy();
            // printf("vel.x=%d, vel.y=%d\nspd=%.2f\n\n", vel.x, vel.y, vel.len());
            return true;
        }
        return false;
    }

    void render(canvas& c)
    {
        c << stamp(this->c, hb.p.x, hb.p.y);
        // c << col << move_to(hb.tl.x+hb.sz.x/2, 0) << line(0, SCREEN_Y);
    }
};

int main(int argc, char const *argv[])
{
    int key_buf = 0;

    gout.open(SCREEN_X, SCREEN_Y);
    gin.timer(20);

    Paddle* p = new Paddle({100, SCREEN_Y-20}, 100, 20);
    Ball ball({0, 0}, 7, p);

    int block_w = 50;
    int block_h = 17;
    int cols = SCREEN_X/block_w;
    if (cols * block_w < SCREEN_X)
    {
        cols++;
    }
    std::vector<Block*> blocks;
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            blocks.push_back(new Block(
                rect(x*(block_w+1), y*(block_h+1), block_w, block_h),
                color(150, 255, 255)
            ));
        }
    }

    event ev;
    while (gin >> ev)
    {
        if (ev.type == ev_key)
        {
            int kc = std::abs(ev.keycode);
            if (std::abs(kc) == key_left || std::abs(kc) == key_right)
            {
                key_buf += ev.keycode;
            }

            if (kc == key_space)
            {
                ball.launch(); 
            }

            if (kc == key_escape)
            {
                return 0;
            }
        }

        if (ev.type == ev_timer)
        {
            switch (key_buf)
            {
            case key_left:  p->move(-6); break;
            case key_right: p->move(+6); break;
            default: break;
            }

            gout << stamp(cls, 0, 0);

            ball.do_bounce(p->hitbox(), true);
            if (ball.out()) ball.reset();
            ball.move();

            int remove_from = -1;
            for (int i = 0; i < blocks.size(); i++)
            {
                Block*& b = blocks[i];

                if (ball.do_bounce(b))
                {
                    delete b;
                    blocks[i] = nullptr;
                    if (remove_from == -1) remove_from = i;
                }
                else
                {
                    b->render(gout);
                }
            }
            if (remove_from != -1)
            {
                clean_vector(blocks);
            }

            p->render(gout);
            ball.render(gout);
            gout << refresh;
        }
    }

    return 0;
}
