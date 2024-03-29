/** \file export_appdata.h
 * Define NeL appdata for 3dsmax
 *
 * $Id: export_appdata.h,v 1.9 2004/07/20 16:25:22 berenguier Exp $
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

#ifndef NL_EXPORT_APPDATA_H
#define NL_EXPORT_APPDATA_H

// ***************************************************************************

#define NEL3D_APPDATA_LOD_NAME_COUNT_MAX			10

// ***************************************************************************

#define NEL3D_APPDATA_LOD							1423062537
#define NEL3D_APPDATA_LOD_NAME_COUNT				(NEL3D_APPDATA_LOD)
#define NEL3D_APPDATA_LOD_NAME						(NEL3D_APPDATA_LOD_NAME_COUNT+1)
#define NEL3D_APPDATA_LOD_BLEND_IN					(NEL3D_APPDATA_LOD_NAME+NEL3D_APPDATA_LOD_NAME_COUNT_MAX)
#define NEL3D_APPDATA_LOD_BLEND_OUT					(NEL3D_APPDATA_LOD_BLEND_IN+1)
#define NEL3D_APPDATA_LOD_COARSE_MESH				(NEL3D_APPDATA_LOD_BLEND_OUT+1)
#define NEL3D_APPDATA_LOD_DYNAMIC_MESH				(NEL3D_APPDATA_LOD_COARSE_MESH+1)
#define NEL3D_APPDATA_LOD_DIST_MAX					(NEL3D_APPDATA_LOD_DYNAMIC_MESH+1)
#define NEL3D_APPDATA_LOD_BLEND_LENGTH				(NEL3D_APPDATA_LOD_DIST_MAX+1)
#define NEL3D_APPDATA_LOD_MRM						(NEL3D_APPDATA_LOD_BLEND_LENGTH+1)
#define NEL3D_APPDATA_LOD_SKIN_REDUCTION			(NEL3D_APPDATA_LOD_MRM+1)
#define NEL3D_APPDATA_LOD_NB_LOD					(NEL3D_APPDATA_LOD_SKIN_REDUCTION+1)
#define NEL3D_APPDATA_LOD_DIVISOR					(NEL3D_APPDATA_LOD_NB_LOD+1)
#define NEL3D_APPDATA_LOD_DISTANCE_FINEST			(NEL3D_APPDATA_LOD_DIVISOR+1)
#define NEL3D_APPDATA_LOD_DISTANCE_MIDDLE			(NEL3D_APPDATA_LOD_DISTANCE_FINEST+1)
#define NEL3D_APPDATA_LOD_DISTANCE_COARSEST			(NEL3D_APPDATA_LOD_DISTANCE_MIDDLE+1)

// Last APPDATA_LOD value is						1423062560

#define NEL3D_APPDATA_ACCEL							(1423062561)
#define NEL3D_APPDATA_ACCEL_TYPE					3
#define  NEL3D_APPDATA_ACCEL_NOT_ACCELERATOR		0
#define  NEL3D_APPDATA_ACCEL_PORTAL					1
#define  NEL3D_APPDATA_ACCEL_CLUSTER				2
#define NEL3D_APPDATA_ACCEL_FATHER_VISIBLE			4
#define NEL3D_APPDATA_ACCEL_VISIBLE_FROM_FATHER		8
#define NEL3D_APPDATA_ACCEL_DYNAMIC_PORTAL			16
#define NEL3D_APPDATA_ACCEL_CLUSTERIZED				32
#define NEL3D_APPDATA_ACCEL_AUDIBLE_NOT_LIKE_VISIBLE	64
#define	NEL3D_APPDATA_ACCEL_FATHER_AUDIBLE			128
#define NEL3D_APPDATA_ACCEL_AUDIBLE_FROM_FATHER		256
#define NEL3D_APPDATA_ACCEL_DEFAULT					NEL3D_APPDATA_ACCEL_CLUSTERIZED
#define NEL3D_APPDATA_OCC_MODEL						(84682540)
#define NEL3D_APPDATA_OPEN_OCC_MODEL				(84682541)
#define NEL3D_APPDATA_SOUND_GROUP					(84682542)
#define NEL3D_APPDATA_ENV_FX						(84682543)

#define NEL3D_APPDATA_INSTANCE_SHAPE				(1970)
#define NEL3D_APPDATA_INSTANCE_NAME					(1423062562)
#define NEL3D_APPDATA_DONT_ADD_TO_SCENE				(1423062563)
#define NEL3D_APPDATA_IGNAME						(1423062564)
#define NEL3D_APPDATA_INTERFACE_FILE				(1423062700)
#define NEL3D_APPDATA_INTERFACE_THRESHOLD			(1423062701)
#define NEL3D_APPDATA_GET_INTERFACE_NORMAL_FROM_SCENE_OBJECTS (1423062702)



#define NEL3D_APPDATA_DONTEXPORT					(1423062565)

#define NEL3D_APPDATA_LUMELSIZEMUL					(1423062567)
#define NEL3D_APPDATA_SOFTSHADOW_RADIUS				(1423062568)
#define NEL3D_APPDATA_SOFTSHADOW_CONELENGTH			(1423062569)

#define NEL3D_APPDATA_FLOATING_OBJECT			    (1423062570)

// Vegetable
#define NEL3D_APPDATA_VEGETABLE						(1423062580)
#define NEL3D_APPDATA_VEGETABLE_ALPHA_BLEND			(1423062581)
#define NEL3D_APPDATA_VEGETABLE_ALPHA_BLEND_ON_LIGHTED	(1423062582)
#define NEL3D_APPDATA_VEGETABLE_ALPHA_BLEND_OFF_LIGHTED	(1423062583)
#define NEL3D_APPDATA_VEGETABLE_ALPHA_BLEND_OFF_DOUBLE_SIDED	(1423062584)
#define NEL3D_APPDATA_BEND_CENTER					(1423062585)
#define NEL3D_APPDATA_BEND_FACTOR					(1423062586)

/// Animations
#define NEL3D_APPDATA_AUTOMATIC_ANIMATION			(1423062617)
#define NEL3D_APPDATA_EXPORT_ANIMATED_MATERIALS		(1423062587)
#define NEL3D_APPDATA_EXPORT_NOTE_TRACK				(1423062566)
#define NEL3D_APPDATA_EXPORT_NODE_ANIMATION			(1423062800)
#define NEL3D_APPDATA_EXPORT_ANIMATION_PREFIXE_NAME (1423062801)
#define NEL3D_APPDATA_EXPORT_SSS_TRACK				(1423062802)

/// Export RealTime light
#define NEL3D_APPDATA_EXPORT_REALTIME_LIGHT			(1423062588)
// UseLightingLocalAttenuation
#define NEL3D_APPDATA_USE_LIGHT_LOCAL_ATTENUATION	(1423062589)
/// Export LightMap light
#define NEL3D_APPDATA_EXPORT_LIGHTMAP_LIGHT			(1423062590)
/// Export As Sun Light
#define NEL3D_APPDATA_EXPORT_AS_SUN_LIGHT			(1423062591)

/// Choose a vertexProgram
#define NEL3D_APPDATA_VERTEXPROGRAM_ID				(1423062592)

/// WindTree VertexProgram.
#define NEL3D_APPDATA_VPWT_LEVELMAX					3
const uint32	NEL3D_AppDataVPWTFreq[NEL3D_APPDATA_VPWT_LEVELMAX]= 
	{1423062593, 1423062594, 1423062595};
const uint32	NEL3D_AppDataVPWTFreqWD[NEL3D_APPDATA_VPWT_LEVELMAX]= 
	{1423062596, 1423062597, 1423062598};
const uint32	NEL3D_AppDataVPWTDistXY[NEL3D_APPDATA_VPWT_LEVELMAX]= 
	{1423062599, 1423062600, 1423062601};
const uint32	NEL3D_AppDataVPWTDistZ[NEL3D_APPDATA_VPWT_LEVELMAX]= 
	{1423062602, 1423062603, 1423062604};
const uint32	NEL3D_AppDataVPWTBias[NEL3D_APPDATA_VPWT_LEVELMAX]= 
	{1423062605, 1423062606, 1423062607};
#define NEL3D_APPDATA_VPWT_USE_SPEC					(1423062608)
#define NEL3D_APPDATA_VPWT_FREQ_SCALE				(1423062609)
#define NEL3D_APPDATA_VPWT_DIST_SCALE				(1423062610)

/// SWT weight for bones
#define NEL3D_APPDATA_EXPORT_SWT					(1423062611)
#define NEL3D_APPDATA_EXPORT_SWT_WEIGHT				(1423062612)

/// Collision Flags
#define NEL3D_APPDATA_COLLISION						(1423062613)
#define NEL3D_APPDATA_COLLISION_EXTERIOR			(1423062614)

/// Lod for bone.
#define NEL3D_APPDATA_BONE_LOD_DISTANCE				(1423062615)


#define NEL3D_APPDATA_VEGETABLE_FORCE_BEST_SIDED_LIGHTING	(1423062616)



/// Lod for skeleton / Character shape
#define NEL3D_APPDATA_CHARACTER_LOD					(1423062618)

/// skeleton scale
#define NEL3D_APPDATA_EXPORT_BONE_SCALE				(1423062619)
#define NEL3D_APPDATA_EXPORT_BONE_SCALE_NAME_EXT	(1423062620)

// remanence
#define NEL3D_APPDATA_USE_REMANENCE					(1423062631)
#define NEL3D_APPDATA_REMANENCE_SLICE_NUMBER		(1423062632)
#define NEL3D_APPDATA_REMANENCE_SAMPLING_PERIOD     (1423062633)
#define NEL3D_APPDATA_REMANENCE_ROLLUP_RATIO        (1423062635)
#define NEL3D_APPDATA_REMANENCE_SHIFTING_TEXTURE    (1423062634)

// UseLightingLocalAttenuation
#define NEL3D_APPDATA_LIGHT_DONT_CAST_SHADOW_INTERIOR		(1423062636)
#define NEL3D_APPDATA_LIGHT_DONT_CAST_SHADOW_EXTERIOR		(1423062637)

// Lightmap compression
#define NEL3D_APPDATA_EXPORT_LMC_ENABLED			(1423062638)
#define NEL3D_APPDATA_EXPORT_LMC_MAX_LIGHT_GROUP	16
#define NEL3D_APPDATA_EXPORT_LMC_AMBIENT_START		(1423062639)
#define NEL3D_APPDATA_EXPORT_LMC_DIFFUSE_START		NEL3D_APPDATA_EXPORT_LMC_AMBIENT_START + NEL3D_APPDATA_EXPORT_LMC_MAX_LIGHT_GROUP
// End at 1423062671.

// CollisionMeshGeneration
#define NEL3D_APPDATA_CAMERA_COLLISION_MESH_GENERATION	(1423062671)

// RealTimeAmbientLightAddSun
#define NEL3D_APPDATA_REALTIME_AMBIENT_ADD_SUN			(1423062672)


// Next free appData is 1423062673
// WARNING!!!!!!!!!  : 700+ and 800+ are already used....


// ***************************************************************************

#define NEL3D_APPDATA_LOD_BLEND_IN_DEFAULT			1
#define NEL3D_APPDATA_LOD_BLEND_OUT_DEFAULT			1
#define NEL3D_APPDATA_LOD_COARSE_MESH_DEFAULT		0
#define NEL3D_APPDATA_LOD_DYNAMIC_MESH_DEFAULT		0
#define NEL3D_APPDATA_LOD_DIST_MAX_DEFAULT			1000.f
#define NEL3D_APPDATA_LOD_BLEND_LENGTH_DEFAULT		5.f
#define NEL3D_APPDATA_LOD_SKIN_REDUCTION_DEFAULT	1
#define NEL3D_APPDATA_LOD_NB_LOD_DEFAULT			11
#define NEL3D_APPDATA_LOD_DIVISOR_DEFAULT			20
#define NEL3D_APPDATA_LOD_DISTANCE_FINEST_DEFAULT	5.f
#define NEL3D_APPDATA_LOD_DISTANCE_MIDDLE_DEFAULT	30.f
#define NEL3D_APPDATA_LOD_DISTANCE_COARSEST_DEFAULT	200.f

#define NEL3D_APPDATA_SOFTSHADOW_RADIUS_DEFAULT		1.4f
#define NEL3D_APPDATA_SOFTSHADOW_CONELENGTH_DEFAULT	15.0f

#define NEL3D_APPDATA_BEND_FACTOR_DEFAULT			1.0f

// ***************************************************************************

#define NEL3D_RADIAL_NORMAL_COUNT					4
#define NEL3D_RADIAL_FIRST_SM						28
#define NEL3D_APPDATA_RADIAL_NORMAL_SM				2089254647
#define NEL3D_APPDATA_RADIAL_NORMAL_SM_28			2089254647
#define NEL3D_APPDATA_RADIAL_NORMAL_SM_29			2089254648
#define NEL3D_APPDATA_RADIAL_NORMAL_SM_30			2089254649
#define NEL3D_APPDATA_RADIAL_NORMAL_SM_31			2089254650

// ***************************************************************************

#endif // NL_EXPORT_APPDATA_H

/* End of export_appdata.h */
