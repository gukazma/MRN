#include <MRN/Mesh/Mesh.h>
#include <MRN/Mesh/Implement/OSGB/OSGBMeshImpl.h>
namespace MRN
{
	Mesh::Mesh(const boost::filesystem::path& path_, DataType type) {
        switch (type) {
        case MRN::Mesh::OSGB: 
             m_meshImplement = std::make_shared<OSGBMeshImpleMesh>();
             break;
        case MRN::Mesh::GLTF: break;
        default: break;
        }
        m_meshImplement->read(path_);
	}

    void Mesh::write(const boost::filesystem::path& path_)
    {
        m_meshImplement->write(path_);
    }


    MyMeshOcf& Mesh::getNativMesh()
    {
        return m_meshImplement->getNativeMesh();
    }

}