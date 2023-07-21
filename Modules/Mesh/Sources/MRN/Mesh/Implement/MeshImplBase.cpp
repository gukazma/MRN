#include <MRN/Mesh/Implement/MeshImplBase.h>
#include <CGAL/Surface_mesh/IO.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <fstream>
namespace SMS = CGAL::Surface_mesh_simplification;
namespace MRN
{
void MeshImplBase::write(const boost::filesystem::path& path_) {
    std::ofstream f(path_.generic_string());
    VertexColorMap vpmap;
    double         stop_ratio =  0.1;
    SMS::Count_ratio_stop_predicate<SurfaceMesh> stop(stop_ratio);
    int                                           r = SMS::edge_collapse(m_nativeMesh, stop);

    bool created;
    boost::tie(vpmap, created) = m_nativeMesh.property_map<VertexIndex, CGAL::IO::Color>("v:color");
    if (path_.extension() == ".ply")
        CGAL::IO::write_PLY(f, m_nativeMesh, CGAL::parameters::vertex_color_map(vpmap));
    if (path_.extension() == ".off") CGAL::IO::write_OFF(f, m_nativeMesh);
    if (path_.extension() == ".obj") CGAL::IO::write_OBJ(f, m_nativeMesh);
    if (path_.extension() == ".obj") CGAL::IO::write_OBJ(f, m_nativeMesh);
    if (path_.extension() == ".stl") CGAL::IO::write_STL(f, m_nativeMesh);
}
SurfaceMesh& MeshImplBase::getNativeMesh()
{
    return m_nativeMesh;
}
void MeshImplBase::join(MeshImplBase& other) {}
}