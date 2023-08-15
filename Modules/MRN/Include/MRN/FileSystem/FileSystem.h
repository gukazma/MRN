#pragma once
#include "Tile.h"
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <memory>
#include <vector>
namespace MRN {
class FileSystemBase;
class FileSystem
{
public:
    enum Type
    {
        SoarscapeOSGB = 0,
    };
    FileSystem(const boost::filesystem::path& dir_, Type type = SoarscapeOSGB);
    void getTileArray(TileArray& tileArray);

private:
    std::shared_ptr<FileSystemBase> m_impl;
};
}   // namespace MRN