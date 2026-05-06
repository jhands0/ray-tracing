#include <fstream>
#include <sstream>
#include "model.h"

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
}

int Model::n_verts() const { return verts.size(); }
int Model::n_faces() const { return faces.size(); }

Vec3 Model::vert(const int i) const { return verts[i]; }

Vec3 Model::vert(const int face_i, const int nth_vert) const { return verts[faces[face_i + nth_vert]]; }