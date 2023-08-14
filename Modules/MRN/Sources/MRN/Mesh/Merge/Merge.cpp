#include <MRN/Mesh/Merge/Merge.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include "meshReconstruction.h"
#include <boost/filesystem.hpp>
#include <vcg/space/index/kdtree/kdtree.h>
#include <vcg/complex/algorithms/clean.h>
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
    std::array<const char*, 3> args = {" ", "-V4", "merge.ply"};

    meshReconstruction(3, args.data());
    MyMesh mergemesh;
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mergemesh, "merge.ply");
    std::cout << "===================================================" << std::endl;
    std::cout << "KDTree" << std::endl;
    // Construction of the kdTree
    vcg::ConstDataWrapper<MyMesh::VertexType::CoordType> wrapperVcg(
        &mergemesh.vert[0].P(),
        mergemesh.vert.size(),
        size_t(mergemesh.vert[1].P().V()) - size_t(mergemesh.vert[0].P().V()));
    vcg::KdTree<MyMesh::ScalarType> kdTreeVcg(wrapperVcg);
    

    MyMesh mesh;
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh, "plymcout.ply");
    for (size_t i = 0; i < mesh.vert.size(); i++) {
        auto& v = mesh.vert[i];
        unsigned int index = 0;
        float        minidistance;
        kdTreeVcg.doQueryClosest(v.P(), index, minidistance);
        
        auto& closestV = mergemesh.vert[index];
        v.C()          = closestV.C();
        
    }
    vcg::tri::io::ExporterPLY<MyMesh>::Save(
        mesh, "plymcout.ply", vcg::tri::io::Mask::IOM_VERTCOLOR);
    //boost::filesystem::remove("merge.ply");
}
void Merge::getMerged(MyMesh& mesh)
{
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh, "plymcout.ply");
}
}   // namespace MRN