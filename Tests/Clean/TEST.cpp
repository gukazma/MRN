#include <gtest/gtest.h>
#include <MRN/Mesh/Types.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
TEST(MRN, Clean)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path inputPath   = MRNDataPath / "output/plymcout.ply";
    boost::filesystem::path outputPath   = MRNDataPath / "output/plymcout_remove.ply";
    MyMesh mesh;
    vcg::tri::io::Importer<MyMesh>::Open(mesh, inputPath.generic_path().string().c_str());

    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
    vcg::tri::Clean<MyMesh>::RemoveSmallConnectedComponentsSize(mesh, 20);
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
    vcg::tri::io::Exporter<MyMesh>::Save(mesh, outputPath.generic_path().string().c_str(), vcg::tri::io::Mask::IOM_VERTCOLOR);

}