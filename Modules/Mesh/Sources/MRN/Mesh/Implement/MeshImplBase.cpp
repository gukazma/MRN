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

namespace SMS = CGAL::Surface_mesh_simplification;
namespace MRN
{
void MeshImplBase::write(const boost::filesystem::path& path_) {
    std::ofstream f(path_.generic_string());
    VertexColorMap vpmap;
    /*double         stop_ratio =  0.1;
    SMS::Count_ratio_stop_predicate<SurfaceMesh> stop(stop_ratio);
    int                                           r = SMS::edge_collapse(m_nativeMesh, stop);*/

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
    CGAL::Point_set_3<Point3> borderPoints;
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
        minbox = abs(bbox_other.xmin() - bbox.xmax());
        xyflag = 0;
    }   // left
    if (abs(bbox_other.ymax() - bbox.ymin()) <= minbox) {
        minbox = abs(bbox_other.xmin() - bbox.xmax());
        xyflag = 0;
    }   // down
    if (abs(bbox_other.ymin() - bbox.ymax()) <= minbox) {
        minbox = abs(bbox_other.xmin() - bbox.xmax());
        xyflag = 0;
    }   // up

    float threshold = 10.0f;
    for (auto& v : m_nativeMesh.vertices()) {
        auto p = m_nativeMesh.point(v);
        if (bbox_other.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) {
            switch (xyflag) {
            case 0:
                if (bbox_other.xmin() - p.x() < threshold) {
                    borderPoints.insert(p);
                    borderVertices.push_back(v);
                }
                break;
            case 1:
                if (p.x() - bbox_other.xmax() < threshold) {
                    borderPoints.insert(p);
                    borderVertices.push_back(v);
                }
                break;
            case 2:
                if (p.y() - bbox_other.ymax() < threshold) {
                    borderPoints.insert(p);
                    borderVertices.push_back(v);
                }
                break;
            case 3:
                if (bbox_other.ymin() - p.y() < threshold) {
                    borderPoints.insert(p);
                    borderVertices.push_back(v);
                }
                break;
            default: break;
            }
        }
    }
    for (auto& v : other.m_nativeMesh.vertices()) {
        auto p = other.m_nativeMesh.point(v);
        if (bbox.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) {
            Point3 p1;
            switch (xyflag) {
            case 0:
                if (p.x() - bbox.xmax() < threshold) {
                    borderPoints.insert(p);
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            case 1:
                if (bbox.xmin() - p.x() < threshold) {
                    borderPoints.insert(p);
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            case 2:
                if (bbox.ymin() - p.y() < threshold) {
                    borderPoints.insert(p);
                    borderVertices_other.push_back(v);
                    p1 = p;
                }
                break;
            case 3:
                if (p.y() - bbox.ymax() < threshold) {
                    borderPoints.insert(p);
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

}
void MeshImplBase::computeLabel(
    std::unordered_map<SurfaceMesh::Vertex_index, int>&  vertexlabels_,
    std::unordered_map<SurfaceMesh::Vertex_index, bool>& vertexBorderFlags_,
    std::unordered_map<SurfaceMesh::Face_index, int>&    facetLabels_)
{
    unsigned int        num_thread            = boost::thread::hardware_concurrency();
    auto                bbox                  = CGAL::Polygon_mesh_processing::bbox(m_nativeMesh);
    std::vector<double> dims                  = {bbox.x_span(), bbox.y_span(), bbox.z_span()};
    auto                maxElement            = boost::max_element(dims);
    uint32_t            num_expect_per_thread = 1000;
    size_t              maxDinIndex           = std::distance(dims.begin(), maxElement);
    std::vector<size_t> buckets(num_expect_per_thread, 0);
    // 将mesh根据最长bbox边方向，均匀分割成1000份，放到buckets中。
    int bucketIndex = 0;
    for (const auto& vertex : m_nativeMesh.vertices()) {
        auto p = m_nativeMesh.point(vertex);
        switch (maxDinIndex) {
        case 0:
            bucketIndex = int(num_expect_per_thread * (p.x() - bbox.xmin()) / bbox.x_span());
            break;
        case 1:
            bucketIndex = int(num_expect_per_thread * (p.y() - bbox.ymin()) / bbox.y_span());
            break;
        case 2:
            bucketIndex = int(num_expect_per_thread * (p.z() - bbox.zmin()) / bbox.z_span());
            break;
        default: break;
        }
        buckets[bucketIndex]++;
    }

    // 将篓筐buckets打上标记
    size_t totalVertices         = std::accumulate(buckets.begin(), buckets.end(), size_t(0));
    size_t num_vertex_per_thread = (size_t)std::ceil(double(totalVertices) / double(num_thread));
    std::vector<int> split;
    size_t           n = 0;
    for (size_t i = 0; i < num_expect_per_thread; i++) {
        n += buckets[i];
        if (n >= num_vertex_per_thread) {
            split.push_back(i);
            n = 0;
        }
    }
    split.push_back(std::numeric_limits<int>::max());
    // 给顶点打上标签
    for (const auto& vertex : m_nativeMesh.vertices()) {
        auto p = m_nativeMesh.point(vertex);
        switch (maxDinIndex) {
        case 0:
            bucketIndex = int(num_expect_per_thread * (p.x() - bbox.xmin()) / bbox.x_span());
            break;
        case 1:
            bucketIndex = int(num_expect_per_thread * (p.y() - bbox.ymin()) / bbox.y_span());
            break;
        case 2:
            bucketIndex = int(num_expect_per_thread * (p.z() - bbox.zmin()) / bbox.z_span());
            break;
        default: break;
        }

        // 顶点所在的篓筐分在哪个大段内
        for (size_t i = 0; i < split.size(); i++) {
            if (bucketIndex <= split[i]) {
                vertexlabels_[vertex] = i;
                break;
            }
        }
    }

    // 给每个face打上标签
    for (const auto& facet : m_nativeMesh.faces()) {
        auto edge = m_nativeMesh.halfedge(facet);
        int  label0 = vertexlabels_[m_nativeMesh.target(edge)];
        int  label1 = vertexlabels_[m_nativeMesh.target(m_nativeMesh.next(edge))];
        int  label2 = vertexlabels_[m_nativeMesh.target(m_nativeMesh.prev(edge))];

        // 如果面的顶点都在一个大段内
        if (label0 == label1 && label0 == label2) {
            facetLabels_[facet] = label0;
        }
        else {
            if (label0 == label1 || label0 == label2)
                facetLabels_[facet] = label0;
            else if (label1 == label2)
                facetLabels_[facet] = label1;
            else
                facetLabels_[facet] = std::min(label0, std::min(label1, label2));
        }
    }

    // 标记顶点是否在大段的边界上
    for (const auto& vertex : m_nativeMesh.vertices()) {
        int faceLabel = -1;
        bool bHasDifferenceLabel = false;
        for (const auto& facet : m_nativeMesh.faces_around_target(m_nativeMesh.halfedge(vertex))) {
            if (facet == m_nativeMesh.null_face()) continue;

            if (faceLabel == -1) {
                faceLabel = facetLabels_[facet];
            }
            else if (facetLabels_[facet] != faceLabel)
            {
                bHasDifferenceLabel = true;
                break;
            }
        }
        vertexBorderFlags_[vertex] = bHasDifferenceLabel;
    }
}
}