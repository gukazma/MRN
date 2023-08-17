#include <gtest/gtest.h>
#include <MRN/Mesh/Types.h>
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <MRN/Mesh/Utils/Cut.h>
TEST(MRN, Cut_impl)
{
    auto                    env         = boost::this_process::environment();
    boost::filesystem::path MRNDataPath = env["MRNDATA"].to_string();
    boost::filesystem::path inputDir    = MRNDataPath / "ply/plymcout.ply";
    boost::filesystem::path output      = MRNDataPath / "output/testcut.ply";
    MyMesh                  mesh;
    vcg::tri::io::Importer<MyMesh>::Open(mesh, inputDir.generic_string().c_str());
    MyMesh cutmesh;
    auto             bbox = mesh.bbox;
    vcg::Box3<float> box;
    box.min = bbox.min;
    box.max = box.min + vcg::Point3f{bbox.DimX() / 2, bbox.DimY() / 2, bbox.DimZ() / 2.0f};
    MRN::cut(cutmesh, mesh, bbox);
    vcg::tri::io::ExporterPLY<MyMesh>::Save(
        mesh, output.generic_string().c_str(), vcg::tri::io::Mask::IOM_VERTCOLOR);
}