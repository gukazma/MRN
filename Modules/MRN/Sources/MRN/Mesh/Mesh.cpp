#include <MRN/Mesh/Implement/OSGB/OSGBMeshImpl.h>
#include <MRN/Mesh/Mesh.h>
namespace MRN {
Mesh::Mesh(const boost::filesystem::path& path_, VendorID type)
{
    switch (type) {
    case SoarscapeOSGB: m_meshImplement = std::make_shared<OSGBMeshImpleMesh>(); break;
    default: m_meshImplement = std::make_shared<MeshImplBase>(); break;
    }
    m_meshImplement->read(path_);
}

void Mesh::write(const boost::filesystem::path& path_) {
    m_meshImplement->write(path_);
}

void Mesh::write(const Tile& tile)
{
    m_meshImplement->write(tile);
}

void Mesh::simpilify(float percent_)
{
    m_meshImplement->simpilify(percent_);
}

Mesh::~Mesh()
{
    m_meshImplement.reset();
}

Mesh::Mesh() {
    m_meshImplement = std::make_shared<MeshImplBase>();
}


MyMesh& Mesh::getNativMesh()
{
    return m_meshImplement->getNativeMesh();
}

}   // namespace MRN