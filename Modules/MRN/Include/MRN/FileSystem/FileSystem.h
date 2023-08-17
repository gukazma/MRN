#pragma once
#include "Tile.h"
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <MRN/Types.h>
#include <memory>
#include <vector>
namespace MRN {
class FileSystemBase;
class FileSystem
{
public:

    FileSystem(const boost::filesystem::path& dir_, VendorID type = SoarscapeOSGB);
    void getTileArray(TileArray& tileArray);

private:
    std::shared_ptr<FileSystemBase> m_impl;
};
}   // namespace MRN