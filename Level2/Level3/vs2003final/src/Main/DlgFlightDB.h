#pragma once
#include "../Common/FlightManager.h"
// CDlgFlightDB dialog
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\MFCExControl\SortAndPrintListCtrl.h"
#include "afxwin.h"
#include "CoolBtn.h"
class CDlgFlightDB : public CDialog
{
	DECLARE_DYNAMIC(CDlgFlightDB)

public:
	CDlgFlightDB(int nProjID,Terminal* _pTerm,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFlightDB();
	CSortAndPrintListCtrl	m_listCtrlSchdList;
	CSortAndPrintListCtrl m_wndListCtrl;
	CListBox m_wndListBox;
	CToolBar m_wndToolBar;
	int m_nRClickItem;
	int m_nProjID;
    BOOL m_IsEdit ;
	BOOL m_loadOpr ;
	int m_nItemDragSrc;
	int m_nItemDragDes;
	CImageList*   m_pDragImage;
	BOOL m_bDragging;
	CPoint		m_ptLastMouse;

// Dialog Data
	enum { IDD = IDD_FLIGHTDB };

public:
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLbnSelchangeListGroups();
	afx_msg void OnToolbarAdd();
	afx_msg void OnToolbarEdit();
	afx_msg void OnToolbarDel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedExport();
	afx_msg void OnBegindragLstFlight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLoadFromTemplate();
	afx_msg void OnSaveAsTemplate();
	DECLARE_MESSAGE_MAP()
public:
	FlightManager* m_pFlightMan;
	Terminal* m_pTerm;
	std::vector<FlightGroup::CFlightGroupFilter* > m_vFilter;
	int AppendOneFlightIntoListCtrl( const Flight* _pFlight ,int _nIndex);
	void SetFlightInListCtrl( int _nIndex, const Flight& _flight );
	void SetFlightSelected(int _nIndex);
	void LoadFlight(Flight* _pSelectFlight/*=NULL*/);
	void PopulateGroupsList();
	void OnInitToolbar();
	void AddDraggedItemToList();
	void PopulateFlightList(int _nItem);
	void LoadFromTemplateDatabase();
	void OnClickMultiBtn();
	void ReadData();
	void SaveData();
protected:
	CCoolBtn m_Load;
public:
	afx_msg void OnLvnItemchangedListFlightData(NMHDR *pNMHDR, LRESULT *pResult);
};
