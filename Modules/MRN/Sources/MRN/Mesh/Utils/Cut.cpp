#include "MRN/Mesh/Utils/Cut.h"
#include <unordered_map>
#include <boost/optional.hpp>
namespace MRN {
void cut(MyMesh& outputMesh, MyMesh& originmesh, const vcg::Box3f& bbox)
{
    std::unordered_map<vcg::Point3f, boost::optional<int>> vimap; 
    for (size_t i = 0; i < originmesh.face.size(); i++) {
        auto& face = originmesh.face[i];
        if (face.IsD()) continue;
        auto& v0   = face.V(0);
        auto& v1   = face.V(1);
        auto& v2   = face.V(2);
        if (bbox.IsIn(v0->P()) || bbox.IsIn(v1->P()) || bbox.IsIn(v2->P())) {
            int index = vimap.size();
            if (!vimap[v0->P()].has_value()) {
                auto &v = vcg::tri::Allocator<MyMesh>::AddVertex(outputMesh, v0->P());
                (*v).C().Import(v0->C());
                vimap[v0->P()] = index;
            }
            index = vimap.size();
            if (!vimap[v1->P()].has_value()) {
                auto& v = vcg::tri::Allocator<MyMesh>::AddVertex(outputMesh, v1->P());
                (*v).C().Import(v1->C());
                vimap[v1->P()] = index;
            }
            index = vimap.size();
            if (!vimap[v2->P()].has_value()) {
                auto& v = vcg::tri::Allocator<MyMesh>::AddVertex(outputMesh, v2->P());
                (*v).C().Import(v2->C());
                vimap[v2->P()] = index;
            }
            vcg::tri::Allocator<MyMesh>::AddFace(
                outputMesh, vimap[v0->P()].value(), vimap[v1->P()].value(), vimap[v2->P()].value());
        }
    }
}

}   // namespace MRN