#pragma once
#include <MRN/Mesh/Implement/MeshImplBase.h>
namespace MRN
{
	class OSGBMeshImpleMesh : public MeshImplBase
	{
    public:
        virtual void read(const boost::filesystem::path& path_) override;
	};
}