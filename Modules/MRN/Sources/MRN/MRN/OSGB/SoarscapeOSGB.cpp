#include "MRN/MRN/OSGB/SoarscapeOSGB.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
namespace MRN
{
bool SoarscapeOSGB::check()
{
    for (const auto& dir : fs::directory_iterator(m_path)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_+")) continue;
        std::vector<std::string> tokens;
        const std::string        sep = "+";
        boost::split(tokens, dir.path().filename().string(), boost::is_any_of(sep));
        if (tokens.size() == 3) return true;
    }
    return false;
}
void SoarscapeOSGB::calculateTileArray() {}
void SoarscapeOSGB::merge() {}
void SoarscapeOSGB::cutCake() {}
void SoarscapeOSGB::writeTile() {}
}
