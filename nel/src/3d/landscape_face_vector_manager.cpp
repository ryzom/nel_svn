/** \file landscape_face_vector_manager.cpp
 * TODO: File description
 */

/* Copyright, 2001 Nevrax Ltd.
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

#include "nel/3d/landscape_face_vector_manager.h"
#include "nel/misc/debug.h"


using namespace std;
using namespace NLMISC;


namespace NL3D
{


// ***************************************************************************
#define	NL3D_FACE_VECTOR_NUMBLOCK	33

// ***************************************************************************
CLandscapeFaceVectorManager::CLandscapeFaceVectorManager()
{
	// Allow 2^32 triangles at max. each list has at max 2^i triangles.
	_Blocks.resize(NL3D_FACE_VECTOR_NUMBLOCK, NULL);
}

// ***************************************************************************
CLandscapeFaceVectorManager::~CLandscapeFaceVectorManager()
{
	purge();
}

// ***************************************************************************
void					CLandscapeFaceVectorManager::purge()
{
	for(uint i=0; i<NL3D_FACE_VECTOR_NUMBLOCK; i++)
	{
		TLandscapeIndexType	*ptr= _Blocks[i];
		// For each node in list, delete.
		while(ptr)
		{
			// Get the ptr on next free list.
			TLandscapeIndexType	*next= *(TLandscapeIndexType**)ptr;
			delete []  ptr;
			ptr= next;
		}
		// list is empty.
		_Blocks[i]= NULL;
	}
}

// ***************************************************************************
uint	CLandscapeFaceVectorManager::getBlockIdFromNumTri(uint numTris)
{
	return getPowerOf2(numTris);
}

// ***************************************************************************
TLandscapeIndexType	*CLandscapeFaceVectorManager::createFaceVector(uint numTri)
{
	// get the BlockId from the number of tri in this fv
	uint	blockId= getBlockIdFromNumTri(numTri);

	// If no more free FaceVector, allocate.
	if(_Blocks[blockId]==NULL)
	{
		// Allocate a block of max tris. +1 is for the NumTris entry at index 0.
		uint	numTriMax= 1<<blockId;
		// allocate max of (sizeof(uint32*), (numTriMax*3+1)*sizeof(uint32));
		uint	sizeInByteToAllocate= max(sizeof(TLandscapeIndexType*), (numTriMax*3 + 1)*sizeof(TLandscapeIndexType));
		_Blocks[blockId]= new TLandscapeIndexType[(sizeInByteToAllocate + (sizeof(TLandscapeIndexType) - 1)) /sizeof(TLandscapeIndexType)];
		// Init it as a free faceVector, with no Next.
		*(TLandscapeIndexType**)_Blocks[blockId]= NULL;
	}

	// Pop a FaceVector from the free list.
	TLandscapeIndexType		*ret= _Blocks[blockId];
	// Make the head list point to next
	_Blocks[blockId]= *(TLandscapeIndexType**)ret;

	// There is numTri triangles.
	*ret= numTri;

	return ret;
}

// ***************************************************************************
void					CLandscapeFaceVectorManager::deleteFaceVector(TLandscapeIndexType	*fv)
{
	// get the BlockId from the number of tri in this fv (ie *fv)
	uint	blockId= getBlockIdFromNumTri(*fv);

	// Append this block to the free list. Write the ptr directly on fv.
	*(TLandscapeIndexType**)fv= _Blocks[blockId];
	_Blocks[blockId]= fv;
}


} // NL3D
