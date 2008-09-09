/**
 * \file adpcm_xaudio2.cpp
 * \brief CAdpcmXAudio2
 * \date 2008-09-07 03:53GMT
 * \author Jan Boon (Kaetemi)
 * CAdpcmXAudio2
 * 
 * $Id$
 */

/* 
 * Copyright (C) 2008  Jan Boon (Kaetemi)
 * 
 * This file is part of NLSOUND XAudio2 Driver.
 * NLSOUND XAudio2 Driver is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 * 
 * NLSOUND XAudio2 Driver is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with NLSOUND XAudio2 Driver; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 */

#include "stdxaudio2.h"
#include "adpcm_xaudio2.h"

// STL includes

// NeL includes
#include <nel/misc/debug.h>

// Project includes
#include "buffer_xaudio2.h"

using namespace std;
// using namespace NLMISC;

namespace NLSOUND {

CAdpcmXAudio2::CAdpcmXAudio2() 
: _SourceVoice(NULL), _SourceData(NULL), _SourceSize(0), _SampleRate(0), 
_Loop(false), _BufferNext(0), _SampleNb(0), _AdpcmSize(0),
_AdpcmIndex(0)
{
	_State.PreviousSample = 0;
	_State.StepIndex = 0;
}

CAdpcmXAudio2::~CAdpcmXAudio2()
{
	
}

/// Submit the next ADPCM buffer, only 1 buffer can be submitted at a time!
void CAdpcmXAudio2::submitSourceBuffer(CBufferXAudio2 *buffer)
{
	_Mutex.enter();
	if (_Buffer) nlerror("Only 1 ADPCM buffer can be submitted at a time! Call flushSourceBuffers first in CSourceXAudio2 stop().");
	_SourceSize = buffer->getSize();
	_SourceData = buffer->getData();
	_AdpcmSize = buffer->getFreq() / 40;
	_SampleNb = _AdpcmSize * 2;
	nlassert(_SampleNb < _BufferMax);
	processBuffers();
	_Mutex.leave();
}

/// Reset the decoder, clear the queued buffer
void CAdpcmXAudio2::flushSourceBuffers()
{
	_Mutex.enter();
	_SourceData = NULL;
	//_SourceSize = 0;
	//_SampleRate = 0;
	_State.PreviousSample = 0;
	_State.StepIndex = 0;
	_AdpcmIndex = 0;
	//_PcmSize = 0;
	//_AdpcmSize = 0;
	_Mutex.leave();
}

void CAdpcmXAudio2::processBuffers()
{
	if (_SourceData)
	{
		XAUDIO2_VOICE_STATE voice_state;
		_SourceVoice->GetState(&voice_state);
		while (voice_state.BuffersQueued < _BufferNb)
		{
			// ADPCM = 4bit, PCM = 16bit // 1 adpcm byte = 2 samples
			uint maxinbytes = _SourceSize - _AdpcmIndex;
			uint inbytes = min(maxinbytes, _AdpcmIndex);

			if (inbytes > 0)
			{
				IBuffer::decodeADPCM(_SourceData + (_AdpcmIndex / 2), _Buffer[_BufferNext], inbytes * 2, _State);
				
				XAUDIO2_BUFFER xbuffer;
				xbuffer.AudioBytes = inbytes * 4;
				xbuffer.Flags = 0;
				xbuffer.LoopBegin = 0;
				xbuffer.LoopCount = 0;
				xbuffer.LoopLength = 0;
				xbuffer.pAudioData = (BYTE *)_Buffer[_BufferNext];
				xbuffer.pContext = NULL;
				xbuffer.PlayBegin = 0;
				xbuffer.PlayLength = 0;
				_SourceVoice->SubmitSourceBuffer(&xbuffer);

				_AdpcmIndex += inbytes;
				_BufferNext = (_BufferNext + 1) % _BufferNb;
				++voice_state.BuffersQueued;
			}
		}
	}
}

void CAdpcmXAudio2::OnVoiceProcessingPassStart(UINT32 BytesRequired)
{    
	
}

void CAdpcmXAudio2::OnVoiceProcessingPassEnd()
{ 
	
}

void CAdpcmXAudio2::OnStreamEnd()
{ 
	
}

void CAdpcmXAudio2::OnBufferStart(void *pBufferContext)
{    
	
}

void CAdpcmXAudio2::OnBufferEnd(void *pBufferContext)
{ 
	// The "correct" way would be to decode the buffer on a seperate thread,
	// but since ADPCM decoding should be really fast, there is no problem
	// with doing it here directly.
	_Mutex.enter();
	processBuffers();
	_Mutex.leave();
}

void CAdpcmXAudio2::OnLoopEnd(void *pBufferContext)
{    
	
}

void CAdpcmXAudio2::OnVoiceError(void *pBufferContext, HRESULT Error)
{ 
	
}

} /* namespace NLSOUND */

/* end of file */
