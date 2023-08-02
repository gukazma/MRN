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
    SurfaceMesh& getNativeMesh();
    void         join(MeshImplBase& other);
    void         simplify();
    void removeSmallComponents(size_t threshold);

public:
    void computeLabel(std::unordered_map<SurfaceMesh::Vertex_index, int>&  vertexlabels_,
                      std::unordered_map<SurfaceMesh::Vertex_index, bool>& vertexBorderFlags,
                      std::unordered_map<SurfaceMesh::Face_index, int>&    facetLabels
        );

protected:
    SurfaceMesh m_nativeMesh;
};
}