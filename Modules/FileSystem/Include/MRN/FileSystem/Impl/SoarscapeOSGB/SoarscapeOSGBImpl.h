#pragma once
#include <MRN/FileSystem/Impl/FileSystemBase.h>

namespace MRN
{
	class SoarscapeOSGBImpl : public FileSystemBase
	{
    public:
        SoarscapeOSGBImpl(const boost::filesystem::path& path_);
        virtual void getTileArray(TileArray& tileArray);
	};
}