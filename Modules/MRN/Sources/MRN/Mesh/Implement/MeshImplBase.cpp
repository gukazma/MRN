#include <MRN/Mesh/Implement/MeshImplBase.h>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/range/algorithm.hpp>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/import.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/space/index/kdtree/kdtree.h>
namespace MRN {
void MeshImplBase::read(const boost::filesystem::path& path_) {
    vcg::tri::io::Importer<MyMesh>::Open(m_nativeMesh, path_.generic_string().c_str());
}
void MeshImplBase::write(const boost::filesystem::path& path_)
{
    vcg::tri::io::Exporter<MyMesh>::Save(m_nativeMesh, path_.generic_string().c_str());
}
void MeshImplBase::simpilify(float percent_)
{
    int                                       FinalSize = m_nativeMesh.FN() * percent_;
    vcg::tri::TriEdgeCollapseQuadricParameter qparams;
    qparams.QualityThr  = .3;
    qparams.PreserveBoundary  = false;
    double TargetError  = std::numeric_limits<double>::max();
    bool   CleaningFlag = false;

    vcg::tri::UpdateBounding<MyMesh>::Box(m_nativeMesh);

    // decimator initialization
    vcg::LocalOptimization<MyMesh> DeciSession(m_nativeMesh, &qparams);

    int t1 = clock();
    DeciSession.Init<MyTriEdgeCollapse>();
    int t2 = clock();
    printf("Initial Heap Size %i\n", int(DeciSession.h.size()));

    DeciSession.SetTargetSimplices(FinalSize);
    DeciSession.SetTimeBudget(0.5f);
    DeciSession.SetTargetOperations(100000);
    if (TargetError < std::numeric_limits<float>::max()) DeciSession.SetTargetMetric(TargetError);

    while (DeciSession.DoOptimization() && m_nativeMesh.fn > FinalSize &&
           DeciSession.currMetric < TargetError)
        printf("Current Mesh size %7i heap sz %9i err %9g \n",
               m_nativeMesh.fn,
               int(DeciSession.h.size()),
               DeciSession.currMetric);

    int t3 = clock();
    printf("mesh  %d %d Error %g \n", m_nativeMesh.vn, m_nativeMesh.fn, DeciSession.currMetric);
    printf("\nCompleted in (%5.3f+%5.3f) sec\n",
           float(t2 - t1) / CLOCKS_PER_SEC,
           float(t3 - t2) / CLOCKS_PER_SEC);
    int dup   = vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m_nativeMesh);
    int unref = vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(m_nativeMesh);
    printf("Removed %i duplicate and %i unreferenced vertices from mesh \n", dup, unref);
}
MyVertex MeshImplBase::getClosest(vcg::Point3f point)
{
    std::cout << "===================================================" << std::endl;
    std::cout << "KDTree" << std::endl;
    // Construction of the kdTree
    vcg::ConstDataWrapper<MyMesh::VertexType::CoordType> wrapperVcg(
        &m_nativeMesh.vert[0].P(),
        m_nativeMesh.vert.size(),
        size_t(m_nativeMesh.vert[1].P().V()) - size_t(m_nativeMesh.vert[0].P().V()));
    vcg::KdTree<MyMesh::ScalarType> kdTreeVcg(wrapperVcg);
    unsigned int                             index = 0;
    float                                    minidistance;
    kdTreeVcg.doQueryClosest(point, index, minidistance);
    return m_nativeMesh.vert[index];
}
MyMesh& MeshImplBase::getNativeMesh()
{
    return m_nativeMesh;
}
}