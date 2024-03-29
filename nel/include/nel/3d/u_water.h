/** \file u_water.h
 * User interface for water manipulation
 */

/* Copyright, 2000, 2001 Nevrax Ltd.
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

#ifndef NL_U_WATER_INSTANCE_H
#define NL_U_WATER_INSTANCE_H

#include "nel/misc/types_nl.h"
#include "u_instance.h"


namespace NLMISC
{
	class CVector2f;
	class CVector;
};


namespace NL3D {

class UDriver;

class UWaterHeightMap;


/**
 * Helps to get infos about a water model
 * You can get this interface by using a dynamic_cast on a UInstance that is a water instance
 * \author Nicolas Vizerie
 * \author Nevrax France
 * \date 2001
 */
class UWaterInstance : public UInstance
{
public:
	/** Get the ID of the water height map attached with this surface of water.
	  * Once you got it, you can get an interface on it from the water height map manager.
	  * NB : a water height map is usually shared between several wtare instances
	  */
	uint32	getWaterHeightMapID() const;

	/** When displaying the water height field, the value in the height field is actually multiplied
	  * by a factor to get the height in world space. This returns this factor
	  */
	float	getHeightFactor() const;

	/// Get the height of the water in world space at the given location.
	float   getHeight(const NLMISC::CVector2f &pos);

	/** Get the attenuated height of the water in world space at the given location.
	  * You must provide the viewer position
	  * This is useful if you want to compute the position of a floating object
	  */
	float   getAttenuatedHeight(const NLMISC::CVector2f &pos, const NLMISC::CVector &viewer);

	/// Proxy interface

	/// Constructors
	UWaterInstance() { _Object = NULL; }
	UWaterInstance(class CWaterModel *object) { _Object = (ITransformable*)object; };
	/// Attach an object to this proxy
	void			attach(class CWaterModel *object) { _Object = (ITransformable*)object; };
	/// Detach the object
	void			detach() { _Object = NULL; }
	/// Return true if the proxy is empty() (not attached)
	bool			empty() const {return _Object==NULL;}
	/// For advanced usage, get the internal object ptr
	class CWaterModel	*getObjectPtr() const {return (CWaterModel*)_Object;}
};


/// Interface to the water height map manager
class UWaterHeightMapManager
{
public:
	/// Retrieve a water heightmap from its ID. An assertion is raised if a wrong ID is provided
	static UWaterHeightMap &getWaterHeightMapFromID(uint32 ID);

	/** Blend factor for water surfaces that use CTextureBlend (day / night mgt for example)
	  * The blend factor if clamped to [0, 1]
	  * NB : when transition has finished, one should call releaseBlendTexture to eventually release textures from system memory
	  */
	static void		setBlendFactor(UDriver *driver, float value);
	/** release blend textures from memory
	  */
	static void		releaseBlendTextures();

};

/// Interface to water height maps
class UWaterHeightMap
{
public:
	virtual ~UWaterHeightMap() {}

	/// get the size in meter of a heightmap texel
	virtual float	getUnitSize() const =0;

	/** Apply a perturbation on this heightmap at the given location.
	  * \param pos, The x and y coords of the perturbation.
	  * \param strenght Strenght of the impulsion
	  * \param radius   Radius of the impulsion
	  */
	virtual void	perturbate(const NLMISC::CVector2f &pos, float strenght, float radius) =0;

	/** Apply a point perturbation on this heightmap at the given location.
	  * \param pos, The x and y coords of the perturbation.
	  * \param strenght Strenght of the impulsion
	  */
	virtual void	perturbatePoint(const NLMISC::CVector2f &pos, float strenght) =0;

	/** Get the value of the height map at the given location. To get the height in world coordinate,
	  * You should use getHeight
	  */
	virtual float	getHeight(const NLMISC::CVector2f &pos) =0;
};





} // NL3D


#endif // NL_U_WATER_INSTANCE_H

/* End of u_water_instance.h */
