#include <MRN/Mesh/Implement/MeshImplBase.h>
#include <CGAL/Surface_mesh/IO.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Point_set_3.h>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/range/algorithm.hpp>
#include <wrap/io_trimesh/export.h>
namespace SMS = CGAL::Surface_mesh_simplification;
namespace MRN
{
void MeshImplBase::write(const boost::filesystem::path& path_) {
    vcg::tri::io::Exporter<MyMeshOcf>::Save(m_nativeMesh, path_.generic_string().c_str());
}
MyMeshOcf& MeshImplBase::getNativeMesh()
{
    return m_nativeMesh;
}
}