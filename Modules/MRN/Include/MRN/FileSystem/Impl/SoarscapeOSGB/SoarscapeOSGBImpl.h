#pragma once
#include <MRN/FileSystem/Impl/FileSystemBase.h>
#include <vcg/space/box3.h>

namespace MRN
{
	struct tileCoord
	{
        int x, y;
        tileCoord(int x1, int y1) : x(x1) , y(y1){}
        bool operator<(const tileCoord& tc) const
        {
            return ((x < tc.x) || (x == tc.x && y < tc.y));
        }
	};

	class SoarscapeOSGBImpl : public FileSystemBase
	{
    public:
        SoarscapeOSGBImpl(const boost::filesystem::path& path_);
        virtual void getTileArray(TileArray& tileArray);

	private:
        std::string tile_intToString(int tileNumber, int n);
        vcg::Box3<float> getOsgbBox(boost::filesystem::path tilePath);
	};
}