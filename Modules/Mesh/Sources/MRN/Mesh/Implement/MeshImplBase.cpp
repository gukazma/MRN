#include <MRN/Mesh/Implement/MeshImplBase.h>
#include <CGAL/Surface_mesh/IO.h>
#include <fstream>
namespace MRN
{
void MeshImplBase::write(const boost::filesystem::path& path_) {
    std::ofstream f(path_.generic_string());
    if (path_.extension() == ".ply") CGAL::IO::write_PLY(f, m_nativeMesh);
    if (path_.extension() == ".off") CGAL::IO::write_OFF(f, m_nativeMesh);
    if (path_.extension() == ".obj") CGAL::IO::write_OBJ(f, m_nativeMesh);
    if (path_.extension() == ".obj") CGAL::IO::write_OBJ(f, m_nativeMesh);
    if (path_.extension() == ".stl") CGAL::IO::write_STL(f, m_nativeMesh);
}
}