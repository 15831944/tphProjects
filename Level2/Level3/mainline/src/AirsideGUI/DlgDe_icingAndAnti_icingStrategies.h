#pragma once
#include "NodeViewDbClickHandler.h"
class CAirportDatabase;

// CDlgDe_icingAndAnti_icingStrategies dialog
#include "../MFCExControl/CoolTree.h"
#include "../InputAirside/DeiceAndAnticeManager.h"
#include "../MFCExControl/XTResizeDialog.h"


enum TreeItemType
{
	NOTYPE = 0,

	FLIGHTTYPE,
	DEPARTURE_STAND,
//
	DE_NUMBER,
	DE_FLOWRATE,
//
	ANTI_NUMBER,
	ANTI_FLOWRATE,
//
	DEICE_PAD,
	PAD_QUEUE,
	PAD_WAITTIME,
	PAD_METHOD,
	PAD_STATE,
//
	DEP_STAND,
	DEPSTAND_WITHINTIME,
	DEPSTAND_FLUIDTIME,
	DEPSTAND_INSPECTIONTIME,
	DEPSTAND_STATE,

//
	LEADOUT_DISTIANCE,
	LEADOUT_WITHINTIME,
	LEADOUT_FLUIDTIME,
	LEADOUT_INSPECTIONTIME,
	LEADOUT_STATE,

	REMOTE_STAND,
	RESTAND_WITHINTIME,
	RESTAND_FLUIDTIME,
	RESTAND_INSPECTIONTIME,
	RESTAND_METHOD,
	RESTAND_STATE
};
struct CTreeNodeData
{
	CTreeNodeData()
		:dwData(0)
		,emType(NOTYPE)
	{

	}
	DWORD dwData;
	TreeItemType emType;
};

class CDlgDe_icingAndAnti_icingStrategies : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDe_icingAndAnti_icingStrategies)

public:
	CDlgDe_icingAndAnti_icingStrategies(int nProjID,CAirportDatabase* pAirportDB,PSelectFlightType pSelectFlightType,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDe_icingAndAnti_icingStrategies();

// Dialog Data
	enum { IDD = IDD_DIALOG_DE_ICINGANDANTI_ICINGSTRATEGIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

protected:
	void OnInitToolbar();
	void OnInitTreeCtrl();
	void SetTreeContent();
	void OnUpdateToolbar();

	void InitPrecipitonTypeItem(HTREEITEM hItem,CDeiceAndAnticePrecipitionType* pPrecipition);
	void InitSurfaceConditionItem(HTREEITEM hItem,CDeiceAndAnticeInitionalSurfaceCondition* pSurface);
	void InitVehiclesItem(HTREEITEM hItem,IceVehicles* pDeiceVehicles,IceVehicles* pAntiVehicles);
	void InitPriorityItem(HTREEITEM hItem,CDeiceAndAnticeInitionalSurfaceCondition* pSurface);
	void SetPriorityNodeContent(HTREEITEM hItem,PriorityType emType,CDeiceAndAnticeInitionalSurfaceCondition* pSurface);

	void CreatePositionMethod(HTREEITEM hItem);
	void CreateEngineState(HTREEITEM hItem);

	void HandlePositionMethod(HTREEITEM hItem);
	void HandleEngineState(HTREEITEM hItem);

	void DeletePriorityItems(HTREEITEM hItem);
	HTREEITEM GetPriorityItem(HTREEITEM hItem, int nPriority);
protected:
	afx_msg void OnAddFlightType();
	afx_msg void OnRemoveFlightType();
	afx_msg void OnEditFlightType();
	
	afx_msg void OnAddDepStand();
	afx_msg void OnRemoveDepStand();
	afx_msg void OnEditDepStand();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg	void ChangeDepStand();
	afx_msg void ChangeLeadOutLine();
	afx_msg void ChangeDeicePad();
	afx_msg void ChangeRemote();

	afx_msg void OnSave();
protected:
	CCoolTree m_wndTreeCtrl;
	int m_nProjID;
	CToolBar m_wndToolbar;

	PSelectFlightType	m_pSelectFlightType;
	CDeiceAndAnticeManager m_deiceAnticeManager;
	std::vector<HTREEITEM>m_vDepartureStand;
	std::vector<HTREEITEM>m_vDepStand;
	std::vector<HTREEITEM>m_vRemoteStand;
	std::vector<HTREEITEM>m_vDeicePad;
};
