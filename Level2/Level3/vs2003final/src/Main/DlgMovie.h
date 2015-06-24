#ifndef __DLGMOVIE_H__
#define __DLGMOVIE_H__

#pragma once

// DlgMovie.h : header file
//
#include "resource.h"
#include "FrameCtrl.h"
#include "InPlaceComboBoxEx.h"

class CTermPlanDoc;
class C3DView;
class IRender3DView;


enum ANIMSPEEDPRESETS {
	_1X = 0,
	_5X,
	_10X,
	_25X,
	_50X,
	_100X,
};


/////////////////////////////////////////////////////////////////////////////
// CMovieListCtrl
#define IDC_BUTTON_DOWNLIST 0

class CMovieListCtrl : public CListCtrl
{
// Construction
public:
	CMovieListCtrl();

// Attributes
public:
	CTermPlanDoc* m_pDoc;

protected:
	int m_nAnimSpeedSelItem;
	CInPlaceComboBoxEx m_AnimSpeedComboBox;

// Operations
public:

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxDispListCtrl)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InsertMovie( int _nIdx ); //_nIdx is index of movie in pDoc->m_movies
	virtual ~CMovieListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPaxDispListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDlgMovie dialog

class CDlgMovie : public CDialog
{
// Construction
public:
	CDlgMovie(CTermPlanDoc* pTPDoc, CWnd* pParent = NULL);   // standard constructor

	virtual ~CDlgMovie();

// Dialog Data
	//{{AFX_DATA(CDlgMovie)
	enum { IDD = IDD_DIALOG_MOVIE };
	CFrameCtrl m_ctrlKeyFrames;
	CStatic	m_toolbarcontenter;
	CButton	m_btnBarFrame;
	CMovieListCtrl	m_lstMovies;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMovie)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTermPlanDoc* m_pDoc;
	IRender3DView*  GetCur3DView();

	CToolBar m_ToolBar;

	BOOL m_bNewMovie;
	BOOL m_bAutoGoto;

	long m_nSelectedTime;
	//int m_nSelectedKF;
	int m_nSelectedMovie;

	BOOL m_bRequestCancel;

	//HBITMAP m_hMarkerBitmap;

	void Initialize();

	void UpdateMovieList();
	void UpdateKeyFrames();

	void SelectTime(long _time);

	// Generated message map functions
	//{{AFX_MSG(CDlgMovie)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnAddMovie();
	afx_msg void OnDeleteMovie();
	afx_msg void OnRecordMovie();
	afx_msg void OnPreviewMovie();
	afx_msg void OnPreviewMovieWithAnimation();
	afx_msg LRESULT OnInsertKF(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteKF(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectTime(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMoveKF(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGetDispInfoListMovies(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditListMovies(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedListMovies(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ifndef __DLGMOVIE_H__
