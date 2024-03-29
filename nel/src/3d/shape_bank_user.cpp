/** \file shape_bank_user.cpp
 * Implementation of the user interface managing instance groups.
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

#include "nel/3d/shape_bank_user.h"
#include "nel/3d/driver_user.h"
#include "nel/misc/file.h"
#include "nel/misc/path.h"
#include "nel/misc/big_file.h"

using namespace NLMISC;
using namespace std;

namespace NL3D
{

// ***************************************************************************

void CShapeBankUser::addShapeCache(const std::string &shapeCacheName)
{
	_ShapeBank.addShapeCache( shapeCacheName );
}

// ***************************************************************************

void CShapeBankUser::removeShapeCache(const std::string &shapeCacheName)
{
	_ShapeBank.removeShapeCache( shapeCacheName );
}

// ***************************************************************************

void CShapeBankUser::reset()
{
	_ShapeBank.reset();
}

// ***************************************************************************

void CShapeBankUser::setShapeCacheSize(const std::string &shapeCacheName, sint32 maxSize)
{
	_ShapeBank.setShapeCacheSize( shapeCacheName, maxSize );
}

// ***************************************************************************

void CShapeBankUser::linkShapeToShapeCache(const std::string &shapeName, const std::string &shapeCacheName)
{
	_ShapeBank.linkShapeToShapeCache( shapeName, shapeCacheName );
}

// ***************************************************************************
void CShapeBankUser::preLoadShapesFromDirectory(const std::string &shapeCacheName,
	const std::string &path, const std::string &wildCardNotLwr, bool recurs, NLMISC::IProgressCallback *progress, bool flushTextures /*= false*/)
{
	// List all files From the path
	vector<string>	listFile;
	CPath::getPathContent(path, recurs, false, true, listFile);

	// preLoad
	if(!listFile.empty())
		_ShapeBank.preLoadShapes(shapeCacheName, listFile, wildCardNotLwr, progress, flushTextures, _DriverUser->getDriver());
}

// ***************************************************************************
void CShapeBankUser::preLoadShapesFromBNP(const std::string &shapeCacheName,
		const std::string &bnpName, const std::string &wildCardNotLwr, NLMISC::IProgressCallback *progress, bool flushTextures /*= false*/)
{
	// List all files From the bnp
	vector<string>	listFile;
	CBigFile::getInstance().list(bnpName, listFile);

	// preLoad
	if(!listFile.empty())
		_ShapeBank.preLoadShapes(shapeCacheName, listFile, wildCardNotLwr, progress, flushTextures, _DriverUser->getDriver());
}

// ***************************************************************************
UShape	CShapeBankUser::getShape(const std::string &shapeName)
{
	// get the IShape from the bank
	UShape	ret;
	ret.attach(_ShapeBank.getShape(shapeName));

	return ret;
}

// ***************************************************************************
void	CShapeBankUser::buildSystemGeometryForshape(const std::string &shapeName)
{
	_ShapeBank.buildSystemGeometryForshape(shapeName);
}


} // NL3D
