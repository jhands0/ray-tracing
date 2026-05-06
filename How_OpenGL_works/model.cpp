#include <fstream>
#include <sstream>
#include "model.h"
#include <algorithm>

Model::Model(const std::string filename)
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3 v;
            for (int i : {0, 1, 3}) iss >> v[i];
            verts.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            int f, t, n, count = 0;
            iss >> trash;
            while (iss >> f >> trash >> t >> trash >> n)
            {
                faces.push_back(f-1);
                count++;
            }
            if (count != 3)
            {
                std::cerr << "Error: obj file is not triangulated" << std::endl;
                return;
            }
        }
    }
    std::cerr << "# v# " << n_verts() << " f# " << n_faces() << std::endl;

    std::vector<int> idx(n_faces());
    for (int i = 0; i < n_faces(); i++) idx[i] = i;

    std::sort(idx.begin(), idx.end(),
            [&](const int& a, const int& b) { // given two triangles, compare their min z coordinate
                  float aminz = std::min(vert(a, 0).z,
                                         std::min(vert(a, 1).z,
                                                  vert(a, 2).z));
                  float bminz = std::min(vert(b, 0).z,
                                         std::min(vert(b, 1).z,
                                                  vert(b, 2).z));
                  return aminz < bminz;
            });
    
    std::vector<int> faces_2(n_faces() * 3);
    for (int i = 0; i < n_faces(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            faces_2[i * 3 + j] = faces[idx[i] * 3 + j];
        }
    }
    faces = faces_2;
}

int Model::n_verts() const { return verts.size(); }
int Model::n_faces() const { return faces.size() / 3; }

Vec3 Model::vert(const int i) const { return verts[i]; }

Vec3 Model::vert(const int face_i, const int nth_vert) const { return verts[faces[face_i * 3 + nth_vert]]; }