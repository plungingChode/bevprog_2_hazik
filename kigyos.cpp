#include "graphics.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>

using namespace genv;

const int AREA_HEIGHT = 40;
const int AREA_WIDTH = 40;
const int SCALE = 15;

const int MAX_X = AREA_WIDTH * SCALE - 1;
const int MAX_Y = AREA_HEIGHT * SCALE - 1;

struct Point
{
    int x, y;

    friend bool operator==(const Point& p1, const Point& p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }
};

enum GameState
{
    MENU = 1,
    PLAYING = 2,
    GAME_OVER = 3,
    EXIT = 0
};

struct Snek
{
private:
    std::vector<Point> body;

    Point direction;
    bool alive;

public:
    Snek(int start_x, int start_y)
    {
        direction.x = 0;
        direction.y = -1;
        alive = true;

        for (int i = 0; i < 5; i++)
        {
            Point p;
            p.x = start_x;
            p.y = start_y + i;
            body.push_back(p);
        }
    }

    Snek() : Snek(AREA_WIDTH / 2, AREA_HEIGHT / 2)
    {
    }

    void render() const
    {
        gout << color(133, 188, 94);
        const Point& head = body[0];
        gout << move_to(head.x  * SCALE, head.y * SCALE)
             << box(SCALE - 1, SCALE - 1);

        // clear last segment
        const Point& tail = body[body.size() - 1];
        gout << color(0, 0, 0)
             << move_to(tail.x * SCALE, tail.y * SCALE)
             << box(SCALE - 1, SCALE - 1);

        // render segment after head
        const Point& neck = body[1];
        gout << color(255, 255, 255)
             << move_to(neck.x * SCALE, neck.y * SCALE)
             << box(SCALE - 1, SCALE - 1);
    }
    
    void render_special(int r, int g, int b)
    {
        const Point& head = body[0];
        gout << color(133, 188, 94);
        gout << move_to(head.x  * SCALE, head.y * SCALE)
             << box(SCALE - 1, SCALE - 1);

        for (int i = 1; i < body.size(); i++)
        {
            const Point& p = body[i];
            gout << color(r, g, b);
            gout << move_to(p.x  * SCALE, p.y * SCALE)
                 << box(SCALE - 1, SCALE - 1);
        }
    }

    void move()
    {
        Point& head = body[0];
        for (int i = body.size() - 1; i > 0; i--)
        {
            Point& curr = body[i];
            Point& ahead = body[i - 1];

            curr.x = ahead.x;
            curr.y = ahead.y;
        }
        head.x += direction.x;
        head.y += direction.y;

        if (head.x > AREA_WIDTH - 1 || head.x < 0)
        {
            head.x = AREA_WIDTH - std::abs(head.x);
        }
        if (head.y > AREA_HEIGHT - 1 || head.y < 0)
        {
            head.y = AREA_HEIGHT - std::abs(head.y);
        }
    }

    void turn(int x, int y)
    {
        if (std::abs(direction.x - x) != 2)
        {
            direction.x = x;
        }
        if (std::abs(direction.y - y) != 2)
        {
            direction.y = y;
        }
    }

    bool collides_self() const
    {
        const Point& head = body[0];
        for (int i = 1; i < body.size(); i++)
        {
            if (head == body[i])
            {
                return true;
            }
        }
        return false;
    }

    bool collides(const std::vector<Point>& hitbox)
    {
        const Point& head = body[0];
        for (const Point& p : hitbox)
        {
            if (p == head)
            {
                return true;
            }
        }
        return false;
    }

    bool collides(const Point& p)
    {
        return p == body[0];
    }

    void kill()
    {
        alive = false;
    }

    bool is_alive()
    {
        return alive;
    }

    void add_segment()
    {
        Point& tail = body[body.size() - 1];
        Point& before_tail = body[body.size() - 2];
        
        Point new_tail;
        new_tail.x = tail.x - (before_tail.x - tail.x);
        new_tail.y = tail.y - (before_tail.y - tail.y);

        body.push_back(new_tail);
    }

    std::vector<Point> hitbox() const
    {
        return body;
    }
};

struct Walls
{
private:
    std::vector<Point> w;

public:
    Walls()
    {
        for (int i = 0; i < 10; i++)
        {
            Point p;
            p.x = 5 + i;
            p.y = 20;
            w.push_back(p);
        }

        for (int i = 0; i < 10; i++)
        {
            Point p;
            p.x = 25;
            p.y = 5 + i;
            w.push_back(p);
        }
    }

    std::vector<Point> hitbox() const
    {
        return w;
    }

    void render() const
    {
        gout << color(0, 255, 255);
        for (const Point& p : w)
        {
            gout << move_to(p.x  * SCALE, p.y * SCALE)
                 << box(SCALE, SCALE);
        }
    }
};

struct Food
{
private:
    Point pos;

    bool overlaps(Point p, std::vector<Point> hitbox)
    {
        for (const Point& v : hitbox)
        {
            if (p == v) return true;
        }
        return false;
    }

    Point rand_point()
    {
        Point p;

        p.x = std::rand() % (AREA_HEIGHT - 4) + 4;
        p.y = std::rand() % (AREA_WIDTH - 4) + 4;

        return p;
    }

public:
    Food(const Walls& w, const Snek& s)
    {
        respawn(w, s);
    }

    void respawn(const Walls& w, const Snek& s)
    {
        Point p = rand_point();
        while(overlaps(p, s.hitbox()) || 
              overlaps(p, w.hitbox()))
        {
            p = rand_point();
        }
        this->pos = p;
    }

    void render()
    {
        gout << color(255, 0, 0)
             << move_to(pos.x * SCALE, pos.y * SCALE)
             << box(SCALE - 1, SCALE - 1); 
    }

    Point hitbox()
    {
        return pos;
    }
};

void clear_screen()
{
    gout << color(0, 0, 0)
         << move_to(0, 0)
         << box_to(MAX_X, MAX_Y);
}

int game_loop()
{
    const int TICK_TIME = 120;

    std::srand(std::time(nullptr));

    clear_screen();

    Snek s(2, 10);
    Walls w;
    Food f(w, s);

    s.render();
    w.render();
    f.render();
    gout << refresh;

    gin.timer(TICK_TIME);
    bool running = true;

    int input_buffer = -1;
    bool need_add_segment = false;
    int death_animation = 0;

    event ev;
    while (gin >> ev)
    {
        if (ev.type == ev_key)
        {
            input_buffer = std::abs(ev.keycode);            

            if (ev.keycode == key_escape)
            {
                return EXIT;
            }

            // r == restart
            if (ev.keycode == 114)
            {
                return PLAYING;
            }

            if (ev.keycode == key_space)
            {
                if (running)
                {
                    gin.timer(-1);
                    running = false;
                }
                else
                {
                    gin.timer(TICK_TIME);
                    running = true;
                }
            }
        }

        if (ev.type == ev_timer)
        {
            if (need_add_segment)
            {
                s.add_segment();
                need_add_segment = false;
            }

            if (input_buffer != -1)
            {
                switch (input_buffer)
                {
                    case key_up: s.turn(0, -1); break;
                    case key_down: s.turn(0, 1); break;
                    case key_left: s.turn(-1, 0); break;
                    case key_right: s.turn(1, 0); break;
                }
            }
            input_buffer = -1;

            if (!s.collides_self() && 
                !s.collides(w.hitbox()))
            {
                s.render();
            }
            else
            {
                break;
            }

            if (s.collides(f.hitbox()))
            {
                need_add_segment = true;
                f.respawn(w, s);
                f.render();
            }

            s.move();
            gout << refresh;
        }
    }

    gin.timer(TICK_TIME * 3);
    int death_blink_cnt = 0;
    while (gin >> ev)
    {
        if (ev.type == ev_timer)
        {
            if (death_blink_cnt % 2)
            {
                s.render_special(255, 0, 0);
            }
            else
            {
                s.render_special(255, 255, 255);
            }
            gout << refresh;
            if (death_blink_cnt < 6)
            {
                death_blink_cnt++;   
            }
            else
            {
                break;
            }
        }
    }

    std::cout << "go to game over\n";
    gin.timer(-1);
    return GAME_OVER;
}

void center_text(std::string txt, int y)
{
    gout << move_to(MAX_X / 2 - gout.twidth(txt) / 2, y)
         << text(txt);
}

int game_over_loop()
{
    clear_screen();

    int line_height = gout.cascent() + gout.cdescent();
    int start_y = MAX_Y / 2;
    
    gout << color(255, 255, 255);
    center_text("GAME OVER", start_y);
    center_text("Play again? [Y / N]", start_y + line_height + 5);
    gout << refresh;

    event ev;
    while (gin >> ev)
    {
        if (ev.type == ev_key)
        {
            if (ev.keycode == key_enter ||
                ev.keycode == 121) // key Y
            {
                return PLAYING;
            }

            if (ev.keycode == 110) // key N
            {
                return MENU;
            }

            if (ev.keycode == key_escape)
            {
                return EXIT;
            }
        }

    }
}

void render_option(std::string option, int y)
{
    int twidth = gout.twidth(option);
    int line_height = gout.cascent() + gout.cdescent();
    gout << color(0, 0, 0)
         << move_to(MAX_X / 2 - twidth / 2 - 10, y - line_height - 2)
         << box_to(MAX_X / 2 + twidth / 2 + 10, y + 7); 

    gout << color(255, 255 ,255);
    center_text(option, y);
}

void render_selected_option(std::string option, int y)
{
    int twidth = gout.twidth(option);
    int line_height = gout.cascent() + gout.cdescent();
    gout << color(255, 255, 255)
         << move_to(MAX_X / 2 - twidth / 2 - 10, y - line_height - 2)
         << box_to(MAX_X / 2 + twidth / 2 + 10, y + 7);

    gout << color(0, 0, 0)
         << move_to(MAX_X / 2 - twidth / 2 - 8, y - line_height)
         << box_to(MAX_X / 2 + twidth / 2 + 8, y + 5);

    gout << color(255, 255 ,255);
    center_text(option, y);
}

int menu_loop()
{
    const int LINE_HEIGHT = gout.cascent() + gout.cdescent();
    const int INIT_Y = MAX_Y / 3;
    const int PADDING = 30;

    int cursor = 0;

    std::vector<std::string> options = {
        "Play",
        "Exit"
    };
    std::vector<int> option_codes = {
        PLAYING,
        EXIT
    };
    
    std::map<int, int> option_locations;

    clear_screen();
    gout << color(133, 188, 94);
    center_text("Kigyos", INIT_Y);

    gout << color(255, 255, 255);
    for (size_t i = 0; i < options.size(); i++)
    {
        int y = INIT_Y + 40 + (i + 1) * PADDING;

        option_locations[i] = y;
        center_text(options[i], y);
    }

    render_selected_option(
        options[0], 
        option_locations[0]);

    gout << refresh;

    event ev;
    while(gin >> ev)
    {
        if (ev.type == ev_key)
        {
            if (ev.keycode == key_enter)
            {
                return option_codes[cursor];
            }

            if (ev.keycode == key_up ||
                ev.keycode == key_down)
            {
                render_option(
                    options[cursor], 
                    option_locations[cursor]);

                if (ev.keycode == key_up)
                {
                    cursor++;
                    if (cursor > options.size() - 1)
                    {
                        cursor = 0;
                    }
                }
                else
                {
                    cursor--;
                    if (cursor < 0)
                    {
                        cursor = options.size() - 1;
                    }
                }

                render_selected_option(
                    options[cursor], 
                    option_locations[cursor]);
            }

            gout << refresh;
        }
    }
}

int main()
{
    gout.open(AREA_WIDTH * SCALE, AREA_HEIGHT * SCALE);

    int state = menu_loop();
    int prev_state = PLAYING;
    while(true)
    {
        std::cout << state << '\n';
        switch (state)
        {
            case GAME_OVER:
                std::cout << "game over begin\n";
                state = game_over_loop();
                break;

            case PLAYING:
                std::cout << "play again begin\n";
                state = game_loop();
                break;

            case MENU:
                std::cout << "menu begin\n";
                state = menu_loop();
                break;

            case EXIT:
                break;
        }

        if (prev_state == state)
        {
            // break looping out of control
            break;
        }
        else
        {
            prev_state = state;
        }
    }
    return 0;
}