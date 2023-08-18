/****************************************************************************
 * VCGLib                                                            o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2012                                           \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                                                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
 * for more details.                                                         *
 *                                                                           *
 ****************************************************************************/

#include <vcg/complex/complex.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/import.h>


#include <vcg/complex/algorithms/crease_cut.h>
#include <vcg/complex/algorithms/curve_on_manifold.h>
#include <vcg/complex/algorithms/cut_tree.h>

using namespace vcg;

class MyEdge;
class MyFace;
class MyVertex;
struct MyUsedTypes : public UsedTypes<Use<MyVertex>::AsVertexType, Use<MyEdge>::AsEdgeType,
                                      Use<MyFace>::AsFaceType>
{};

class MyVertex : public Vertex<MyUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::Qualityf,
                               vertex::Color4b, vertex::VEAdj, vertex::VFAdj, vertex::BitFlags>
{};
class MyEdge : public Edge<MyUsedTypes, edge::VertexRef, edge::VEAdj, edge::EEAdj, edge::BitFlags>
{};
class MyFace
    : public Face<MyUsedTypes, face::VertexRef, face::Normal3f, face::Qualityf, face::Color4b,
                  face::VFAdj, face::FFAdj, face::Mark, face::Color4b, face::BitFlags>
{};
class MyMesh : public tri::TriMesh<std::vector<MyVertex>, std::vector<MyEdge>, std::vector<MyFace>>
{};

/**
 * In this sample we take a torus we compute a poly line on it that open it into a disk and we open
 * it. Then using the COM (Curve On Manifold) framework we smooth this polyline keeping it on the
 * surface of the torus and then first we refine the torus surface with this smooth polyline and
 * then we open it along these new edges.
 *
 * Optionally you can use your own mesh and polyline by passing them as parameters.
 */
int main(int argc, char** argv)
{

    MyMesh mesh;
    vcg::tri::io::Importer<MyMesh>::Open(mesh, "D:/data/MRNDATA/output/plymcout.ply");

    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
    vcg::tri::Clean<MyMesh>::RemoveSmallConnectedComponentsSize(mesh, 20);
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
    vcg::tri::io::Exporter<MyMesh>::Save(
        mesh, "D:/data/MRNDATA/output/plymcout_remove.ply", vcg::tri::io::Mask::IOM_VERTCOLOR);

    return 0;
}
