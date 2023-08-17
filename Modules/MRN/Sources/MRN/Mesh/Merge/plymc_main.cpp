/*
 *  plymc_main.cpp
 *  filter_plymc
 *
 *  Created by Paolo Cignoni on 10/23/09.
 *  Copyright 2009 ISTI - CNR. All rights reserved.
 *
 */
#include "meshReconstruction.h"
#include "simplemeshprovider.h"
#include <vcg/complex/algorithms/create/plymc/plymc.h>
#define _PLYMC_VER "4.0"

using namespace std;
using namespace vcg;

int saveMask = vcg::tri::io::Mask::IOM_ALL;

int meshReconstruction(int v_size, const char* mergePath, const char* plycutPath)
{

    Histogram<float>                                         h;
    tri::PlyMC<SMesh, SimpleMeshProvider<SMesh>>             pmc;
    tri::PlyMC<SMesh, SimpleMeshProvider<SMesh>>::Parameter& p = pmc.p;
    p.basename                                                 = plycutPath;
    p.MergeColor = true;
    p.VoxSize                                                  = v_size;
    p.NCell                                                  = 0;
    string alnfile;
    pmc.MP.AddSingleMesh(mergePath);
    pmc.Process();

    return 0;
}
