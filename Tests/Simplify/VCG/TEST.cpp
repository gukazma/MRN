#include <gtest/gtest.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/clip.h>
#include <vcg/complex/complex.h>

// input output
#include <wrap/io_trimesh/import_off.h>
#include <wrap/io_trimesh/export_off.h>


class MyFace;
class MyVertex;

struct MyUsedTypes
    : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyFace>::AsFaceType>
{};

class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::BitFlags>
{};
class MyFace : public vcg::Face<MyUsedTypes, vcg::face::VertexRef, vcg::face::BitFlags>
{};
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>
{};

TEST(Simplify, VCG1)
{
    MyMesh mr, ml;
    vcg::tri::io::ImporterOFF<MyMesh>::Open(ml, "D:/data/MRNDATA/output/mesh0.off");
    vcg::tri::io::ImporterOFF<MyMesh>::Open(mr, "D:/data/MRNDATA/output/mesh1.off");
    vcg::tri::Append<MyMesh, MyMesh>::Mesh(ml, mr, false, true);
    vcg::tri::io::ExporterOFF<MyMesh>::Save(ml, "D:/data/MRNDATA/output/meshJoin0.off");
    vcg::tri::io::ExporterOFF<MyMesh>::Save(mr, "D:/data/MRNDATA/output/meshJoin1.off");
}