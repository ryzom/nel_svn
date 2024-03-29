/** \file tile_utility.cpp
 * TODO: File description
 *
 * $Id: tile_utility.cpp,v 1.9 2007/03/19 09:55:27 boucher Exp $
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

#include "tile_utility.h"

#include "nel/misc/types_nl.h"
#include "nel/3d/tile_bank.h"
#include "nel/misc/file.h"
#include "../nel_patch_lib/rpo.h"

#define TILE_UTILITY_CLASS_ID	Class_ID(0x2301c0, 0x4c156b46)

extern ClassDesc* GetRGBAddDesc();
extern HINSTANCE hInstance;

using namespace NLMISC;
using namespace NL3D;

class Tile_utility : public UtilityObj 
{
public:
		HWND			hPanel;
		IUtil			*iu;
		Interface		*ip;
		static CTileBank Bank;
		static sint		Land;
		static std::string	Path;

		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);
		
		void DeleteThis() { }		

		void Load (const std::string& path);
		void SetLand (sint land);
		void SetupUI ();
		bool SetupMaterial ()  const;
public:

		//Constructor/Destructor
		Tile_utility();
		~Tile_utility();		
};

CTileBank	Tile_utility::Bank;
sint		Tile_utility::Land;
std::string		Tile_utility::Path;

static Tile_utility theTile_utility;

class Tile_utilityClassDesc:public ClassDesc2 
{
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) 
	{
		return &theTile_utility;
	}
	const TCHAR *	ClassName() {return "NeL Tile Bank";}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return TILE_UTILITY_CLASS_ID;}
	const TCHAR* 	Category() {return _T("NeL Tools");}
	const TCHAR*	InternalName() { return _T("NeL tile bank utility"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};

static Tile_utilityClassDesc Tile_utilityDesc;
ClassDesc2* GetTile_utilityDesc() {return &Tile_utilityDesc;}

static BOOL CALLBACK Tile_utilityDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		{
			//  load the sampler dropdown

			// Get the module path
			HMODULE hModule = hInstance;
			if (hModule)
			{
				// Get module file name
				char moduldeFileName[512];
				if (GetModuleFileName (hModule, moduldeFileName, 512))
				{
					// Get version info size
					DWORD doomy;
					uint versionInfoSize=GetFileVersionInfoSize (moduldeFileName, &doomy);
					if (versionInfoSize)
					{
						// Alloc the buffer
						char *buffer=new char[versionInfoSize];

						// Find the verion resource
						if (GetFileVersionInfo(moduldeFileName, 0, versionInfoSize, buffer))
						{
							uint *versionTab;
							uint versionSize;
							if (VerQueryValue (buffer, "\\", (void**)&versionTab,  &versionSize))
							{
								// Get the pointer on the structure
								VS_FIXEDFILEINFO *info=(VS_FIXEDFILEINFO*)versionTab;
								if (info)
								{
 									// Setup version number
									char version[512];
									sprintf (version, "Version %d.%d.%d.%d", 
										info->dwFileVersionMS>>16, 
										info->dwFileVersionMS&0xffff, 
										info->dwFileVersionLS>>16,  
										info->dwFileVersionLS&0xffff);
									SetWindowText (GetDlgItem (hWnd, IDC_VERSION), version);
								}
								else
									SetWindowText (GetDlgItem (hWnd, IDC_VERSION), "VS_FIXEDFILEINFO * is NULL");
							}
							else
								SetWindowText (GetDlgItem (hWnd, IDC_VERSION), "VerQueryValue failed");
						}
						else
							SetWindowText (GetDlgItem (hWnd, IDC_VERSION), "GetFileVersionInfo failed");

						// Free the buffer
						delete [] buffer;
					}
					else
						SetWindowText (GetDlgItem (hWnd, IDC_VERSION), "GetFileVersionInfoSize failed");
				}
				else
					SetWindowText (GetDlgItem (hWnd, IDC_VERSION), "GetModuleFileName failed");
			}
			else
				SetWindowText (GetDlgItem (hWnd, IDC_VERSION), "hInstance NULL");


			theTile_utility.Init(hWnd);
		}
		break;
	case WM_DESTROY:
		//  load the sampler dropdown
		theTile_utility.Destroy(hWnd);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		theTile_utility.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
		break;
	case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			switch (id) 
			{
				case IDC_BANK_PATH:
					{
						static char sPath[256];
						static bool bFirst=false;
						if (!bFirst)
						{
							sPath[0]=0;
							bFirst=true;
						}
 						OPENFILENAME ofn;
						ofn.lStructSize=sizeof (ofn);
						ofn.hwndOwner=NULL;
						ofn.lpstrFilter="Rykol bank files (*.bank)\0*.bank\0All Files (*.*)\0*.*\0";
						ofn.lpstrCustomFilter=NULL;
						ofn.nMaxCustFilter=0;
						ofn.nFilterIndex=0;
						ofn.lpstrFile=sPath;
						ofn.nMaxFile=256;
						ofn.lpstrFileTitle=NULL;
						ofn.nMaxFileTitle=NULL;
						ofn.lpstrInitialDir=NULL;
						ofn.lpstrTitle="Choose a bank file";
						ofn.Flags=OFN_ENABLESIZING|OFN_FILEMUSTEXIST;
						ofn.nFileOffset=0;
						ofn.nFileExtension=0;
						ofn.lpstrDefExt=0;
						ofn.lCustData=0;
						ofn.lpfnHook=0;
						ofn.lpTemplateName=0;
						if (GetOpenFileName(&ofn))
						{
							theTile_utility.Load (sPath);
							theTile_utility.SetLand (theTile_utility.Land);
							theTile_utility.SetupUI ();
						}
					}
					break;
				case IDC_LAND:
					{
						switch (HIWORD(wParam))
						{
							case CBN_SELCHANGE:
								{
									HWND hwndComboBox = (HWND) lParam;
									int nCur=SendMessage (hwndComboBox, CB_GETCURSEL, 0, 0);
									nlassert (nCur>=0);
									nlassert (nCur<theTile_utility.Bank.getLandCount());
									theTile_utility.SetLand (nCur);
									theTile_utility.SetupUI ();
								}
								break;
						}
					}
					break;
				case IDC_SETUP:
					{
						if (!theTile_utility.SetupMaterial ())
							MessageBox (NULL, "Select some nel patch object..", "Rykol tile", MB_OK|MB_ICONEXCLAMATION);
					}
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}



//--- Tile_utility -------------------------------------------------------
Tile_utility::Tile_utility()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
	Bank.clear();
	Land=0;
	SetupUI ();
}

Tile_utility::~Tile_utility()
{

}

void Tile_utility::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		Tile_utilityDlgProc,
		GetString(IDS_PARAMS),
		0);
	SetupUI ();
}
	
void Tile_utility::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void Tile_utility::Init(HWND hWnd)
{

}

void Tile_utility::Destroy(HWND hWnd)
{

}

void Tile_utility::Load (const std::string& path)
{
	try
	{
		Path=path;
		CIFile file;
		if (!file.open (path))
		{
			char tmp[1024];
			sprintf (tmp, "File not found: %s", path);
			MessageBox (NULL, tmp, "Error..", MB_OK|MB_ICONEXCLAMATION);
		}
		else
		{
			Bank.clear();
			Bank.serial (file);
			SetBankPathName (path);
		}
	}
	catch (EStream stream)
	{
		char tmp[1024];
		sprintf (tmp, "Error while loading %s:\n\n%s", path, stream.what());
		MessageBox (NULL, tmp, "Error..", MB_OK|MB_ICONEXCLAMATION);
	}
}

void Tile_utility::SetLand (sint land)
{
	if ((land>=0)&&(land<Bank.getLandCount()))
	{
		// Land number
		Land=land;
	}
	else
		Land=0;
	SetBankTileSetSet (Land);
}
	
void Tile_utility::SetupUI ()
{
	// Clear combo box
	HWND hCombo=NULL;
	if (hPanel)
	{
		hCombo=GetDlgItem (hPanel, IDC_LAND);
		nlassert (hCombo);
		SendMessage (hCombo, CB_RESETCONTENT, 0, 0);

		// Enable combo box
		if (Bank.getLandCount())
			EnableWindow (hCombo, TRUE);
		else 
			EnableWindow (hCombo, FALSE);
	}

	for (int nLand=0; nLand<Bank.getLandCount(); nLand++)
	{
		std::string name=Bank.getLand(nLand)->getName();
		if (hCombo)
		{
			SendMessage (hCombo, CB_INSERTSTRING, -1, (LPARAM) name.c_str());
		}
	}

	// Cur sel
	if (hCombo)
	{
		SendMessage (hCombo, CB_SETCURSEL, Land, 0);
	}

	// Button name
	if (hPanel)
	{
		HWND hwnd=GetDlgItem (hPanel, IDC_BANK_PATH);
		nlassert (hwnd);
		HWND hwndStatic1=GetDlgItem (hPanel, IDC_TILE_COUNT1);
		nlassert (hwndStatic1);
		HWND hwndStatic2=GetDlgItem (hPanel, IDC_TILE_COUNT2);
		nlassert (hwndStatic2);
		HWND hwndStatic3=GetDlgItem (hPanel, IDC_TILE_COUNT3);
		nlassert (hwndStatic3);
		if (Bank.getLandCount())
		{
			// Button text
			char sName[256];
			_splitpath (Path.c_str(), NULL, NULL, sName, NULL);
			char *sName2=sName;
			if (*sName2)
				*sName2=toupper (*sName2);
			sName2++;
			while (*sName2)
			{
				*sName2=tolower (*sName2);
				sName2++;
			}
			SetWindowText (hwnd, sName);

			// Static text
			char sTmp[256];
			sprintf (sTmp, "%d diffuse tiles.", Bank.getNumBitmap (CTile::diffuse));
			SetWindowText (hwndStatic1, sTmp);
			sprintf (sTmp, "%d additive tiles.", Bank.getNumBitmap (CTile::additive));
			SetWindowText (hwndStatic2, sTmp);
		}
		else
		{
			SetWindowText (hwnd, "Click to choose a bank..");
			SetWindowText (hwndStatic1, "");
			SetWindowText (hwndStatic2, "");
			SetWindowText (hwndStatic3, "");
		}
	}
}

bool Tile_utility::SetupMaterial () const
{
	// Num sel node
	int numSel=ip->GetSelNodeCount();

	// Num mat set
	bool bSet=false;

	// Time
	TimeValue t=ip->GetTime();

	// Multi
	MultiMtl* multi=NewDefaultMultiMtl();
	multi->SetNumSubMtls (Bank.getTileCount()+1);
	multi->SetName ("Rykol Bank");

	// Default mtl
	Mtl* firstMtl=multi->GetSubMtl (0);

	// Mtl param
	firstMtl->SetDiffuse (Color (0.5f,0.5f,0.5f), t);
	firstMtl->SetAmbient (Color (0,0,0), t);
	firstMtl->SetName ("Rykol Tile Default");
	firstMtl->SetShininess (0.0, t);
	firstMtl->SetSpecular (Color (0,0,0), t);

	int i;
	for (i=0; i<Bank.getTileCount(); i++)
	{
		// Active ?
		bool bActive=false;
		const CTile *tile=Bank.getTile (i);
		if (!tile->isFree())
		{

			// New Mtl
			Mtl* mtl=multi->GetSubMtl (i+1);

			// Mtl param
			mtl->SetDiffuse (Color (1.f,1.f,1.f), t);
			mtl->SetAmbient (Color (0,0,0), t);
			mtl->SetName ("Rykol Tile");
			mtl->SetShininess (0.0, t);
			mtl->SetSpecular (Color (0,0,0), t);

			if ((tile->getRelativeFileName(CTile::diffuse)!="")||(tile->getRelativeFileName(CTile::additive)!=""))
			{
				bActive=true;
				Texmap* rgb=(Texmap*)GetRGBAddDesc()->Create (FALSE);

				// Assign BitmapTex
				mtl->SetSubTexmap (ID_DI, rgb);
				mtl->SubTexmapOn (ID_DI);

				if (tile->getRelativeFileName(CTile::diffuse)!="")
				{
					// New BitmapTex
					BitmapTex* tex=NewDefaultBitmapTex();

					// BitmapTex param
					tex->SetAlphaSource (ALPHA_NONE);
					tex->SetAlphaAsMono (FALSE);
					tex->SetAlphaAsRGB (FALSE);
					tex->SetMapName (const_cast<char*>((Bank.getAbsPath()+tile->getRelativeFileName(CTile::diffuse)).c_str()));

					// Assign BitmapTex
					rgb->SetSubTexmap (0, tex);
					rgb->SubTexmapOn (0);
					tex->ActivateTexDisplay (TRUE);
					mtl->SetActiveTexmap(tex);
					mtl->SetMtlFlag(MTL_TEX_DISPLAY_ENABLED);
					mtl->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
				}

				if (tile->getRelativeFileName(CTile::additive)!="")
				{
					// New BitmapTex
					BitmapTex* tex=NewDefaultBitmapTex();

					// BitmapTex param
					tex->SetAlphaSource (ALPHA_NONE);
					tex->SetAlphaAsMono (FALSE);
					tex->SetAlphaAsRGB (FALSE);
					tex->SetMapName (const_cast<char*>((Bank.getAbsPath()+tile->getRelativeFileName(CTile::additive)).c_str()));

					// Assign BitmapTex
					rgb->SetSubTexmap (1, tex);
					rgb->SubTexmapOn (1);
				}
			}

		}
		if (!bActive)
		{
			multi->SetSubMtl (i+1, NULL);
		}
	}

	// For each sel node
	for (i=0; i<numSel; i++)
	{
		// Node
		INode *pNode=ip->GetSelNode(i);
		nlassert (pNode);

		ObjectState os=pNode->EvalWorldState(t);
		if (os.obj)
		{
			if (os.obj->CanConvertToType(RYKOLPATCHOBJ_CLASS_ID))
			{
				pNode->SetMtl (multi);
				bSet=true;
			}
		}
	}

	ip->ForceCompleteRedraw();

	// Ok
	if (!bSet)
		delete multi;
	return bSet;
}
