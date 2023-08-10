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
    for (const auto& dir : fs::directory_iterator(inputDir)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_+")) continue;
        //std::cout << "Path: " << dir << std::endl;
        boost::filesystem::path meshpath = dir.path() / dir.path().filename();
        meshpath  += boost::filesystem::path("_15_0.osgb");
        std::cout << "Path: " << meshpath << std::endl;
        meshs.emplace_back(std::move(MRN::Mesh(meshpath)));
    }
    MRN::Merge merge;
    merge.init(meshs);
    merge.process();
    MyMesh outputmesh;
    merge.getMerged(outputmesh);

    vcg::tri::io::ExporterPLY<MyMesh>::Save(outputmesh, "test.ply");
}