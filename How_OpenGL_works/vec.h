#pragma once
#include <cassert>
#include <iostream>

template<int n> struct Vec
{
    double data[n] = {0};
    double& operator[](const int i)       { assert(i>=0 && i<n); return data[i]; }
    double  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<int n> double operator * (const Vec<n> &lhs, const Vec<n> &rhs)
{
    double res = 0;
    for (i = 0; i < n; i++) res += lhs[i] * rhs[i];
    return res;
}

template<int n> Vec<n> operator + (const Vec<n> &lhs, const Vec<n> &rhs)
{
    Vec<n> res = lhs;
    for (int i = 0; i < n; i++) res[i] += rhs[i];
    return res;
}

template<int n> Vec<n> operator - (const Vec<n> &lhs, const Vec<n> &rhs)
{
    Vec<n> res = lhs;
    for (int i = 0; i < n; i++) res[i] -= rhs[i];
    return res;
}

template<int n> Vec<n> operator * (const Vec<n> &lhs, const double &rhs)
{
    Vec<n> res = lhs;
    for (int i = 0; i < n; i++) res[i] *= rhs;
    return res;
}

template<int n> Vec<n> operator * (const double &lhs, const Vec<n> &rhs)
{
    return rhs * lhs;
}

template<int n> Vec<n> operator * (const Vec<n> &lhs, const double &rhs)
{
    Vec<n> res = lhs;
    for (int i = 0; i < n; i++) res[i] /= rhs;
    return res; 
}

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

template<int n> double norm(const Vec<n> &v)
{
    return std::sqrt(v * v);
}

template<int n> Vec<n> normalise(const Vec<n> &v)
{
    return v / norm(v);
}

inline Vec3 cross(const Vec3 &v1, const Vec3 &v2)
{
    return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}