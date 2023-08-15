#pragma once
#include <MRN/Mesh/Types.h>
#include <boost/filesystem/path.hpp>
#include <memory>
namespace MRN {
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
    void     write(const boost::filesystem::path& path_);
    void     simpilify(float percent_);
    MyVertex getClosest(vcg::Point3f point);

    MyMesh& getNativMesh();

private:
    std::shared_ptr<MeshImplBase> m_meshImplement;
};
}   // namespace MRN