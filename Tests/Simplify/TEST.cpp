#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <MRN/Mesh/Mesh.h>
#include <MRN/Mesh/Simplify.h>
#include <MRN/Mesh/Implement/Simplifiy/VCGSimplify.h>
TEST(Simplify, VCG)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    MRN::Mesh               mesh(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    MRN::simplify<MRN::VCGSimplify>(&mesh, 10.0f);
}
