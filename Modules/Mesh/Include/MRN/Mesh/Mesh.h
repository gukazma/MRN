#pragma once
#include <memory>
#include <boost/filesystem/path.hpp>
#include <MRN/Mesh/Types.h>
namespace MRN
{
class MeshImplBase;
class Mesh
{
public:
    enum DataType
    {
        OSGB = 0,
        GLTF,
    };
    explicit Mesh(const boost::filesystem::path& path_, DataType type = OSGB);
    void write(const boost::filesystem::path& path_);
    void         join(Mesh& other);
    SurfaceMesh& getNativMesh();

private:
    std::shared_ptr<MeshImplBase> m_meshImplement;
};
}