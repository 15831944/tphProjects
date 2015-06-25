#pragma once

#include "../MFCExControl/ListCtrlEx.h"
#include "../Common/FLT_CNST.H"
#include "NodeViewDbClickHandler.h"
#include "../InputAirside/FlightTypeRestriction.h"
#include "../InputAirside/FlightTypeRestrictionList.h"
#include "../MFCExControl/ComboBoxListBox.h"
#include "../Common/WinMsg.h"
#include "../InputAirside/HoldPosition.h"
#include "../MFCExControl/listctrlex.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgTaxiwayFlightTypeRestriction dialog

class CDlgTaxiwayFlightTypeRestriction : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTaxiwayFlightTypeRestriction)

public:
	CDlgTaxiwayFlightTypeRestriction(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaxiwayFlightTypeRestriction();

// Dialog Data
	enum { IDD = IDD_DIALOG_TAXIWAY_FLIGHT_TYPE_RESTRICTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	int m_nProjID;
	CListCtrlEx m_MyList;
	CToolBar m_MyToolBar;
	std::vector<int> m_vTaxiwaysIDs;
	std::vector<IntersectionNode> m_vNodeList;
	CStringList m_strListTaxiway;
	CStringList m_strListNode;
	int m_nTaxiwayCount;
	int m_nCurNodeCount;
	int m_nCurFirtNodeID;

public:
	PSelectFlightType	       m_pSelectFlightType;	
	CFlightTypeRestrictionList *m_pFlightTypeRestrictionList;
	CFlightTypeRestriction     *m_pFlightTypeRestriction;
public:
	virtual BOOL OnInitDialog(void);
	void InitListCtrlEx();
	void SetListCtrlEx(BOOL bReset);
	void InitToolBar();
	void ResizeDlg();
	void UpdateToolBar();
	BOOL LoadListData();
	void GetAllTaxiway();
	void GetNodeListByTaxiwayID(int nTaxiwayID);
	BOOL IsSamePos();

	afx_msg void OnNewFlightType();
	afx_msg void OnDelFlightType();
	afx_msg void OnEditFlightType();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
};
