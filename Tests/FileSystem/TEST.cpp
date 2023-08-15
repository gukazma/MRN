#include <gtest/gtest.h>
#include <MRN/FileSystem/FileSystem.h>
#include <boost/process.hpp>
TEST(MRN, FileSystem)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path inputDir    = MRNDataPath / "osgb/MRN";
    MRN::FileSystem filesystem(inputDir);
    MRN::TileArray          tilearray;
    filesystem.getTileArray(tilearray);
    auto tp = tilearray[3][0][1]->box;
    int  tag = 0;
}