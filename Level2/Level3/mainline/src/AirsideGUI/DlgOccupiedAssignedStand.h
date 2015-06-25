#pragma once
#include "NodeViewDbClickHandler.h"
#include "..\InputAirside\OccupiedAssignedStandAction.h"
#include "..\MFCExControl\CoolTree.h"
#include "afxcmn.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgOccupiedAssignedStand dialog

class InputAirside;
class CAirportDatabase;
class CAirportGroundNetwork; 
class CDlgOccupiedAssignedStand : public CXTResizeDialog
{
public:
// 	enum CriteriaNodeType
// 	{
// 		EXITS_TYPE=1,
// 		PARKINGSTANDS_TYPE,
// 		FLIGHTTYPE_TYPE,
// 		TIMEWINDOW_TYPE,
// 		STRATEGY_TYPE,
// 
// 		DELAYTIME_TYPE,
// 		TODYNAMICALSTAND_TYPE,
// 		TOINTERSECTION_TYPE,
// 		TOTEMPORARYPARKING_TYPE,
// 		TOSTAND_TYPE
// 		
// 	};
	enum CriteriaLevel
	{
		LEVEL_EXIT,
		LEVEL_PARKINGSTAND,
		LEVEL_FLIGHTTYPE,
		LEVEL_TIMEWINDOW,
		LEVEL_STRATEGY,
		LEVEL_STRATEGYDETAIL
	};
 	typedef std::vector<std::pair<CString,int> > AltObjectVector;
 	typedef std::vector<std::pair<CString,int> >::iterator AltObjectVectorIter;
// 
 	typedef std::map<CString, AltObjectVector> AltObjectVectorMap;
 	typedef std::map<CString, AltObjectVector>::iterator AltObjectVectorMapIter;
	DECLARE_DYNAMIC(CDlgOccupiedAssignedStand)

public:
	CDlgOccupiedAssignedStand(int nProjID, PFuncSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOccupiedAssignedStand();

// Dialog Data
	enum { IDD = IDD_DIALOG_OCCUPIEDASSIGNEDSTANDCRITERIA };
private:
	int m_nProjID;
	CToolBar m_toolbarCriteria;
	CCoolTree m_TreeCriteria;
	CButton btSave;
	CButton btOk;
	CButton btCancel;
	bool m_bWindowClose;
	PFuncSelectFlightType  m_pSelectFlightType;

	COccupiedAssignedStandCriteria m_OccupiedAssignedStandCriteria;

 	AltObjectVectorMap m_TaxiwayVectorMap;
 	AltObjectVectorMap* m_pTaxiwayVectorMap;
	AltObjectVectorMap m_StandVectorMap;
	AltObjectVectorMap* m_pStandVectorMap;
	AltObjectVectorMap m_TemporaryParkingVectorMap;
	AltObjectVectorMap* m_pTemporaryParkingVectorMap;
// 
 	CAirportDatabase* m_pAirportDB;
	CAirportGroundNetwork* m_pAltNetwork;
	std::vector<Taxiway> m_vectTaxiway;
// 	HTREEITEM m_hSelItem;
	
private:
 	void InitToolBar();
	void ReloadTree();
	HTREEITEM addExitsItem(CAirSideCriteriaExits *exits);
	HTREEITEM addParkingStandsItem(HTREEITEM parentItem,CAirSideCriteriaParkingStands *parkingStands);
	HTREEITEM addFlightTypeItem(HTREEITEM parentItem,CAirSideCriteriaFlightType *flightType);
	HTREEITEM addTimeWinItem(HTREEITEM parentItem,CAirSideCreteriaTimeWin *timeWin);

// 
 	void GetTaxiwayMap();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg	void OnContextMenu(CWnd* pWnd, CPoint point);


	DECLARE_MESSAGE_MAP()
	afx_msg void OnNewExits();
	afx_msg void OnNewParkingStands();
	afx_msg void OnNewFlightType();
	afx_msg void OnNewTimeWindow();
	afx_msg void OnStrategyUp();
	afx_msg void OnStrategyDown();
	afx_msg void OnDelButton();

	void GetTemporaryParkingMap();
	int getSelItemLevel();
	int getItemIndex(HTREEITEM selItem);
	void exchangeStrategyItem(HTREEITEM &item1,HTREEITEM &item2);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLbnSelchangeListFlttype();
	afx_msg void OnBnClickedSave();
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	
protected:
	virtual void OnOK();
	
	

	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedListOccupiedPriority(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListOccupiedPriority(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
};
