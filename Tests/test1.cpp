#include <gtest/gtest.h>
#include <MRN/Mesh/Mesh.h>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
TEST(MRN, JOIN)
{
    std::cout << "Test1" << std::endl;
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    MRN::Mesh               mesh14(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    MRN::Mesh               mesh15(MRNDataPath / "osgb/Tile_+000_+015/Tile_+000_+015_15_0.osgb");

    mesh14.join(mesh15);
    mesh14.removeSmallComponents(50);
    mesh14.simplify();
    mesh14.write(MRNDataPath / "output/tile14+15_replace+removeSmall+simplifyLT_test.ply");
}