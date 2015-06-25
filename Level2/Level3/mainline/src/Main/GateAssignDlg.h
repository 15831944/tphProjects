//{{AFX_INCLUDES()
#include "gtchart.h"
//}}AFX_INCLUDES
#if !defined(AFX_GATEASSIGNDLG_H__C34F893D_5262_4233_ADB1_0C0E31C701AB__INCLUDED_)
#define AFX_GATEASSIGNDLG_H__C34F893D_5262_4233_ADB1_0C0E31C701AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once 
#endif // _MSC_VER > 1000
// GateAssignDlg.h : header file
//

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "GannChartFrameWnd.h"	// Added by ClassView

#include "inputs\GatePriorityInfo.h"
#include "inputs\AllPaxTypeFlow.h"
#include "inputs\PaxFlowConvertor.h"
#include "../Engine/gate.h"
#include "algorithm"
#include "set" 
#include "afxwin.h"
#include "afxcmn.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CGateAssignDlg dialog

class CAirportDatabase;
class CGateAssignmentMgr;
class CGateAssignDlg : public CDialog
{
// Construction
public:
	enum GateDefineType{
		StandType,
		ArrGateType,
		DepGateType
	};
public:
	CGateAssignDlg(int nProjID,CAirportDatabase* pAirportDatabase, CWnd* pParent);   // standard constructor
	virtual ~CGateAssignDlg();

	int FreshGataData( int _nGateIndex );
	int FreshLinkLineData(int _nGateIndex);
	int FreshIntermediateGataData();	

	virtual void SetAssignedFlight() = 0;
	virtual GateDefineType GetGateDefineType() = 0;
	virtual void SetGate() = 0;
	virtual void SetUnassignedFlight();

	void UpdateRelativeData();
// Dialog Data
	//{{AFX_DATA(CGateAssignDlg)
	enum { IDD = IDD_DIALOG_GATEASSIGNMENT };
	CButton m_btnCheck;
	CButton	m_btnUnassignAll;
	CButton	m_btnSummary;
	CButton	m_btnFlightPriority;
	CButton	m_btnGatePriority;
	CButton	m_btnSave;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CListCtrl	m_listctrlFlightList;
	CGTChart	m_gtcharCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGateAssignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSortableHeaderCtrl	m_ctlHeaderCtrl;	
	CImageList* m_pDragImage;
	CGateAssignmentMgr* m_pGateAssignmentMgr;

	int m_nPrjID;


	void SortFlightOperation( std::vector<CFlightOperationForGateAssign*>& vOperations);
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	
	// after every change, reload all the assigned gate information.

	// just load the gate lable.
	void OnDefineArrGate();
	void OnDefineDepGate();
	//use to Upadate Detail Gate Flow (only temp)
	CPaxFlowConvertor m_paxFlowConvertor;
	CAllPaxTypeFlow m_allFlightFlow;
	BOOL InitFlowConvetor();
	void UpdateFlow();
	///////////////////////////////////////
	
	// Generated message map functions
	//{{AFX_MSG(CGateAssignDlg)
	afx_msg void OnClickListFlight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBegindragListFlight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnColumnclickListFlight(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnClickBtnAddItemGtchartctrl(BOOL bSuccess, long nLineIndex);
	afx_msg void OnClickBtnDelItemGtchartctrl(BOOL bSuccess, long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData);
	afx_msg void OnDblClickItemGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData);
	afx_msg void OnItemResizedGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration);
	afx_msg void OnItemMovedGtchartctrl(long nLineIndexFrom, long nLineIndexTo, long nItemIndex, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration);
	afx_msg void OnCheckFlightOperationMoveGtchartctral(long nLineIndexFrom, long nLineIndexTo,long nItemIndex,long FAR* pnLineData);
	afx_msg void OnRClickGtchartctrl(long nItemIndex, long nLineIndex);
	afx_msg void OnShowItemToolTipGtchartctrl(long nLineIndex, long nItemIndex, long timeCurrent);

	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnDefineArrGate( UINT nID );
	afx_msg void OnDefineDepGate( UINT nID );
	DECLARE_MESSAGE_MAP()

protected:
	BOOL BuildPopMenuByNumber(CMenu* _pArrMneu, CMenu* _pDepMenu);
	BOOL BuildPopMenuByLevel( CMenu*_pArrMenu, CMenu *_pDepMenu);
	bool InitRClickMenu();
	int DefineArrDepGate( UINT nID, bool _bArrGate);
	int getIndexInAllGateList( const CString &_strGate,GateType emGateType);//get index in arrival vector or in departure vector
	GateType IfIndexInArrGateList(const CString& _strGate);//get gate type in arrival vector and departure vector
	bool deleteAllNewMenu();
	BOOL BuildPopMenu(CMenu* _pArrMenu, CMenu* _pDepMenu );
	void InitGTCtrl();
	virtual void InitFlightListCtrl();
	int GetIndexInGateList( const CString& _strGate );
	void ClearFlightInGtChart(CFlightOperationForGateAssign* flightOp);
	int GetAllGateProcessor();
	bool DisplayDrageErrorMessage(long lIdx,CString& strError);
	bool DisplayGateAssignErrorMessage(long Idx, CString& strError);
	void InsertGateAssignment(long nLineIndexTo,CAssignGate* pGate,CFlightOperationForGateAssign* flightOp);

	CImageList m_imageList;
	int m_nDragIndex;
	bool m_bDragging;
	int m_nLineNum;	
	std::vector<CString> m_vectAllGateName;
	std::vector<CString> m_vectArrGateName;//arrival gate name vector
	std::vector<CString> m_vectDepGateName;//departure gate name vector

	//////////////////////////////////////////////////////////////////////////
	// for rclick menu
	CMenu m_menuRClick;
	CMenu* m_pmenuSub;
	std::vector<CMenu*> m_vectAllNewMenu;

	void CheckGateInPopMenu( long lLine, long lItem, CMenu* _pPopMenu );
	void CheckMenuItem( CMenu* _pSubMenu, const CString& _strGate, bool _bClearAllCheck = false );

private:
	//void AssignUnAssignFlightOperationAccordingToGatePriority();	// according to gate priority assign flight operation
	//void AssignUnAssignFlightOperationAccordingToFlightPriority();	// according to flight priority assign flight operation
public:
	CProgressCtrl m_progAssignStatus;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATEASSIGNDLG_H__C34F893D_5262_4233_ADB1_0C0E31C701AB__INCLUDED_)
