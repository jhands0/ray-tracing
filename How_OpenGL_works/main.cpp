// Compilation instructions:
// cmake -S . -B ./build
// cmake --build ./build

#include <cmath>
#include <tuple>
#include "vec.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width = 500;
constexpr int height = 500;

constexpr TGAColor white =  {255, 255, 255, 255};
constexpr TGAColor green =  {0, 255, 0, 255};
constexpr TGAColor red =    {0, 0, 255, 255};
constexpr TGAColor blue =   {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color)
{
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep)
    {
        std::swap(ax, ay);
        std::swap(bx, by);
    }

    if (ax > bx)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    int y = ay;
    int ierror = 0;
    for (int x = ax; x <= bx; x++)
    {   
        if (steep) 
            framebuffer.set(y, x, color);
        
        else 
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by - ay);
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx - ax) * (ierror > bx - ax);
    }
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy)
{
    return .5 * ((by - ay)*(bx + ax) + (cy - by)*(cx + bx) + (ay - cy)*(ax + cx));
}

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &framebuffer, TGAImage &zbuffer, TGAColor color)
{
    int box_min_x = std::min(std::min(ax, bx), cx);
    int box_min_y = std::min(std::min(ay, by), cy);
    int box_max_x = std::max(std::max(ax, bx), cx);
    int box_max_y = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area < 1) return;
    
#pragma omp parallel for
    for (int x = box_min_x; x <= box_max_x; x++)
    {
        for (int y = box_min_y; y <= box_max_y; y++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            
            // Lines to create wireframes
            //float max = std::max(std::max(alpha + beta, beta + gamma), alpha + gamma);
            //if (max > 0.90f) {}

            // Lines for rainbow triangles
            //uint8_t red = gamma * 255;
            //uint8_t green = beta * 255;
            //uint8_t blue = alpha * 255;
            //TGAColor color = {blue, green, red};

            unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if (z <= zbuffer.get(x, y)[0]) continue;

            framebuffer.set(x, y, color);
            zbuffer.set(x, y, {z});
        }
    }
}

Vec3 rotate(Vec3 v)
{
    constexpr double angle = M_PI / 6;
    constexpr Matrix<3, 3> Ry = {{
        {std::cos(angle), 0, std::sin(angle)},
        {0, 1, 0},
        {-std::sin(angle), 0, std::cos(angle)}
    }};
    return Ry * v;
}

// Scale model to dimensions of output file
std::tuple<int, int, int> project(Vec3 vert)
{
    return { 
        (vert.x + 1.) * width / 2, 
        (vert.y + 1.) * height / 2, 
        (vert.z + 1.) * 255./2 
    };
}

int main(int argc, char **argv)
{
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    if (argc < 2)
    {
        int ax = 17, ay = 4, az = 13;
        int bx = 55, by = 39, bz = 128;
        int cx = 23, cy = 59, cz = 255;

        triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer, zbuffer, red);
    }
    else if (argc == 2)
    {
        Model model = Model(argv[1]);

        for (int i = 0; i < model.n_faces(); i++)
        {
            auto [ax, ay, az] = project(rotate(model.vert(i, 0)));
            auto [bx, by, bz] = project(rotate(model.vert(i, 1)));
            auto [cx, cy, cz] = project(rotate(model.vert(i, 2)));
            TGAColor color;
            for (int c = 0; c < 3; c++) color[c] = std::rand() % 255;
            triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer, zbuffer, color);
        }
    }
    else 
    {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    return 0;
}