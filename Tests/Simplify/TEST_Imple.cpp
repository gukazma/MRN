#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <MRN/Mesh/Mesh.h>
TEST(MRN, SimplifyImple)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path input = MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb";
    boost::filesystem::path output = MRNDataPath / "output/test.osgb";
    MRN::Mesh               mesh(input);

    mesh.simpilify(0.01);

    mesh.write(output);
}