/** \file pool_memory.h
 * Pool memory allocation
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

#ifndef NL_POOL_MEMORY_H
#define NL_POOL_MEMORY_H

#include "types_nl.h"
#include <list>
#include <vector>

namespace NLMISC
{


/**
 * Pool memory allocation
 *
 * This memory manager allocates bloc of elements and free all the
 * elements at the same time. Useful for temporary allocation.
 *
 * \author Cyril 'Hulud' Corvazier
 * \author Nevrax France
 * \date 2001
 */

template<class T>
class CPoolMemory
{
public:
	/*
	 * Constructor
	 *
	 * \param blockSize is the default bloc size
	 */
	CPoolMemory (uint blockSize=10)
	{
		_BlockSize=blockSize;
		_BlockPointer=_BlockList.end();
	}

	/*
	 * Allocate an element.
	 *
	 * Return a pointer on the element.
	 */
	T*		allocate ()
	{
		// End of block ?
		if ( (_BlockPointer!=_BlockList.end()) && (_BlockPointer->size()>=_BlockSize) )
		{
			// Get next block
			_BlockPointer++;
		}

		// Out of memory ?
		if (_BlockPointer==_BlockList.end())
		{
			// Add a block
			_BlockList.resize (_BlockList.size()+1);

			// Pointer on the new block
			_BlockPointer=_BlockList.end ();
			_BlockPointer--;

			// Reserve it
			_BlockPointer->reserve (_BlockSize);
		}

		// Allocate
		_BlockPointer->resize (_BlockPointer->size()+1);

		// Return the pointer
		return &*((_BlockPointer->end ()-1));
	}

	/*
	 * Free all the elements allocated since last free(). Memory is kept for next allocations.
	 */
	void	free ()
	{
		typename std::list< std::vector<T> >::iterator ite=_BlockList.begin();
		while (ite!=_BlockList.end())
		{
			// Clear the block
			ite->clear ();

			// Check size in not zero
			nlassert (ite->capacity ()>0);

			ite++;
		}
		// Pointer at the begining
		_BlockPointer=_BlockList.begin();
	}

	/*
	 * Purge memory. All the memory used by the allocator is freid for real.
	 */
	void	purge ()
	{
		_BlockList.clear();
		_BlockPointer=_BlockList.end();
	}

private:
	uint									_BlockSize;
	std::list< std::vector<T> >				_BlockList;
	typename std::list< std::vector<T> >::iterator	_BlockPointer;
};


} // NLMISC


#endif // NL_POOL_MEMORY_H

/* End of pool_memory.h */
