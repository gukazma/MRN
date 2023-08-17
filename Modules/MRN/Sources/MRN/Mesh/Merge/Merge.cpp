#include "meshReconstruction.h"
#include <MRN/Mesh/Merge/Merge.h>
#include <boost/filesystem.hpp>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/space/index/kdtree/kdtree.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
namespace fs = boost::filesystem;
namespace MRN {
Merge::Merge(const boost::filesystem::path& path_)
    : m_path(path_)
{
    boost::filesystem::create_directory(m_path.parent_path() / "merge");
    m_mergePath = m_path.parent_path() / "merge/merge.ply";
    m_plymcout  = m_path.parent_path() / "merge/plymcout.ply";
}
void Merge::init()
{
    MyMesh mergemesh;
    for (const auto& dir : fs::directory_iterator(m_path)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_")) continue;
        // std::cout << "Path: " << dir << std::endl;
        for (const auto& file : fs::directory_iterator(dir.path())) {
            if (boost::algorithm::contains(file.path().filename().string(), "_0.osgb")) {
                Mesh mesh(file.path());
                vcg::tri::Append<MyMesh, MyMesh>::Mesh(mergemesh, mesh.getNativMesh());
            }
        }
    }
    
    vcg::tri::io::ExporterPLY<MyMesh>::Save(mergemesh, m_mergePath.generic_string().c_str(),
                                            vcg::tri::io::Mask::IOM_VERTCOLOR);
}
void Merge::process()
{
    std::string                output = "-o" + m_plymcout.generic_path().string();
    std::array<const char*, 4> args   = {
        " ", "-V4", m_mergePath.generic_string().c_str(), output.c_str()};

    meshReconstruction(4, args.data());
    MyMesh mergemesh;
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mergemesh, m_mergePath.generic_path().string().c_str());
    std::cout << "===================================================" << std::endl;
    std::cout << "Generate mesh color" << std::endl;
    // Construction of the kdTree
    vcg::ConstDataWrapper<MyMesh::VertexType::CoordType> wrapperVcg(
        &mergemesh.vert[0].P(),
        mergemesh.vert.size(),
        size_t(mergemesh.vert[1].P().V()) - size_t(mergemesh.vert[0].P().V()));
    vcg::KdTree<MyMesh::ScalarType> kdTreeVcg(wrapperVcg);


    MyMesh mesh;
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh, m_plymcout.generic_path().string().c_str());
    for (size_t i = 0; i < mesh.vert.size(); i++) {
        auto&        v     = mesh.vert[i];
        unsigned int index = 0;
        float        minidistance;
        kdTreeVcg.doQueryClosest(v.P(), index, minidistance);

        auto& closestV = mergemesh.vert[index];
        v.C()          = closestV.C();
    }
    vcg::tri::io::ExporterPLY<MyMesh>::Save(
        mesh, m_plymcout.generic_path().string().c_str(), vcg::tri::io::Mask::IOM_VERTCOLOR);
    boost::filesystem::remove(m_mergePath.generic_path().string().c_str());
}
void Merge::getMerged(MyMesh& mesh)
{
    vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh, m_plymcout.generic_path().string().c_str());
    boost::filesystem::remove(m_plymcout.generic_path().string().c_str());
}
}   // namespace MRN