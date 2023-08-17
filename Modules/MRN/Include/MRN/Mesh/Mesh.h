#pragma once
#include <MRN/Mesh/Types.h>
#include <boost/filesystem/path.hpp>
#include <memory>
#include <MRN/FileSystem/Tile.h>
#include <MRN/Types.h>
namespace MRN
{
class MeshImplBase;
class Mesh
{
public:

    explicit Mesh(const boost::filesystem::path& path_, VendorID type = SoarscapeOSGB);
    void write(const boost::filesystem::path& path_);
    void     write(const Tile& tile);
    void       simpilify(float percent_);
    ~Mesh();
    Mesh();
    MyMesh& getNativMesh();

private:
    std::shared_ptr<MeshImplBase> m_meshImplement;
};
}   // namespace MRN