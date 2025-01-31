#pragma once
#include "afxcmn.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "../Common/ACTypeDoor.h"
// DlgOperatingDoorSpecification dialog
class CAirportDatabase;
class OperatingDoorSpec;
class FltOperatingDoorData;
class StandOperatingDoorData;
class DlgOperatingDoorSpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgOperatingDoorSpecification)

public:
	DlgOperatingDoorSpecification( CAirportDatabase *pAirPortdb,  CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgOperatingDoorSpecification();

// Dialog Data
	enum { IDD = IDD_DLGOPERATINGDOORSPECIFICATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeOperatingdoor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();

	afx_msg void AddNewItem();
	afx_msg void RemoveItem();

	void InitTree();

	void AddNewFltItem();
	void AddNewStandItem(HTREEITEM hSelItem,FltOperatingDoorData* pFltData);
	void AddNewDoorItem(HTREEITEM hSelItem, StandOperatingDoorData* pStandData, ACTYPEDOORLIST* pACTypeDoors, const CString& strACType);

	void DelFltItem(HTREEITEM hSelItem);
	void DelStandItem(HTREEITEM hSelItem,FltOperatingDoorData* pFltData);
	void DelDoorItem(HTREEITEM hSelItem, StandOperatingDoorData* pStandData);

	void EditFltItem(HTREEITEM hSelItem);
	void EditStandItem(HTREEITEM hSelItem,FltOperatingDoorData* pFltData);
	void EditDoorItem(HTREEITEM hSelItem, StandOperatingDoorData* pStandData, ACTYPEDOORLIST* pACTypeDoors, const CString& strACType);	

private:
	CAirportDatabase* m_pAirportDatabase;
	CToolBar m_wndToolbar;
	CARCTreeCtrl m_wndTreeCtrl;
	OperatingDoorSpec* m_pOperatingDoorSpec;
};
