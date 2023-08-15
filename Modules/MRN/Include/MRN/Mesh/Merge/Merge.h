#pragma once
#include <MRN/Mesh/Mesh.h>
#include <vector>
namespace MRN {
class Merge
{
public:
    Merge();

    void init(std::vector<Mesh> meshs);

    void process();

    void getMerged(MyMesh& mesh);
};
}   // namespace MRN