#pragma once
#include <boost/filesystem/path.hpp>
#include <MRN/Mesh/Types.h>
namespace MRN
{
class MeshImplBase
{
public:
    MeshImplBase() = default;
    virtual void read(const boost::filesystem::path& path_) = 0;
    virtual void write(const boost::filesystem::path& path_);

protected:
    SurfaceMesh m_nativeMesh;
};
}