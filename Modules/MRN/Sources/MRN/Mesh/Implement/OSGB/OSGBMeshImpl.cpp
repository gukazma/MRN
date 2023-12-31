#include <CGAL/IO/Color.h>
#include <MRN/Mesh/Implement/OSGB/OSGBMeshImpl.h>
#include <fstream>
#include <osg/NodeVisitor>
#include <osg/Texture2D>
#include <osg/PagedLOD>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/LOD>
#include <CGAL/IO/Color.h>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <vcg/complex/algorithms/clean.h>
#include <vector>


namespace MRN {
class OSGBMeshVisitor : public osg::NodeVisitor
{
public:
    OSGBMeshVisitor(MyMesh& mesh_)
        : m_mesh(mesh_)
        , osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
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
        vcg::tri::UpdateTopology<MyMesh>::VertexFace(m_mesh);
        vcg::tri::UpdateBounding<MyMesh>::Box(m_mesh);
        vcg::tri::UpdateFlags<MyMesh>::FaceBorderFromVF(m_mesh);
        vcg::tri::UpdateFlags<MyMesh>::VertexBorderFromFaceBorder(m_mesh);
    }
    MyMesh& m_mesh;
};

void OSGBMeshImpleMesh::read(const boost::filesystem::path& path_)
{
    if (!(path_.extension() == ".osgb" || path_.extension() == ".osgt")) {
        MeshImplBase::read(path_);
        return;
    }
    osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(path_.generic_string().c_str());
    OSGBMeshVisitor meshVisitor(m_nativeMesh);
    node->accept(meshVisitor);
}

void OSGBMeshImpleMesh::write(const boost::filesystem::path& path_)
{
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m_nativeMesh);
    if (path_.extension() == ".osgb" || path_.extension() == ".osgt") {
        osg::ref_ptr<osg::Vec3Array>                   vertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec4Array>                   colors   = new osg::Vec4Array();
        std::unordered_map<vcg::Point3f, int>          vimap;
        std::unordered_map<vcg::Point3f, vcg::Color4b> vcmap;

        osg::ref_ptr<osg::DrawElementsUInt> indices =
            new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
        for (size_t i = 0; i < m_nativeMesh.vert.size(); i++) {
            auto& v = m_nativeMesh.vert[i];

            if (v.IsD()) continue;
            auto p   = v.P();
            vimap[p] = vertices->size();
            vertices->push_back({p[0], p[1], p[2]});
            auto c   = v.C();
            vcmap[p] = c;
            colors->push_back({c[0] / 255.0f, c[1] / 255.0f, c[2] / 255.0f, c[3] / 255.0f});
        }

        for (size_t i = 0; i < m_nativeMesh.face.size(); i++) {
            if (m_nativeMesh.face[i].IsD()) continue;
            indices->push_back(vimap[m_nativeMesh.face[i].V(0)->P()]);
            indices->push_back(vimap[m_nativeMesh.face[i].V(1)->P()]);
            indices->push_back(vimap[m_nativeMesh.face[i].V(2)->P()]);
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
void OSGBMeshImpleMesh::write(const Tile& tile)
{
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m_nativeMesh);
    auto path = tile.tilePath;
    if (path.extension() == ".osgb" || path.extension() == ".osgt") {
        osg::ref_ptr<osg::Vec3Array>                   vertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec4Array>                   colors   = new osg::Vec4Array();
        std::unordered_map<vcg::Point3f, int>          vimap;
        std::unordered_map<vcg::Point3f, vcg::Color4b> vcmap;

        osg::ref_ptr<osg::DrawElementsUInt> indices =
            new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
        for (size_t i = 0; i < m_nativeMesh.vert.size(); i++) {
            auto& v = m_nativeMesh.vert[i];

            if (v.IsD()) continue;
            auto p   = v.P();
            vimap[p] = vertices->size();
            vertices->push_back({p[0], p[1], p[2]});
            auto c   = v.C();
            vcmap[p] = c;
            colors->push_back({c[0] / 255.0f, c[1] / 255.0f, c[2] / 255.0f, c[3] / 255.0f});
        }

        for (size_t i = 0; i < m_nativeMesh.face.size(); i++) {
            if (m_nativeMesh.face[i].IsD()) continue;
            indices->push_back(vimap[m_nativeMesh.face[i].V(0)->P()]);
            indices->push_back(vimap[m_nativeMesh.face[i].V(1)->P()]);
            indices->push_back(vimap[m_nativeMesh.face[i].V(2)->P()]);
        }

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        geometry->setColorArray(colors.get());
        geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry->setVertexArray(vertices);
        geometry->addPrimitiveSet(indices);
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(geometry.get());

        osg::ref_ptr<osg::Group>    group    = new osg::Group;
        // ��ȡbbox��Sphere
        osg::Vec3f          bboxMin(tile.box.min.X(), tile.box.min.Y(), tile.box.min.Z());
        osg::Vec3f          bboxMax(tile.box.max.X(), tile.box.max.Y(), tile.box.max.Z());
        osg::BoundingBox    bbox(bboxMin, bboxMax);
        osg::BoundingSphere sphere(bbox);
        for (int i = 0; i < tile.parentPaths.size(); i++) {
            osg::ref_ptr<osg::PagedLOD> pagedLod = new osg::PagedLOD();
            pagedLod->setRangeMode(osg::LOD::PIXEL_SIZE_ON_SCREEN);
            pagedLod->setCenterMode(osg::LOD::USER_DEFINED_CENTER);
            pagedLod->setCenter(sphere.center());
            pagedLod->setRadius(sphere.radius());
            pagedLod->addChild(geode.get(), 0, tile.threshold, "");
            pagedLod->setDatabasePath("");
            std::string flag;
            if (tile.firstTile) {
                flag =
                    "../../" +
                    tile.parentPaths[i].tilePath.parent_path().parent_path().filename().string() +
                    "/";
                std::cout << flag << std::endl;
            }
            else {
                flag = "../";
            }
            
            std::string parentFileName =
                flag + tile.parentPaths[i].tilePath.parent_path().filename().generic_string() +
                "/" + tile.parentPaths[i].tilePath.filename().generic_string();
            pagedLod->setFileName(1, parentFileName);
            pagedLod->setRange(1, tile.threshold, 1e30);
            group->addChild(pagedLod.get());
        }

        osgDB::writeNodeFile(*(group.get()), path.generic_string());
    }
    else {
        MeshImplBase::write(path);
    }
}
}
