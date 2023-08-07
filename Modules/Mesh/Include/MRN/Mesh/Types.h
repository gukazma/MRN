#pragma once
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/color.h>
#include <wrap/io_trimesh/export_off.h>
class MyFace;
class MyVertex;

struct MyUsedTypes
    : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyFace>::AsFaceType>
{};

class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f,
                                    vcg::vertex::Color4b, vcg::vertex::BitFlags>
{
public:
    static bool HasColor() { return true; }
};
class MyFace : public vcg::Face<MyUsedTypes, vcg::face::VertexRef, vcg::face::Normal3f,
                                vcg::face::Color4b, vcg::face::BitFlags>
{};
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>
{};

class MyVertexOcf;
class MyFaceOcf;

struct MyUsedTypesOcf
    : public vcg::UsedTypes<vcg::Use<MyVertexOcf>::AsVertexType, vcg::Use<MyFaceOcf>::AsFaceType>
{};

class MyVertexOcf
    : public vcg::Vertex<MyUsedTypesOcf,
                         vcg::vertex::InfoOcf,   //   <--- Note the use of the 'special' InfoOcf
                                                 //   component
                         vcg::vertex::Coord3f, vcg::vertex::QualityfOcf, vcg::vertex::Color4b,
                         vcg::vertex::BitFlags, vcg::vertex::Normal3f, vcg::vertex::VFAdjOcf>
{};

class MyFaceOcf
    : public vcg::Face<MyUsedTypesOcf,
                       vcg::face::InfoOcf,   //   <--- Note the use of the 'special' InfoOcf
                                             //   component
                       vcg::face::FFAdjOcf, vcg::face::VFAdjOcf, vcg::face::Color4bOcf,
                       vcg::face::VertexRef, vcg::face::BitFlags, vcg::face::Normal3fOcf>
{};

// the mesh class must make use of the 'vector_ocf' containers instead of the classical std::vector
class MyMeshOcf : public vcg::tri::TriMesh<vcg::vertex::vector_ocf<MyVertexOcf>,
                                           vcg::face::vector_ocf<MyFaceOcf>>
{};