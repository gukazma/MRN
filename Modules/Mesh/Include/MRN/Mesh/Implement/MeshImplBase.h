#pragma once
#include <boost/filesystem/path.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
namespace MRN
{
class MeshImplBase
{
public:
    using Kernel = CGAL::Simple_cartesian<float>;
    using Point3 = Kernel::Point_3;
    using Vec3 = Kernel::Vector_3;
    using SurfaceMesh = CGAL::Surface_mesh<Kernel>;
    using VertexIndex = SurfaceMesh::Vertex_index;
    using FaceIndex = SurfaceMesh::Face_index;
    using VPMap = boost::property_map<VertexIndex, Point3>;
    virtual void read(const boost::filesystem::path& path_) = 0;

protected:
    SurfaceMesh m_nativeMesh;
};
}