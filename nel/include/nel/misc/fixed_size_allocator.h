/** \file fixed_size_allocator.h
 */

/* Copyright, 2000, 2001, 2002, 2003 Nevrax Ltd.
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

#ifndef NL_FIXED_SIZE_ALLOCATOR_H
#define NL_FIXED_SIZE_ALLOCATOR_H


namespace NLMISC
{

/** An allocator that can allocate and deallocate blocks of fixed size in O(1)
  * Blocks are managed by chunks. Any number of blocks can be allocated, but once
  * a block is allocated, a whole chunk need to be.
  * With 32 bits pointers, there may be a 4 - 12 bytes overhead per object
  * NB: Unlike CBlockManager, when a chunk contains no allocated blocks, it will be freed.
  *     If only one chunk remains, it isn't deleted, however.
  *     Another motivation for that class is that it can be used for a memory arena style allocator, because
  *     size isn't a fixed parameter of template. A fixed size allocator implemented as a template for type T can be layered on this implementation.
  *
  * NB : number of blocks per chunks must be at least 3
  *
  * \author Nicolas Vizerie
  * \author Nevrax France
  * \date 2004
  */
class CFixedSizeAllocator
{
public:
	CFixedSizeAllocator(uint numBytesPerBlock, uint numBlockPerChunk);
	~CFixedSizeAllocator();
	/// alloc a block
	void *alloc();
	/// destroy and dealloc a block
	void free(void *block);
	//
	uint getNumBytesPerBlock() const { return _NumBytesPerBlock; }
	uint getNumBlockPerChunk() const { return _NumBlockPerChunk; }
	//
	uint getNumAllocatedBlocks() const { return _NumAlloc; }
private:
	class CChunk;
	class CNode
	{
	public:
		CChunk *Chunk; // the Chunk this node belongs to.
		// NB: blocks starts here in memory when node is allocated
		CNode  *Next;  // points the next free block. Valid only if node is not allocated, otherwise, block datas overlap tht field.
		CNode  **Prev; // points the previous node 'Next' field. Valid only if node is not allocated, otherwise, block datas overlap that field.
		// Unlink this node from the chunk it is in
		// Then it is considered allocated
	public:
		void *unlink();
		// Link back this node to the chunk it belongs to.
		// Then, it is added as a free node in the chunk and in the allocator
		// If all node of a chunk are unused, and it is not the last chunk of the allocator, it
		// is removed.
		void link();
	};
	class CChunk
	{
	public:
		uint						NumFreeObjs;
		CFixedSizeAllocator			*Allocator;
		uint8						*Mem;
	public:
		// ctor
		CChunk();
		// dtor
		~CChunk();
		// init & allocate memory
		void init(CFixedSizeAllocator *fsa);
		// access to a node
		CNode &getNode(uint index);
		// get the block size in bytes (header included)
		uint  getBlockSizeWithOverhead() const;
		/// a chunk has been given back
		inline void add();
		/// a chunk has been taken
		inline void grab();
	};
	friend class CChunk;
	friend class CNode;
	//
	CNode	*_FreeSpace;
	uint     _NumChunks; // The number of created chunks. Once one of them is created, we manage
						 // to keep it alive in order to avoid chunk creation / destructions
						 // if only a few blocks are used
	uint	_NumBytesPerBlock;
	uint	_NumBlockPerChunk;
	//
	uint	_NumAlloc;
};

} // NLMISC

#endif
