#if !defined(AFX_PAXTRACKDLG_H__AE7611AA_3582_466F_BCEB_0E05355559FE__INCLUDED_)
#define AFX_PAXTRACKDLG_H__AE7611AA_3582_466F_BCEB_0E05355559FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxTrackDlg.h : header file
//

#include "results\paxlivetrack.h"
// declare
class Terminal;

/////////////////////////////////////////////////////////////////////////////
// CPaxTrackDlg dialog

class CPaxTrackDlg : public CDialog
{
// Construction
public:
	CPaxTrackDlg( Terminal* _pTerm, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxTrackDlg)
	enum { IDD = IDD_DIALOG_PAXLIVETRACK };
	CListCtrl	m_listTrack;
	CString	m_strTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxTrackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaxTrackDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPaxLiveTrack	m_PaxTrack;
	Terminal*		m_pTerm;

private:
	void InitListCtrl( void );
	void LoadTrackToList( void );
	void SaveTrackToFile( const CString& _strName );
public:
	bool Init( long _lLogIndex, Terminal* _pTerm, const CString& _strPath );
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXTRACKDLG_H__AE7611AA_3582_466F_BCEB_0E05355559FE__INCLUDED_)
