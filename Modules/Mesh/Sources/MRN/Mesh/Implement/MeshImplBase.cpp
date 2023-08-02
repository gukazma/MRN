#include <MRN/Mesh/Implement/MeshImplBase.h>
#include <CGAL/Surface_mesh/IO.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Bounded_normal_change_filter.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk_placement.h>
// Midpoint placement policy
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
//Placement wrapper
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Constrained_placement.h>
#include <CGAL/polygon_mesh_processing.h>
#include <fstream>
namespace SMS = CGAL::Surface_mesh_simplification;
namespace MRN
{
//typedef boost::graph_traits<SurfaceMesh>::halfedge_descriptor halfedge_descriptor;
//typedef boost::graph_traits<SurfaceMesh>::edge_descriptor edge_descriptor;
//struct Border_is_constrained_edge_map
//{
//    const SurfaceMesh*                      sm_ptr;
//    typedef edge_descriptor                  key_type;
//    typedef bool                             value_type;
//    typedef value_type                       reference;
//    typedef boost::readable_property_map_tag category;
//    Border_is_constrained_edge_map(const SurfaceMesh& sm)
//        : sm_ptr(&sm)
//    {}
//    friend value_type get(const Border_is_constrained_edge_map& m, const key_type& edge)
//    {
//        return CGAL::is_border(edge, *m.sm_ptr);
//    }
//};

void MeshImplBase::simplify() 
{
    double                                            stop_ratio = 0.1;
    SMS::Count_ratio_stop_predicate<SurfaceMesh>      stop(stop_ratio);
    SMS::Bounded_normal_change_filter<>               filter;
    SMS::internal::LindstromTurk_params               lp(0.5, 0.5, 0.5);
    typedef SMS::LindstromTurk_placement<SurfaceMesh> Placement;
    SMS::edge_collapse(m_nativeMesh,
                       stop,
                       CGAL::parameters::get_cost(SMS::LindstromTurk_cost<SurfaceMesh>())
                           .filter(filter)
                           .get_placement(Placement()));
}
void MeshImplBase::write(const boost::filesystem::path& path_)
{
    std::ofstream f(path_.generic_string());
    VertexColorMap vpmap;
    // simplify
    //double stop_ratio =  0.1;
    //SMS::Count_ratio_stop_predicate<SurfaceMesh> stop(stop_ratio);
    //SMS::Bounded_normal_change_filter<>          filter;
    //SMS::internal::LindstromTurk_params          lp(0.5, 0.5, 0.5);
    //typedef SMS::LindstromTurk_placement<SurfaceMesh> Placement;
    //SMS::edge_collapse(m_nativeMesh,
    //                   stop,
    //                   CGAL::parameters::get_cost(SMS::LindstromTurk_cost<SurfaceMesh>(lp))
    //                       .filter(filter)
    //                       .get_placement(Placement(lp)));
    //typedef SMS::Constrained_placement<SMS::Midpoint_placement<SurfaceMesh>,
    //                                   Border_is_constrained_edge_map> Placement;
    //SurfaceMesh::Property_map<halfedge_descriptor, std::pair<Point3, Point3>>
    //    constrained_halfedges;
    //constrained_halfedges = m_nativeMesh.add_property_map<halfedge_descriptor, std::pair<Point3, Point3>>("h:vertices").first;
    //std::size_t nb_border_edges = 0;
    //for (halfedge_descriptor hd : halfedges(m_nativeMesh)) {
    //    if (CGAL::is_border(hd, m_nativeMesh)) {
    //        constrained_halfedges[hd] =
    //            std::make_pair(m_nativeMesh.point(source(hd, m_nativeMesh)),
    //                           m_nativeMesh.point(target(hd, m_nativeMesh)));
    //        ++nb_border_edges;
    //    }
    //}
    //// Contract the surface mesh as much as possible
    //SMS::Count_ratio_stop_predicate<SurfaceMesh> stop(0);
    //Border_is_constrained_edge_map               bem(m_nativeMesh);
    //int r = SMS::edge_collapse(
    //    m_nativeMesh,
    //    stop,
    //    CGAL::parameters::edge_is_constrained_map(bem).get_placement(Placement(bem)));

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
void MeshImplBase::join(MeshImplBase& other) {
    VertexColorMap vcmap =
        m_nativeMesh.add_property_map<VertexIndex, CGAL::IO::Color>("v:color").first;
    std::vector<VertexIndex> borderVertices, borderVertices_other;
    std::map<VertexIndex, VertexIndex> replaceVVMap;

    // Calculate the bounding box
    auto bbox = CGAL::bounding_box(m_nativeMesh.points().begin(), m_nativeMesh.points().end());
    auto bbox_other = CGAL::bounding_box(other.m_nativeMesh.points().begin(), other.m_nativeMesh.points().end());
    // xyflag is used to judge the other mesh location
    double minbox = std::numeric_limits<double>::max();
    int xyflag = 0;
    if (abs(bbox_other.xmin() - bbox.xmax()) <= minbox) {
        minbox = abs(bbox_other.xmin() - bbox.xmax());
        xyflag = 0;
    }   // right
    if (abs(bbox_other.xmax() - bbox.xmin()) <= minbox) {
        minbox = abs(bbox_other.xmax() - bbox.xmin());
        xyflag = 1;
    }   // left
    if (abs(bbox_other.ymax() - bbox.ymin()) <= minbox) {
        minbox = abs(bbox_other.ymax() - bbox.ymin());
        xyflag = 2;
    }   // down
    if (abs(bbox_other.ymin() - bbox.ymax()) <= minbox) {
        minbox = abs(bbox_other.ymin() - bbox.ymax());
        xyflag = 3;
    }   // up

    float threshold = 0.5f;
    for (auto& v : m_nativeMesh.vertices()) {
        auto p = m_nativeMesh.point(v);
        if (m_nativeMesh.is_border(v)) {
            switch (xyflag) {
            case 0:
                if (bbox_other.xmin() - p.x() < threshold) {
                    borderVertices.push_back(v);
                }
                break;
            case 1:
                if (p.x() - bbox_other.xmax() < threshold) {
                    borderVertices.push_back(v);
                }
                break;
            case 2:
                if (p.y() - bbox_other.ymax() < threshold) {
                    borderVertices.push_back(v);
                }
                break;
            case 3:
                if (bbox_other.ymin() - p.y() < threshold) {
                    borderVertices.push_back(v);
                }
                break;
            default: break;
            }
        }
    }
    for (auto& v : other.m_nativeMesh.vertices()) {
        auto p = other.m_nativeMesh.point(v);
        if (other.m_nativeMesh.is_border(v)) {
            Point3 p1;
            switch (xyflag) {
            case 0:
                if (p.x() - bbox.xmax() < threshold) {
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            case 1:
                if (bbox.xmin() - p.x() < threshold) {
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            case 2:
                if (bbox.ymin() - p.y() < threshold) {
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            case 3:
                if (p.y() - bbox.ymax() < threshold) {
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            default: break;
            }

            if (p1 == p)
            {
                VertexIndex vtemp;
                double      distance = std::numeric_limits<double>::max();
                for (auto& v0 : borderVertices) {
                    double td = CGAL::squared_distance(m_nativeMesh.point(v0), p);
                    if (td < distance) {
                        vtemp    = v0;
                        distance = td;
                    }
                }
                replaceVVMap[v] = vtemp;
            }
        }
    }

    for (auto& f : other.m_nativeMesh.faces()) {
        CGAL::Vertex_around_face_iterator<SurfaceMesh> vbegin, vend;
        boost::tie(vbegin, vend) =
            CGAL::vertices_around_face(other.m_nativeMesh.halfedge(f), other.m_nativeMesh);

        VertexIndex v1, v2, v3;
        if (replaceVVMap.find(*vbegin) != replaceVVMap.end())
            v1 = replaceVVMap[*vbegin];
        else {
            v1        = m_nativeMesh.add_vertex(other.m_nativeMesh.point(*vbegin));
            vcmap[v1] = other.m_nativeMesh.property_map<VertexIndex, CGAL::IO::Color>("v:color")
                            .first[*vbegin];
        }

        vbegin++;
        if (replaceVVMap.find(*vbegin) != replaceVVMap.end()) 
            v2 = replaceVVMap[*vbegin];
        else {
            v2        = m_nativeMesh.add_vertex(other.m_nativeMesh.point(*vbegin));
            vcmap[v2] = other.m_nativeMesh.property_map<VertexIndex, CGAL::IO::Color>("v:color")
                            .first[*vbegin];
        }
        
        vbegin++;
        if (replaceVVMap.find(*vbegin) != replaceVVMap.end())
            v3 = replaceVVMap[*vbegin];
        else
        {
            v3        = m_nativeMesh.add_vertex(other.m_nativeMesh.point(*vbegin));
            vcmap[v3] = other.m_nativeMesh.property_map<VertexIndex, CGAL::IO::Color>("v:color")
                            .first[*vbegin];
        }

        m_nativeMesh.add_face(v1, v2, v3);
    }

    CGAL::Polygon_mesh_processing::duplicate_non_manifold_vertices(m_nativeMesh);
    CGAL::Polygon_mesh_processing::stitch_borders(m_nativeMesh);
}

void MeshImplBase::removeSmallComponents(size_t threshold)
{
    std::vector<bool> visited(m_nativeMesh.num_vertices(), false);
    std::vector<VertexIndex> needRemoveVertices;
    int                      nRemovedComponents = 0;
    for (auto& v : m_nativeMesh.vertices()) {
        if (visited[v]) continue;

        std::queue<VertexIndex> queue;
        queue.push(v);
        visited[v] = true;
        std::size_t numVertex = 0;
        std::vector<VertexIndex> vertices;
        while (!queue.empty()) {
            VertexIndex vertexID = queue.front();
            queue.pop();
            numVertex++;
            vertices.push_back(vertexID);
            for (auto hv : m_nativeMesh.vertices_around_target(m_nativeMesh.halfedge(vertexID))) {
                if (visited[hv] == false) {
                    queue.push(hv);
                    visited[hv] = true;
                }
            }
        }

        if (numVertex < threshold) {
            needRemoveVertices.insert(needRemoveVertices.end(), vertices.begin(), vertices.end());
            nRemovedComponents++;
        }
    }

    std::set<FaceIndex> needRemoveFaces;
    for (auto& vertex : needRemoveVertices) {
        for (auto face : m_nativeMesh.faces_around_target(m_nativeMesh.halfedge(vertex))) {
            if (face != m_nativeMesh.null_face()) {
                needRemoveFaces.insert(face);
            }
        }
    }
    for (auto& face : needRemoveFaces) {
        CGAL::Euler::remove_face(m_nativeMesh.halfedge(face), m_nativeMesh);
        //m_nativeMesh.remove_face(face); 
    }

    m_nativeMesh.collect_garbage();
    std::cout << "remove " << nRemovedComponents << "small components" << std::endl;
}

}