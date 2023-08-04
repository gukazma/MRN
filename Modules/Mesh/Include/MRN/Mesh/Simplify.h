#pragma once

namespace MRN
{
class Mesh;
template<class T> void simplify(Mesh* mesh_, float level_)
{
    T t;
    t(mesh_, level_);
}
}