#include "MRN/MRN/OSGB/MRNSoarscapeOSGB.h"
#include <MRN/FileSystem/FileSystem.h>
#include <MRN/Mesh/Merge/Merge.h>
#include <MRN/Mesh/Mesh.h>
#include <MRN/Mesh/Utils/Cut.h>
#include <MRN/Mesh/Utils/Simplify.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <wrap/io_trimesh/export_ply.h>
#include <osg/PagedLOD>
#include <osgDB/WriteFile>
#include <tbb/tbb.h>
namespace fs = boost::filesystem;
namespace MRN {
bool MRNSoarscapeOSGB::check()
{
    for (const auto& dir : fs::directory_iterator(m_path)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_")) continue;
        std::vector<std::string> tokens;
        const std::string        sep = "+";
        boost::split(tokens, dir.path().filename().string(), boost::is_any_of(sep));
        if (tokens.size() == 3) return true;
    }
    return false;
}
void MRNSoarscapeOSGB::calculateTileArray()
{
    MRN::FileSystem filesystem(m_path);
    filesystem.getTileArray(m_tileArray);
}
void MRNSoarscapeOSGB::merge()
{
    std::cout << "Begin merge mesh .......... " << std::endl;
    MyMesh     mergeMesh;
    MRN::Merge merge(m_path);
    merge.init();
    merge.process();
    merge.getMerged(mergeMesh);
    for (size_t level = 0; level < m_tileArray.size(); level++) {
        std::cout << "Get level mesh......" << std::endl;
        MRN::simplify(mergeMesh, 0.5);
        vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mergeMesh);
        std::cout << "level ====================== " << level << std::endl;
        const auto&            tileArray = m_tileArray[level];
        tbb::parallel_for(tbb::blocked_range<size_t>(0, tileArray.size()),
                          [&](const tbb::blocked_range<size_t>& r) {
                              for (size_t x = r.begin(); x != r.end(); x++) {
                                  const auto& tileVector = tileArray[x];
                                  for (size_t y = 0; y < tileVector.size(); y++) {
                                      if (!tileVector[y].has_value()) continue;
                                      const auto& tile = tileVector[y].value();
                                      MyMesh      cutmesh;
                                      MRN::cut(cutmesh, mergeMesh, tile.box);
                                      boost::filesystem::path savePath =
                                          tile.tilePath.parent_path() / tile.tilePath.stem();
                                      savePath += ".ply";
                                      vcg::tri::io::ExporterPLY<MyMesh>::Save(
                                          cutmesh,
                                          savePath.generic_string().c_str(),
                                          vcg::tri::io::Mask::IOM_VERTCOLOR);
                                  }
                              }
            });
        
    }
}
void MRNSoarscapeOSGB::writeTile() {
    for (size_t level = 0; level < m_tileArray.size(); level++) {
        std::cout << "writeTile ====================== " << level << std::endl;
        const auto&            tileArray = m_tileArray[level];
        for (size_t x = 0; x < tileArray.size(); x++) {
            const auto& tileVector = tileArray[x];
            for (size_t y = 0; y < tileVector.size(); y++) {
                if (!tileVector[y].has_value()) continue;
                std::string tilePath = tileVector[y]->tilePath.generic_string();
                boost::filesystem::path meshPath =
                    tilePath.substr(0, tilePath.find(".osgb")) + ".ply";
                Mesh        mesh(meshPath);
                mesh.write(tileVector[y].get());
                boost::filesystem::remove(meshPath);
            }
        }
    }
    
    // 生成根节点
    osg::ref_ptr<osg::PagedLOD> rootNode = new osg::PagedLOD;
    rootNode->setDatabasePath("");
    Tile        lastTile         = m_tileArray.back().back().back().get();
    std::string lastTileFileName = "./RootData/" +
                                   lastTile.tilePath.parent_path().filename().generic_string() +
                                   "/" + lastTile.tilePath.filename().generic_string();
    rootNode->setFileName(0, lastTileFileName);
    rootNode->setRange(0, 0, 1e30);
    std::string rootSavePath =
        lastTile.tilePath.parent_path().parent_path().parent_path().generic_string() +
        "/RootNode.osgb";
    osgDB::writeNodeFile(*(rootNode.get()), rootSavePath);
}
}   // namespace MRN
