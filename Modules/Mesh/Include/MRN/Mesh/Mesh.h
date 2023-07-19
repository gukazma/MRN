#pragma once
#include <memory>
#include <boost/filesystem/path.hpp>
namespace MRN
{
class MeshImplBase;
class Mesh
{
public:
    explicit Mesh(const boost::filesystem::path& path_);
    
private:
    std::unique_ptr<MeshImplBase> m_meshImplement;
};
}