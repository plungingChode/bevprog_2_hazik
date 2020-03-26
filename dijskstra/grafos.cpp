#include "graphics.hpp"
#include <map>
#include <vector>
#include <set>
#include <climits>
#include <algorithm>
#include <sstream>
#include <string>
#include <fstream>

#define INFTY INT_MAX

using namespace genv;

struct vertex
{
    static int cnt;

    int nr;
    int x, y;

    vertex(int x, int y) : x(x), y(y) 
    {
        cnt++;
        nr = cnt;
    }

    void render(const color& c) const
    {
        gout << move_to(x-12, y-12) << c << box(25, 25) 
             << move(-17, -5) << color(0, 0, 0) << text(std::to_string(nr));
    }

    bool intersects(int x, int y)
    {
        return x >= this->x-12 && x <= this->x+13 &&
               y >= this->y-12 && y <= this->y+13;
    }
};

int vertex::cnt = 0;

struct edge
{
    const vertex *c1, *c2;
    const int len;

    edge(const vertex *c1, const vertex *c2, int len)
        : c1(c1), c2(c2), len(len)
    {}

    void render(const color& c) const
    {
        gout << move_to(c1->x, c1->y) << c << line_to(c2->x, c2->y);

        if (len > 1)
        {
            const int dx = c2->x - c1->x;
            const int dy = c2->y - c1->y;

            int off_x = 0;
            int off_y = 0;

            if (dx < 40 && dx > 0)
            {
                off_x = 10;
            }
            else if (dx > -40 && dx < 0)
            {
                off_x = -10;
            }

            if (dy < 40 && dy > 0)
            {
                off_y = 10;
            }
            else if (dy > -40 && dy < 0)
            {
                off_y = -10;
            }

            const int center_x = dx / 2 + off_x;
            const int center_y = dy / 2 + off_y;

            gout << move_to(c1->x + center_x, c1->y + center_y) << c << text(std::to_string(len));

        }
    }
};

struct graph
{
    std::vector<vertex*> csucsok;
    std::vector<edge*> elek;
    std::map<vertex*, std::vector<vertex*>> neighbors;

    void add_vertex(int x, int y)
    {
        csucsok.push_back(new vertex(x ,y));
    }

    void add_vertex(vertex *const &c)
    {
        csucsok.push_back(c);
    }

    void connect(int c1, int c2, int len)
    {
        connect(csucsok[c1-1], csucsok[c2-1], len);
    }   

    void connect(vertex *const &c1, vertex *const &c2, int len = 1)
    {
        elek.push_back(new edge(c1, c2, len));
        neighbors[c1].push_back(c2);
        neighbors[c2].push_back(c1);
    }

    int length(vertex *u, vertex *v)
    {
        for (edge *const &e : elek)
        {
            if ((e->c1 == u && e->c2 == v) ||
                (e->c1 == v && e->c2 == u))
            {
                return e->len;
            }
        }
        return INFTY;
    }

    std::vector<vertex*> find_path(vertex *from, vertex *to)
    {
        std::set<vertex*> q;
        std::map<vertex*, int> dist;
        std::map<vertex*, vertex*> prev;

        for (vertex *c : csucsok)
        {
            dist[c] = INFTY;
            prev[c] = nullptr;
            q.insert(c);
        }
        dist[from] = 0;

        while (!q.empty())
        {
            // u == 2
            vertex *u;
            int min = -1;
            for (vertex *p : q)
            {
                int d = dist[p];
                if (min == -1 || min > d)
                {
                    min = d;
                    u = p;
                }
            }
            q.erase(u);

            if (u == to) break;

            for (vertex *&nb : neighbors[u])
            {
                if (q.find(nb) != q.end()) // nb eleme q
                {
                    int alt = dist[u] + length(u, nb);
                    if (alt < dist[nb])
                    {
                        dist[nb] = alt;
                        prev[nb] = u;
                    }
                }
            }
        }

        std::vector<vertex*> path;
        path.push_back(to);

        vertex* prv = prev[to];
        while (prv && prev[prv])
        {
            path.push_back(prv);
            prv = prev[prv];
        }
        path.push_back(prv);

        std::reverse(path.begin(), path.end());
        return path;
    }

    static graph read_file(const std::string& fname)
    {
        std::ifstream f(fname);

        graph g;
        int cnt;

        f >> cnt;
        for (int i = 0; i < cnt; i++)
        {
            int x, y;
            f >> x >> y;
            g.add_vertex(x, y);
        }

        f >> cnt;
        for (int i = 0; i < cnt; i++)
        {
            int c1, c2, len;
            f >> c1 >> c2 >> len;
            g.connect(c1, c2, len);
        }

        return g;
    }
};