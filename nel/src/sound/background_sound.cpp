/** \file background_sound.cpp
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

#include "stdsound.h"

#include "background_sound.h"
#include "audio_mixer_user.h"
#include "nel/misc/path.h"
#include <numeric>
/*#include "simple_sound.h"
#include "sound_bank.h"
#include "sample_bank.h"
#include "driver/sound_driver.h"
#include "driver/buffer.h"
*/
using namespace std;
using namespace NLMISC;


namespace NLSOUND {

CBackgroundSound::CBackgroundSound()
: _Duration(0), _DurationValid(false)
{
}

CBackgroundSound::~CBackgroundSound()
{
}

void CBackgroundSound::serial(NLMISC::IStream &s)
{
	CSound::serial(s);

	s.serialCont(_Sounds);

	if (s.isReading())
		_DurationValid = false;
}

void CBackgroundSound::importForm(const std::string& filename, NLGEORGES::UFormElm& formRoot)
{
	NLGEORGES::UFormElm *psoundType;
	std::string dfnName;

	// some basic checking.
	formRoot.getNodeByName(&psoundType, ".SoundType");
	nlassert(psoundType != NULL);
	psoundType->getDfnName(dfnName);
	nlassert(dfnName == "background_sound.dfn");

	// Call the base class
	CSound::importForm(filename, formRoot);

	// Read the array of sound with there respective filter.
	{
		_Sounds.clear();

		NLGEORGES::UFormElm *psoundList;

		formRoot.getNodeByName(&psoundList, ".SoundType.Sounds");

		if (psoundList != 0 && psoundList->isArray())
		{
			uint size;
			psoundList->getArraySize(size);

			for (uint i=0; i<size; ++i)
			{
				TSoundInfo	sound;
				NLGEORGES::UFormElm	*psoundItem;

				psoundList->getArrayNode(&psoundItem, i);

				if (psoundItem != NULL)
				{
					// Read the sound name.
					std::string soundName;
					psoundItem->getValueByName(soundName, "Sound");
					sound.SoundName = CStringMapper::map(CFile::getFilenameWithoutExtension(soundName));


					// Read the environnement flag.
					for (uint j=0; j<UAudioMixer::TBackgroundFlags::NB_BACKGROUND_FLAGS; j++)
					{
						char tmp[200];
						sprintf(tmp, "Filter%2.2u", j);
						psoundItem->getValueByName(sound.Filter.Flags[j], tmp);
					}
				}

				_Sounds.push_back(sound);
			}
		}
	}

	_DurationValid = false;
}

uint32 CBackgroundSound::getDuration()
{
	if (_DurationValid)
		return _Duration;

	vector<sint32>	durations;
	CAudioMixerUser *mixer = CAudioMixerUser::instance();
	std::vector<TSoundInfo>::const_iterator first(_Sounds.begin()), last(_Sounds.end());
	for (; first != last; ++first)
	{
		CSound *sound = mixer->getSoundId(first->SoundName);
		if (sound != NULL)
			durations.push_back(sound->getDuration());
	}
	if (durations.empty())
		return 0;
	_Duration = *(std::max_element(durations.begin(), durations.end()));
	_DurationValid = true;

	return _Duration;
}

void CBackgroundSound::getSubSoundList(std::vector<std::pair<std::string, CSound*> > &subsounds) const
{
	CAudioMixerUser *mixer = CAudioMixerUser::instance();
	std::vector<TSoundInfo>::const_iterator first(_Sounds.begin()), last(_Sounds.end());
	for (; first != last; ++first)
	{
		CSound *sound = mixer->getSoundId(first->SoundName);
		subsounds.push_back(make_pair(CStringMapper::unmap(first->SoundName), sound));
	}
}

float CBackgroundSound::getMaxDistance() const
{
	CAudioMixerUser *mixer = CAudioMixerUser::instance();
	float ret = 0.0f;
	std::vector<TSoundInfo>::const_iterator first(_Sounds.begin()), last(_Sounds.end());
	for (; first != last; ++first)
	{
		CSound *sound = mixer->getSoundId(first->SoundName);
		if (sound != 0)
		{
			ret = max(ret, sound->getMaxDistance());
		}
	}
	if (ret == 0)
		ret  = 1;

	return ret;
}



} // NLSOUND
