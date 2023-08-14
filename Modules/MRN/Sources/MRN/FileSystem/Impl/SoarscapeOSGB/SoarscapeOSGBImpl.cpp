#include <MRN/Mesh/Mesh.h>
#include <MRN/FileSystem/Impl/SoarscapeOSGB/SoarscapeOSGBImpl.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <iostream>

namespace fs = boost::filesystem;
namespace MRN
{
SoarscapeOSGBImpl::SoarscapeOSGBImpl(const boost::filesystem::path& path_)
    : FileSystemBase(path_)
{}
void SoarscapeOSGBImpl::getTileArray(TileArray& tileArray) {
    if (!fs::exists(m_dir) || !fs::is_directory(m_dir)) {
        std::cout << "Invalid directory: " << m_dir << std::endl;
        return;
    }

    // 提取所有tile文件夹
    std::vector<boost::filesystem::directory_entry> tileDir;
    for (const auto& dir : fs::directory_iterator(m_dir)) {
        if (!fs::is_directory(dir)) continue;
        if (!boost::algorithm::contains(dir.path().filename().string(), "Tile_+")) continue;
        tileDir.push_back(dir);
        //std::cout << "Path: " << dir << std::endl;
    }

    // 提取原始最小层级
    int minLevel = 0;
    if (!tileDir.empty())
    {
        for (const auto& file : fs::directory_iterator(*tileDir.begin())) {
            if (boost::algorithm::contains(file.path().filename().string(), "_0.osgb"))
            {
                std::string fileName = file.path().filename().string();
                minLevel = atoi(fileName.substr(fileName.find("_0.osgb") - 2, 2).c_str());
                break;
            }
        }
    }
    std::string minLevelName;
    if (minLevel != 0)
        minLevelName = std::to_string(minLevel) + "_0.osgb";
    else
    {
        std::cout << "Failed to extract minLevel!" << std::endl;
        return;
    }
    
    // 获得所有的最小层级osgb
    std::unique_ptr<tileCoord> maxTile(
        new tileCoord(std::numeric_limits<int>::min(), std::numeric_limits<int>::min()));
    std::unique_ptr<tileCoord> minTile(
        new tileCoord(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()));
    std::map<tileCoord, Tile> coordTileMap;
    for (const auto& dir : tileDir)
    {
        for (const auto& file : fs::directory_iterator(dir)) {
            std::string fileName = file.path().filename().string();
            if (boost::algorithm::contains(fileName, minLevelName)) {
                
                int  tileNumberX = atoi(fileName.substr(fileName.find("Tile_+") + 6, 3).c_str());
                int tileNumberY = atoi(fileName.substr(fileName.find(
                    tile_intToString(tileNumberX, 3) + "_+") + 5, 3).c_str());

                tileCoord tc(tileNumberX, tileNumberY);
                if (minTile->x > tc.x) minTile->x = tc.x;
                if (minTile->y > tc.y) minTile->y = tc.y;
                if (maxTile->x < tc.x) maxTile->x = tc.x;
                if (maxTile->y < tc.y) maxTile->y = tc.y;

                Tile t;
                t.level = minLevel;
                t.tilePath = file.path();
                MRN::Mesh mesh(file.path());
                t.box            = mesh.getNativMesh().bbox;
                coordTileMap[tc] = t;
            }
        }
    }

    fs::path RootDataPath = m_dir.parent_path() / "RootData";
    if (!fs::exists(RootDataPath)) fs::create_directory(RootDataPath); 
    int l = 0;
    while (true) {
        int tax = 0, tay = 0;
        minLevel--;
        std::vector < std::vector<boost::optional<Tile>>> temp_xy;
        for (int t_x = minTile->x; t_x < maxTile->x; t_x += 2) {
            std::vector<boost::optional<Tile>> temp_x;
            for (int t_y = minTile->y; t_y < maxTile->y; t_y += 2) {
                Tile t;
                vcg::Box3<float> tempBox;
                float            max = FLT_MAX;
                float            min = -FLT_MAX;
                tempBox.max          = vcg::Point3f(min, min, min);
                tempBox.min          = vcg::Point3f(max, max, max);
                auto updateBbox      = [](vcg::Box3<float>& tBox1, vcg::Box3<float> tBox2) {
                    if (tBox1.max.X() < tBox2.max.X()) tBox1.max.X() = tBox2.max.X();
                    if (tBox1.max.Y() < tBox2.max.Y()) tBox1.max.Y() = tBox2.max.Y();
                    if (tBox1.max.Z() < tBox2.max.Z()) tBox1.max.Z() = tBox2.max.Z();
                    if (tBox1.min.X() > tBox2.min.X()) tBox1.min.X() = tBox2.min.X();
                    if (tBox1.min.Y() > tBox2.min.Y()) tBox1.min.Y() = tBox2.min.Y();
                    if (tBox1.min.Z() > tBox2.min.Z()) tBox1.min.Z() = tBox2.min.Z();
                };

                if (l == 0)
                {
                    if (coordTileMap.find(tileCoord(t_x, t_y)) != coordTileMap.end()) {
                        Tile temp = (*coordTileMap.find(tileCoord(t_x, t_y))).second;
                        updateBbox(tempBox, temp.box);
                        t.parentPaths.push_back(temp);
                    }
                    if (coordTileMap.find(tileCoord(t_x, t_y + 1)) != coordTileMap.end()) {
                        Tile temp = (*coordTileMap.find(tileCoord(t_x, t_y + 1))).second;
                        updateBbox(tempBox, temp.box);
                        t.parentPaths.push_back(temp);
                    }
                    if (coordTileMap.find(tileCoord(t_x + 1, t_y)) != coordTileMap.end()) {
                        Tile temp = (*coordTileMap.find(tileCoord(t_x + 1, t_y))).second;
                        updateBbox(tempBox, temp.box);
                        t.parentPaths.push_back(temp);
                    }
                    if (coordTileMap.find(tileCoord(t_x + 1, t_y + 1)) != coordTileMap.end()) {
                        Tile temp = (*coordTileMap.find(tileCoord(t_x + 1, t_y + 1))).second;
                        updateBbox(tempBox, temp.box);
                        t.parentPaths.push_back(temp);
                    }
                }
                else
                {
                    t.parentPaths.push_back(tileArray[l - 1][t_x][t_y].get());
                    updateBbox(tempBox, tileArray[l - 1][t_x][t_y].get().box);
                    if (t_y + 1 < maxTile->y)
                    {
                        t.parentPaths.push_back(tileArray[l - 1][t_x][t_y + 1].get());
                        updateBbox(tempBox, tileArray[l - 1][t_x][t_y + 1].get().box);
                    }
                    if (t_x + 1 < maxTile->x) 
                    {
                        t.parentPaths.push_back(tileArray[l - 1][t_x + 1][t_y].get());
                        updateBbox(tempBox, tileArray[l - 1][t_x + 1][t_y].get().box);
                    }
                    if (t_x + 1 < maxTile->x && t_y + 1 < maxTile->y) 
                    {
                        t.parentPaths.push_back(tileArray[l - 1][t_x + 1][t_y + 1].get());
                        updateBbox(tempBox, tileArray[l - 1][t_x + 1][t_y + 1].get().box);
                    }
                }

                if (t.parentPaths.empty()) {
                    boost::optional<Tile> t_null;
                    temp_x.push_back(t_null);
                }
                else {
                    t.box                   = tempBox;
                    t.level                 = minLevel;
                    std::string rootDir     = "RootTiles_L" + std::to_string(minLevel);
                    std::string newTileName = "Tile_+" + tile_intToString(tax, 4) + "_+" +
                                              tile_intToString(tay, 4) + "_L" +
                                              std::to_string(minLevel) + ".osgb";
                    fs::path tempPath = RootDataPath / rootDir;
                    if (!fs::exists(tempPath)) fs::create_directory(tempPath);
                    t.tilePath      = tempPath / newTileName;
                    t.simplifyLevel = 1.0f;
                    temp_x.push_back(t);
                }

                tay++;
            }
            temp_xy.push_back(temp_x);
            tax++;
        }

        tileArray.push_back(temp_xy);
        if (tax == 1 && tay == 1)
            break;
        l++;
        maxTile->x = tileArray[l - 1].size();
        maxTile->y = tileArray[l - 1][0].size();
        minTile->x = 0;
        minTile->y = 0;
    }
}

// 瓦块坐标转化为字符串 eg：1 -> 001
std::string SoarscapeOSGBImpl::tile_intToString(int tileNumber, int n) {
    std::string sTile = std::to_string(tileNumber);
    while (sTile.length() < n) {
        sTile = "0" + sTile;
    }
    return sTile;
}

}