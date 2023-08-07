#include <gtest/gtest.h>
#include<vcg/complex/complex.h>
#include<vcg/complex/algorithms/create/platonic.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
// input output
#include <wrap/io_trimesh/export_off.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/NodeVisitor>
#include <osg/Texture2D>
class MyFace;
class MyVertex;

struct MyUsedTypes
    : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyFace>::AsFaceType>
{};

class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f,
                                    vcg::vertex::Color4b, vcg::vertex::BitFlags>
{};
class MyFace : public vcg::Face<MyUsedTypes, vcg::face::VertexRef, vcg::face::Normal3f,
                                vcg::face::FFAdj, vcg::face::BitFlags>
{};
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>
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
                coordVec.push_back({v.x(), v.y(), v.z()});
                /*Point3      p = Point3(v.x(), v.y(), v.z());
                VertexIndex vertex_index = m_nativeMeshPreference.add_vertex(p);
                auto        coord        = textureCoordArray->at(i);
                std::cout << "color:" << image->getColor(coord).r() << image->getColor(coord).g()
                          << image->getColor(coord).b() << std::endl;
                vcmap[vertex_index]      =  CGAL::IO::Color(image->getColor(coord).r()*255,
                                                      image->getColor(coord).g() * 255,
                                                      image->getColor(coord).b() * 255);*/
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
            vcg::tri::BuildMeshFromCoordVectorIndexVector(m_mesh, coordVec, indexVec);
        }
    }
    MyMesh m_mesh;
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

    vcg::tri::BuildMeshFromCoordVectorIndexVector(diskMesh, coordVec, indexVec);
    vcg::tri::io::ExporterOFF<MyMesh>::Save(meshVisitor.m_mesh, "disc.off");
}