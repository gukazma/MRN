#include <gtest/gtest.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/NodeVisitor>
#include <osg/Texture2D>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

// stuff to define the mesh
#include <vcg/complex/complex.h>

// io
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import.h>

// local optimization
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>

using namespace vcg;
using namespace tri;

/**********************************************************
Mesh Classes for Quadric Edge collapse based simplification

For edge collpases we need verteses with:
- V->F adjacency
- per vertex incremental mark
- per vertex Normal


Moreover for using a quadric based collapse the vertex class
must have also a Quadric member Q();
Otherwise the user have to provide an helper function object
to recover the quadric.

******************************************************/
// The class prototypes.
class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes : public UsedTypes<Use<MyVertex>::AsVertexType, Use<MyEdge>::AsEdgeType,
                                      Use<MyFace>::AsFaceType>
{};

class MyVertex : public Vertex<MyUsedTypes, vertex::VFAdj, vertex::Coord3f, vertex::Color4b,
                               vertex::Mark,
                               vertex::Qualityf, vertex::BitFlags>
{
public:
    vcg::math::Quadric<double>& Qd() { return q; }

private:
    math::Quadric<double> q;
};

class MyEdge : public Edge<MyUsedTypes>
{};

typedef BasicVertexPair<MyVertex> VertexPair;

class MyFace : public Face<MyUsedTypes, face::VFAdj, face::VertexRef, face::BitFlags>
{};

// the main mesh class
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>
{};


class MyTriEdgeCollapse
    : public vcg::tri::TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse,
                                              QInfoStandard<MyVertex>>
{
public:
    typedef vcg::tri::TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse,
                                             QInfoStandard<MyVertex>>
                                         TECQ;
    typedef MyMesh::VertexType::EdgeType EdgeType;
    inline MyTriEdgeCollapse(const VertexPair& p, int i, BaseParameterClass* pp)
        : TECQ(p, i, pp)
    {}
};
class OSGBMeshVisitor : public osg::NodeVisitor
{
public:
    OSGBMeshVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {}

    void apply(osg::Geode& geode) override
    {
        for (size_t i = 0; i < geode.getNumChildren(); i++) {
            std::vector<vcg::Point3f>            coordVec;
            std::vector<vcg::Point3i>            indexVec;
            std::map<vcg::Point3f, vcg::Color4b> pcmap;
            std::vector<vcg::Color4b>            colors;
            osg::Geometry*                       geom = geode.getDrawable(i)->asGeometry();
            if (geom == nullptr) continue;
            // get texture
            osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(
                geom->getStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
            osg::Image*     image = texture->getImage();
            osg::Vec2Array* textureCoordArray =
                dynamic_cast<osg::Vec2Array*>(geom->getTexCoordArray(0));
            // get vertex
            osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
            for (size_t i = 0; i < vertices->size(); i++) {
                auto         v = vertices->at(i);
                vcg::Point3f p(v.x(), v.y(), v.z());
                auto         coord = textureCoordArray->at(i);
                coordVec.push_back(p);
                colors.push_back(vcg::Color4b(image->getColor(coord).r() * 255,
                                              image->getColor(coord).g() * 255,
                                              image->getColor(coord).b() * 255,
                                              255));
            }

            auto faces_uint   = dynamic_cast<osg::DrawElementsUInt*>(geom->getPrimitiveSet(0));
            auto faces_ushort = dynamic_cast<osg::DrawElementsUShort*>(geom->getPrimitiveSet(0));

            if (!faces_uint && !faces_ushort) continue;
            if (!vertices) continue;

            std::vector<uint32_t> faces;
            if (faces_uint) {
                faces.clear();
                for (size_t i = 0; i < faces_uint->size(); i++) {
                    faces.push_back(faces_uint->getElement(i));
                }
            }
            if (faces_ushort) {
                faces.clear();
                for (size_t i = 0; i < faces_ushort->size(); i++) {
                    faces.push_back(faces_ushort->getElement(i));
                }
            }

            for (size_t i = 0; i < faces.size(); i += 3) {
                vcg::Point3i index;
                index.X() = faces[i];
                index.Y() = faces[i + 1];
                index.Z() = faces[i + 2];
                indexVec.push_back(index);
            }
            auto v_iter = vcg::tri::Allocator<MyMesh>::AddVertices(m_mesh, coordVec.size());
            // vertices
            for (size_t i = 0; i < coordVec.size(); i++, v_iter++) {
                (*v_iter).P()[0] = coordVec[i].X();
                (*v_iter).P()[1] = coordVec[i].Y();
                (*v_iter).P()[2] = coordVec[i].Z();
                (*v_iter).C().Import(colors[i]);
            }

            for (size_t i = 0; i < indexVec.size(); i++) {
                vcg::tri::Allocator<MyMesh>::AddFace(m_mesh,
                                                        &m_mesh.vert[indexVec[i].X()],
                                                        &m_mesh.vert[indexVec[i].Y()],
                                                        &m_mesh.vert[indexVec[i].Z()]);
            }
        }
    }
    MyMesh m_mesh;
};
TEST(MRN, Simplify)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    auto                    input = MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb";
    auto                    node  = osgDB::readNodeFile(input.generic_string());

    OSGBMeshVisitor* visitor = new OSGBMeshVisitor;
    node->accept(*visitor);
    auto& mesh      = visitor->m_mesh;
    int FinalSize = 100;
    
    TriEdgeCollapseQuadricParameter qparams;
    qparams.QualityThr  = .3;
    double TargetError  = std::numeric_limits<double>::max();
    bool   CleaningFlag = false;
   


    vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

    // decimator initialization
    vcg::LocalOptimization<MyMesh> DeciSession(mesh, &qparams);

    int t1 = clock();
    DeciSession.Init<MyTriEdgeCollapse>();
    int t2 = clock();
    printf("Initial Heap Size %i\n", int(DeciSession.h.size()));

    DeciSession.SetTargetSimplices(FinalSize);
    DeciSession.SetTimeBudget(0.5f);
    DeciSession.SetTargetOperations(100000);
    if (TargetError < std::numeric_limits<float>::max()) DeciSession.SetTargetMetric(TargetError);

    while (DeciSession.DoOptimization() && mesh.fn > FinalSize &&
           DeciSession.currMetric < TargetError)
        printf("Current Mesh size %7i heap sz %9i err %9g \n",
               mesh.fn,
               int(DeciSession.h.size()),
               DeciSession.currMetric);

    int t3 = clock();
    printf("mesh  %d %d Error %g \n", mesh.vn, mesh.fn, DeciSession.currMetric);
    printf("\nCompleted in (%5.3f+%5.3f) sec\n",
           float(t2 - t1) / CLOCKS_PER_SEC,
           float(t3 - t2) / CLOCKS_PER_SEC);
    vcg::tri::io::ExporterPLY<MyMesh>::Save(mesh, "aaa.ply", ::vcg::tri::io::Mask::IOM_VERTCOLOR);
    //MRN::Mesh               mesh(MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
}
