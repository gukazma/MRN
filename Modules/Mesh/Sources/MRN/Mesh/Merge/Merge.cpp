#include <MRN/Mesh/Merge/Merge.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include "meshReconstruction.h"
#include <boost/filesystem.hpp>
namespace MRN {
Merge::Merge() {}
void Merge::init(std::vector<Mesh> meshs) {
    MyMesh mergemesh;
    for (auto mesh : meshs) {
        vcg::tri::Append<MyMesh, MyMesh>::Mesh(mergemesh, mesh.getNativMesh());
    }

    vcg::tri::io::ExporterPLY<MyMesh>::Save(mergemesh, "merge.ply", vcg::tri::io::Mask::IOM_VERTCOLOR);

}
void Merge::process() {
    std::array<const char*, 3> args = {" ", "-V5", "merge.ply"};

    meshReconstruction(3, args.data());

    //boost::filesystem::remove("merge.ply");
}
void Merge::getMerged(MyMesh& mesh)
{
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh, "plymcout.ply");
}
}   // namespace MRN