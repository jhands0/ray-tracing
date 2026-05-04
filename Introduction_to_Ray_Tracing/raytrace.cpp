#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

template<typename T>
class Vec3
{
    public:
        T x, y, z;
        Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
        Vec3(T xx) : x(xx), y(xx), z(xx) {}
        Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
        Vec3& normalize()
        {
            T nor_sqr = length_sqr();
            if (nor_sqr > 0)
            {
                T nor_inv = 1 / sqrt(nor_sqr);
                x *= nor_inv, y *= nor_inv, z *= nor_inv;
            }
            return *this;
        }

        Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
        Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
        Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
        Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
        Vec3<T> operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
        Vec3<T> operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
        Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }

        friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
        {
            os << "[" << v.x << " " << v.y << " " << v.z << "]";
            return os;
        }

        T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
        T length_sqr() const { return x * x + y * y + z * z; }
        T length() const { return sqrt(length_sqr()); }

};

typedef Vec3<float> Vec3f;

class Sphere
{
    public:
        Vec3f center;
        float radius, radius_sqr;
        Vec3f surface_color, emission_color;
        float transparency, reflection;
        Sphere (
            const Vec3f &c,
            const float &r,
            const Vec3f &sc,
            const float &refl = 0,
            const float &transp = 0,
            const Vec3f &ec = 0
        ) : center(c), radius(r), radius_sqr(r * r), surface_color(sc), emission_color(ec), transparency(transp), reflection(refl)

        bool intersect(const Vec3f &ray_origin, const Vec3f &ray_dir, float &t0, float &t1) const
        {
            Vec3f l = center - ray_origin;
            float tca = l.dot(ray_dir);
            if (tca < 0) return false;
            float d2 = l.dot(l) - tca * tca;
            if (d2 > radius_sqr) return false;
            float thc = sqrt(radius_sqr - d2);
            t0 = tca - thc;
            t1 = tca + thc;

            return true;
        }
};

#define MAX_RAY_DEPTH 5

int main(int argc, char **argv)
{
    srand48(42);
    std::vector<Sphere> spheres;
    spheres.push_back(Sphere(Vec3f( 0.0, ), 0.0, Vec3f( 0.0, ), 0.0, 0.0));

    //Light
    spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));
    //render(spheres);

    return 0;
}