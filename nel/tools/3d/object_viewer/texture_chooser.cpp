/** \file texture_chooser.cpp
 * A dailog that helps to choose particles texture
 * $Id: texture_chooser.cpp,v 1.13 2007/03/19 09:55:26 boucher Exp $
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
 * MA 02
*/

#include "std_afx.h"
#include "object_viewer.h"
#include "texture_chooser.h"
#include "multi_tex_dlg.h"

#include "nel/3d/texture_file.h"
#include "nel/3d/texture_bump.h"
#include "nel/3d/ps_particle_basic.h"

#include "nel/misc/path.h"

// size of the bitmap that is displayed
const uint tSize = 25;

/////////////////////////////////////////////////////////////////////////////
// CTextureChooser dialog


CTextureChooser::CTextureChooser(NL3D::CPSMultiTexturedParticle *mtp /*= NULL*/, CParticleWorkspace::CNode *ownerNode) 
	: _CurrBitmap(0),
	  _Wrapper(NULL),
	  _Texture(NULL),
	  _EnableRemoveButton(false),
	  _MTP(mtp),
	  _Node(ownerNode),
	  _MultiTexDlg(NULL)

{
	//{{AFX_DATA_INIT(CTextureChooser)
	//}}AFX_DATA_INIT
}

CTextureChooser::~CTextureChooser()
{
	if (_MultiTexDlg)
	{
		_MultiTexDlg->DestroyWindow();
		delete _MultiTexDlg;
	}
	if (_CurrBitmap)
		::DeleteObject(_CurrBitmap);
}

BOOL CTextureChooser::EnableWindow( BOOL bEnable)
{
	GetDlgItem(IDC_BROWSE_TEXTURE)->EnableWindow(bEnable);
	GetDlgItem(IDC_REMOVE_TEXTURE)->EnableWindow(bEnable);
	if (_MTP)
	{
		GetDlgItem(IDC_ENABLE_MULTITEXTURING)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_MULTITEXTURING)->EnableWindow((bEnable ? true: false /* VC WARNING */) & _MTP->isMultiTextureEnabled());
	}
	return CEditAttribDlg::EnableWindow(bEnable);
}

void CTextureChooser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextureChooser)
	DDX_Control(pDX, IDC_ENABLE_MULTITEXTURING, m_MultiTexCtrl);
	//}}AFX_DATA_MAP
}


void CTextureChooser::init(uint32 x, uint32 y, CWnd *pParent)
{
	Create(IDD_TEXTURE_CHOOSER, pParent);
	RECT r;
	GetClientRect(&r);
	MoveWindow(x, y, r.right, r.bottom);	

	if (!_EnableRemoveButton)
	{
		GetDlgItem(IDC_REMOVE_TEXTURE)->ShowWindow(SW_HIDE);
	}
	if (!_MTP)
	{
		GetDlgItem(IDC_EDIT_MULTITEXTURING)->ShowWindow(SW_HIDE);
		m_MultiTexCtrl.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MULTITEX_BORDER)->ShowWindow(SW_HIDE);
	}
	else
	{
		if (_MTP->isMultiTextureEnabled())
		{
			m_MultiTexCtrl.SetCheck(1);	
		}
		updateMultiTexCtrl();
	}
	ShowWindow(SW_SHOW);
}


void CTextureChooser::textureToBitmap()
{
	if (!_Texture) 
	{
		if (_CurrBitmap)
		{
			::DeleteObject(_CurrBitmap);
			_CurrBitmap = NULL;
		}

		return;
	}
	
	if (_CurrBitmap)
	{
		::DeleteObject(_CurrBitmap);	
	}	
	// for a bumpmap, show its heightmap
	NL3D::CTextureBump *tb = dynamic_cast<NL3D::CTextureBump *>( (NL3D::ITexture *) _Texture);
	NL3D::ITexture *tex = tb ? tb->getHeightMap()  : _Texture;
	if (!tex) tex = _Texture;
	tex->generate();
	// make copy of the texture
	NLMISC::CBitmap cb(* ((NL3D::ITexture *) tex));
	
	cb.convertToType(NLMISC::CBitmap::RGBA);
	cb.resample(tSize, tSize);

	uint32 *dat  = (uint32 *) &(cb.getPixels()[0]);
	_CurrBitmap = ::CreateBitmap(tSize, tSize, 1, 32, dat);		
	tex->release();
	Invalidate();
	UpdateData(TRUE);
}



BEGIN_MESSAGE_MAP(CTextureChooser, CDialog)
	//{{AFX_MSG_MAP(CTextureChooser)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_REMOVE_TEXTURE, OnRemoveTexture)
	ON_BN_CLICKED(IDC_EDIT_MULTITEXTURING, OnEditMultitexturing)
	ON_BN_CLICKED(IDC_ENABLE_MULTITEXTURING, OnEnableMultitexturing)
	ON_BN_CLICKED(IDC_BROWSE_TEXTURE, OnBrowseTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureChooser message handlers

BOOL CTextureChooser::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	nlassert(_Wrapper);	
	_Texture = _Wrapper->get();

	// generate the bitmap
	
	textureToBitmap();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextureChooser::OnBrowseTexture() 
{
	std::string texName("*.tga");
	/// get the name of the previously set texture if there is one
	if (dynamic_cast<NL3D::CTextureFile *>(_Wrapper->get()))
	{
		texName = (static_cast<NL3D::CTextureFile *>(_Wrapper->get()))->getFileName();
	}
	CFileDialog fd(TRUE, ".tga", texName.c_str(), 0, NULL, this);
	if (fd.DoModal() == IDOK)
	{
		// Add to the path
		char drive[256];
		char dir[256];
		char path[256];

		// Add search path for the texture
		_splitpath (fd.GetPathName(), drive, dir, NULL, NULL);
		_makepath (path, drive, dir, NULL, NULL);
		NLMISC::CPath::addSearchPath (path);

		try
		{
			NL3D::CTextureFile *tf = new NL3D::CTextureFile(std::string(fd.GetFileName()));
			_Wrapper->setAndUpdateModifiedFlag(tf);
			_Texture = tf;
			textureToBitmap();
		}
		catch (NLMISC::Exception &e)
		{
			MessageBox(e.what(), "error loading texture");
		}		
	
	}
}

void CTextureChooser::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (_CurrBitmap)
	{
		HDC bitmapDc = ::CreateCompatibleDC(dc);
		HGDIOBJ old = ::SelectObject(bitmapDc, _CurrBitmap);
		
		::BitBlt(dc, 10, 10, tSize, tSize, bitmapDc, 0, 0, SRCCOPY);

		::SelectObject(bitmapDc, old);
		::DeleteDC(bitmapDc);
	}
}

void CTextureChooser::OnRemoveTexture() 
{
	if (_Texture)
	{
		_Texture->release();
		_Texture = NULL;
	}
	_Wrapper->setAndUpdateModifiedFlag(NULL);
	textureToBitmap();
	Invalidate();
}
	


void CTextureChooser::childPopupClosed(CWnd *child)
{
	nlassert(child == _MultiTexDlg);
	_MultiTexDlg->DestroyWindow();
	delete _MultiTexDlg;
	_MultiTexDlg = NULL;
	EnableWindow(TRUE);
}

void CTextureChooser::updateMultiTexCtrl()
{	
	GetDlgItem(IDC_EDIT_MULTITEXTURING)->EnableWindow(_MTP->isMultiTextureEnabled());	
	
}

void CTextureChooser::OnEditMultitexturing() 
{
	nlassert(_MTP);
	EnableWindow(FALSE);
	_MultiTexDlg = new CMultiTexDlg(_Node, _MTP, this, this);
	_MultiTexDlg->init(this);	
}

void CTextureChooser::OnEnableMultitexturing() 
{
	_MTP->enableMultiTexture(m_MultiTexCtrl.GetCheck() ? true : false /* VC warning */);	
	updateMultiTexCtrl();	
}


