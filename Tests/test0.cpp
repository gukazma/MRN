#include <gtest/gtest.h>
#include <MRN/Mesh/Mesh.h>
TEST(MRN, OSGBRead)
{
    MRN::Mesh mesh("D:/data/�ϲ����ڵ�/test/Tile_+000_+014/Tile_+000_+014_15_0.osgb");
    mesh.write("D:/data/test.ply");
}