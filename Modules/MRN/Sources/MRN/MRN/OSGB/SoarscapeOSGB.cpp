#include "MRN/MRN/OSGB/SoarscapeOSGB.h"
#include <MRN/FileSystem/FileSystem.h>
#include <MRN/Mesh/Merge/Merge.h>
#include <MRN/Mesh/Mesh.h>
#include <MRN/Mesh/Utils/Cut.h>
#include <MRN/Mesh/Utils/Simplify.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <wrap/io_trimesh/export_ply.h>
namespace fs = boost::filesystem;
namespace MRN {
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
void SoarscapeOSGB::calculateTileArray()
{
    MRN::FileSystem filesystem(m_path);
    filesystem.getTileArray(m_tileArray);
}
void SoarscapeOSGB::merge()
{
    std::vector<MRN::Mesh> meshs;
    for (const auto& dir : fs::directory_iterator(m_path)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_+")) continue;
        // std::cout << "Path: " << dir << std::endl;
        for (const auto& file : fs::directory_iterator(dir.path())) {
            if (boost::algorithm::contains(file.path().filename().string(), "_0.osgb")) {
                meshs.emplace_back(std::move(MRN::Mesh(file)));
            }
        }
    }
    MyMesh     mergeMesh;
    MRN::Merge merge;
    merge.init(meshs);
    merge.process();
    merge.getMerged(mergeMesh);
    for (size_t level = 0; level < m_tileArray.size(); level++) {
        MyMesh levelMesh;
        std::cout << "Get level mesh......" << std::endl;
        vcg::tri::Append<MyMesh, MyMesh>::MeshCopyConst(levelMesh, mergeMesh);
        float simplifyPercent = 1.0f / std::pow(2, level + 1);
        std::cout << "Simplify level mesh with simplify percent: " << simplifyPercent << std::endl;
        MRN::simplify(levelMesh, simplifyPercent);
        std::cout << "level ====================== " << level << std::endl;
        const auto&            tileArray = m_tileArray[level];
        std::vector<MRN::Mesh> meshs;
        for (size_t x = 0; x < tileArray.size(); x++) {
            const auto& tileVector = tileArray[x];
            for (size_t y = 0; y < tileVector.size(); y++) {
                if (!tileVector[y].has_value()) continue;
                const auto& tile = tileVector[y].value();
                MyMesh      tileMesh;
                vcg::tri::Append<MyMesh, MyMesh>::MeshCopyConst(tileMesh, levelMesh);
                MRN::cut(tileMesh, tile.box);
                boost::filesystem::path savePath =
                    tile.tilePath.parent_path() / tile.tilePath.stem();
                savePath += ".ply";
                std::cout << "tile path: " << tile.tilePath << std::endl;
                std::cout << "savePath: " << savePath << std::endl;
                vcg::tri::io::ExporterPLY<MyMesh>::Save(
                    tileMesh, savePath.generic_string().c_str(), vcg::tri::io::Mask::IOM_VERTCOLOR);
            }
        }
    }
}
void SoarscapeOSGB::cutCake() {}
void SoarscapeOSGB::writeTile() {}
}   // namespace MRN