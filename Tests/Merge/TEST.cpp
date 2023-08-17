#include <MRN/Mesh/Mesh.h>
#include <MRN/Mesh/Merge/Merge.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <gtest/gtest.h>
#include <boost/algorithm/string.hpp>
#include <wrap/io_trimesh/export_ply.h>
namespace fs = boost::filesystem;
TEST(MRN, MERGE)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path inputDir = MRNDataPath / "osgb/MRN";
    std::vector<MRN::Mesh>  meshs;
    MRN::Merge merge(inputDir);
    merge.init();
    merge.process();
    MyMesh outputmesh;
    merge.getMerged(outputmesh);

    vcg::tri::io::ExporterPLY<MyMesh>::Save(outputmesh, "test.ply");
}