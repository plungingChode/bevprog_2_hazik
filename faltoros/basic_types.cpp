#include <iostream>
#include <cmath>

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

    vec2& operator-=(const vec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    vec2& operator*=(const int& i)
    {
        x *= i;
        y *= i;
        return *this;
    }

    float len() const
    {
        return std::sqrt(x*x + y*y);
    }
};

vec2 operator-(const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x-rhs.x, lhs.y-rhs.y);
}

vec2 operator+(const vec2& lhs, const vec2& rhs)
{
    return vec2(lhs.x+rhs.x, lhs.y+rhs.y);
}

vec2 operator*(const vec2& lhs, const int& rhs)
{
    return vec2(lhs.x*rhs, lhs.y*rhs);
}

std::ostream& operator<<(std::ostream& o, const vec2& v)
{
    o << v.x << ", " << v.y;
    return o;
}

struct rect
{
    vec2 p;
    vec2 sz;

    rect(vec2 topleft, vec2 sz) : p(topleft), sz(sz) {}
    rect(int x, int y, int w, int h) : rect({x, y}, {w, h}) {}
    rect(vec2 sz) : rect({0, 0}, sz) {}   

    bool intersects(const rect& r) const
    {
        return p.x+sz.x > r.p.x && r.p.x+r.sz.x > p.x
            && p.y+sz.y > r.p.y && r.p.y+r.sz.y > p.y;
    }
};