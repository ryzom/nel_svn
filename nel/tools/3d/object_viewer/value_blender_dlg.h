/** \file value_blender_dlg.h
 * a dialog to choose 2 values that are linearly blended in a particle system
 *
 * $Id: value_blender_dlg.h,v 1.6 2004/06/17 08:00:11 vizerie Exp $
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


#if !defined(AFX_VALUEBLENDERDLG_H__4242C860_C538_45BD_8348_C6DF314D688A__INCLUDED_)
#define AFX_VALUEBLENDERDLG_H__4242C860_C538_45BD_8348_C6DF314D688A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "particle_workspace.h"

struct IPopupNotify;
class CEditAttribDlg ;


// user of the dialog must provide an implementation of this struct to provide indivual values edition
struct IValueBlenderDlgClient
{		
	/** Create a dialog to edit a single value.
	 *  \param index must be 0 or 1, it says which value is being edited	 
	 */
	virtual CEditAttribDlg *createDialog(uint index, CParticleWorkspace::CNode *ownerNode) = 0;

	/// dtor
	virtual ~IValueBlenderDlgClient() {}
} ;

class CValueBlenderDlg : public CDialog
{
// Construction
public:	
	/** Create the dialog.
	 * \param createInterface interface that allows to create a dialog to edit one of the 2 values used for the blend.
	 * \param destroyInterface true if this object must take care to call 'delete' on the 'createInterface' pointer
	 */
	CValueBlenderDlg(IValueBlenderDlgClient *createInterface,
					 bool destroyInterface,
					 CWnd* pParent,
					 IPopupNotify *pn,
					 CParticleWorkspace::CNode *ownerNode
					);   // standard constructor
	// dtor
	~CValueBlenderDlg() ;
	// non modal display
	void init(CWnd *pParent);

// Dialog Data
	//{{AFX_DATA(CValueBlenderDlg)
	enum { IDD = IDD_VALUE_BLENDER };
	CStatic	m_Value2;
	CStatic	m_Value1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValueBlenderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void childPopupDestroyed(CWnd *child);	
	IValueBlenderDlgClient *_CreateInterface ;
	// the 2 dialog used to choose the blending value
	CEditAttribDlg		   *_Dlg1, *_Dlg2 ;
	IPopupNotify			  *_PN;
	bool					  _DestroyInterface;
	CParticleWorkspace::CNode *_Node;
	// Generated message map functions
	//{{AFX_MSG(CValueBlenderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALUEBLENDERDLG_H__4242C860_C538_45BD_8348_C6DF314D688A__INCLUDED_)
