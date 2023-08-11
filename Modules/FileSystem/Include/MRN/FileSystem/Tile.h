#pragma once
#include <boost/filesystem/path.hpp>
#include <boost/optional/optional.hpp>
namespace MRN
{
struct Tile
{
    int                     level;
    boost::filesystem::path tilePath;
    std::vector<Tile>       parentPaths;
    double                  threshold;
    float                   simplifyLevel = 1.0f;
    Tile() { 
        level = 0;
        tilePath = "";
        threshold = 0;
    };
    Tile(const Tile& other)
        : level(other.level)
        , tilePath(other.tilePath)
        , parentPaths(other.parentPaths)
        , threshold(other.threshold)
        , simplifyLevel(other.simplifyLevel){};
};
class TileArray : public std::vector<std::vector<std::vector<boost::optional<Tile>>>>
{
    size_t level() { return this->size(); }
    size_t rows()
    {
        if (this->size() == 0) return 0;
        return (*this)[0].size();
    }

    size_t columns()
    {
        if (this->size() == 0) return 0;
        
        if((*this)[0].size() == 0) return 0;

        return (*this)[0][0].size();
    }
};
}