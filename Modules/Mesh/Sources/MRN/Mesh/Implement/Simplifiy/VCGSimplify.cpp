#include "MRN/Mesh/Implement/Simplifiy/VCGSimplify.h"
#include "MRN/Mesh/Mesh.h"
#include <vcg/complex/base.h>
#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>
namespace MRN {
void VCGSimplify::operator()(Mesh* mesh_, float level_)
{
    auto& nativemesh = mesh_->getNativMesh();
}
}   // namespace MRN