#include <MRN/Mesh/Implement/MeshImplBase.h>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/range/algorithm.hpp>
#include <wrap/io_trimesh/export.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>

namespace MRN
{
void MeshImplBase::write(const boost::filesystem::path& path_) {
    vcg::tri::io::Exporter<MyMeshOcf>::Save(m_nativeMesh, path_.generic_string().c_str());
}
void MeshImplBase::simpilify(float percent_)
{
    m_nativeMesh.face.EnableVFAdjacency();
    m_nativeMesh.face.EnableFFAdjacency();
    m_nativeMesh.vert.EnableVFAdjacency();
    vcg::tri::UpdateTopology<MyMeshOcf>::VertexFace(m_nativeMesh);
    vcg::tri::UpdateTopology<MyMeshOcf>::FaceFace(m_nativeMesh);
    

    vcg::tri::UpdateFlags<MyMeshOcf>::FaceBorderFromFF(m_nativeMesh);

    m_nativeMesh.face.EnableNormal();   // remove this line and you will throw an exception for a
                                        // missing
                                // 'normal' component
    vcg::tri::UpdateNormal<MyMeshOcf>::PerVertexPerFace(m_nativeMesh);

    //m_nativeMesh.vert.EnableQuality();

    int FinalSize = m_nativeMesh.face.size() * percent_;
    double TargetError = 1000000;
    vcg::tri::TriEdgeCollapseQuadricParameter qparams;
    vcg::tri::UpdateBounding<MyMeshOcf>::Box(m_nativeMesh);

    // decimator initialization
    vcg::LocalOptimization<MyMeshOcf> DeciSession(m_nativeMesh, &qparams);

    DeciSession.Init<MyTriEdgeCollapse>();
    printf("Initial Heap Size %i\n", int(DeciSession.h.size()));

    DeciSession.SetTargetSimplices(FinalSize);
    DeciSession.SetTimeBudget(0.5f);
    DeciSession.SetTargetOperations(100000);

    if (TargetError < std::numeric_limits<float>::max()) DeciSession.SetTargetMetric(TargetError);

    while (DeciSession.DoOptimization() && m_nativeMesh.fn > FinalSize &&
        DeciSession.currMetric < TargetError)
    {}

}
MyMeshOcf& MeshImplBase::getNativeMesh()
{
    return m_nativeMesh;
}
}