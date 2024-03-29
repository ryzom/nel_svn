/** \file shadow_skin.cpp
 */

/* Copyright, 2000-2004 Nevrax Ltd.
 *
 * This file is part of NEVRAX NEL.
 * NEVRAX NEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * NEVRAX NEL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with NEVRAX NEL; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#include "std3d.h"

#include "nel/3d/shadow_skin.h"
#include "nel/3d/skeleton_model.h"
#include "nel/3d/ray_mesh.h"
#include "nel/misc/fast_mem.h"
#include "nel/misc/vector_2f.h"

using namespace NLMISC;
using namespace std;

namespace NL3D
{


// ***************************************************************************
// The number of byte to process per block
const	uint	NL_BlockByteL1= 4096;

// Number of vertices per block to process For ShadowMap generation
uint	CShadowSkin::NumCacheVertexShadow= NL_BlockByteL1 / sizeof(CShadowVertex);


// ***************************************************************************
void		CShadowSkin::applySkin(CVector *dst, std::vector<CMatrix3x4> &boneMat3x4)
{
	if(Vertices.empty())
		return;
	uint	numVerts= Vertices.size();
	CShadowVertex	*src= &Vertices[0];

	// Then do the skin
	for(;numVerts>0;)
	{
		// number of vertices to process for this block.
		uint	nBlockInf= min(NumCacheVertexShadow, numVerts);
		// next block.
		numVerts-= nBlockInf;

		// cache the data in L1 cache.
		CFastMem::precache(src, nBlockInf * sizeof(CShadowVertex));

		//  for all InfluencedVertices only.
		for(;nBlockInf>0;nBlockInf--, src++, dst++)
		{
			boneMat3x4[ src->MatrixId ].mulSetPoint( src->Vertex, *dst );
		}
	}
}


// ***************************************************************************
bool	CShadowSkin::getRayIntersection(const CMatrix &toRaySpace, CSkeletonModel &skeleton,
							   const std::vector<uint32> &matrixInfluences, float &dist2D, float &distZ, bool computeDist2D)
{
	// *** render the shadow skin into a temp RAM buffer
	// enlarge temp buffer
	static std::vector<CVector>	skinInRaySpace;
	if(Vertices.size()>skinInRaySpace.size())
		skinInRaySpace.resize(Vertices.size());

	// compute matrixes
	static	vector<CMatrix3x4>		boneMat3x4;
	computeBoneMatrixes3x4PreMul(boneMat3x4, toRaySpace, matrixInfluences, &skeleton);

	// apply the skinning
	applySkin(&skinInRaySpace[0], boneMat3x4);

	// *** return the distance to the ray intersection
	return CRayMesh::getRayIntersection(skinInRaySpace, Triangles, dist2D, distZ, computeDist2D);
}



} // NL3D
