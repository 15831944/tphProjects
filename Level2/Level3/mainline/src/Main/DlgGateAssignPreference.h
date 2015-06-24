#pragma once
#include "../Inputs/GateAssignmentMgr.h"
#include "../Inputs/GateAssignPreferenceMan.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgGateAssignPreference dialog
class CDlgGateAssignPreference : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgGateAssignPreference)
protected:
	enum NodeType{Gate_NODE , FLIGHT_NODE ,FLIHT_NODE_DURATION ,OVERLAp_NODE,OVERLAp_NODE_FLIGHT};
	class CNodeData
	{
	public:
		 CNodeData(void* PPoint ,NodeType _type):m_Data(PPoint),m_type(_type){} ;
	public:
		NodeType m_type ;
		void* m_Data ;
	};
public:
	CDlgGateAssignPreference(CGateAssignPreferenceMan* _PreferenceMan,InputTerminal* _terminal,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGateAssignPreference();

// Dialog Data
	enum { IDD = IDD_DIALOG_GATEASSIGN_PREFERENCE };

protected:
	std::vector<CNodeData*> m_NodeTypes ;
	CARCTreeCtrl m_TreeCtrl ;
	CToolBar m_ToolBar ;
	InputTerminal* m_terminal ;
protected:
	CGateAssignPreferenceMan* m_PreferenceMan ;
	CGateAssignmentMgr* m_pGateAssignmentMgr;
	CString m_Caption ;
	int m_Type ;
protected:
	void OnOK() ;
	void OnCancel() ;
protected:
	void OnNewButton() ;
	void OnEditButton() ;
	void OnDelButton() ;

	void OnEditButton(HTREEITEM _item = NULL) ;
protected:
	void NewGate() ;
	void NewGateFlight(CGateAssignPreferenceItem* _PreferenceItem) ;
	void NewOverLapFlight(CGateOverLapFlightTY* _OverFlight) ;

	void EditGate() ;
	void EditGateFlight() ;
	void EditOverLapFlight() ;
protected:
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
	void OnSize(UINT nType, int cx, int cy) ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	BOOL OnInitDialog() ;
	void InitToolBar() ;
protected:
	void InitTree() ;

	void AddPreferenceItem(CGateAssignPreferenceItem* _itemData) ;

	void AddPreferenceSubItem(CGatePreferenceSubItem* _itemData, HTREEITEM _ParItem) ;

	void AddOverLapItem(CGateOverLapFlightTYItem* _flightTy, HTREEITEM _ParItem) ;

	virtual CString FormatGateNodeName(ProcessorID& _id) = 0 ;
	CString FormatFlightNodeName(FlightConstraint& _flight) ;

private:
	bool FindGateReferenceTreeNode(CGateAssignPreferenceItem* pItem,HTREEITEM& hItem);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnTvnChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};
class CDlgArrivalGateAssignPreference : public CDlgGateAssignPreference
{
public:
      CDlgArrivalGateAssignPreference(CArrivalGateAssignPreferenceMan* _PreferenceMan,InputTerminal* _terminal,CWnd* pParent = NULL) ;
	  ~CDlgArrivalGateAssignPreference() {} ;
protected:
	  CString FormatGateNodeName(ProcessorID& _id) ;
};
class CDlgDepGateAssignPreference : public CDlgGateAssignPreference
{
public:
	CDlgDepGateAssignPreference(CDepGateAssignPreferenceMan* _PreferenceMan,InputTerminal* _terminal,CWnd* pParent = NULL) ;
	~CDlgDepGateAssignPreference() {} ;
protected:
	CString FormatGateNodeName(ProcessorID& _id) ;
};
