#include <MRN/FileSystem/Impl/FileSystemBase.h>

MRN::FileSystemBase::FileSystemBase(const boost::filesystem::path& path_)
    : m_dir(path_)
{}