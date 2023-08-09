#include <gtest/gtest.h>
#include <MRN/FileSystem/FileSystem.h>
#include <boost/process.hpp>
TEST(MRN, soarscapeFileSystem)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path inputDir    = MRNDataPath / "osgb/MRN";
    MRN::FileSystem filesystem(inputDir);
    MRN::TileArray          tilearray;
    filesystem.getTileArray(tilearray);
}