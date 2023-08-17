#pragma once
#include <boost/filesystem/path.hpp>
#include <memory>
#include <MRN/Types.h>
namespace MRN {
class MRNBaseImpl;
class MRN
{
public:
    MRN(const boost::filesystem::path& path_, VendorID type = SoarscapeOSGB);
    bool check();
    void calculateTileArray();
    void merge();
    void writeTile();

private:
    std::shared_ptr<MRNBaseImpl> m_impl;
};
}   // namespace MRN