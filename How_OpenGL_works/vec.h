#pragma once
#include <cmath>
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
    for (int i = 0; i < n; i++) res += lhs[i] * rhs[i];
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

template<int n> Vec<n> operator / (const Vec<n> &lhs, const double &rhs)
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

template<int n> struct dt;

template<int n_rows, int n_cols> struct Matrix
{
    Vec<n_cols> rows[n_rows] = {{}};

        Vec<n_cols>& operator [] (const int idx)    { assert(idx >= 0 && idx < n_rows); return rows[idx]; }
    const Vec<n_cols>& operator [] (const int idx) const { assert(idx >= 0 && idx < n_rows); return rows[idx]; }

    double det() const 
    {
        return dt<n_cols>::det(*this);
    }

    double cofactor(const int row, const int col) const 
    {
        Matrix<n_rows - 1, n_cols - 1> submatrix;
        for (int i = n_rows - 1; i--; )
        {
            for (int j = n_cols - 1; j--; ) submatrix[i][j] = rows[i + int(i >= row)][j + int(j >= col)];
        }
        return submatrix.det() * ((row + col) % 2 ? -1 : 1);
    }

    Matrix<n_rows, n_cols> inverse_transpose() const
    {
        Matrix<n_rows, n_cols> adjugate_transpose;
        for (int i = 0; i < n_rows; i++)
        {
            for (int j = 0; j < n_cols; j++) adjugate_transpose[i][j] = cofactor(i, j);
        }
        return adjugate_transpose / (adjugate_transpose[0] * rows[0]);
    }

    Matrix<n_rows, n_cols> invert() const 
    {
        return inverse_transpose().transpose();
    }

    Matrix<n_cols, n_rows> transpose() const
    {
        Matrix<n_cols, n_rows> ret;
        for (int i = 0; i < n_cols; i++)
        {
            for (int j = 0; j < n_rows; j++) ret[i][j] = rows[j][i];
        }
        return ret;
    }
};

template<int n_rows, int n_cols> Vec<n_cols> operator * (const Vec<n_rows> &lhs, const Matrix<n_rows, n_cols> &rhs)
{
    return (Matrix<1, n_rows> {{lhs}}*rhs)[0];
}

template<int n_rows, int n_cols> Vec<n_rows> operator * (const Matrix<n_rows, n_cols> &lhs, const Vec<n_cols> &rhs)
{
    Vec<n_rows> ret;
    for (int i = 0; i < n_rows; i++) ret[i] = lhs[i] * rhs;
    return ret;
}

template<int R1, int C1, int C2> Matrix<R1, C2> operator * (const Matrix<R1, C1> &lhs, const Matrix<C1, C2> &rhs)
{
    Matrix<R1, C2> result;
    for (int i = 0; i < R1; i++)
    {
        for (int j = 0; j < C2; j++)
        {
            for (int k = 0; k < C1; k++) result[i][j] = lhs[i][k] * rhs[j][k];
        }
    }
    return result;
}

template<int n_rows, int n_cols> Matrix<n_rows, n_cols> operator * (const Matrix<n_rows, n_cols> &lhs, const double &val)
{
    Matrix<n_rows, n_cols> result;
    for (int i = 0; i < n_rows; i++) result[i] = lhs[i] * val;
    return result;
}

template<int n_rows, int n_cols> Matrix<n_rows, n_cols> operator / (const Matrix<n_rows, n_cols> &lhs, const double &val)
{
    Matrix<n_rows, n_cols> result;
    for (int i = 0; i < n_rows; i++) result[i] = lhs[i] / val;
    return result;
}

template<int n_rows, int n_cols> Matrix<n_rows, n_cols> operator + (const Matrix<n_rows, n_cols> &lhs, const Matrix<n_rows, n_cols> &rhs)
{
    Matrix<n_rows, n_cols> result;
    for (int i = 0; i < n_rows; i++)
    {
        for (int j = 0; j < n_cols; j++) result[i][j] = lhs[i][j] + rhs[i][j];
    }
    return result;
}

template<int n_rows, int n_cols> Matrix<n_rows, n_cols> operator - (const Matrix<n_rows, n_cols> &lhs, const Matrix<n_rows, n_cols> &rhs)
{
    Matrix<n_rows, n_cols> result;
    for (int i = 0; i < n_rows; i++)
    {
        for (int j = 0; j < n_cols; j++) result[i][j] = lhs[i][j] - rhs[i][j];
    }
    return result;
}

template<int n> struct dt 
{
    static double det(const Matrix<n, n> &src)
    {
        double ret = 0;
        for (int i = 0; i < n; i++) ret += src[0][i] * src.cofactor(0, i);
        return ret;
    }
};

template<> struct dt<1>
{
    static double det(const Matrix<1, 1> &src)
    {
        return src[0][0];
    }
};