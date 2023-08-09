#include <MRN/FileSystem/Impl/SoarscapeOSGB/SoarscapeOSGBImpl.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>
namespace fs = boost::filesystem;
namespace MRN
{
SoarscapeOSGBImpl::SoarscapeOSGBImpl(const boost::filesystem::path& path_)
    : FileSystemBase(path_)
{}
void SoarscapeOSGBImpl::getTileArray(TileArray& tileArray) {
    if (!fs::exists(m_dir) || !fs::is_directory(m_dir)) {
        std::cout << "Invalid directory: " << m_dir << std::endl;
        return;
    }

    for (const auto& dir : fs::directory_iterator(m_dir)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_+")) continue;
        std::cout << "Path: " << dir << std::endl;
    }
}
}