#pragma once
#include "NodeViewDbClickHandler.h"
#include "..\MFCExControl\CoolTree.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "InputAirside\takeoffsequencing.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgTakeoffSequence dialog
class TakeoffSequencing;
class TakeoffSequencingList;
class CAirportDatabase;
class CPriorityData;
class CDlgTakeoffSequence : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTakeoffSequence)

public:
	CDlgTakeoffSequence(int nProjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTakeoffSequence();

// Dialog Data
	enum { IDD = IDD_TAKEOFFSEQUENCE };
	enum TreeNodeType
	{
		TREENODE_TIME = PriorityCount,
		TREENODE_PRIORITY,
		TREENODE_CHANGE_CONDITION,
		TREENODE_CROSS,
		TREENODE_ENCROACH,
		TREENODE_TAKEOFF,
		TREENODE_FLIGHTTYPE
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void SetAirportDB(CAirportDatabase* pDB);
protected:
	CToolBar	m_wndSequenceToolbar;
	CARCTreeCtrl	m_wndTreeSequence;
	HTREEITEM  m_hTakeOffPositions;
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMsgNewSequence();
	afx_msg void OnMsgDelSequence();
	afx_msg void OnMsgEditSequence();
	void ReadData();
	void InitSequenceTreeCtrl();
	void UpdateState();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	int m_nProjID;
	TakeoffSequencingList* m_pTakeoffSequencingList;
	PSelectFlightType m_SelFlightType ;
	CAirportDatabase* m_AirportDB ;
public:
	afx_msg void OnTvnSelchangedTreeTakeoffsequence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void InitChangeConditionItem(HTREEITEM _PriorityItem,const CPriorityData& _priorityData);
	void InitFlightTyItem(HTREEITEM _PriorityItem,const CPriorityData& _priorityData);
	CString FormatCrossNodeCstring(const CPriorityData& _priorityData);
	CString FormatEnCroachNodeCSring(const CPriorityData& _priorityData);
	CString FormatTakeoffNodeCSring(const CPriorityData& _priorityData);
};
