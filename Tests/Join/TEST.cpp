#include <MRN/Mesh/Mesh.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <gtest/gtest.h>
TEST(MRN, JOIN)
{
    std::cout << "Test1" << std::endl;
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    MRN::Mesh               mesh14(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    MRN::Mesh               mesh15(MRNDataPath / "osgb/Tile_+000_+015/Tile_+000_+015_15_0.osgb");
    // mesh14.write(MRNDataPath / "output/tile14.ply");
    // mesh15.write(MRNDataPath / "output/tile15.ply");
    mesh14.join(mesh15);
    mesh14.write(MRNDataPath / "output/tile14+15.ply");
}

TEST(MRN, JOINNEWVERSION)
{
    std::cout << "JOIN NEW VERSION" << std::endl;
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path outputDir   = MRNDataPath / "output";
    MRN::Mesh               mesh0(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    MRN::Mesh               mesh1(MRNDataPath / "osgb/Tile_+000_+015/Tile_+000_+015_15_0.osgb");
    MRN::SurfaceMesh&       surfacemesh0 = mesh0.getNativMesh();
    MRN::SurfaceMesh&       surfacemesh1 = mesh1.getNativMesh();
    auto                    visbodermap0 =
        surfacemesh0.add_property_map<MRN::VertexIndex, bool>("v:is_border", false).first;
    auto visbodermap1 =
        surfacemesh1.add_property_map<MRN::VertexIndex, bool>("v:is_border", false).first;
}