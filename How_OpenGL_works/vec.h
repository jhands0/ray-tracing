#pragma once
#include <cassert>
#include <iostream>

template<int n> struct Vec
{
    double data[n] = {0};
    double& operator[](const int i)       { assert(i>=0 && i<n); return data[i]; }
    double  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<int n> std::ostream& operator << (std::ostream& out, const Vec<n>& v)
{
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

template<> struct Vec<2>
{
    double x = 0, y = 0;
    double& operator[](const int i)       { assert(i>=0 && i<2); return i ? y : x; }
    double  operator[](const int i) const { assert(i>=0 && i<2); return i ? y : x; }
};

template<> struct Vec<3>
{
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i>=0 && i<3); return i ? (i==1 ? y : z) : x; }
    double  operator[](const int i) const { assert(i>=0 && i<3); return i ? (i==1 ? y : z) : x; }
};

template<> struct Vec<4>
{
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i)       { assert(i>=0 && i<4); return i<2 ? (i ? y : x) : (i==2 ? z : w); }
    double  operator[](const int i) const { assert(i>=0 && i<4); return i<2 ? (i ? y : x) : (i==2 ? z : w); }
    Vec<2> xy() const { return {x, y}; }
    Vec<3> xyz() const { return {x, y, z}; }
};

typedef Vec<2> Vec2;
typedef Vec<3> Vec3;
typedef Vec<4> Vec4;