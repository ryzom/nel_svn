/** \file win_event_emitter.cpp
 * class CWinEnventEmitter
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

#include "stdmisc.h"

#include "nel/misc/events.h"
#include "nel/misc/event_emitter.h"
#include "nel/misc/win_event_emitter.h"
#include "nel/misc/event_server.h"

#ifdef NL_OS_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

/**
  * Needed for definition of WM_MOUSEWHEEL. It should be in winuser.h
  * but not under win98.. strange..
  */
#include <zmouse.h>


namespace NLMISC {

/*------------------------------------------------------------------*\
							submitEvents()
\*------------------------------------------------------------------*/
void CWinEventEmitter::submitEvents(CEventServer & server, bool allWindows)
{
	MSG	msg;
	while ( PeekMessageW(&msg,allWindows?NULL:(HWND)_HWnd,0,0,PM_REMOVE) )
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	// Dispatch sent messages
	_InternalServer.setServer (&server);
	_InternalServer.pump (allWindows);
}


/*------------------------------------------------------------------*\
							processMessage()
\*------------------------------------------------------------------*/


TKeyButton getKeyButton (bool _altButton, bool _shiftButton, bool _ctrlButton)
{
	TKeyButton button=noKeyButton;
	if (_altButton)
		(int&)button|=altKeyButton;
	if (_shiftButton)
		(int&)button|=shiftKeyButton;
	if (_ctrlButton)
		(int&)button|=ctrlKeyButton;

	return button;
}

/*TMouseButton getMouseButton (uint32 wParam, bool _altButton)
{
	TMouseButton button=noButton;
	if (wParam&MK_CONTROL)
		(int&)button|=ctrlButton;
	if (wParam&MK_LBUTTON)
		(int&)button|=leftButton;
	if (wParam&MK_RBUTTON)
		(int&)button|=rightButton;
	if (wParam&MK_MBUTTON)
		(int&)button|=middleButton;
	if (wParam&MK_SHIFT)
		(int&)button|=shiftButton;
	if (_altButton)
		(int&)button|=altButton;
}*/


TMouseButton CWinEventEmitter::getButtons() const
{
	uint result = (_CtrlButton ? ctrlButton : 0)
				 | (_AltButton ? altButton : 0)
				 | (_ShiftButton ? shiftButton : 0)
				 | (_CtrlButton ? ctrlButton : 0)
				 | (_MouseButtons[0] ? leftButton : 0)
				 | (_MouseButtons[1] ? rightButton : 0)
				 | (_MouseButtons[2] ? middleButton : 0);
	return (TMouseButton) result;
}


bool CWinEventEmitter::processMessage (uint32 hWnd, uint32 msg, uint32 wParam, uint32 lParam, CEventServer *server)
{
	if (!server)
		server=&_InternalServer;

	/// Process IME messages
	/*if ( _IMEEventsEnabled && (ImmIsUIMessage( ImmGetDefaultIMEWnd((HWND)_HWnd), msg, wParam, lParam) == TRUE) )
	{
		server->postEvent( new CEventIME(msg, wParam, lParam, this) );
		return true; // trap message (however DefWindowProc will still be called in some instances by the event listener)
	}*/

	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (_KeyboardEventsEnabled)
		{
			// Ctrl, shit or alt ?
			if ((int)wParam==VK_MENU)
				_AltButton=true;
			if ((int)wParam==VK_CONTROL)
				_CtrlButton=true;
			if ((int)wParam==VK_SHIFT)
				_ShiftButton=true;

			// Post the message
			if (wParam < KeyCount)
				server->postEvent (new CEventKeyDown ((NLMISC::TKey)wParam, getKeyButton(_AltButton, _ShiftButton, _CtrlButton), (((int) lParam)&(1<<30))==0, this));
		}
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (_KeyboardEventsEnabled)
		{
			// Ctrl, shit or alt ?
			if ((int)wParam==VK_MENU)
				_AltButton=false;
			if ((int)wParam==VK_CONTROL)
				_CtrlButton=false;
			if ((int)wParam==VK_SHIFT)
				_ShiftButton=false;

			// Post the message
			if (wParam < KeyCount)
				server->postEvent (new CEventKeyUp ((NLMISC::TKey)wParam, getKeyButton(_AltButton, _ShiftButton, _CtrlButton), this));
		}
		break;
	case WM_CHAR:
		if (_KeyboardEventsEnabled)
		{
			//if (wParam < KeyCount)
			//nlinfo("WM_CHAR with %u", wParam);
			server->postEvent (new CEventChar ((ucchar)wParam, getKeyButton(_AltButton, _ShiftButton, _CtrlButton), this));
		}
		break;
	/*case WM_IME_CHAR:
		if (_KeyboardEventsEnabled && _IMEEventsEnabled)
		{
			server->postEvent (new CEventChar ((ucchar)wParam, getKeyButton(_AltButton, _ShiftButton, _CtrlButton), this));
		}
		break;*/
	case WM_ACTIVATE:
		if (WA_INACTIVE==LOWORD(wParam))
			server->postEvent (new CEventActivate (false, this));
		else
		{
			// Reset flags state
			resetButtonFlagState ();

			// Post the message
			server->postEvent (new CEventActivate (true, this));
		}
		break;
	case WM_KILLFOCUS:
		server->postEvent (new CEventSetFocus (false, this));
		break;
	case WM_SETFOCUS:
		// Reset flags state
		resetButtonFlagState ();

		// Post the message
		server->postEvent (new CEventSetFocus (true, this));
		break;
	case WM_MOUSEMOVE:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		{
			if (_MouseEventsEnabled)
			{
				// MSWindows coordinates to NeL window coordinate
				float fX, fY;
				RECT client;

				float xPos = (float)GET_X_LPARAM(lParam);
				float yPos = (float)GET_Y_LPARAM(lParam);

				GetClientRect ((HWND)hWnd, &client);
				fX=xPos/(float)(client.right-client.left);
				fY=1.f-yPos/(float)(client.bottom-client.top);

				// buttons
				TMouseButton button=getButtons();

				// Reswitch
				switch (msg)
				{
				case WM_MOUSEMOVE:
					server->postEvent (new CEventMouseMove (fX, fY, button, this));
					break;

				case WM_RBUTTONDOWN:
					_MouseButtons[1] = true;
					server->postEvent (new CEventMouseDown (fX, fY, (TMouseButton)(rightButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				case WM_MBUTTONDOWN:
					_MouseButtons[2] = true;
					server->postEvent (new CEventMouseDown (fX, fY, (TMouseButton)(middleButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				case WM_LBUTTONDOWN:
					_MouseButtons[0] = true;
					server->postEvent (new CEventMouseDown (fX, fY, (TMouseButton)(leftButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;

				case WM_RBUTTONUP:
					_MouseButtons[1] = false;
					server->postEvent (new CEventMouseUp (fX, fY, (TMouseButton)(rightButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				case WM_MBUTTONUP:
					_MouseButtons[2] = false;
					server->postEvent (new CEventMouseUp (fX, fY, (TMouseButton)(middleButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				case WM_LBUTTONUP:
					_MouseButtons[0] = false;
					server->postEvent (new CEventMouseUp (fX, fY, (TMouseButton)(leftButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;

				case WM_RBUTTONDBLCLK:
					server->postEvent (new CEventMouseDblClk (fX, fY, (TMouseButton)(rightButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				case WM_MBUTTONDBLCLK:
					server->postEvent (new CEventMouseDblClk (fX, fY, (TMouseButton)(middleButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				case WM_LBUTTONDBLCLK:
					server->postEvent (new CEventMouseDblClk (fX, fY, (TMouseButton)(leftButton|(button&~(leftButton|middleButton|rightButton))), this));
					break;
				}
				break;
			}
			break;
		}
	case WM_DESTROY:
		server->postEvent (new CEventDestroyWindow (this));
		break;
	case WM_DISPLAYCHANGE:
		server->postEvent (new CEventDisplayChange (LOWORD(lParam), HIWORD(lParam), wParam, this));
		break;
	case WM_MOUSEWHEEL:
		if (_MouseEventsEnabled)
		{
			// MSWindows coordinates to NeL window coordinate
			float fX, fY;
			RECT client;
			GetClientRect ((HWND)hWnd, &client);
			if (client.right-client.left > 0)
				fX=(float)LOWORD(lParam)/(float)(client.right-client.left);
			else
				fX=0;
			if (client.bottom-client.top > 0)
				fY=1.f-(float)HIWORD(lParam)/(float)(client.bottom-client.top);
			else
				fY=0;

			// buttons
			TMouseButton button=getButtons();

			server->postEvent (new CEventMouseWheel (fX, fY, button, (short) HIWORD(wParam)>=0, this));
			break;
		}
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_NOTIFY:
	//case WM_INPUTLANGCHANGEREQUEST:
	case WM_INPUTLANGCHANGE:
		if ( _IMEEventsEnabled )
		{
			server->postEvent( new CEventIME( msg, wParam, lParam, this ) );
			return true; // trap message
		}
		break;
	}
	return false;
}

//==========================================================
void CWinEventEmitter::resetButtonFlagState ()
{
	_CtrlButton=( (GetAsyncKeyState(VK_CONTROL)&0x8000) != 0);
	_ShiftButton=( (GetAsyncKeyState(VK_SHIFT)&0x8000) != 0);
	_AltButton=( (GetAsyncKeyState(VK_MENU)&0x8000) != 0);
	//
	_MouseButtons[0]=( (GetAsyncKeyState(VK_LBUTTON)&0x8000) != 0);
	_MouseButtons[1]=( (GetAsyncKeyState(VK_RBUTTON)&0x8000) != 0);
	_MouseButtons[2]=( (GetAsyncKeyState(VK_MBUTTON)&0x8000) != 0);



}

//==========================================================
TMouseButton CWinEventEmitter::buildFlags() const
{
	uint flags = (_CtrlButton ? ctrlButton : 0)
				 | (_ShiftButton ? shiftButton : 0)
				 | (_AltButton   ? altButton : 0)
				 | (_MouseButtons[0] ? leftButton : 0)
				 | (_MouseButtons[1] ? rightButton : 0)
				 | (_MouseButtons[2] ? middleButton : 0);
	return (TMouseButton) flags;
}


} // NLMISC

#endif // NL_OS_WINDOWS
