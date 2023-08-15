#pragma once
#include <MRN/Mesh/Types.h>
#include <boost/filesystem/path.hpp>
#include <unordered_map>
#include <MRN/FileSystem/Tile.h>
namespace MRN
{
class MeshImplBase
{
public:
    MeshImplBase() = default;
    virtual void read(const boost::filesystem::path& path_);
    virtual void write(const boost::filesystem::path& path_);
    virtual void write(const Tile& tile);
    void simpilify(float percent_);
    MyVertex getClosest(vcg::Point3f point);

    MyMesh& getNativeMesh();

protected:
    MyMesh m_nativeMesh;
};
}   // namespace MRN