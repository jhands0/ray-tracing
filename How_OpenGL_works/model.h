#include <vector>
#include "vec.h"

class Model 
{
    std::vector<Vec3> verts = {};
    std::vector<int> faces = {};
public:
    Model(const std::string filename);
    int n_verts() const;
    int n_faces() const;
    Vec3 vert(const int i) const;
    Vec3 vert(const int face_i, const int nth_vert) const;
};