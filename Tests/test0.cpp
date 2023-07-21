#include <gtest/gtest.h>
#include <MRN/Mesh/Mesh.h>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
TEST(MRN, OSGBRead)
{
    auto      env = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    MRN::Mesh mesh(MRNDataPath/"osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    mesh.write("D:/data/test.ply");
    mesh.write(MRNDataPath /"ouput/read.ply");
}