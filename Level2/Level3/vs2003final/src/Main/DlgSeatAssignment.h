#pragma once


// CDlgSeatAssignment dialog
#include "../MFCExControl/CoolTree.h"
#include "../InputOnBoard/CSeatingAssignmentCabinData.h"
#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../Engine/terminal.h"
#include "DlgOnboardFlightList.h"
class InputOnboard;
enum NodeType
{
	MINPAX,
	MAXPAX,
	NUMOFPAX,
	NOTYPE
};
struct TreeNodeData
{
	TreeNodeData()
		:dwData(0)
		,emType(NOTYPE)
		,emSeatType(FREE_STEATING)
	{

	}
	DWORD dwData;
	NodeType emType;
	SeatAssignmentType emSeatType;
};
class CDlgSeatAssignment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSeatAssignment)

public:
	CDlgSeatAssignment(InputOnboard* pInputOnboard,Terminal* pTerm,int nProjID,FlightType flightType,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatAssignment();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEATASSIGNMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void InitToolBar();
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSave();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	void OnNewFlight();
	void OnDelFlight();
	bool FlightExist(COnboardFlight *flight);
	void OnSelChangeFlight();
	void ResetTreeContent();
	CSeatingAssignmentCabinData* getCurSeatingAss();
	
	DECLARE_MESSAGE_MAP()
public:
	void OnInitTreeCtrl();
	void OnInitFlightList();
protected:
	CToolBar m_toolBarFlight;
	CListBox m_listFlight;
	CCoolTree m_wndTreeCtrl;
	Terminal *m_pTerminal;
	int m_nProjID;
	InputOnboard* m_pInputOnboard;
	FlightType m_eFlightType;
	CSeatingAssignmentCabinList m_steatingAssignmentCabinList;
};
