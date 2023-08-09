#pragma once
#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include <boost/optional.hpp>
#include "Tile.h"
namespace MRN
{
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
}