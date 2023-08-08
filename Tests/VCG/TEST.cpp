#include <gtest/gtest.h>
#include<vcg/complex/complex.h>
#include<vcg/complex/algorithms/create/platonic.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
// input output
#include <wrap/io_trimesh/export_off.h>
#include <vcg/complex/algorithms/update/color.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/NodeVisitor>
#include <osg/Texture2D>
#include <map>
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


class OSGBMeshVisitor : public osg::NodeVisitor
{
public:
    OSGBMeshVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {}

    void apply(osg::Geode& geode) override
    {
        for (size_t i = 0; i < geode.getNumChildren(); i++) {
            std::vector<vcg::Point3f> coordVec;
            std::vector<vcg::Point3i> indexVec;
            std::map<vcg::Point3f, vcg::Color4b> pcmap;
            std::vector<vcg::Color4b> colors;
            osg::Geometry* geom = geode.getDrawable(i)->asGeometry();
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
                auto v = vertices->at(i);
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
                index.Y() = faces[i+1];
                index.Z() = faces[i+2];
                indexVec.push_back(index);
            }
            auto v_iter = vcg::tri::Allocator<MyMeshOcf>::AddVertices(m_mesh, coordVec.size());
            m_mesh.face.EnableColor();
            // vertices
            for (size_t i = 0; i < coordVec.size(); i++, v_iter++) {
                (*v_iter).P()[0] = coordVec[i].X();
                (*v_iter).P()[1] = coordVec[i].Y();
                (*v_iter).P()[2] = coordVec[i].Z();
                (*v_iter).C().Import(colors[i]);
            }

            for (size_t i = 0; i < indexVec.size(); i++) {
                vcg::tri::Allocator<MyMeshOcf>::AddFace(m_mesh,
                                                     &m_mesh.vert[indexVec[i].X()],
                                                     &m_mesh.vert[indexVec[i].Y()],
                                                     &m_mesh.vert[indexVec[i].Z()]);
            }
        }
    }
    MyMeshOcf m_mesh;
};

TEST(VCG, CREATE)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path path = MRNDataPath / "osgb/Tile_+000_+014/Tile_+000_+014_15_0.osgb";
    MyMesh diskMesh;
    auto            node = osgDB::readNodeFile(path.generic_string().c_str());
    OSGBMeshVisitor meshVisitor;
    node->accept(meshVisitor);
    // Create a simple triangle mesh using just a vector of coords and a vector of indexes
    std::vector<vcg::Point3f> coordVec;
    std::vector<vcg::Point3i> indexVec;
    coordVec.push_back(vcg::Point3f(0, 0, 0));
    for (int i = 0; i < 36; ++i) {
        float angleRad = float(i) * M_PI / 18.0;
        coordVec.push_back(vcg::Point3f(sin(angleRad), cos(angleRad), 0));
        indexVec.push_back(vcg::Point3i(0, i + 1, 1 + (i + 1) % 36));
    }
   
    vcg::tri::io::ExporterOFF<MyMeshOcf>::Save(
        meshVisitor.m_mesh, "disc.off", vcg::tri::io::Mask::IOM_VERTCOLOR);
}