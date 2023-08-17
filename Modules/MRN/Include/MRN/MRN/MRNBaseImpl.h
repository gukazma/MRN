#pragma once

namespace MRN
{
	class MRNBaseImpl
	{
    public:
        MRNBaseImpl() = default;
        virtual bool check() { return false; }
        virtual void calculateTileArray() {  }
        virtual void merge() {  }
        virtual void writeTile() {  }
	};
}