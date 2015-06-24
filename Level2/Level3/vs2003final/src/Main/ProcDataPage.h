#if !defined(AFX_PROCDATAPAGE_H__1EBC5CBD_283C_4586_8642_E37E22B55BFE__INCLUDED_)
#define AFX_PROCDATAPAGE_H__1EBC5CBD_283C_4586_8642_E37E22B55BFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcDataPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProcDataPage dialog


#include "..\inputs\in_term.h"
#include "..\inputs\procdata.h"
#include "..\inputs\AllPaxTypeFlow.h"
#include "..\inputs\PaxFlowConvertor.h"
#include "ResizePS\ResizablePage.h"
#include <MFCExControl/SplitterControl.h>
#include "ColorTreeCtrl.h"
#include "..\MFCExControl\CoolTree.h"
#include "resource.h"



#include <algorithm>

class CProcDataPage : public CResizablePage
{
	DECLARE_DYNCREATE(CProcDataPage)

// Construction
public:
	InputTerminal* GetInputTerm();
	void SetSplitterRange();
	void DoResize(int nDelta);
	CSplitterControl m_wndSplitter;
	void InitSpiltter();
	void ClearControl();
	CProcDataPage();
	CProcDataPage( enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath );
	~CProcDataPage();

// Dialog Data
	//{{AFX_DATA(CProcDataPage)
	enum { IDD = IDD_PROPPAGE_PROCDATA };

	CStatic	m_staticLinkage;


	CCoolTree	m_TreeData;
	CStatic	m_staticLabelGate;
	CListBox	m_listboxGate;
	CListBox	m_listboxProcessor;
	CListBox	m_listOtherProc;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProcDataPage)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	bool SelectLinkedDest(ProcessorID _id);
	void ReloadLinkedDestList( MiscData* _pMiscData );
//	virtual bool OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	void ReloadStationList(MiscData* _pMiscData);
	bool SelectPSS(ProcessorID _id);
	void DelPSS();
	void AddPSS();
	HTREEITEM m_hScheduledPickedUp;
	HTREEITEM m_hRandomAmountOfTimePasses;
	HTREEITEM m_hHoldingAreaOfLoader;
	HTREEITEM m_hExitBeltOfSorter;
	HTREEITEM m_hCapacity;
	HTREEITEM m_hAutoCapacity;
	HTREEITEM m_hCapacityFather;
	HTREEITEM m_hItemSpeed;
	HTREEITEM m_hDecSpeedItem;
	HTREEITEM m_hNormalSpeedItem;
	HTREEITEM m_hAccSpeedItem;
	HTREEITEM m_hSpeed;
	HTREEITEM m_hEscalatorCap;
	HTREEITEM m_hEscalatorSpeed;
	HTREEITEM m_hStopAtFloor;
	HTREEITEM m_hItemLoader;
	HTREEITEM m_hItemSimpleConveyor;
	HTREEITEM m_hItemUnitBelt;
	HTREEITEM m_hItemMergeBox;
	HTREEITEM m_hItemFeeder;
	HTREEITEM m_hItemScanner;
	HTREEITEM m_hItemSplitter;
	HTREEITEM m_hItemPusher;
	HTREEITEM m_hItemFlowBelt;
	HTREEITEM m_hItemSorter;
	HTREEITEM m_hConveyorIsFull;
	HTREEITEM m_hNextProcAvail;
	HTREEITEM m_hReversible;
	HTREEITEM m_hOtherCondition;
	HTREEITEM m_hPSS;
	HTREEITEM m_hCurrentSelItem;
    HTREEITEM m_hSumOfGroupSerTimeMod;
	HTREEITEM m_hAverageOfGroupSerTimeMod;
	HTREEITEM m_hMinimumOfGroupSerTimeMod;
	HTREEITEM m_hMaximumOfGroupSerTimeMod;
	HTREEITEM m_hRandomOfGroupSerTimeMod;
	HTREEITEM m_hConnectBridge;
	HTREEITEM m_hStandConnect;
		

	void DelWaitArea();
	void AddWaitArea();
	void ReloadWaitAreaList( MiscData* _pMiscData, bool _bReloadLoaderHolding = false );
	bool SelectWaitArea(ProcessorID _id);
	HTREEITEM m_hWaitArea;
	HTREEITEM m_hBCSN;
	void SetItemTextEx(HTREEITEM hItem,COOLTREE_NODE_INFO* pInfo,CString strText,DWORD dwValue);
	void SetEditValueRange(HTREEITEM hItem);
	void SaveDataForPopulationCapacity();
	void SaveDataForTerminateTime();
	void SaveDataForDisallowGroups();
	void SaveDataForWaitInQueue();
	void SaveDataForApplyServiceTimeAfterGreeting();
	float GetValueTreeItem(HTREEITEM hItem);
	BOOL IsCheckTreeItem(HTREEITEM hItem);



	CImageList m_ilTreeData;
	HTREEITEM m_hRecycleFreq;
	HTREEITEM m_hGroupSerTimeMod;
	HTREEITEM m_hCapOrDR;
	void InsertTreeForBehavior();
	HTREEITEM m_hPopulationCapacity;
	HTREEITEM m_hTerminateTime;
	HTREEITEM m_hDisallowGroups;
	HTREEITEM m_hOtherBehavior;
	HTREEITEM m_hLinkage;
	HTREEITEM m_hWaitInQueue;

	void AddDependentProcessors();
	void DeleteDependentProcessors(HTREEITEM hItem);
	void ReloadDependentProcessorList( MiscData* _pMiscData );
	bool SelectDependentProcessor(ProcessorID _id);
	HTREEITEM m_hDependentProcessors;

	//------------- Proc Entry Related Functions -------------------------------	
	void LoadProcessorList();


	//------------------------Capacity----------------------------
	void ReloadCapacityList();
	HTREEITEM m_hTreeItemCapacity;
	std::map<HTREEITEM,int> m_mapMobTreeItemMobElement;
	
	// holding area's greeting time
	HTREEITEM m_hApplyServiceTimeAfterGreeting;

	//hold are service full
	HTREEITEM m_hCorralService;
	HTREEITEM m_hTimeToCorralService;

	CString GetMobileElementString(int nIndex);
	void DeleteCapacityItem(HTREEITEM hItem);
private:
	void DelLinkedDest();
	void AddWaitAreaForLoader();
	void DelWaitAreaForLoader();
	void SetCapacityValue( int _iValue );	
// Implementation
protected:
	void DelGreetingHall();
	void ReloadHoldArerList ( MiscData* _pMiscData );
	bool SelectGreetingHall(ProcessorID _id);
	void AddGreetingHall();
	void ReloadGrouSerTimeMod( MiscData *_pMiscData );
	void ReloadExitBeltList( MiscData* _pMiscData );
	void SetToolBarState( const HTREEITEM hItem );	
	void DelExitBeltOfSorter();
	void AddExitBeltOfSorter();
	void AddLinkedDest();
	HTREEITEM m_hGateItem;
	HTREEITEM m_hProc1Item;
	HTREEITEM m_hLinkedDestItem;
	HTREEITEM m_hGreetingHall;
	int m_nProcDataType;
	CToolBar m_ToolBar;
	CToolBar m_ToolBarLeft;
	void DeleteAllChildItems(HTREEITEM _hParentItem);
	virtual void ReLoadDetailed();
	void ReloadGateList( MiscData* _pMiscData );
	virtual void ReloadLinkProc1List( MiscData* _pMiscData );
	void ReloadLinkProc2List( MiscData* _pMiscData );
	void AddGate();

	// Hightlight the gate in the list. if not exist, return false.
	bool SelectGate( ProcessorID _id );	
	bool SelectStand(const ProcessorID& _id);
	void DelGate();

	void InitToolbar();	
	
	void DelPro1();
	virtual void AddPro1();
	virtual void SetTree();

	virtual void InitSpecificBehvaior();

	void SetControl();
	bool SelectExitBelt(ProcessorID _id);
	
	// Hightlight the link proc in the list. if not exist, return false.
	bool SelectLinkProc1(ProcessorID _id);
	bool SelectLinkProc2(ProcessorID _id);

	bool SelectWaitAreaForLoader(ProcessorID _id);		
	
	// base the cur selection of the proc list. return the point to MISCDATA
	// return NULL for no selection on the proc list.
	MiscData* GetCurMiscData();

	//------------- Link Proc Functions -------------------------------
	ProcessorIDList* GetLinkProc1List( MiscData* _pMiscData );



	//use to Upadate Detail Gate Flow (only temp)
	CPaxFlowConvertor m_paxFlowConvertor;
	CAllPaxTypeFlow m_allFlightFlow;
	BOOL InitFlowConvetor();
	void UpdateFlow();
	///////////////////////////////////////
	
	
	CString m_csTreeItemLabel;
	InputTerminal* m_pInTerm;
	CString m_csProjPath;

	// Hightlight the proc in the list. if not exist, return false.
	bool SelectEntryProc(ProcessorID _id);



	//------------- Gate Related Functions -------------------------------

	void AddBridgeConnector();
	void DelBridgeConnector(HTREEITEM hItem);
	bool SelectBridgeConnector(ProcessorID _id);
	void ReloadBridgeConnectorList( MiscData* _pMiscData );


	//Bridge connector
	void AddStandConnector();
	void DelStandConnector(HTREEITEM hItem);
	bool SelectStandConnector(ProcessorID _id);
	void ReloadStandConnectorList(MiscData* _pMiscData);
	


	//----------------- Value Functions -------------------------------
	virtual void SetValue( MiscData* _pMiscData );
	void SetSpinRange();



	// Set Conveyor Type in Misc Data;
	void SetConveyorType( HTREEITEM _hItem );
		

	// Generated message map functions
	//{{AFX_MSG(CProcDataPage)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelchangeListProcessor();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual afx_msg void OnToolbarbuttonadd();
	virtual afx_msg void OnToolbarbuttondel();
	virtual afx_msg void OnSelchangedTreeData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLinkageOnetoone();
	virtual afx_msg void OnEditOrSpinValue();
	virtual afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSelectMobileElementType(UINT nID);
	void OnSelAllNoPax();
	void OnSelCombination();
	void DeleteCapacityCombinationItem(HTREEITEM hItem,HTREEITEM hSunItem);
	protected:
		HTREEITEM hCurrentTreeItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCDATAPAGE_H__1EBC5CBD_283C_4586_8642_E37E22B55BFE__INCLUDED_)
