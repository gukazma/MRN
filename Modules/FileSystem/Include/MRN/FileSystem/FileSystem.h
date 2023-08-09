#pragma once
#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include <boost/optional.hpp>
namespace MRN
{
class FileSystemBase;
class FileSystem
{
public:
    struct Tile
    {
        int level;
        boost::filesystem::path tilePath;
        boost::filesystem::path parentPath;
        double                  threshold;
    };
    enum Type
    {
        SoarscapeOSGB = 0,
    };
    class TileArray : public std::vector<std::vector<boost::optional<Tile>>>
    {
        size_t rows() { return this->size();
        }
        size_t columns() { 
            if (this->size() == 0) return 0;
            return (*this)[0].size();
        }
    };
    FileSystem(const boost::filesystem::path& dir_, Type type);
    
private:
    std::shared_ptr<FileSystemBase> m_impl;
};
}