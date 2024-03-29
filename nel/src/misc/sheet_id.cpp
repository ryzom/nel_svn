/** \file sheet_id.cpp
 * This class defines a sheet id
 */

/* Copyright, 2002 Nevrax Ltd.
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

/* This class is case unsensitive. It means that you can call build() and
 * buildIdVector() with string with anycase, it'll work.
 */

#include "stdmisc.h"

#include "nel/misc/file.h"
#include "nel/misc/path.h"

#include "nel/misc/sheet_id.h"
#include "nel/misc/common.h"
#include "nel/misc/hierarchical_timer.h"

using namespace std;

namespace NLMISC {

CSheetId::CChar CSheetId::_AllStrings;
CStaticMap<uint32,CSheetId::CChar> CSheetId::_SheetIdToName;
CStaticMap<CSheetId::CChar,uint32, CSheetId::CCharComp> CSheetId::_SheetNameToId;
//map<uint32,std::string> CSheetId::_SheetIdToName;
//map<std::string,uint32> CSheetId::_SheetNameToId;
vector<std::string> CSheetId::_FileExtensions;
bool CSheetId::_Initialised=false;
bool CSheetId::_RemoveUnknownSheet=true;
bool CSheetId::_DontHaveSheetKnowledge = false;

const CSheetId CSheetId::Unknown(0);

void CSheetId::cbFileChange (const std::string &filename)
{
	nlinfo ("SHEETID: %s changed, reload it", filename.c_str());

	loadSheetId();
}

//-----------------------------------------------
//	CSheetId
//
//-----------------------------------------------
CSheetId::CSheetId( uint32 sheetRef)
{
	_Id.Id = sheetRef;

#ifdef NL_DEBUG_SHEET_ID
	// Yoyo: don't access the static map, because of order of static ctor call.
	// For now, all static CSheetId are 0 (eg: CSheetId::Unknown)
	if(sheetRef)
	{
		CStaticMap<uint32, CChar>::iterator it(_SheetIdToName.find(sheetRef));
		if (it != _SheetIdToName.end())
		{
			_DebugSheetName = it->second.Ptr;
		}
		else
			_DebugSheetName = NULL;
	}
	else
	{
		_DebugSheetName = NULL;
	}
#endif
}


//-----------------------------------------------
//	CSheetId
//
//-----------------------------------------------
CSheetId::CSheetId( const string& sheetName )
{
	if (!buildSheetId(sheetName))
	{
		if(sheetName.empty())
			nlwarning("SHEETID: Try to create an CSheetId with empty name. TODO: check why.");
		else
			nlwarning("SHEETID: The sheet '%s' is not in sheet_id.bin, setting it to Unknown",sheetName.c_str());
		//std::string stack;
		//NLMISC::getCallStack(stack);
		//std::vector<std::string> contexts;
		//NLMISC::explode(stack, string("\n"), contexts);
		//nldebug("Dumping callstack :");
		//for (uint i=0; i<contexts.size(); ++i)
		//	nldebug("  %3u : %s", i, contexts[i].c_str());
		*this = Unknown;
	}

} // CSheetId //


//-----------------------------------------------
//	Build
//
//-----------------------------------------------
bool CSheetId::buildSheetId(const std::string& sheetName)
{
	nlassert(_Initialised);
	nlassert(!_DontHaveSheetKnowledge);

	// try looking up the sheet name in _SheetNameToId
	CStaticMap<CChar,uint32,CCharComp>::const_iterator itId;
	CChar c;
	c.Ptr = new char [sheetName.size()+1];
	strcpy(c.Ptr, sheetName.c_str());
	toLower(c.Ptr);

	itId = _SheetNameToId.find (c);
	delete [] c.Ptr;
	if( itId != _SheetNameToId.end() )
	{
		_Id.Id = itId->second;
#ifdef NL_DEBUG_SHEET_ID
		// store debug info
		_DebugSheetName = itId->first.Ptr;
#endif
		return true;
	}

	// we failed to find the sheet name in the sheetname map so see if the string is numeric
	if (sheetName.size()>1 && sheetName[0]=='#')
	{
		uint32 numericId;
		NLMISC::fromString((const char*)(sheetName.c_str()+1), numericId);
		if (NLMISC::toString("#%u",numericId)==sheetName)
		{
			_Id.Id= numericId;
			return true;
		}
	}
	return false;
}

void CSheetId::loadSheetId ()
{
	H_AUTO(CSheetIdInit);
	nldebug("Loading sheet_id.bin");

	// Open the sheet id to sheet file name association
	CIFile file;
	std::string path = CPath::lookup("sheet_id.bin", false, false);
	if(!path.empty() && file.open(path))
	{
		// clear entries
		_FileExtensions.clear ();
		_SheetIdToName.clear ();
		_SheetNameToId.clear ();

		// reserve space for the vector of file extensions
		_FileExtensions.resize(1 << (NL_SHEET_ID_TYPE_BITS));

		// Get the map from the file
		map<uint32,string> tempMap;
		contReset(tempMap);
		file.serialCont(tempMap);
		file.close();

		if (_RemoveUnknownSheet)
		{
			uint32 removednbfiles = 0;
			uint32 nbfiles = tempMap.size();

			// now we remove all files that not available
			map<uint32,string>::iterator itStr2;
			for( itStr2 = tempMap.begin(); itStr2 != tempMap.end(); )
			{
				if (CPath::exists ((*itStr2).second))
				{
					++itStr2;
				}
				else
				{
					map<uint32,string>::iterator olditStr = itStr2;
					//nldebug ("Removing file '%s' from CSheetId because the file not exists", (*olditStr).second.c_str ());
					itStr2++;
					tempMap.erase (olditStr);
					removednbfiles++;
				}
			}
			nlinfo ("SHEETID: Removed %d files on %d from CSheetId because these files doesn't exists", removednbfiles, nbfiles);
		}

		// Convert the map to one big string and 1 static map (id to name)
		{
			// Get the number and size of all strings
			vector<CChar> tempVec; // Used to initialise the first map
			uint32 nNb = 0;
			uint32 nSize = 0;
			map<uint32,string>::const_iterator it = tempMap.begin();
			while (it != tempMap.end())
			{
				nSize += it->second.size()+1;
				nNb++;
				it++;
			}

			// Make the big string (composed of all strings) and a vector referencing each string
			tempVec.resize(nNb);
			_AllStrings.Ptr = new char[nSize];
			it = tempMap.begin();
			nSize = 0;
			nNb = 0;
			while (it != tempMap.end())
			{
				tempVec[nNb].Ptr = _AllStrings.Ptr+nSize;
				strcpy(_AllStrings.Ptr+nSize, it->second.c_str());
				toLower(_AllStrings.Ptr+nSize);
				nSize += it->second.size()+1;
				nNb++;
				it++;
			}

			// Finally build the static map (id to name)
			_SheetIdToName.reserve(tempVec.size());
			it = tempMap.begin();
			nNb = 0;
			while (it != tempMap.end())
			{
				_SheetIdToName.add(pair<uint32, CChar>::pair(it->first, CChar(tempVec[nNb])));

				nNb++;
				it++;
			}

			// The vector of all small string is not needed anymore we have all the info in
			// the static map and with the pointer AllStrings referencing the beginning.
		}

		// Build the invert map (Name to Id) & file extension vector
		{
			uint32 nSize = _SheetIdToName.size();
			_SheetNameToId.reserve(nSize);
			CStaticMap<uint32,CChar>::iterator itStr;
			for( itStr = _SheetIdToName.begin(); itStr != _SheetIdToName.end(); ++itStr )
			{
				// add entry to the inverse map
				_SheetNameToId.add( make_pair((*itStr).second, (*itStr).first) );

				// work out the type value for this entry in the map
				TSheetId sheetId;
				sheetId.Id=(*itStr).first;
				uint32 type = sheetId.IdInfos.Type;

				// check whether we need to add an entry to the file extensions vector
				if (_FileExtensions[type].empty())
				{
					// find the file extension part of the given file name
					_FileExtensions[type] = toLower(CFile::getExtension((*itStr).second.Ptr));
				}
				nSize--;
			}
			_SheetNameToId.endAdd();
		}
	}
	else
	{
		nlerror("<CSheetId::init> Can't open the file sheet_id.bin");
	}
	nldebug("Finished loading sheet_id.bin: %u entries read",_SheetIdToName.size());
}


//-----------------------------------------------
//	init
//
//-----------------------------------------------
void CSheetId::init(bool removeUnknownSheet)
{
	// allow multiple calls to init in case libraries depending on sheetid call this init from their own
	if (_Initialised)
		return;

//	CFile::addFileChangeCallback ("sheet_id.bin", cbFileChange);

	_RemoveUnknownSheet = removeUnknownSheet;

	loadSheetId ();
	_Initialised=true;


} // init //

void CSheetId::initWithoutSheet()
{
	_Initialised = true;
	_DontHaveSheetKnowledge = true;
}



//-----------------------------------------------
//	uninit
//
//-----------------------------------------------
void CSheetId::uninit()
{
	delete [] _AllStrings.Ptr;
} // uninit //

//-----------------------------------------------
//	operator=
//
//-----------------------------------------------
CSheetId& CSheetId::operator=( const CSheetId& sheetId )
{
	if (!_Initialised) init(false);

	if(this == &sheetId)
	{
		return *this;
	}

	_Id.Id = sheetId.asInt();

#ifdef NL_DEBUG_SHEET_ID
	_DebugSheetName = sheetId._DebugSheetName;
#endif

    return *this;


} // operator= //


//-----------------------------------------------
//	operator=
//
//-----------------------------------------------
CSheetId& CSheetId::operator=( const string& sheetName )
{
	nlassert(_Initialised);
	nlassert(!_DontHaveSheetKnowledge);

	CStaticMap<CChar,uint32,CCharComp>::const_iterator itId;
	CChar c;
	c.Ptr = new char [sheetName.size()+1];
	strcpy(c.Ptr, sheetName.c_str());
	toLower(c.Ptr);

	itId = _SheetNameToId.find (c);
	delete [] c.Ptr;
	if( itId != _SheetNameToId.end() )
	{
		_Id.Id = (*itId).second;
		return *this;
	}
	*this = Unknown;
	return *this;

} // operator= //


//-----------------------------------------------
//	operator=
//
//-----------------------------------------------
CSheetId& CSheetId::operator=( uint32 sheetRef )
{
	if (!_Initialised) init(false);

	_Id.Id = sheetRef;

	return *this;

} // operator= //



//-----------------------------------------------
//	operator<
//
//-----------------------------------------------
bool CSheetId::operator < (const CSheetId& sheetRef ) const
{
	if (!_Initialised) init(false);

	if (_Id.Id < sheetRef.asInt())
	{
		return true;
	}

	return false;

} // operator< //



//-----------------------------------------------
//	toString
//
//-----------------------------------------------
string CSheetId::toString(bool ifNotFoundUseNumericId) const
{
	if (!_Initialised) init(false);

	CStaticMap<uint32,CChar>::const_iterator itStr = _SheetIdToName.find (_Id.Id);
	if( itStr != _SheetIdToName.end() )
	{
		return string((*itStr).second.Ptr);
	}
	else
	{
		// This nlwarning is commented out because the loggers are mutexed, therefore
		// you couldn't use toString() within a nlwarning().
		//nlwarning("<CSheetId::toString> The sheet %08x is not in sheet_id.bin",_Id.Id);
		if (ifNotFoundUseNumericId)
		{
			return NLMISC::toString( "#%u", _Id.Id );
		}
		else
		{
			return NLMISC::toString( "<Sheet %d not found in sheet_id.bin>", _Id.Id );
		}
	}

} // toString //

void CSheetId::serial(NLMISC::IStream	&f) throw(NLMISC::EStream)
{
	f.serial( _Id.Id );

#ifdef NL_DEBUG_SHEET_ID
	CStaticMap<uint32, CChar>::iterator it(_SheetIdToName.find(_Id.Id));
	if (it != _SheetIdToName.end())
		_DebugSheetName = it->second.Ptr;
	else
		_DebugSheetName = NULL;
#endif
}



//-----------------------------------------------
//	display
//
//-----------------------------------------------
void CSheetId::display()
{
	if (!_Initialised) init(false);

	CStaticMap<uint32,CChar>::const_iterator itStr;
	for( itStr = _SheetIdToName.begin(); itStr != _SheetIdToName.end(); ++itStr )
	{
		//nlinfo("%d %s",(*itStr).first,(*itStr).second.c_str());
		nlinfo("SHEETID: (%08x %d) %s",(*itStr).first,(*itStr).first,(*itStr).second.Ptr);
	}

} // display //



//-----------------------------------------------
//	display
//
//-----------------------------------------------
void CSheetId::display(uint32 type)
{
	if (!_Initialised) init(false);

	CStaticMap<uint32,CChar>::const_iterator itStr;
	for( itStr = _SheetIdToName.begin(); itStr != _SheetIdToName.end(); ++itStr )
	{
		// work out the type value for this entry in the map
		TSheetId sheetId;
		sheetId.Id=(*itStr).first;

		// decide whether or not to display the entry
		if (type==sheetId.IdInfos.Type)
		{
			//nlinfo("%d %s",(*itStr).first,(*itStr).second.c_str());
			nlinfo("SHEETID: (%08x %d) %s",(*itStr).first,(*itStr).first,(*itStr).second.Ptr);
		}
	}

} // display //



//-----------------------------------------------
//	buildIdVector
//
//-----------------------------------------------
void CSheetId::buildIdVector(std::vector <CSheetId> &result)
{
	if (!_Initialised) init(false);

	CStaticMap<uint32,CChar>::const_iterator itStr;
	for( itStr = _SheetIdToName.begin(); itStr != _SheetIdToName.end(); ++itStr )
	{
		result.push_back( (CSheetId)(*itStr).first );
	}

} // buildIdVector //


//-----------------------------------------------
//	buildIdVector
//
//-----------------------------------------------
void CSheetId::buildIdVector(std::vector <CSheetId> &result, uint32 type)
{
	if (!_Initialised) init(false);
	nlassert(type < (1 << (NL_SHEET_ID_TYPE_BITS)));

	CStaticMap<uint32,CChar>::const_iterator itStr;
	for( itStr = _SheetIdToName.begin(); itStr != _SheetIdToName.end(); ++itStr )
	{
		// work out the type value for this entry in the map
		TSheetId sheetId;
		sheetId.Id=(*itStr).first;

		// decide whether or not to use the entry
		if (type==sheetId.IdInfos.Type)
		{
			result.push_back( (CSheetId)sheetId.Id );
		}
	}

} // buildIdVector //

//-----------------------------------------------
//	buildIdVector
//
//-----------------------------------------------
void CSheetId::buildIdVector(std::vector <CSheetId> &result, std::vector <std::string> &resultFilenames,uint32 type)
{
	if (!_Initialised) init(false);
	nlassert(type < (1 << (NL_SHEET_ID_TYPE_BITS)));

	CStaticMap<uint32,CChar>::const_iterator itStr;
	for( itStr = _SheetIdToName.begin(); itStr != _SheetIdToName.end(); ++itStr )
	{
		// work out the type value for this entry in the map
		TSheetId sheetId;
		sheetId.Id=(*itStr).first;

		// decide whether or not to use the entry
		if (type==sheetId.IdInfos.Type)
		{
			result.push_back( (CSheetId)sheetId.Id );
			resultFilenames.push_back( (*itStr).second.Ptr );
		}
	}

} // buildIdVector //

//-----------------------------------------------
//	buildIdVector
//
//-----------------------------------------------
void CSheetId::buildIdVector(std::vector <CSheetId> &result,const std::string &fileExtension)
{
	uint32 type=typeFromFileExtension(fileExtension);
	if (type!=(uint32)~0)
		buildIdVector(result, type);

} // buildIdVector //

//-----------------------------------------------
//	buildIdVector
//
//-----------------------------------------------
void CSheetId::buildIdVector(std::vector <CSheetId> &result, std::vector <std::string> &resultFilenames,const std::string &fileExtension)
{
	uint32 type=typeFromFileExtension(fileExtension);
	if (type != (uint32)~0)
		buildIdVector(result,resultFilenames, type);

} // buildIdVector //


//-----------------------------------------------
//	typeFromFileExtension
//
//-----------------------------------------------
uint32 CSheetId::typeFromFileExtension(const std::string &fileExtension)
{
	if (!_Initialised) init(false);

	unsigned i;
	for (i=0;i<_FileExtensions.size();i++)
		if (toLower(fileExtension)==_FileExtensions[i])
			return i;

	return ~0;
} // typeFromFileExtension //


//-----------------------------------------------
//	fileExtensionFromType
//
//-----------------------------------------------
const std::string &CSheetId::fileExtensionFromType(uint32 type)
{
	if (!_Initialised) init(false);
	nlassert(type < (1<<(NL_SHEET_ID_TYPE_BITS)));

	return _FileExtensions[type];

} // fileExtensionFromType //

//-----------------------------------------------
//	build
//
//-----------------------------------------------
void	CSheetId::buildSheetId(uint32 shortId, uint32 type)
{
	nlassert(shortId < (1<<NL_SHEET_ID_ID_BITS));
	nlassert(type < (1<<(NL_SHEET_ID_TYPE_BITS)));

	_Id.IdInfos.Id= shortId;
	_Id.IdInfos.Type= type;

#ifdef NL_DEBUG_SHEET_ID
	CStaticMap<uint32, CChar>::iterator it(_SheetIdToName.find(_Id.Id));
	if (it != _SheetIdToName.end())
	{
		_DebugSheetName = it->second.Ptr;
	}
	else
		_DebugSheetName = NULL;
#endif

}

} // NLMISC
