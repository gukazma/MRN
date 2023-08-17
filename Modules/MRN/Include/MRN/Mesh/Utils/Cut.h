#pragma once
#include <MRN/Mesh/Types.h>
namespace MRN {
void cut(MyMesh&  outputMesh, MyMesh& originmesh, const vcg::Box3f& bbox);
}