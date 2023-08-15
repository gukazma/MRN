#include "MRN/Mesh/Utils/Simplify.h"

namespace MRN {
void simplify(MyMesh& mesh_, float percent_)
{
    int                                       FinalSize = mesh_.FN() * percent_;
    vcg::tri::TriEdgeCollapseQuadricParameter qparams;
    qparams.QualityThr       = .3;
    qparams.PreserveBoundary = false;
    double TargetError       = std::numeric_limits<double>::max();
    bool   CleaningFlag      = false;

    vcg::tri::UpdateBounding<MyMesh>::Box(mesh_);

    // decimator initialization
    vcg::LocalOptimization<MyMesh> DeciSession(mesh_, &qparams);

    int t1 = clock();
    DeciSession.Init<MyTriEdgeCollapse>();
    int t2 = clock();
    printf("Initial Heap Size %i\n", int(DeciSession.h.size()));

    DeciSession.SetTargetSimplices(FinalSize);
    DeciSession.SetTimeBudget(0.5f);
    DeciSession.SetTargetOperations(100000);
    if (TargetError < std::numeric_limits<float>::max()) DeciSession.SetTargetMetric(TargetError);

    while (DeciSession.DoOptimization() && mesh_.fn > FinalSize &&
           DeciSession.currMetric < TargetError)
        printf("Current Mesh size %7i heap sz %9i err %9g \n",
               mesh_.fn,
               int(DeciSession.h.size()),
               DeciSession.currMetric);

    int t3 = clock();
    printf("mesh  %d %d Error %g \n", mesh_.vn, mesh_.fn, DeciSession.currMetric);
    printf("\nCompleted in (%5.3f+%5.3f) sec\n",
           float(t2 - t1) / CLOCKS_PER_SEC,
           float(t3 - t2) / CLOCKS_PER_SEC);
    int dup   = vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh_);
    int unref = vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh_);
    printf("Removed %i duplicate and %i unreferenced vertices from mesh \n", dup, unref);
}

}   // namespace MRN