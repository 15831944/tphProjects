#if !defined(AFX_DLGDBAIRCRAFTTYPES_H__20FE1DED_630F_4101_ABC8_E9DE4C75A408__INCLUDED_)
#define AFX_DLGDBAIRCRAFTTYPES_H__20FE1DED_630F_4101_ABC8_E9DE4C75A408__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDBAircraftTypes.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDlgDBAircraftTypes dialog
#include <MFCExControl/SplitterControl.h>
#include "../MFCExControl/SortAndPrintListCtrl.h"
#include "afxwin.h"
#include "CoolBtn.h"
class Terminal;
class CACCategory;
class CACTypesManager;
class CACType ;
class CDlgDBAircraftTypes : public CDialog
{
// Construction
public:
	CDlgDBAircraftTypes(BOOL bMenu,Terminal* _pTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDBAircraftTypes() ;

	void setAcManager( CACTypesManager* _pAcMan );
	void OnClickMultiBtn();

// Dialog Data
	//{{AFX_DATA(CDlgDBAircraftTypes)
	enum { IDD = IDD_AIRCRAFTDB };
	CStatic			m_cstLable;
	CStatic			m_lblCategory;
	CStatic			m_lblMember;
	CButton			m_btnOk;
	CSortAndPrintListCtrl		m_lstACTypes;
	//CTabCtrl		m_tabCtrl;
	CSortAndPrintListCtrl		m_lstACCats;
	CListBox		m_lstCategory;
// 	CSortableHeaderCtrl m_HeaderCtrlACTypes;
// 	CSortableHeaderCtrl m_HeaderCtrlACCats;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDBAircraftTypes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
    virtual void OnCancel() ;


	void AddDraggedItem();
	void DoResize(int nDelta);
	void OnRelationship();
	void InitColumnName();
	void InitACTypeListHeader();
	void InitACatListHeader();
	void EditActypeItem(int selItem,CACType* pACT);
	
	void AddActypeItem(CACType* pACT);
	void PopulateACTypeList();
	void PopulateACCatList();
	void FreeItemMemory();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonLoaddefault();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLvnItemchangedListActypes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewACCat();
	afx_msg void OnNewACType();
	afx_msg void OnEditACType();
	afx_msg void OnEditACCat();
	afx_msg void OnDeleteACType();
	afx_msg void OnDeleteACCat();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBegindragListActypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClickListActypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickAcTypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickAcCats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelchangeListCategory();
	afx_msg void OnBnClickedCheckFlightSchedule();
	

	CAirportDatabase*	m_pCurAirportDatabase;
	CACTypesManager*	m_pAcMan;

	CCoolBtn	m_button_LoadDefault;
	CToolBar	m_wndToolBarTop;
	CToolBar	m_wndToolBarBottom;
	CButton		m_btnCheck;

	DECLARE_MESSAGE_MAP()

private:
	Terminal*			m_pTerm;
	BOOL	m_CommitOperation ;
	BOOL	m_LoadDefaultOperation ;
	BOOL	m_IsEdited;
	BOOL	m_bDragging;
	// comment by Benny:
	// NOTE: I don't know the real meaning of this variable
	// but it seems to be a flag for dialog to decide
	// whether load the ACTypes that not included in this Terminal's flight schedules
	BOOL	m_bMenu;
	std::vector<CString> m_vNameSplit;
	     
	CImageList*   m_pDragImage;
	int			  m_nItemDragSrc;
	HTREEITEM	  m_hItemDragDes;
	// void OnRelationship() ;
	/* void InitColumnName();
	 void InitACTypeListHeader();
	 void InitACatListHeader();*/
protected:

	CButton		m_buttonCancel;
	CPoint		m_ptLastMouse;
	CImageList	m_ilNodes;
	CSplitterControl m_wndSplitter;

protected:
	afx_msg void OnLoadFromTemplate();
	afx_msg void OnSaveAsTemplate();
};
class CDlgDBAircraftTypesForDefault : public CDlgDBAircraftTypes
{
public:
	CDlgDBAircraftTypesForDefault(BOOL bMenu,Terminal* _pTerm,CAirportDatabase* _theAirportDB, CWnd* pParent = NULL) ;
	virtual ~CDlgDBAircraftTypesForDefault() ;
public:
     virtual BOOL OnInitDialog();
	 virtual void OnOK() ;
	 virtual void OnCancel() ;

private:
//	// discarded by Benny, 2009-07-21, to use CDlgDBAircraftTypes::m_pCurAirportDatabase instead
// 	CAirportDatabase* m_pCurAirportDatabase ;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDBAIRCRAFTTYPES_H__20FE1DED_630F_4101_ABC8_E9DE4C75A408__INCLUDED_)
