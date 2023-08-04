#pragma once

namespace MRN
{
class Mesh;
struct VCGSimplify
{
    void operator()(Mesh* mesh_, float level_);
};
}