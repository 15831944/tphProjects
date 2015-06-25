#pragma once
#include "afxcmn.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "../Common/ACTypeDoor.h"
#include "../MFCExControl/XTResizeDialog.h"
#include "../Inputs/OperatingDoorSpec.h"


// DlgOperatingDoorSpecification dialog
class CAirportDatabase;

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

	afx_msg void OnToolBarAdd();
	afx_msg void OnToolbarDel();
    afx_msg void OnToolbarEdit();

	void InitTree();

	void AddNewFltItem();
	void AddNewStandItem(HTREEITEM hSelItem,FltOperatingDoorData* pFltData);
	void AddNewDoorItem(HTREEITEM hSelItem, StandOperatingDoorData* pStandData, ACTYPEDOORLIST* pACTypeDoors, const CString& strACType);

	void DelFltItem(HTREEITEM hSelItem);
	void DelStandItem(HTREEITEM hSelItem,FltOperatingDoorData* pFltData);
	void DelDoorItem(HTREEITEM hSelItem, StandOperatingDoorData* pStandData);

    void SelectNextTreeItem(HTREEITEM hSelItem);
	void EditFltItem(HTREEITEM hSelItem);
	void EditStandItem(HTREEITEM hSelItem,FltOperatingDoorData* pFltData);
	void EditDoorItem(HTREEITEM hSelItem, StandOperatingDoorData* pStandData, ACTYPEDOORLIST* pACTypeDoors, const CString& strACType);	

	void DeleteTreeNodeData(StandOperatingDoorData::OperationDoor* pNodeData);
	void DeleteTreeNodeData(const StandOperatingDoorData::OperationDoor& doorOp);
	void ClearTreeNode();
	void DeleteTreeStandNode(StandOperatingDoorData* pStandData);
	void DeleteTreeFltNode(FltOperatingDoorData* pFltData);
private:
	CAirportDatabase* m_pAirportDatabase;
	CToolBar m_wndToolbar;
	CARCTreeCtrl m_wndTreeCtrl;
	OperatingDoorSpec* m_pOperatingDoorSpec;
	std::vector<StandOperatingDoorData::OperationDoor*> m_vDoorOp;
};
