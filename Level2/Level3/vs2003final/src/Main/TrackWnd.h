#if !defined(AFX_TRACKWND_H__5BD5E1B3_DDD8_4F54_B2A2_F5414E9C5B91__INCLUDED_)
#define AFX_TRACKWND_H__5BD5E1B3_DDD8_4F54_B2A2_F5414E9C5B91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrackWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrackWnd window
#include "results\paxlivetrack.h"
#include "results\paxdestdiagnoseinfo.h"

class Terminal;
class DiagnoseEntry;

#define WM_TRACKWND_END	(WM_USER+928)

class CTrackWnd : public CWnd
{
// Construction
public:
	CTrackWnd(Terminal* _pTerm, CString strProjPath, CWnd* pParent);

	void ShowInfo(DiagnoseEntry* _pEntry, int nItem, int nSubItem);
// Attributes
public:

// Operations
public:	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrackWnd)
protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	CWnd* GetParent() const;

// Implementation
public:
	virtual ~CTrackWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrackWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonSave();
	afx_msg void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	DECLARE_MESSAGE_MAP()

private:
	CPaxLiveTrack	m_PaxTrack;					// miss flight diagnose info
	CPaxDestDiagnoseInfo m_DestDiagnoseInfo;	// pax dest diagnose info

	Terminal*		m_pTerm;
	CString			m_strTitle;
	DiagnoseEntry*	m_pDiagnoseEntry;

	CWnd*			m_pUsedParentWnd;
	BOOL			m_bWndClosing;

	BOOL m_bClickSave;
	int m_nSubItem;
	int m_nItem;
	CString m_strProjPath;
	CButton m_btnOK;
	CButton m_btnSave;
	CStatic m_staticPaxID;
	CListCtrl m_listTrack;

private:
	
	bool Init( Terminal* _pTerm, const CString& _strPath  );
	void InitCtrl( void );
	void LoadDataToList( void );
	void SaveTrackToFile( const CString& _strName );
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKWND_H__5BD5E1B3_DDD8_4F54_B2A2_F5414E9C5B91__INCLUDED_)
