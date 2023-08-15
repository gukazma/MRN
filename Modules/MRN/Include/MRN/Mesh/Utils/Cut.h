#pragma once
#include <MRN/Mesh/Types.h>
namespace MRN {
void cut(MyMesh& mesh, const vcg::Box3f& bbox);
}