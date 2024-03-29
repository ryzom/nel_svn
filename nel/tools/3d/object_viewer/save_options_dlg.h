/** \file save_options_dlg.h
 *
 * $Id: save_options_dlg.h,v 1.1 2004/06/17 08:02:06 vizerie Exp $
 */

/* Copyright, 2000-2004 Nevrax Ltd.
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


#if !defined(AFX_SAVE_OPTIONS_DLG_H__0BDBD07A_2A59_4BC5_BBF0_0DE3425EA3BC__INCLUDED_)
#define AFX_SAVE_OPTIONS_DLG_H__0BDBD07A_2A59_4BC5_BBF0_0DE3425EA3BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// save_options_dlg.h : header file
//

// 3 options message box, useful to confirm save for several files    
class CSaveOptionsDlg : public CDialog
{
// Construction
public:
	enum TChoice { Yes, No, SaveAll, Stop };
	CSaveOptionsDlg(const CString &caption, const CString &message, CWnd* pParent = NULL);   // standard constructor
	// returns choice of the user after DoModal has been called
	TChoice getChoice() const { return _Choice; }	

// Dialog Data
	//{{AFX_DATA(CSaveOptionsDlg)
	enum { IDD = IDD_SAVE_OPTIONS_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	// Generated message map functions
	//{{AFX_MSG(CSaveOptionsDlg)
	virtual void OnOK();
	afx_msg void OnSaveAll();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	TChoice _Choice;
	CString _Caption;
	CString _Message;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVE_OPTIONS_DLG_H__0BDBD07A_2A59_4BC5_BBF0_0DE3425EA3BC__INCLUDED_)
