#include "MRN/MRN/OSGB/SoarscapeOSGB.h"
#include <MRN/FileSystem/FileSystem.h>
#include <MRN/Mesh/Mesh.h>
#include <MRN/Mesh/Merge/Merge.h>
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
void SoarscapeOSGB::calculateTileArray() {
    MRN::FileSystem filesystem(m_path);
    filesystem.getTileArray(m_tileArray);
}
void SoarscapeOSGB::merge() {
    std::vector<MRN::Mesh> meshs;
    for (const auto& dir : fs::directory_iterator(m_path)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_+")) continue;
        // std::cout << "Path: " << dir << std::endl;
        for (const auto& file : fs::directory_iterator(dir.path())) {
            if (boost::algorithm::contains(file.path().filename().string(), "_0.osgb"))
            {
                meshs.emplace_back(std::move(MRN::Mesh(file)));
            }
        }
    }
    MyMesh mergeMesh;
    MRN::Merge merge;
    merge.init(meshs);
    merge.process();
    merge.getMerged(mergeMesh);
}
void SoarscapeOSGB::cutCake() {}
void SoarscapeOSGB::writeTile() {}
}
