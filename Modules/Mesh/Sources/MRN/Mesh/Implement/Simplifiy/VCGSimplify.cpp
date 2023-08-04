#include "MRN/Mesh/Implement/Simplifiy/VCGSimplify.h"
#include "MRN/Mesh/Mesh.h"
#include <vcg/complex/base.h>
namespace MRN {
void VCGSimplify::operator()(Mesh* mesh_, float level_)
{
    auto& nativemesh = mesh_->getNativMesh();
}
}   // namespace MRN