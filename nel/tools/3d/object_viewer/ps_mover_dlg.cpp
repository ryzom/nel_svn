/** \file ps_mover_dlg.cpp
 * this dialog display coordinate of an instance of a located in a particle system 
 * $Id: ps_mover_dlg.cpp,v 1.13 2007/03/19 09:55:26 boucher Exp $
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

#include "std_afx.h"
#include "object_viewer.h"
#include "ps_mover_dlg.h"
#include "particle_tree_ctrl.h"
#include "particle_dlg.h"
#include "main_frame.h"
#include "nel/3d/ps_located.h"
#include "nel/3d/particle_system.h"


#include "editable_range.h"
#include "direction_attr.h"

/////////////////////////////////////////////////////////////////////////////
// CPSMoverDlg dialog


CPSMoverDlg::CPSMoverDlg(CParticleWorkspace::CNode *ownerNode, CWnd *parent, NL3D::CEvent3dMouseListener *ml,  NL3D::CPSLocated *editedLocated, uint32 editedLocatedIndex)   // standard constructor
	: CDialog(CPSMoverDlg::IDD, parent),
	  _Node(ownerNode),
	  _EditedLocated(editedLocated),
	  _EditedLocatedIndex(editedLocatedIndex),
	  _DirectionDlg(NULL),
	  _MouseListener(ml),
	  _Scale(NULL), _XScale(NULL), _YScale(NULL), _ZScale(NULL),
	  _ScaleText(NULL), _XScaleText(NULL), _YScaleText(NULL), _ZScaleText(NULL)
{

	

	const NLMISC::CVector &pos = _EditedLocated->getPos()[_EditedLocatedIndex];
	
	m_X.Format("%.3g", pos.x);
	m_Y.Format("%.3g", pos.y);
	m_Z.Format("%.3g", pos.z);

	//{{AFX_DATA_INIT(CPSMoverDlg)
	//}}AFX_DATA_INIT


}

CPSMoverDlg::~CPSMoverDlg()
{
	cleanScaleCtrl();
	if (_DirectionDlg) 
	{
		_DirectionDlg->DestroyWindow();
		delete _DirectionDlg;
	}
}

void CPSMoverDlg::updateListener(void)
{
	if(_ParticleDlg->MainFrame->isMoveElement())
	{
		const NLMISC::CVector &pos = _EditedLocated->getPos()[_EditedLocatedIndex];
		NLMISC::CMatrix m;
		m = _MouseListener->getModelMatrix();
		m.setPos(pos);
		_MouseListener->setModelMatrix(m);
		_Node->setModified(true);
	}
}

void CPSMoverDlg::updatePosition(void)
{
	UpdateData();
	const NLMISC::CVector &pos = _EditedLocated->getPos()[_EditedLocatedIndex];
	
	m_X.Format("%.3g", pos.x);
	m_Y.Format("%.3g", pos.y);
	m_Z.Format("%.3g", pos.z);

	UpdateData(FALSE);
}

void CPSMoverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSMoverDlg)
	DDX_Control(pDX, IDC_SUB_COMPONENT, m_SubComponentCtrl);
	DDX_Text(pDX, IDC_XPOS, m_X);
	DDX_Text(pDX, IDC_YPOS, m_Y);
	DDX_Text(pDX, IDC_ZPOS, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSMoverDlg, CDialog)
	//{{AFX_MSG_MAP(CPSMoverDlg)
	ON_BN_CLICKED(IDC_UPDATE_XPOS, OnUpdateXpos)
	ON_BN_CLICKED(IDC_UPDATE_YPOS, OnUpdateYpos)
	ON_BN_CLICKED(IDC_UPDATE_ZPOS, OnUpdateZpos)
	ON_LBN_SELCHANGE(IDC_SUB_COMPONENT, OnSelchangeSubComponent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSMoverDlg message handlers

void CPSMoverDlg::OnUpdateXpos() 
{
	UpdateData();
	NLMISC::CVector &pos = _EditedLocated->getPos()[_EditedLocatedIndex];
	float x;
	if (::sscanf(m_X, "%f", &x) == 1)
	{
		pos.x = x;
		updateListener();
	}
	else
	{
		MessageBox("invalid entry", "error");
	}
	UpdateData(FALSE);
}

void CPSMoverDlg::OnUpdateYpos() 
{
	UpdateData();
	NLMISC::CVector &pos = _EditedLocated->getPos()[_EditedLocatedIndex];
	float y;
	if (::sscanf(m_Y, "%f", &y) == 1)
	{
		pos.y = y;
		updateListener();
	}
	else
	{
		MessageBox("invalid entry", "error");
	}
	UpdateData(FALSE);	
}

void CPSMoverDlg::OnUpdateZpos() 
{
	UpdateData();
	NLMISC::CVector &pos = _EditedLocated->getPos()[_EditedLocatedIndex];
	float z;
	if (::sscanf(m_Z, "%f", &z) == 1)
	{
		pos.z = z;
		updateListener();		
	}
	else
	{
		MessageBox("invalid entry", "error");
	}
	UpdateData(FALSE);	
}

BOOL CPSMoverDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	uint numBound = _EditedLocated->getNbBoundObjects();
	
	uint nbCandidates = 0;

	for (uint k = 0; k < numBound; ++k)
	{
		if (dynamic_cast<NL3D::IPSMover *>(_EditedLocated->getBoundObject(k)))
		{
			uint insertedLine = m_SubComponentCtrl.AddString(_EditedLocated->getBoundObject(k)->getName().c_str());
			m_SubComponentCtrl.SetItemData(insertedLine, (DWORD) _EditedLocated->getBoundObject(k));
			++nbCandidates;			
		}
	}

	if (nbCandidates > 0)
	{
		m_SubComponentCtrl.SetCurSel(0);	
		_EditedLocated->getOwner()->setCurrentEditedElement(_EditedLocated, _EditedLocatedIndex, (NL3D::CPSLocatedBindable *) m_SubComponentCtrl.GetItemData(0));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPSMoverDlg::cleanScaleCtrl(void)
{
	#define REMOVE_WINDOW(w) if (w) { w->DestroyWindow(); delete w; w = NULL; }
									
	REMOVE_WINDOW(_Scale);
	REMOVE_WINDOW(_XScale);
	REMOVE_WINDOW(_YScale);
	REMOVE_WINDOW(_ZScale);

	REMOVE_WINDOW(_ScaleText);
	REMOVE_WINDOW(_XScaleText);
	REMOVE_WINDOW(_YScaleText);
	REMOVE_WINDOW(_ZScaleText);
	REMOVE_WINDOW(_DirectionDlg);
}

void CPSMoverDlg::createScaleControls(void)
{
	cleanScaleCtrl();

			
	NL3D::IPSMover *m = getMoverInterface();
	if (!m) return;

	const sint xPos = 10;
	sint yPos = 330;
	


	RECT r;

	if (m->supportUniformScaling() && ! m->supportNonUniformScaling() )
	{
		_Scale = new CEditableRangeFloat("UNIFORM SCALE", _Node, 0.f, 4.f);
		_UniformScaleWrapper.M = m;
		_UniformScaleWrapper.Index = _EditedLocatedIndex;

		_Scale->setWrapper(&_UniformScaleWrapper);
		_Scale->init(100 + xPos, yPos, this);

		_ScaleText = new CStatic;
		_ScaleText ->Create("Scale : ", SS_LEFT, CRect(xPos, yPos + 10, xPos + 60, yPos + 32), this);
		_ScaleText ->ShowWindow(SW_SHOW);

		_Scale->GetClientRect(&r);
		yPos += r.bottom;
	}
	else if (m->supportNonUniformScaling())
	{
		

		// dialog for edition of x scale
		_XScale = new CEditableRangeFloat("X SCALE", _Node, 0.f, 4.f);
		_XScaleWrapper.M = m;
		_XScaleWrapper.Index = _EditedLocatedIndex;
		_XScale->setWrapper(&_XScaleWrapper);
		_XScale->init(xPos + 70, yPos, this);
		_XScale->GetClientRect(&r);

		_XScaleText = new CStatic;
		_XScaleText ->Create("x Scale : ", SS_LEFT, CRect(xPos, yPos + 10, xPos + 60, yPos + 32), this);
		_XScaleText ->ShowWindow(SW_SHOW);

		yPos += r.bottom;

		// dialog for edition of y scale
		_YScale = new CEditableRangeFloat("Y SCALE", _Node, 0.f, 4.f);
		_YScaleWrapper.M = m;
		_YScaleWrapper.Index = _EditedLocatedIndex;
		_YScale->setWrapper(&_YScaleWrapper);
		_YScale->init(xPos + 70, yPos, this);
		_YScale->GetClientRect(&r);
		

		_YScaleText = new CStatic;
		_YScaleText ->Create("y Scale : ", SS_LEFT, CRect(xPos, yPos + 10, xPos + 60, yPos + 32), this);
		_YScaleText ->ShowWindow(SW_SHOW);

		yPos += r.bottom;

		// dialog for edition of x scale
		_ZScale = new CEditableRangeFloat("Z SCALE", _Node, 0.f, 4.f);
		_ZScaleWrapper.M = m;
		_ZScaleWrapper.Index = _EditedLocatedIndex;
		_ZScale->setWrapper(&_ZScaleWrapper);
		_ZScale->init(xPos + 70, yPos, this);
		_ZScale->GetClientRect(&r);
		
		_ZScaleText = new CStatic;
		_ZScaleText ->Create("z Scale : ", SS_LEFT, CRect(xPos, yPos + 10, xPos + 60, yPos + 32), this);
		_ZScaleText ->ShowWindow(SW_SHOW);
	}


	if (m->onlyStoreNormal())
	{
		_DirectionDlg = new CDirectionAttr(" ELEMENT DIRECTION");
		_DirectionDlg->init(xPos, yPos, this);
		_DirectionWrapper.M = m;
		_DirectionWrapper.Index = _EditedLocatedIndex;
		_DirectionDlg->setWrapper(&_DirectionWrapper);
	}

	
}

void CPSMoverDlg::OnSelchangeSubComponent() 
{
	UpdateData();	
	createScaleControls();
	_EditedLocated->getOwner()->setCurrentEditedElement(_EditedLocated, _EditedLocatedIndex, getLocatedBindable());
	UpdateData(FALSE);
}


NL3D::IPSMover *CPSMoverDlg::getMoverInterface(void)
{
	UpdateData();
	nlassert(_EditedLocated);
	sint currIndex = m_SubComponentCtrl.GetCurSel();
	if (currIndex == -1) return NULL;	
	return dynamic_cast<NL3D::IPSMover *>((NL3D::CPSLocatedBindable *)(m_SubComponentCtrl.GetItemData(currIndex)));
}

void CPSMoverDlg::init(CParticleDlg	*parent)
{
	_ParticleDlg= parent;

	Create(CPSMoverDlg::IDD, parent);
	ShowWindow(SW_SHOW);
	createScaleControls();
}


NL3D::CPSLocatedBindable *CPSMoverDlg::getLocatedBindable(void)
{
	UpdateData();
	nlassert(_EditedLocated);
	sint currIndex = m_SubComponentCtrl.GetCurSel();
	if (currIndex == -1) return NULL;
	
	return (NL3D::CPSLocatedBindable *) m_SubComponentCtrl.GetItemData(currIndex);
}


