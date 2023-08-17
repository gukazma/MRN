#include <gtest/gtest.h>
#include <boost/process.hpp>
#include <MRN/MRN.h>
TEST(MRN, MRN)
{
    auto                         env         = boost::this_process::environment();
    boost::filesystem::path      MRNDataPath = env["MRNDATA"].to_string();
    auto                         inputdir    = "D:/data/MRNLargeData/MRN";
    MRN::MRN mrn(inputdir);
    EXPECT_TRUE(mrn.check());

    mrn.calculateTileArray();
    mrn.merge();
    mrn.writeTile();
}