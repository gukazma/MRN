#include <gtest/gtest.h>
#include <boost/process.hpp>
#include <MRN/MRN/OSGB/SoarscapeOSGB.h>
#include <MRN/MRN.hpp>
TEST(MRN, MRN)
{
    auto                         env         = boost::this_process::environment();
    boost::filesystem::path      MRNDataPath = env["MRNDATA"].to_string();
    auto                         inputdir    = MRNDataPath / "osgb/MRN";
    MRN::MRN<MRN::SoarscapeOSGB> mrn(inputdir);
    EXPECT_TRUE(mrn.check());

    mrn.calculateTileArray();
    mrn.merge();
    mrn.writeTile();
}