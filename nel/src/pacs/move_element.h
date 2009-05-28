/** \file move_element.h
 * Element to insert in sorted lists
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

#ifndef NL_MOVE_ELEMENT_H
#define NL_MOVE_ELEMENT_H

#include "nel/misc/types_nl.h"


namespace NLPACS
{

class CMovePrimitive;

/**
 * Move element linkable in sorted lists.
 *
 * \author Cyril 'Hulud' Corvazier
 * \author Nevrax France
 * \date 2001
 */
class CMoveElement
{
public:
	/// Pointer on the primitive for this move element
	CMovePrimitive	*Primitive;

	/// Next move element in the X List
	CMoveElement	*NextX;

	/// Previous move element in the X List
	CMoveElement	*PreviousX;

	/// Next move element in the Y List
	CMoveElement	*NextY;

	/// Previous move element in the Y List
	CMoveElement	*PreviousY;

	/// Cell coordinate
	uint16			X;
	uint16			Y;
};


} // NLPACS


#endif // NL_MOVE_ELEMENT_H

/* End of move_element.h */
