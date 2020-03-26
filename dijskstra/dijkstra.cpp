#include "graphics.hpp"
#include <vector>
#include "grafos.cpp"

using namespace genv;

const color white = color(255, 255, 255);
const color blue = color(0, 0, 150);

struct GraphApp
{
private:
    const int DEFAULT = 1;
    const int PATH = 2;
    const int SET_LEN = 3;

    canvas cls = canvas(640, 460);

    bool editing = false;
    int new_len = 1;
    edge* selected_edge = nullptr;
    int edge_cursor = 0;

    const color start_col = color(220, 190, 100);
    const color end_col = color(150, 190, 230);

    vertex* start;
    vertex* end;

    graph g;

public:
    GraphApp(const char* src_file) : g(graph::read_file(src_file))
    {
    }

    bool on_mouse_ev(const event& ev)
    {
        if (ev.button == btn_left || ev.button == btn_right)
        {
            for (vertex* v : g.csucsok)
            {
                if (v->intersects(ev.pos_x, ev.pos_y))
                {
                    if (ev.button == btn_left) start = v;
                    if (ev.button == btn_right) end = v;
                    return true;
                }
            }
        }
        return false;
    }

    int on_key_ev(const event& ev)
    {
        if (ev.keycode == key_enter)
        {
            if (editing)
            {
                if (start && end)
                {
                    g.connect(start, end, new_len);
                    new_len = 1;
                }
                return DEFAULT;
            }
            else
            {
                return PATH;
            }
        }

        if (editing)
        {
            if ( ev.keycode >= 49 && ev.keycode <= 57)
            {
                new_len = ev.keycode - 48;
                return SET_LEN;
            }

            if (ev.keycode == key_right)
            {
                edge_cursor++;
                if (edge_cursor > g.elek.size() - 1)
                {
                    edge_cursor = 0;
                }
                selected_edge = g.elek[edge_cursor];
                return DEFAULT;
            }

            if (ev.keycode == key_left)
            {
                edge_cursor--;
                if (edge_cursor < 0)
                {
                    edge_cursor = g.elek.size() - 1;
                }
                selected_edge = g.elek[edge_cursor];
                return DEFAULT;
            }
        }

        if (ev.keycode == 101) // e
        {
            editing = !editing;
            if (editing)
            {
                selected_edge = g.elek[edge_cursor];
            }
            else
            {
                selected_edge = nullptr;
                edge_cursor = 0;
            }
        }
        return 0;
    }

    void render_graph()
    {
        for (edge *const &e : g.elek)
        {
            if (e == selected_edge)
            {
                e->render(color(50, 220, 150));
            }
            else
            {
                e->render(white);   
            }
        }

        for (vertex *const &c : g.csucsok)
        {
            if (c == start)
            {
                c->render(start_col);
            }
            else if (c == end)
            {
                c->render(end_col);
            }
            else
            {
                c->render(white);
            }
        }
    }

    template<typename T>
    bool contains(std::vector<T*> v, const T* e)
    {
        return std::find(v.begin(), v.end(), e) != v.end();
    }

    void render_path()
    {
        std::vector<vertex*> path = g.find_path(start, end);

        std::vector<edge*> temp;
        for (int i = 0; i < path.size() - 1; i++)
        {
            temp.push_back(new edge(path[i], path[i+1], 1));
        }

        for (edge *const &e : g.elek)
        {
            bool is_path = false;
            for (edge* t : temp)
            {
                if ((t->c1 == e->c1 && t->c2 == e->c2) ||
                    (t->c1 == e->c2 && t->c2 == e->c1))
                {
                    is_path = true;
                    break;
                }
            }
            if (is_path)
            {
                e->render(color(255, 100, 100));
            }
            else
            {
                e->render(white);
            }
        }

        for (vertex *const &c : g.csucsok)
        {
            if (contains(path, c))
            {
                c->render(color(255, 100, 100));
            }
            else
            {
                c->render(white);
            }
        }
    }

    void render_status()
    {
        gout << move_to(0, 460) << color(210, 210, 210) << box(640, 20);
        gout << move_to(20, 473);
        if (editing)
        {
            gout << color(0, 0, 0) << text("uj el hossza: " + std::to_string(new_len));
        }
    }

    void run()
    {
        gout.open(640, 480);

        render_graph();
        gout << refresh;

        event ev;
        while (gin >> ev && ev.keycode != key_escape)
        {
            if (ev.type == ev_mouse)
            {
                if (on_mouse_ev(ev))
                {
                    gout << stamp(cls, 0, 0);
                    render_graph();
                    gout << refresh;            
                }
            }

            if (ev.type == ev_key)
            {
                int ret_val = on_key_ev(ev);
                if (ret_val == DEFAULT)
                {
                    gout << stamp(cls, 0, 0);
                    render_graph();
                    gout << refresh;        
                }
                else if (ret_val == PATH)
                {
                    gout << stamp(cls, 0, 0);
                    render_path();
                    gout << refresh;        
                }
                else if (ret_val == SET_LEN)
                {
                    render_status();
                    gout << refresh;
                }
            }
        }

    }
};

int main(int argc, char const *argv[])
{
    GraphApp g("graf.dj");
    g.run();

    return 0;
}
