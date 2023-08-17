#pragma once
#include <MRN/FileSystem/Tile.h>
#include <boost/filesystem/path.hpp>
#include <MRN/MRN/MRNBaseImpl.h>
namespace MRN {
class MRNSoarscapeOSGB : public MRNBaseImpl
{
public:
    MRNSoarscapeOSGB() = default;
    MRNSoarscapeOSGB(const boost::filesystem::path& path_)
        : m_path(path_)
    {}
    // check data format
    bool check() override;
    void calculateTileArray() override;
    void merge() override;
    void writeTile() override;

private:
    boost::filesystem::path m_path;
    TileArray               m_tileArray;
};

}   // namespace MRN
