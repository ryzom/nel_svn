/** \file scheme_manager.cpp
 * a collection of scheme (to set particle atributes)
 *
 * $Id: scheme_manager.cpp,v 1.5 2007/03/19 09:55:26 boucher Exp $
 */

/* Copyright, 2000 Nevrax Ltd.
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

#include "std_afx.h"
#include "scheme_manager.h"
#include "nel/3d/ps_attrib_maker.h"

CSchemeManager SchemeManager;


CSchemeManager::~CSchemeManager()
{
	for(TSchemeMap::iterator it = _SchemeMap.begin(); it != _SchemeMap.end(); ++it)
	{
		delete it->second.second;
	}
}


void CSchemeManager::insertScheme(const std::string &name, NL3D::CPSAttribMakerBase *scheme)
{
	nlassert(scheme);
	TSchemeInfo si(std::string(name), scheme);
	_SchemeMap.insert(TSchemeMap::value_type(std::string(scheme->getType()), si));	
}

void CSchemeManager::getSchemes(const std::string &type, std::vector<TSchemeInfo> &dest)
{
	TSchemeMap::const_iterator lbd = _SchemeMap.lower_bound(type), ubd = _SchemeMap.upper_bound(type);
	dest.clear();
	for (TSchemeMap::const_iterator it = lbd; it != ubd; ++it)
	{
		dest.push_back(it->second);
	}
}

void CSchemeManager::serial(NLMISC::IStream &f) throw(NLMISC::EStream)
{
	
	f.serialCheck((uint32) '_GNM');
	f.serialCheck((uint32) 'MHCS');
	f.serialVersion(1);
	if (!f.isReading())
	{		
		sint32 size = _SchemeMap.size();		
		f.serial(size);					
		for (TSchemeMap::iterator smIt = _SchemeMap.begin(); smIt != _SchemeMap.end(); ++smIt)
		{
			f.serial(smIt->second.first);		  // name
			f.serialPolyPtr(smIt->second.second); // scheme
		}		
	}
	else
	{
		_SchemeMap.clear();
		
		std::string name;
		NL3D::CPSAttribMakerBase *scheme = NULL;
		sint32 size;
		f.serial(size);
		for (sint32 k = 0; k < size; ++k)
		{		
			f.serial(name);
			f.serialPolyPtr(scheme);
			insertScheme(name, scheme);

		}
		
	}
}

void	CSchemeManager::swap(CSchemeManager &other)
{
	this->_SchemeMap.swap(other._SchemeMap);
}

void    CSchemeManager::remove(NL3D::CPSAttribMakerBase *am)
{
	TSchemeMap::iterator smIt;
	for (smIt = _SchemeMap.begin(); smIt != _SchemeMap.end(); ++smIt)
	{
		if (smIt->second.second == am) break;
	}
	if (smIt != _SchemeMap.end())
	{
		delete smIt->second.second; // delete the scheme
		// delet from the collection
		_SchemeMap.erase(smIt);
	}
}



// rename a scheme, given a pointer on it
void  CSchemeManager::rename(NL3D::CPSAttribMakerBase *am, const std::string &newName)
{
	TSchemeMap::iterator smIt;
	for (smIt = _SchemeMap.begin(); smIt != _SchemeMap.end(); ++smIt)
	{
		if (smIt->second.second == am) break;
	}
	if (smIt != _SchemeMap.end())
	{
		smIt->second.first = newName;		
	}
}