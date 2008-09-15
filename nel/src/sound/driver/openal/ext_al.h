/** \file ext_al.h
 * OpenAL extensions
 */

#ifndef NL_EXT_AL_H
#define NL_EXT_AL_H
#include "stdopenal.h"

#ifdef NL_OS_WINDOWS
#	define EFX_CREATIVE_AVAILABLE 1
#	define EAX_AVAILABLE 0
#else
#	define EFX_CREATIVE_AVAILABLE 0
#	define EAX_AVAILABLE 0
#endif

#if EAX_AVAILABLE
#	define OPENAL
#	ifdef NL_OS_WINDOWS
#		include <objbase.h>
#	endif
#	include <eax.h>
#endif
#if EFX_CREATIVE_AVAILABLE
#	include <AL/efx-creative.h>
#	include <AL/EFX-Util.h>
#endif

extern "C"
{

void alExtInit(ALCdevice *device);

#if EAX_AVAILABLE
// EAX
extern bool AlExtEax;
extern EAXSet eaxSet;
extern EAXGet eaxGet;
#endif

// EAX-RAM (see OpenAL Programmer's Guide.pdf and http://icculus.org/alextreg/)
extern bool AlExtXRam;
typedef ALboolean (AL_APIENTRY *EAXSetBufferMode)(ALsizei n, ALuint *buffers, ALint value);
extern EAXSetBufferMode eaxSetBufferMode;
typedef ALenum (AL_APIENTRY *EAXGetBufferMode)(ALuint buffer, ALint *value);
extern EAXGetBufferMode eaxGetBufferMode;

// ALC_EXT_EFX (see Effects Extension Guide.pdf and http://icculus.org/alextreg/)
extern bool AlExtEfx;
#define ALC_MAX_AUXILIARY_SENDS 0x20003
typedef void (AL_APIENTRY *LPALGENEFXOBJECTS)(ALsizei n, ALuint* objects);
typedef void (AL_APIENTRY *LPALDELETEEFXOBJECTS)(ALsizei n, ALuint* objects);
typedef ALboolean (AL_APIENTRY *LPALISEFXOBJECT)(ALuint id);
typedef void (AL_APIENTRY *LPALEFXOBJECTI)(ALuint id, ALenum param, ALint value);
typedef void (AL_APIENTRY *LPALEFXOBJECTIV)(ALuint id, ALenum param, ALint* values);
typedef void (AL_APIENTRY *LPALEFXOBJECTF)(ALuint id, ALenum param, ALfloat value);
typedef void (AL_APIENTRY *LPALEFXOBJECTFV)(ALuint id, ALenum param, ALfloat* values);
typedef void (AL_APIENTRY *LPALGETEFXOBJECTI)(ALuint id, ALenum pname, ALint* value);
typedef void (AL_APIENTRY *LPALGETEFXOBJECTIV)(ALuint id, ALenum pname, ALint* values);
typedef void (AL_APIENTRY *LPALGETEFXOBJECTF)(ALuint id, ALenum pname, ALfloat* value);
typedef void (AL_APIENTRY *LPALGETEFXOBJECTFV)(ALuint id, ALenum pname, ALfloat* values);
// source objects
#define AL_DIRECT_FILTER 0x20005
#define AL_AUXILIARY_SEND_FILTER 0x20006
#define AL_AIR_ABSORPTION_FACTOR 0x20007
#define AL_ROOM_ROLLOFF_FACTOR 0x20008
#define AL_CONE_OUTER_GAINHF 0x20009
#define AL_DIRECT_FILTER_GAINHF_AUTO 0x2000A
#define AL_AUXILIARY_SEND_FILTER_GAIN_AUTO 0x2000B
#define AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO 0x2000C
// effect objects
extern LPALGENEFXOBJECTS alGenEffects;
extern LPALDELETEEFXOBJECTS alDeleteEffects;
extern LPALISEFXOBJECT alIsEffect;
extern LPALEFXOBJECTI alEffecti;
extern LPALEFXOBJECTIV alEffectiv;
extern LPALEFXOBJECTF alEffectf;
extern LPALEFXOBJECTFV alEffectfv;
extern LPALGETEFXOBJECTI alGetEffecti;
extern LPALGETEFXOBJECTIV alGetEffectiv;
extern LPALGETEFXOBJECTF alGetEffectf;
extern LPALGETEFXOBJECTFV alGetEffectfv;
#define AL_EFFECT_FIRST_PARAMETER 0x0000
#define AL_EFFECT_LAST_PARAMETER 0x8000
#define AL_EFFECT_TYPE 0x8001
#define AL_EFFECT_NULL 0x0000
#define AL_EFFECT_REVERB 0x0001
// filter objects
extern LPALGENEFXOBJECTS alGenFilters;
extern LPALDELETEEFXOBJECTS alDeleteFilters;
extern LPALISEFXOBJECT alIsFilter;
extern LPALEFXOBJECTI alFilteri;
extern LPALEFXOBJECTIV alFilteriv;
extern LPALEFXOBJECTF alFilterf;
extern LPALEFXOBJECTFV alFilterfv;
extern LPALGETEFXOBJECTI alGetFilteri;
extern LPALGETEFXOBJECTIV alGetFilteriv;
extern LPALGETEFXOBJECTF alGetFilterf;
extern LPALGETEFXOBJECTFV alGetFilterfv;
#define AL_LOWPASS_GAIN 0x0001
#define AL_LOWPASS_GAINHF 0x0002
#define AL_HIGHPASS_GAIN 0x0001
#define AL_HIGHPASS_GAINLF 0x0002
#define AL_BANDPASS_GAIN 0x0001
#define AL_BANDPASS_GAINLF 0x0002
#define AL_BANDPASS_GAINHF 0x0003
#define AL_FILTER_FIRST_PARAMETER 0x0000
#define AL_FILTER_LAST_PARAMETER 0x8000
#define AL_FILTER_TYPE 0x8001
#define AL_FILTER_NULL 0x0000
#define	AL_FILTER_LOWPASS 0x0001
#define AL_FILTER_HIGHPASS 0x0002
#define AL_FILTER_BANDPASS 0x0003
// submix objects
extern LPALGENEFXOBJECTS alGenAuxiliaryEffectSlots;
extern LPALDELETEEFXOBJECTS alDeleteAuxiliaryEffectSlots;
extern LPALISEFXOBJECT alIsAuxiliaryEffectSlot;
extern LPALEFXOBJECTI alAuxiliaryEffectSloti;
extern LPALEFXOBJECTIV alAuxiliaryEffectSlotiv;
extern LPALEFXOBJECTF alAuxiliaryEffectSlotf;
extern LPALEFXOBJECTFV alAuxiliaryEffectSlotfv;
extern LPALGETEFXOBJECTI alGetAuxiliaryEffectSloti;
extern LPALGETEFXOBJECTIV alGetAuxiliaryEffectSlotiv;
extern LPALGETEFXOBJECTF alGetAuxiliaryEffectSlotf;
extern LPALGETEFXOBJECTFV alGetAuxiliaryEffectSlotfv;
#define AL_EFFECTSLOT_EFFECT 0x0001
#define AL_EFFECTSLOT_GAIN  0x0002
#define AL_EFFECTSLOT_AUXILIARY_SEND_AUTO 0x0003
#define AL_EFFECTSLOT_NULL 0x0000

}

#endif /* #ifndef NL_EXT_AL_H */

/* end of file */
