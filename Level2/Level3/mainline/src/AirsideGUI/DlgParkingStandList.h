#pragma once
//#include "afxcmn.h"
#include "Resource.h"
#include "common/ALTObjectID.h"
#include "..\MFCExControl\CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"

class ALTObject;
class CDlgParkingStandList : public CXTResizeDialog
{


	DECLARE_DYNAMIC(CDlgParkingStandList)

	enum { IDD=IDD_DIALOG_OCCUPIEDASSIGNEDSTAND_ORDER };

public:
	typedef std::vector<int> StandList;

	CDlgParkingStandList(const UINT nID,int nPrjID,StandList &standList, CWnd* pParent = NULL);
	virtual ~CDlgParkingStandList();


	StandList& GetStandList();
 	void SetStandOrderContent(void);
	void addList(StandList &standList);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddList();
	afx_msg void OnDelStand();
	afx_msg void OnStandUp();
	afx_msg void OnStandDown();
	afx_msg void OnTvnSelchangedTreeStand(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);



protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();

	virtual void OnOK();

	CToolBar m_toolBarStand;
	CCoolTree m_treeStandOrder;
	HTREEITEM m_hTreeRoot;

	StandList m_vStandList;
	int m_nPrjID;
};


