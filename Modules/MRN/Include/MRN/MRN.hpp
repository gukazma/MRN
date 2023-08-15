#pragma once
#include <boost/filesystem/path.hpp>
#include <memory>
namespace MRN {
template<class T> class MRN
{
public:
    MRN(const boost::filesystem::path& path_)
        : impl(path_)
    {}
    bool check() { return impl.check(); }
    void calculateTileArray() { impl.calculateTileArray(); }
    void merge() { impl.merge(); }
    void cutCake() { impl.catCake(); }
    void writeTile() { impl.writeTile(); }

private:
    T impl;
};
}   // namespace MRN