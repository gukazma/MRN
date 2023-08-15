#pragma once
#include <MRN/FileSystem/Tile.h>
#include <boost/filesystem/path.hpp>
namespace MRN {
class SoarscapeOSGB
{
public:
    SoarscapeOSGB() = default;
    SoarscapeOSGB(const boost::filesystem::path& path_)
        : m_path(path_)
    {}
    // check data format
    bool check();
    void calculateTileArray();
    void merge();
    void cutCake();
    void writeTile();

private:
    boost::filesystem::path m_path;
    TileArray               m_tileArray;
};

}   // namespace MRN
