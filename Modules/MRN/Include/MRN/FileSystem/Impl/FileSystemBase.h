#pragma once
#include <MRN/FileSystem/Tile.h>
namespace MRN
{
	class FileSystemBase
	{
    public:
        virtual void getTileArray(TileArray& tileArray) {}
        virtual ~FileSystemBase() = default;
        FileSystemBase() = default;
        FileSystemBase(const boost::filesystem::path& path_);
    protected:
        boost::filesystem::path m_dir;
	};
}