#pragma once
#include <boost/filesystem/path.hpp>
#include <memory>
namespace MRN
{
	template <class T>
	class MRN
	{
    public:
        MRN(const boost::filesystem::path& path_)
            : impl(path_)
        {}
		bool check() { 
			return impl.check();
		}

	private:
        T impl;
	};
}