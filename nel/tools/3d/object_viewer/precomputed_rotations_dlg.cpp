/** \file precomputed_rotations_dlg.cpp
 * a dialog to edit precomputed rotations of elements in a particle system
 *
 * $Id: precomputed_rotations_dlg.cpp,v 1.8 2007/03/19 09:55:26 boucher Exp $
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
#include "precomputed_rotations_dlg.h"
#include "nel/3d/ps_particle.h"
#include "nel/3d/ps_mesh.h"
#include "attrib_dlg.h"


/////////////////////////////////////////////////////////////////////////////
// CPrecomputedRotationsDlg dialog


CPrecomputedRotationsDlg::CPrecomputedRotationsDlg(CParticleWorkspace::CNode *ownerNode, NL3D::CPSHintParticleRotateTheSame *prts, CAttribDlg *toDisable)	
	: _Node(ownerNode), _RotatedParticle(prts), _WndToDisable(toDisable)
{
	float minValue, maxValue;
	uint32 nbModels = prts->checkHintRotateTheSame(minValue, maxValue);
	//{{AFX_DATA_INIT(CPrecomputedRotationsDlg)
	m_RotSpeedMax = _T("");
	m_PrecomputedRotations = nbModels ? TRUE : FALSE;
	m_RotSpeedMin = _T("");
	m_NbModels = _T("");
	//}}AFX_DATA_INIT
}


void CPrecomputedRotationsDlg::init(CWnd *pParent, sint x, sint y)
{
	Create(IDD_HINT_ROTATE_THE_SAME, pParent);
	RECT r;
	GetClientRect(&r);
	CRect wr;
	wr.left = r.left + x;
	wr.top = r.top + y;
	wr.bottom = r.bottom + y;
	wr.right = r.right + x;
	MoveWindow(wr);
	ShowWindow(SW_SHOW);
	enablePrecompRotationControl(); 
	updateFromReader();	
}

void CPrecomputedRotationsDlg::enablePrecompRotationControl(void)
{
	nlassert(_RotatedParticle);
	UpdateData();
	m_RotSpeedMinCtrl.EnableWindow(m_PrecomputedRotations);
	m_RotSpeedMaxCtrl.EnableWindow(m_PrecomputedRotations);
	m_NbModelsCtrl.EnableWindow(m_PrecomputedRotations);
	GetDlgItem(IDC_UPDATE_MIN_ROT_SPEED)->EnableWindow(m_PrecomputedRotations);
	GetDlgItem(IDC_UPDATE_MAX_ROT_SPEED)->EnableWindow(m_PrecomputedRotations);
	GetDlgItem(IDC_UPDATE_NB_MODELS)->EnableWindow(m_PrecomputedRotations);	
	UpdateData(FALSE);
}

void CPrecomputedRotationsDlg::updateFromReader(void)
{
	UpdateData();
	if (m_PrecomputedRotations)
	{
		char out[256];
		float minVelocity, maxVelocity;
		sprintf(out, "%d", _RotatedParticle->checkHintRotateTheSame(minVelocity, maxVelocity));
		m_NbModels = out;
		sprintf(out, "%g", minVelocity);
		m_RotSpeedMin = out;
		sprintf(out, "%g", maxVelocity);
		m_RotSpeedMax = out;
	}
	else
	{
		m_NbModels = m_RotSpeedMin = m_RotSpeedMax = "";
	}

	UpdateData(FALSE);
}


void CPrecomputedRotationsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrecomputedRotationsDlg)
	DDX_Control(pDX, IDC_ROT_SPEED_MIN, m_RotSpeedMinCtrl);
	DDX_Control(pDX, IDC_NB_MODELS, m_NbModelsCtrl);
	DDX_Control(pDX, IDC_ROT_SPEED_MAX, m_RotSpeedMaxCtrl);
	DDX_Text(pDX, IDC_ROT_SPEED_MAX, m_RotSpeedMax);
	DDX_Check(pDX, IDC_HINT_PRECOMPUTED_ROTATIONS, m_PrecomputedRotations);
	DDX_Text(pDX, IDC_ROT_SPEED_MIN, m_RotSpeedMin);
	DDX_Text(pDX, IDC_NB_MODELS, m_NbModels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrecomputedRotationsDlg, CDialog)
	//{{AFX_MSG_MAP(CPrecomputedRotationsDlg)
	ON_BN_CLICKED(IDC_UPDATE_MIN_ROT_SPEED, OnUpdateMinRotSpeed)
	ON_BN_CLICKED(IDC_UPDATE_MAX_ROT_SPEED, OnUpdateMaxRotSpeed)
	ON_BN_CLICKED(IDC_UPDATE_NB_MODELS, OnUpdateNbModels)
	ON_BN_CLICKED(IDC_HINT_PRECOMPUTED_ROTATIONS, OnHintPrecomputedRotations)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrecomputedRotationsDlg message handlers

void CPrecomputedRotationsDlg::OnUpdateMinRotSpeed() 
{
	nlassert(_RotatedParticle);
	UpdateData();
	float newValue, valueMin, valueMax;
	if (sscanf(m_RotSpeedMin, "%f", &newValue) == 1)
	{
		uint32 nbModels = _RotatedParticle->checkHintRotateTheSame(valueMin, valueMax);
		valueMin = newValue;
		_RotatedParticle->hintRotateTheSame(nbModels, valueMin, valueMax);
		updateModifiedFlag();
	}
	else
	{
		MessageBox("invalid value !!");
	}
	UpdateData(FALSE);
	updateModifiedFlag();
}

void CPrecomputedRotationsDlg::OnUpdateMaxRotSpeed() 
{
	nlassert(_RotatedParticle);
	UpdateData();
	float newValue, valueMin, valueMax;
	if (sscanf(m_RotSpeedMax, "%f", &newValue) == 1)
	{
		uint32 nbModels = _RotatedParticle->checkHintRotateTheSame(valueMin, valueMax);
		valueMax = newValue;
		_RotatedParticle->hintRotateTheSame(nbModels, valueMin, valueMax);
		updateModifiedFlag();
	}
	else
	{
		MessageBox("invalid value !!");
	}
	UpdateData(FALSE);
	updateModifiedFlag();
}

void CPrecomputedRotationsDlg::OnUpdateNbModels() 
{
	nlassert(_RotatedParticle);
	UpdateData();
	float valueMin, valueMax;
	sint32 newNbModels;
	bool valid = (sscanf(m_NbModels, "%d", &newNbModels) == 1 && newNbModels > 0);
	if (dynamic_cast<NL3D::CPSConstraintMesh *>(_RotatedParticle))
	{
		valid &= (newNbModels < NL3D::ConstraintMeshMaxNumPrerotatedModels);
	}
	if (valid)
	{
		_RotatedParticle->checkHintRotateTheSame(valueMin, valueMax);	
		_RotatedParticle->hintRotateTheSame((uint32) newNbModels, valueMin, valueMax);
		updateModifiedFlag();
	}
	else
	{
		MessageBox("invalid value !!");
	}
	UpdateData(FALSE);
	updateModifiedFlag();
}

void CPrecomputedRotationsDlg::OnHintPrecomputedRotations() 
{
	nlassert(_RotatedParticle);
	UpdateData();
	if (_WndToDisable)
	{
		_WndToDisable->EnableWindow(!m_PrecomputedRotations);
	}
	if (m_PrecomputedRotations)
	{
		_RotatedParticle->hintRotateTheSame(32);
		updateModifiedFlag();
	}
	else
	{
		_RotatedParticle->disableHintRotateTheSame();
		updateModifiedFlag();
	}

	enablePrecompRotationControl(); 
	updateFromReader();
	UpdateData(FALSE);	
}
