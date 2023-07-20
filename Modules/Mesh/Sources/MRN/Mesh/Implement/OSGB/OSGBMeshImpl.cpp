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
            /*for (size_t i = 0; i < vertices->size(); i++) {
                auto v = vertices->at(i);
                Point3      p = Point3(v.x(), v.y(), v.z());
                VertexIndex vertex_index = m_nativeMeshPreference.add_vertex(p);
                auto        coord        = textureCoordArray->at(i);
                std::cout << "color:" << image->getColor(coord).r() << image->getColor(coord).g()
                          << image->getColor(coord).b() << std::endl;
                vcmap[vertex_index]      =  CGAL::IO::Color(image->getColor(coord).r()*255,
                                                      image->getColor(coord).g() * 255,
                                                      image->getColor(coord).b() * 255);
            }*/

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
            std::map<Point3, VertexIndex> pvmap;
            
            for (size_t i = 0; i < faces.size(); i+=3) {
                auto        v0 = vertices->at(faces[i]);
                auto        v1 = vertices->at(faces[i+1]);
                auto        v2 = vertices->at(faces[i+2]);
                Point3      p0 = Point3(v0.x(), v0.y(), v0.z());
                Point3      p1 = Point3(v1.x(), v1.y(), v1.z());
                Point3      p2 = Point3(v2.x(), v2.y(), v2.z());
                

                if (pvmap.find(p0) == pvmap.end())
                {
                    pvmap[p0] = m_nativeMeshPreference.add_vertex(p0);
                    auto coord       = textureCoordArray->at(faces[i]);
                    vcmap[pvmap[p0]] = CGAL::IO::Color(image->getColor(coord).r() * 255,
                                                          image->getColor(coord).g() * 255,
                                                          image->getColor(coord).b() * 255);
                }
                if (pvmap.find(p1) == pvmap.end()) {
                    pvmap[p1]        = m_nativeMeshPreference.add_vertex(p1);
                    auto coord       = textureCoordArray->at(faces[i+1]);
                    vcmap[pvmap[p1]] = CGAL::IO::Color(image->getColor(coord).r() * 255,
                                                       image->getColor(coord).g() * 255,
                                                       image->getColor(coord).b() * 255);
                }
                if (pvmap.find(p2) == pvmap.end()) {
                    pvmap[p2]        = m_nativeMeshPreference.add_vertex(p2);
                    auto coord       = textureCoordArray->at(faces[i+2]);
                    vcmap[pvmap[p2]] = CGAL::IO::Color(image->getColor(coord).r() * 255,
                                                       image->getColor(coord).g() * 255,
                                                       image->getColor(coord).b() * 255);
                }
                FaceIndex face_index =
                    m_nativeMeshPreference.add_face(pvmap[p0], pvmap[p1], pvmap[p2]);
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
