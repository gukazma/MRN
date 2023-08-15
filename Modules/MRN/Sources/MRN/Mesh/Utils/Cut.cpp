#include "MRN/Mesh/Utils/Cut.h"
namespace MRN {
void cut(MyMesh& mesh, const vcg::Box3f& bbox)
{
    for (size_t i = 0; i < mesh.face.size(); i++) {
        auto& face = mesh.face[i];
        auto& v0   = face.V(0);
        auto& v1   = face.V(1);
        auto& v2   = face.V(2);
        if (bbox.IsIn(v0->P()) || bbox.IsIn(v1->P()) || bbox.IsIn(v2->P())) continue;

        vcg::tri::Allocator<MyMesh>::DeleteFace(mesh, face);
    }
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
}

}   // namespace MRN