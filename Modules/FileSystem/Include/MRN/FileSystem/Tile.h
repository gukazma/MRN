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
};
class TileArray : public std::vector<std::vector<boost::optional<Tile>>>
{
    size_t rows() { return this->size(); }
    size_t columns()
    {
        if (this->size() == 0) return 0;
        return (*this)[0].size();
    }

    float simplify_level = 1.0f;
};
}