#include <MRN/Mesh/Implement/OSGB/OSGBMeshImpl.h>
#include <osgDB/ReadFile>
#include <osg/NodeVisitor>
#include <vector>
namespace MRN
{
class OSGBMeshVisitor : public osg::NodeVisitor
{
public:
    OSGBMeshVisitor(SurfaceMesh& mesh)
        : m_NativeMeshPreference(mesh)
    {}

    void apply(osg::Geode& geode)
    {
        for (size_t i = 0; i < geode.getNumChildren(); i++) {
            osg::Geometry* geom = geode.getDrawable(i)->asGeometry();
            if (geom == nullptr) continue;

            osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
            auto faces_uint = dynamic_cast<osg::DrawElementsUInt*>(geom->getPrimitiveSet(0));
            auto faces_ushort = dynamic_cast<osg::DrawElementsUShort*>(geom->getPrimitiveSet(0));

            if (!faces_uint && !faces_ushort) continue;
            if (!vertices) continue;

            std::vector<uint32_t> faces;
            if (!faces_uint) {
                for (size_t i = 0; i < faces_uint->size(); i++) {
                    faces.clear();
                    faces.push_back(faces_uint->getElement(i));
                }
            }
            if (!faces_ushort) {
                for (size_t i = 0; i < faces_ushort->size(); i++) {
                    faces.clear();
                    faces.push_back(faces_ushort->getElement(i));
                }
            }

            VertexPointMap vpmap =
                m_NativeMeshPreference
                    .property_map<VertexIndex, Point3>(
                "v:points").first;

            for (size_t i = 0; i < vertices->size(); i++) {
                auto v = vertices->at(i);
                VertexIndex vertex_index = m_NativeMeshPreference.add_vertex();
                vpmap[vertex_index]                    = Point3(v.x(), v.y(), v.z());
            }
            for (size_t i = 0; i < faces.size(); i+=3) {
                FaceIndex face_index = m_NativeMeshPreference.add_face(
                    VertexIndex(faces[i]), VertexIndex(faces[i+1]), VertexIndex(faces[i+2]));
            }
        }
    }

private:
    SurfaceMesh& m_NativeMeshPreference;
};

void OSGBMeshImpleMesh::read(const boost::filesystem::path& path_) {
    auto            node = osgDB::readNodeFile(path_.generic_string().c_str());
    if (node == nullptr) std::cout << "nullptr" << std::endl;
    OSGBMeshVisitor meshVisitor(m_nativeMesh);
    node->accept(meshVisitor);
}
}
