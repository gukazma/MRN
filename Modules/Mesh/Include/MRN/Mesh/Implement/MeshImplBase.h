#pragma once
#include <boost/filesystem/path.hpp>
#include <MRN/Mesh/Types.h>
#include <unordered_map>
namespace MRN
{
class MeshImplBase
{
public:
    MeshImplBase() = default;
    virtual void read(const boost::filesystem::path& path_) = 0;
    virtual void write(const boost::filesystem::path& path_);
    void simpilify(float percent_);
    MyVertex getClosest(vcg::Point3f point);

    MyMesh& getNativeMesh();

protected:
    MyMesh m_nativeMesh;
};
}