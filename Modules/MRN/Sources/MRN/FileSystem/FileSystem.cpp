#include "MRN/FileSystem/FileSystem.h"
#include <MRN/FileSystem/Impl/SoarscapeOSGB/SoarscapeOSGBImpl.h>
namespace MRN
{

FileSystem::FileSystem(const boost::filesystem::path& dir_, Type type) {
    switch (type) {
    case MRN::FileSystem::SoarscapeOSGB: 
        m_impl = std::make_shared<SoarscapeOSGBImpl>(dir_);
        break;
    default: m_impl = std::make_shared<FileSystemBase>(dir_); break;
    }
    
}

void FileSystem::getTileArray(TileArray& tileArray) {
    m_impl->getTileArray(tileArray);
}

}
