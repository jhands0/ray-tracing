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
        ) : center(c), radius(r), radius_sqr(r * r), surface_color(sc), emission_color(ec), transparency(transp), reflection(refl) {}

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

float mix(const float &a, const float &b, const float &mix)
{
    return b * mix + a * (1 - mix);
}

Vec3f trace(const Vec3f &ray_origin, const Vec3f &ray_dir, const std::vector<Sphere> &spheres, const int &depth)
{
    float tnear = INFINITY;
    const Sphere* sphere = NULL;

    for (unsigned i = 0; i < spheres.size(); i++)
    {
        float t0 = INFINITY, t1 = INFINITY;
        if (spheres[i].intersect(ray_origin, ray_dir, t0, t1))
        {
            if (t0 < 0) t0 = t1;
            if (t0 < tnear)
            {
                tnear = t0;
                sphere = &spheres[i];
            }
        }
    }

    if (!sphere) return Vec3f(2);
    Vec3f surface_color = 0;
    Vec3f phit = ray_origin + ray_dir * tnear;
    Vec3f nhit = phit - sphere->center;
    nhit.normalize();

    float bias = 1e-4;
    bool inside = false;
    if (ray_dir.dot(nhit) > 0) nhit = -nhit, inside = true;
    if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH)
    {
        float facing_ratio = -ray_dir.dot(nhit);
        float fresnel_effect = mix(pow(1 - facing_ratio, 3), 1, 0.1);
        Vec3f refl_dir = ray_dir - nhit * 2 * ray_dir.dot(nhit);
        refl_dir.normalize();
        Vec3f reflection = trace(phit + nhit * bias, refl_dir, spheres, depth + 1);
        Vec3f refraction = 0;

        if (sphere->transparency)
        {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior;
            float cosi = -nhit.dot(ray_dir);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            Vec3f refr_dir = ray_dir * eta + nhit * (eta * cosi - sqrt(k));
            refr_dir.normalize();
            refraction = trace(phit - nhit * bias, refr_dir, spheres, depth + 1);
        }

        surface_color = (reflection * fresnel_effect + refraction * (1 - fresnel_effect) * sphere->transparency) * sphere->surface_color;
    }

    else
    {
        for (unsigned i = 0; i < spheres.size(); i++)
        {
            if (spheres[i].emission_color.x > 0)
            {
                Vec3f transmission = 1;
                Vec3f light_dir = spheres[i].center - phit;
                light_dir.normalize();
                for (unsigned j = 0; i < spheres.size(); j++)
                {
                    if (i != j)
                    {
                        float t0, t1;
                        if (spheres[j].intersect(phit + nhit * bias, light_dir, t0, t1))
                        {
                            transmission = 0;
                            break;
                        }
                    }
                }
                surface_color += sphere->surface_color * transmission * std::max(float(0), nhit.dot(light_dir)) * spheres[i].emission_color;
            }
        }
    }

    return surface_color + sphere->emission_color;
}

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