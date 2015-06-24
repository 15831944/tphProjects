#if !defined(AFX_DlgDBAirports_H__6ED65A8E_ABC9_4F46_907E_89406A90F551__INCLUDED_)
#define AFX_DlgDBAirports_H__6ED65A8E_ABC9_4F46_907E_89406A90F551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDBAirports.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDBAirports dialog
#include <MFCExControl/SplitterControl.h>
#include "DlgSectorProperties.h"
#include "afxwin.h"
#include "../MFCExControl/SortAndPrintListCtrl.h"
#include "CoolBtn.h"
class Terminal;
class CAirportsManager;
class CAirportDatabase ;
class CAirport ;


class CDlgDBAirports : public CDialog
{
// Construction
public:
	CDlgDBAirports(BOOL bMenu,Terminal* _pTerm, CWnd* pParent = NULL);   // standard constructor
	~CDlgDBAirports();

	void setAirportMan( CAirportsManager* _pAirportMan );
    void OnClickMultiBtn();
// Dialog Data
	//{{AFX_DATA(CDlgDBAirports)
	enum { IDD = IDD_AIRPORTDB };
	CStatic	m_cstLable;
	CButton	m_btnOk;
	CSortAndPrintListCtrl	m_lstAirports;
	CListBox	m_lstSectors;
	CSortAndPrintListCtrl	m_lstSectorMembers;
	CButton	m_btnCheck;

	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDBAirports)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel() ;


	afx_msg void OnDestroy();
	afx_msg void OnCtxNewAirport();
	afx_msg void OnCtxEditAirport();
	afx_msg void OnCtxNewSector();
	afx_msg void OnCtxAddAirports();
	afx_msg void OnCtxDeleteSector();
	afx_msg void OnCtxDeleteAirport();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnClickLstAirports(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelchangeListSectors();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBegindragLstAirports(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCheckFlightSchedule();

	DECLARE_MESSAGE_MAP()

protected:
	CString GetSelString(){ return m_csSelString; }
	void PopulateSectorList();
	void PopulateSectorMemberList(int _nItem);
	void PopulateAirportList();
	void AddAirportItem(CAirport* pAirport) ;
	void EditAirportItem(int selItem ,CAirport* pAirport);
	// Generated message map functions
	void FreeItemMemory();
    // Get an airport sector...
    bool GetSector(const char *pszAirport, char *pszSector, unsigned short usSize) const;
	void AddDraggedItemToList();
	void DoResize(int nDelta);

	afx_msg void OnLoadFromTemplate();
	afx_msg void OnSaveAsTemplate();

	
	void OnBnClickedButtonLoaddefaultAirport();

	CAirportDatabase*	m_pCurAirportDatabase;
	CAirportsManager*	m_pAirportMan;

	CCoolBtn m_button_Load;

private:
	Terminal*  m_pTerm;

	BOOL		m_CommitToDefault ;
	BOOL		m_Load_OPer ;
	BOOL		m_IsEdit ;
	BOOL		m_bMenu;
	BOOL		m_bDragging; 
	CString		m_csSelString;
	CPoint		m_ptLastMouse;
	int			m_nItemDragSrc;
	int			m_nItemDragDes;
	


	CImageList*   m_pDragImage;
	CSplitterControl m_wndSplitter;
	CImageList m_ilNodes;
	CToolBar m_wndToolBarBottom;
	CToolBar m_wndToolBarTop;
	CButton m_ButCancel;
};
class CDlgDBAirportsForDefault : public CDlgDBAirports
{
public:
	CDlgDBAirportsForDefault(BOOL bMenu,Terminal* _pTerm,CAirportDatabase* _theAirportDB, CWnd* pParent = NULL);
	~CDlgDBAirportsForDefault() ;
public:
	BOOL OnInitDialog() ;
	void OnOK() ;
	void OnCancel() ;
private:
	// use CDlgDBAirports::m_pCurAirportDatabase instead
// 	CAirportDatabase* m_pCurAirportDatabase ;

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgDBAirports_H__6ED65A8E_ABC9_4F46_907E_89406A90F551__INCLUDED_)
