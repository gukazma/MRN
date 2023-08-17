#pragma once
#include <MRN/Mesh/Mesh.h>
#include <vector>
#include <boost/filesystem/path.hpp>
namespace MRN {
class Merge
{
public:
    Merge(const boost::filesystem::path& path_);

    void init();

    void process();

    void getMerged(MyMesh& mesh);

private:
    boost::filesystem::path m_path;
    boost::filesystem::path m_mergePath;
    boost::filesystem::path m_plymcout;
};
}   // namespace MRN