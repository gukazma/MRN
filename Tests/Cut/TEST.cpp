#include <gtest/gtest.h>
#include <MRN/Mesh/Types.h>
#include <vcg/space/box3.h>
#include <vcg/complex/algorithms/clean.h>
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
TEST(MRN, CUT)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path inputDir    = MRNDataPath / "ply/plymcout.ply";
    boost::filesystem::path output    = MRNDataPath / "output/testcut.ply";
    MyMesh                  mesh;
    vcg::tri::io::Importer<MyMesh>::Open(mesh, inputDir.generic_string().c_str());

    auto bbox =  mesh.bbox;
    vcg::Box3<float> box;
    box.min       = bbox.min;
    box.max       = box.min + vcg::Point3f{bbox.DimX() / 2, bbox.DimY() / 2, bbox.DimZ() / 2.0f};
    for (size_t i = 0; i < mesh.face.size(); i++) {
        auto& face = mesh.face[i];
        auto& v0   = face.V(0);
        auto& v1   = face.V(1);
        auto& v2   = face.V(2);
        if (box.IsIn(v0->P()) || box.IsIn(v1->P()) || box.IsIn(v2->P())) continue; 
            
        vcg::tri::Allocator<MyMesh>::DeleteFace(mesh, face);
    }
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
    vcg::tri::io::ExporterPLY<MyMesh>::Save(mesh, output.generic_string().c_str());
}