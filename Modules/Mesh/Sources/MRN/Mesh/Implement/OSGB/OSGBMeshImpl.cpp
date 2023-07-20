#include <MRN/Mesh/Implement/OSGB/OSGBMeshImpl.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/NodeVisitor>
#include <osg/Texture2D>
#include <CGAL/IO/Color.h>
#include <vector>
#include <fstream>
namespace MRN
{
class OSGBMeshVisitor : public osg::NodeVisitor
{
public:
    OSGBMeshVisitor(SurfaceMesh& mesh)
        : m_nativeMeshPreference(mesh)
        , osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {}

    void apply(osg::Geode& geode) override
    {
        for (size_t i = 0; i < geode.getNumChildren(); i++) {
            osg::Geometry* geom = geode.getDrawable(i)->asGeometry();
            if (geom == nullptr) continue;
            VertexColorMap vcmap =
                m_nativeMeshPreference.add_property_map<VertexIndex, CGAL::IO::Color>("v:color").first;
            // get texture
            osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(
                geom->getStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
            osg::Image* image = texture->getImage();
            osg::Vec2Array* textureCoordArray =
                dynamic_cast<osg::Vec2Array*>(geom->getTexCoordArray(0));
            // get vertex
            osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
            for (size_t i = 0; i < vertices->size(); i++) {
                auto v = vertices->at(i);
                Point3      p = Point3(v.x(), v.y(), v.z());
                VertexIndex vertex_index = m_nativeMeshPreference.add_vertex(p);
                auto        coord        = textureCoordArray->at(i);
                std::cout << "color:" << image->getColor(coord).r() << image->getColor(coord).g()
                          << image->getColor(coord).b() << std::endl;
                vcmap[vertex_index]      =  CGAL::IO::Color(image->getColor(coord).r()*255,
                                                      image->getColor(coord).g() * 255,
                                                      image->getColor(coord).b() * 255);
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
            for (size_t i = 0; i < faces.size(); i+=3) {
                FaceIndex face_index = m_nativeMeshPreference.add_face(
                    VertexIndex(faces[i]), VertexIndex(faces[i+1]), VertexIndex(faces[i+2]));
            }
        }
    }

private:
    SurfaceMesh& m_nativeMeshPreference;
};

void OSGBMeshImpleMesh::read(const boost::filesystem::path& path_) {
    auto            node = osgDB::readNodeFile(path_.generic_string().c_str());
    OSGBMeshVisitor meshVisitor(m_nativeMesh);
    node->accept(meshVisitor);
}
}
