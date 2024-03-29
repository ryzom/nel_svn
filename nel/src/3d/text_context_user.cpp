/** \file text_context_user.cpp
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

#include "nel/3d/text_context_user.h"
#include "nel/misc/hierarchical_timer.h"


namespace NL3D
{

H_AUTO_DECL( NL3D_Render_TextContext_3D )
#define	NL3D_HAUTO_RENDER_3D_TEXTCONTEXT				H_AUTO_USE( NL3D_Render_TextContext_3D )

// ***************************************************************************
NLMISC::CVector UTextContext::CStringInfo::getHotSpotVector(UTextContext::THotSpot hotspot)
{
	H_AUTO2;

	NLMISC::CVector hotspotVector(0,0,0);

	if(hotspot==UTextContext::MiddleLeft)
		hotspotVector = CVector(0,0,-StringHeight/2);

	if(hotspot==UTextContext::TopLeft)
		hotspotVector = CVector(0,0,StringHeight);

	if(hotspot==UTextContext::MiddleBottom)
		hotspotVector = CVector(-StringWidth/2,0,0);

	if(hotspot==UTextContext::MiddleMiddle)
		hotspotVector = CVector(-StringWidth/2,0,-StringHeight/2);

	if(hotspot==UTextContext::MiddleTop)
		hotspotVector = CVector(-StringWidth/2,0,-StringHeight);

	if(hotspot==UTextContext::BottomRight)
		hotspotVector = CVector(-StringWidth,0,0);

	if(hotspot==UTextContext::MiddleRight)
		hotspotVector = CVector(-StringWidth,0,-StringHeight/2);

	if(hotspot==UTextContext::TopRight)
		hotspotVector = CVector(-StringWidth,0,-StringHeight);

	return hotspotVector;
}


// ***************************************************************************
void		UTextContext::CStringInfo::convertTo01Size(UDriver *drv)
{
	uint32	w,h;
	drv->getWindowSize(w,h);
	StringWidth/= w;
	StringHeight/= h;
	StringLine/= h;
}
void		UTextContext::CStringInfo::convertToPixelSize(UDriver *drv)
{
	uint32	w,h;
	drv->getWindowSize(w,h);
	StringWidth*= w;
	StringHeight*= h;
	StringLine*= h;
}



// ***************************************************************************
void CTextContextUser::setColor(NLMISC::CRGBA color)
{
	H_AUTO2;

	_TextContext.setColor(color);
}
void CTextContextUser::setFontSize(uint32 fontSize)
{
	H_AUTO2;

	_TextContext.setFontSize(fontSize);
}
uint32 CTextContextUser::getFontSize() const
{
	H_AUTO2;

	return _TextContext.getFontSize();
}
void CTextContextUser::setHotSpot(THotSpot hotSpot)
{
	H_AUTO2;

	_TextContext.setHotSpot((CComputedString::THotSpot)(uint32)hotSpot) ;
}
UTextContext::THotSpot CTextContextUser::getHotSpot() const
{
	H_AUTO2;

	return (THotSpot)(uint32)_TextContext.getHotSpot();
}
void CTextContextUser::setScaleX(float scaleX)
{
	H_AUTO2;

	_TextContext.setScaleX(scaleX);
}
void CTextContextUser::setScaleY(float scaleY)
{
	H_AUTO2;

	_TextContext.setScaleZ(scaleY);
}
float CTextContextUser::getScaleX() const
{
	H_AUTO2;

	return _TextContext.getScaleX();
}
float CTextContextUser::getScaleY() const
{
	H_AUTO2;

	return _TextContext.getScaleZ();
}
void CTextContextUser::setShaded(bool b)
{
	H_AUTO2;

	_TextContext.setShaded(b);
}
bool CTextContextUser::getShaded() const
{
	H_AUTO2;

	return _TextContext.getShaded();
}
void CTextContextUser::setShadeExtent(float shext)
{
	H_AUTO2;

	_TextContext.setShadeExtent(shext);
}
void CTextContextUser::setShadeColor (NLMISC::CRGBA sc)
{
	H_AUTO2;

	_TextContext.setShadeColor (sc);
}
NLMISC::CRGBA CTextContextUser::getShadeColor () const
{
	H_AUTO2;

	return _TextContext.getShadeColor();
}
void			CTextContextUser::setKeep800x600Ratio(bool keep)
{
	H_AUTO2;

	_TextContext.setKeep800x600Ratio(keep);
}
bool			CTextContextUser::getKeep800x600Ratio() const
{
	H_AUTO2;

	return _TextContext.getKeep800x600Ratio();
}

// ***************************************************************************
uint32 CTextContextUser::textPush(const char *format, ...)
{
	H_AUTO2;

	char *str;
	NLMISC_CONVERT_VARGS (str, format, NLMISC::MaxCStringSize);

	return _TextContext.textPush(ucstring(str)) ;
}
uint32 CTextContextUser::textPush(const ucstring &str)
{
	H_AUTO2;

	return _TextContext.textPush(str) ;
}
void CTextContextUser::setStringColor(uint32 i, CRGBA newCol)
{
	CComputedString	*str= _TextContext.getComputedString(i);
	if(str)
	{
		str->Color= newCol;
	}
}
void CTextContextUser::setStringSelection(uint32 i, uint32 selectStart, uint32 selectSize)
{
	CComputedString	*str= _TextContext.getComputedString(i);
	if(str)
	{
		str->SelectStart= selectStart;
		str->SelectSize= selectSize;
	}
}
void CTextContextUser::resetStringSelection(uint32 i)
{
	CComputedString	*str= _TextContext.getComputedString(i);
	if(str)
	{
		str->SelectStart= 0;
		str->SelectSize= ~0;
	}
}
void CTextContextUser::erase(uint32 i)
{
	H_AUTO2;

	_TextContext.erase(i);
}
UTextContext::CStringInfo		CTextContextUser::getStringInfo(uint32 i)
{
	H_AUTO2;

	CComputedString		*cstr= _TextContext.getComputedString(i);
	if(!cstr)
		return CStringInfo(0, 0, 0);
	else
		return	CStringInfo(cstr->StringWidth, cstr->StringHeight, cstr->StringLine);
}
UTextContext::CStringInfo		CTextContextUser::getStringInfo(const ucstring &str)
{
	H_AUTO2;

	_TextContext.computeStringInfo(str, _CacheString);
	return CStringInfo (_CacheString.StringWidth, _CacheString.StringHeight, _CacheString.StringLine);
}
void CTextContextUser::clear()
{
	H_AUTO2;

	_TextContext.clear();
}
void CTextContextUser::printAt(float x, float y, uint32 i)
{
	H_AUTO2;

	_TextContext.printAt(x, y, i);
	_DriverUser->restoreMatrixContext();
}
void CTextContextUser::printClipAt(URenderStringBuffer &renderBuffer, float x, float y, uint32 i, float xmin, float ymin, float xmax, float ymax)
{
	H_AUTO2;

	_TextContext.printClipAt(static_cast<CRenderStringBuffer&>(renderBuffer), x, y, i, xmin, ymin, xmax, ymax);
	// Don't need to restore Matrix context here since no driver change
}
void CTextContextUser::printClipAtUnProjected(URenderStringBuffer &renderBuffer, class NL3D::CFrustum &frustum, const NLMISC::CMatrix &scaleMatrix, float x, float y, float depth, uint32 i, float xmin, float ymin, float xmax, float ymax)
{
	H_AUTO2;

	_TextContext.printClipAtUnProjected(static_cast<CRenderStringBuffer&>(renderBuffer), frustum, scaleMatrix, x, y, depth, i, xmin, ymin, xmax, ymax);
	// Don't need to restore Matrix context here since no driver change
}
void CTextContextUser::printClipAtOld (float x, float y, uint32 i, float xmin, float ymin, float xmax, float ymax)
{
	static	CRenderStringBuffer	rdrBuffer;
	printClipAt(rdrBuffer, x, y ,i, xmin, ymin, xmax, ymax);
	flushRenderBuffer(&rdrBuffer);
}
void CTextContextUser::printAt(float x, float y, const ucstring &ucstr)
{
	H_AUTO2;

	_TextContext.printAt(x, y, ucstr);
	_DriverUser->restoreMatrixContext();
}
void CTextContextUser::printfAt(float x, float y, const char * format, ...)
{
	H_AUTO2;

	char *str;
	NLMISC_CONVERT_VARGS (str, format, NLMISC::MaxCStringSize);

	_TextContext.printAt(x, y, ucstring(str)) ;
	_DriverUser->restoreMatrixContext();
}

void CTextContextUser::render3D(const CMatrix &mat, const ucstring &ucstr)
{
	NL3D_HAUTO_RENDER_3D_TEXTCONTEXT;

	CComputedString computedStr;
	_TextContext.computeString(ucstr,computedStr);

	computedStr.render3D(*_Driver,mat);

	_DriverUser->restoreMatrixContext();
}
void CTextContextUser::render3D(const CMatrix &mat, const char *format, ...)
{
	NL3D_HAUTO_RENDER_3D_TEXTCONTEXT;

	char *str;
	NLMISC_CONVERT_VARGS (str, format, NLMISC::MaxCStringSize);

	render3D(mat, ucstring(str));

	_DriverUser->restoreMatrixContext();
}


float CTextContextUser::getLastXBound() const
{
	H_AUTO2;

	return 0.0f;
}

// ***************************************************************************
void			CTextContextUser::dumpCacheTexture (const char *filename)
{
	_TextContext.dumpCache (filename);
}


// ***************************************************************************
URenderStringBuffer		*CTextContextUser::createRenderBuffer()
{
	return new CRenderStringBuffer;
}

// ***************************************************************************
void					CTextContextUser::deleteRenderBuffer(URenderStringBuffer *buffer)
{
	delete buffer;
}

// ***************************************************************************
void					CTextContextUser::flushRenderBuffer(URenderStringBuffer *buffer)
{
		nlassert(buffer);
	CRenderStringBuffer	*rdrBuffer= static_cast<CRenderStringBuffer*>(buffer);
	if(rdrBuffer->NumQuads)
	{
		rdrBuffer->flush(*_Driver, _TextContext.getFontManager()->getFontMaterial());

		// must restore the Matrix context if some display done. Need just for Frustum/Matrixes
		_DriverUser->restoreMatrixContextMatrixOnly();
	}
}

// ***************************************************************************
void					CTextContextUser::flushRenderBufferUnProjected(URenderStringBuffer *buffer, bool zwrite)
{
		nlassert(buffer);
	CRenderStringBuffer	*rdrBuffer= static_cast<CRenderStringBuffer*>(buffer);
	if(rdrBuffer->NumQuads)
	{
		rdrBuffer->flushUnProjected(*_Driver, _TextContext.getFontManager()->getFontMaterial(), zwrite);
	}
}


// ***************************************************************************
void CTextContextUser::setLetterColors(ULetterColors * letterColors, uint index)
{
	H_AUTO2;

	_TextContext.setLetterColors(static_cast<CLetterColors*>(letterColors), index);
}

// ***************************************************************************
ULetterColors * CTextContextUser::createLetterColors()
{
	H_AUTO2;

	ULetterColors * uLetterColors = new CLetterColors();
	return uLetterColors;
}

bool CTextContextUser::isSameLetterColors(ULetterColors * letterColors, uint index)
{
	H_AUTO2;

	return _TextContext.isSameLetterColors(static_cast<CLetterColors*>(letterColors), index);
}

} // NL3D
