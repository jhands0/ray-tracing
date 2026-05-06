// Compilation instructions:
// cmake -S . -B ./build
// cmake --build ./build

#include <cmath>
#include "tgaimage.h"

constexpr int width = 128;
constexpr int height = 128;

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

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &framebuffer)
{
    int box_min_x = std::min(std::min(ax, bx), cx);
    int box_min_y = std::min(std::min(ay, by), cy);
    int box_max_x = std::max(std::max(ax, bx), cx);
    int box_max_y = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    
#pragma omp parallel for
    for (int x = box_min_x; x <= box_max_x; x++)
    {
        for (int y = box_min_y; y <= box_max_y; y++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if (std::min(alpha, std::min(beta, gamma)) > .1) 
                framebuffer.set(x, y, {z});
        }
    }
}

int main(int argc, char **argv)
{
    TGAImage framebuffer(width, height, TGAImage::GRAYSCALE);

    int ax = 17, ay = 4, az = 13;
    int bx = 55, by = 39, bz = 128;
    int cx = 23, cy = 59, cz = 255;

    triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}