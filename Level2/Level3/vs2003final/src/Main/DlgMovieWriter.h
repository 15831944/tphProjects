#if !defined(AFX_DLGMOVIEWRITER_H__79144CD9_B904_4122_BFEC_388A91B7AFEA__INCLUDED_)
#define AFX_DLGMOVIEWRITER_H__79144CD9_B904_4122_BFEC_388A91B7AFEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// DlgMovieWriter.h : header file
//
class CTermPlanDoc;
class CMovieWriter;
class CWalkthrough;
class CMovie;
class CVideoParams;
class PanoramaViewData;
class CWalkthrough;
class IMovieReader;
/////////////////////////////////////////////////////////////////////////////
// CDlgMovieWriter dialog

class CDlgMovieWriter : public CDialog
{
// Construction
public:
	CDlgMovieWriter(CTermPlanDoc* pDoc, int nType, int nIdx, CWnd* pParent = NULL);   // standard constructor
	void DoRecord();

	CWalkthrough* GetWalkthrough() const { return m_pWalkthrough; }
	void SetWalkthrough(CWalkthrough* pWalkthrough) { m_pWalkthrough = pWalkthrough; }

	static void BuildMovieFromWalkthrough(const CWalkthrough* pWT, CTermPlanDoc* pDoc, CMovie* movie);
	static void BuildMovieFromPanorama(const PanoramaViewData* pPD, CTermPlanDoc* pDoc, CMovie* movie);
// Dialog Data
	//{{AFX_DATA(CDlgMovieWriter)
	enum { IDD = IDD_DIALOG_MOVIEWRITER };
	CProgressCtrl	m_progressMW;
	CString	m_sText;
	//}}AFX_DATA

	enum { 
		MOVIE = 0, 
		WALKTHROUGH,
		LIVE,
		PANORAMA,
	};

	int DoFakeModal();
	void SetPanoramData(PanoramaViewData* pData){ m_pPanoramViewData = pData; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMovieWriter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void RecordMovie(const CMovie* pMovie, const CVideoParams& videoparams, long starttime, long endtime);

// Implementation
protected:
	PanoramaViewData* m_pPanoramViewData;
	//CMovieWriter* m_pWriter;
	CWalkthrough* m_pWalkthrough;
	CTermPlanDoc* m_pDoc;
	CWnd* m_pParent;
//	BYTE* m_pBits;
	BOOL m_bFirstPaint;
	BOOL m_bIsCancel;
	int m_nMovieIdx;
	int m_nType;

	void RecordLive();
	void RecordMovie();
	void RecordWalkthrough();
	void RecordWalkthroughRender();
	void RecordPanorama();
	void RecordPanoramaRender();
	

	IMovieReader* CreateMovieReader();

	CString GetMovieFileName() const;

	// Generated message map functions
	//{{AFX_MSG(CDlgMovieWriter)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOVIEWRITER_H__79144CD9_B904_4122_BFEC_388A91B7AFEA__INCLUDED_)
