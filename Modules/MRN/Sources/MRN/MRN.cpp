#include "MRN/MRN.h"
#include <MRN/MRN/OSGB/MRNSoarscapeOSGB.h>
namespace MRN {
	MRN::MRN(const boost::filesystem::path& path_, VendorID type)
	{
        switch (type) {
        case SoarscapeOSGB: 
            m_impl = std::make_shared<MRNSoarscapeOSGB>(path_);
            break;
        default: break;
        }
        m_impl = std::make_shared<MRNBaseImpl>();
    }
    bool MRN::check()
    {
        return m_impl->check();

    }
    void MRN::calculateTileArray() {
        m_impl->calculateTileArray();
    }
    void MRN::merge() {
        m_impl->merge();
    }
    void MRN::writeTile() {
        m_impl->writeTile();
    }
}
