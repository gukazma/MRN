#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/IO/Color.h>
namespace MRN
{
	using Kernel         = CGAL::Simple_cartesian<float>;
	using Point3         = Kernel::Point_3;
	using Vec3           = Kernel::Vector_3;
	using Facet			 = std::array<std::size_t, 3>;
	using SurfaceMesh    = CGAL::Surface_mesh<Point3>;
	using VertexIndex    = SurfaceMesh::Vertex_index;
	using EdgeIndex      = SurfaceMesh::Edge_index;
	using FaceIndex      = SurfaceMesh::Face_index;
	using VertexPointMap = SurfaceMesh::Property_map<VertexIndex, Point3>;
	using VertexColorMap = SurfaceMesh::Property_map<VertexIndex, CGAL::IO::Color>;
}