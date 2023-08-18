#pragma once
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/complex.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/import.h>
class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType,
                                           vcg::Use<MyEdge>::AsEdgeType,
                            vcg::Use<MyFace>::AsFaceType>
{};

class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f,
                                    vcg::vertex::Qualityf, vcg::vertex::Color4b, vcg::vertex::VEAdj,
                                    vcg::vertex::VFAdj, vcg::vertex::BitFlags>
{
public:
    vcg::math::Quadric<double>& Qd() { return q; }

private:
    vcg::math::Quadric<double> q;
};

class MyEdge : public vcg::Edge<MyUsedTypes, vcg::edge::VertexRef, vcg::edge::VEAdj,
                                vcg::edge::EEAdj, vcg::edge::BitFlags>
{};

typedef vcg::tri::BasicVertexPair<MyVertex> VertexPair;

class MyFace : public vcg::Face<MyUsedTypes, vcg::face::VertexRef, vcg::face::Normal3f,
                                vcg::face::Qualityf, vcg::face::Color4b, vcg::face::VFAdj, vcg::face::FFAdj, vcg::face::Mark,
                       vcg::face::Color4b, vcg::face::BitFlags>
{};

// the main mesh class
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyEdge>, std::vector<MyFace>>
{};


class MyTriEdgeCollapse
    : public vcg::tri::TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse,
                                              vcg::tri::QInfoStandard<MyVertex>>
{
public:
    typedef vcg::tri::TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse,
                                             vcg::tri::QInfoStandard<MyVertex>> TECQ;
    typedef MyMesh::VertexType::EdgeType EdgeType;
    inline MyTriEdgeCollapse(const VertexPair& p, int i, vcg::BaseParameterClass* pp)
        : TECQ(p, i, pp)
    {}
};

namespace std {
template<> struct hash<vcg::Point3f>
{
    size_t operator()(vcg::Point3f const& vertex) const
    {
        return ((hash<float>()(vertex[0]) ^ (hash<float>()(vertex[2]) << 1)) >> 1) ^
               (hash<float>()(vertex[1]) << 1);
    }
};
}   // namespace std