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
void OSGBMeshImpleMesh::write(const boost::filesystem::path& path_) {
    if (path_.extension() == ".osgb" || path_.extension() == ".osgt") {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec4Array> colors   = new osg::Vec4Array();
        std::map<VertexIndex, bool>  vindexBMap;
        std::map<VertexIndex, size_t>  vindexIMap;
        auto vcmap = m_nativeMesh.property_map<VertexIndex, CGAL::IO::Color>("v:color").first;
        auto vpmap = m_nativeMesh.points();
        for (auto v : m_nativeMesh.vertices()) {
            vindexBMap[v] = false;
        }
        
        osg::ref_ptr<osg::DrawElementsUInt> indices =
            new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);

        auto daw = m_nativeMesh.faces();
        for (auto f : m_nativeMesh.faces()) {
            auto hf0 = m_nativeMesh.halfedge(f);
            auto hf1 = m_nativeMesh.next(hf0);
            auto hf2 = m_nativeMesh.next(hf1);
            // 判断是否是三角形
            if (m_nativeMesh.next(hf2) != hf0) continue;

            auto v0 = m_nativeMesh.target(hf0);
            auto v1 = m_nativeMesh.target(hf1);
            auto v2 = m_nativeMesh.target(hf2);
            if (!vindexBMap[v0]) {
                auto p = vpmap[v0];
                auto c = vcmap[v0];
                vertices->push_back(osg::Vec3(p.x(), p.y(), p.z()));
                colors->push_back(
                    osg::Vec4(c.r() / 255.0, c.g() / 255.0, c.b() / 255.0, c.a() / 255.0));
                vindexBMap[v0] = true;
                vindexIMap[v0] = vertices->size() - 1;
            }
            if (!vindexBMap[v1]) {
                auto p = vpmap[v1];
                auto c = vcmap[v1];
                vertices->push_back(osg::Vec3(p.x(), p.y(), p.z()));
                colors->push_back(
                    osg::Vec4(c.r() / 255.0, c.g() / 255.0, c.b() / 255.0, c.a() / 255.0));
                vindexBMap[v1] = true;
                vindexIMap[v1] = vertices->size() - 1;
            }
            if (!vindexBMap[v2]) {
                auto p = vpmap[v2];
                auto c = vcmap[v2];
                vertices->push_back(osg::Vec3(p.x(), p.y(), p.z()));
                colors->push_back(
                    osg::Vec4(c.r() / 255.0, c.g() / 255.0, c.b() / 255.0, c.a() / 255.0));
                vindexBMap[v2] = true;
                vindexIMap[v2] = vertices->size() - 1;
            }
            

            indices->push_back(vindexIMap[v0]);
            indices->push_back(vindexIMap[v1]);
            indices->push_back(vindexIMap[v2]);
        }
        
        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        geometry->setColorArray(colors.get());
        geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry->setVertexArray(vertices);
        geometry->addPrimitiveSet(indices);
        osgDB::writeNodeFile(*geometry, path_.generic_string());
    }
    else {
        MeshImplBase::write(path_);
    }
}
}
