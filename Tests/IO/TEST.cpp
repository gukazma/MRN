#include <MRN/Mesh/Mesh.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <gtest/gtest.h>
TEST(MRN, OSGBRead)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    MRN::Mesh               mesh(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    // std::cout << MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb" << std::endl;
    mesh.write("D:/data/test.ply");
    mesh.write(MRNDataPath / "output/read.ply");
}

TEST(MRN, OSGBWrite)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    MRN::Mesh               mesh(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    // std::cout << MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb" << std::endl;
    mesh.write(MRNDataPath / "output/read.osgb");
    mesh.write(MRNDataPath / "output/read.osgt");
}