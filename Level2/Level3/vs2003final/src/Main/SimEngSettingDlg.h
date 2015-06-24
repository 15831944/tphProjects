#if !defined(AFX_SIMENGSETTINGDLG_H__F3D0A78B_6D19_4892_89C4_305F03E8EF2C__INCLUDED_)
#define AFX_SIMENGSETTINGDLG_H__F3D0A78B_6D19_4892_89C4_305F03E8EF2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimEngSettingDlg.h : header file
//
#include "..\inputs\in_term.h"

/////////////////////////////////////////////////////////////////////////////
// CSimEngSettingDlg dialog
#include "..\MFCExControl\CoolTree.h"
#include "ProcesserDialog.h"
#include "reports\SimLevelOfService.h"
#include "simTimeRangeDlg.h"
#include "../InputAirside/SimSettingClosure.h"

class OnboardSimSetting;
class LandsideSimSetting;
class LandsideClosure;
class LandsideTimeRange;
class LandsideFacilityLayoutObjectList;
class TimeStepRange;

class CSimEngSettingDlg : public CDialog
{
// Construction
	void SetTreeData();
	void SetGeneralPara( CSimParameter* pSP );
public:

	HTREEITEM m_hTerminal;
	HTREEITEM m_hAirside;
	HTREEITEM m_hLandside;
	HTREEITEM m_hOnboard;

	BOOL m_bAutoCreateProc;
	HTREEITEM m_hAutoCreateProc;;
	HTREEITEM m_hTLOSProcessor;
	HTREEITEM m_hPropogationParams;
	HTREEITEM m_hLocation;
	HTREEITEM m_hFireDrop;
	void AddPipeToTree(HTREEITEM hItem);
	HTREEITEM m_hCongestionImpact;
	void AddAreaToTree(HTREEITEM hItem);
	void ShowSimName(){ m_bShowSimName = true;	}
	CString GetSimName(){ return m_stSimName;	}
	bool GetMaxValueFromTreeItem(HTREEITEM hItem,SERVICE_ITEMS& SI);
	bool GetMaxValueFromTreeItem(HTREEITEM hItem,int& nQLU,int& nQLL,long& lQWSU,long& lQWSL,CString& strAN,int& nADU,int& nADL);
	bool SetTargetLevel(CSimParameter* pSP);
	void SetAutoReport(CSimParameter* pSP);
	void InsertAllTypeChildItem(HTREEITEM hItem,CString* pStrQLU=NULL,CString* pStrQLL=NULL,CString* pStrQWSU=NULL,CString* pStrQWSL=NULL,CString* pStrAN=NULL,CString* pStrDAU=NULL,CString* pStrDAL=NULL);
	HTREEITEM m_hOptional;
	void DisableMenuItem(CMenu* pMenu);
	CWnd* m_pParent;
	HTREEITEM m_hActivityBreakdown;
	HTREEITEM m_hEconomic;
	HTREEITEM m_hBaggage;
	HTREEITEM m_hAO;
	HTREEITEM m_hProcessors;
	HTREEITEM m_hSpace;
	HTREEITEM m_hPassenger;
	HTREEITEM m_hTLOS;
	HTREEITEM m_hAutoReport;
	void InitTree();
	HTREEITEM m_hMPCount;
	HTREEITEM m_TrueRandom;
	HTREEITEM m_hRandomSeed;
	HTREEITEM m_hGeneral;

	HTREEITEM m_hNoPax;
	HTREEITEM m_hBarrier;
	HTREEITEM m_hHub;
	HTREEITEM m_hAvoidance;
	HTREEITEM m_hArea_avoid;
	HTREEITEM m_hPipe_avoid;

	HTREEITEM m_hFlightFilter;
	HTREEITEM m_hFlightFilterStartTime;
	HTREEITEM m_hFlightFilterEndTime;

	HTREEITEM m_hActivity;
	HTREEITEM m_hDistance;
	HTREEITEM m_hTimeInQue;
	HTREEITEM m_hTimeInTerm;
	HTREEITEM m_hTimeInSer;



	HTREEITEM m_hUiti;
	HTREEITEM m_hThroughtput;
	HTREEITEM m_hQueueLen;
	HTREEITEM m_hCriticalQuePara;
	HTREEITEM m_hOccupancy;
	HTREEITEM m_hDensity;
	HTREEITEM m_hSpaceThroutput;
	HTREEITEM m_hCollision;
	HTREEITEM m_hBagCount;
	HTREEITEM m_hBagWaitTime;
	HTREEITEM m_hDistribution;
	HTREEITEM m_hDeliveryTime;
	HTREEITEM m_hNuberOfRun;

	//add by adam 2007-09-27
	HTREEITEM m_hStartDay;
	HTREEITEM m_hEndDay;
	HTREEITEM m_hStartTime;
	HTREEITEM m_hEndTime;
	HTREEITEM m_hInitialPeriodTime;
	HTREEITEM m_hFollowUpTime;
	//End add by adam 2007-09-27

	std::vector<HTREEITEM>m_vAllNoPaxDetailItem;
	std::vector<HTREEITEM>m_vPipeItem;
	std::vector<HTREEITEM>m_vAreaItem;
	
	CSimEngSettingDlg( CSimParameter* _pSimParam, CWnd* pParent);   // standard constructor
	~CSimEngSettingDlg();

// Dialog Data
	//{{AFX_DATA(CSimEngSettingDlg)
	enum { IDD = IDD_DIALOG_SIMENGSETTING };
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CCoolTree	m_coolTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimEngSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
//add by Peter 2007.10.25
protected:
	HTREEITEM m_hRunwayClosures;
	HTREEITEM m_hTaxiwayClosures;
	HTREEITEM m_hStandGroupClosures;
	HTREEITEM m_hVehicles;
	HTREEITEM m_hCyclicGroundRoute;
	CAirsideSimSettingClosure * m_pAirsideSimSettingClosure;

	//itinerant flight
	HTREEITEM m_hItinerantFlight;
	HTREEITEM m_hTrainingFlight;
protected:
	bool LoadAirsideSimClosures(void);
	bool SetRunwayClosureTreeItemContent(void);
	bool SetTaxiwayClosureTreeItemContent(void);
	bool SetStandGroupClosureTreeItemContent(void);
	afx_msg void OnAddAirport(void);
	afx_msg void OnDeleteAirport(void);
	afx_msg void OnAddRunwayClosure(void);
	afx_msg void OnDeleteRunwayClosure(void);
	afx_msg void OnAddTaxiwayClosure(void);
	afx_msg void OnDeleteTaxiwayClosure(void);
	afx_msg void OnAddStandGroupClosure(void);
	afx_msg void OnDeleteStandGroupClosure(void);

	afx_msg void OnAddLandsideTimeRange();
	afx_msg void OnAddLandsideTimeStepRange();
	afx_msg void OnDeleteLandsideTimeRange();
	afx_msg void OnDelLandsideTimeStepRange();
	afx_msg void OnAddLandsideClosure();
	afx_msg void OnDeleteLandsideClosure();
	afx_msg void OnAddParkingLot();
	afx_msg void OnDeleteParkingLot();
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
//end add by Peter 2007.10.25

//onboard
protected:
	HTREEITEM m_hBoardingTime;
	HTREEITEM m_hDeplanementTime;
	OnboardSimSetting* m_pOnboardSimSetting;
protected:
	bool LoadOnboardData();
	void LoadLandsideData();

private:
	LandsideFacilityLayoutObjectList* GetLandsideObjectList();
	void InitLandsideTree(COOLTREE_NODE_INFO& cni);
	void AddLandsideClosureItem(HTREEITEM hItem, LandsideClosure* pLandsideClosure);
	void AddParkinglotClosureItem(HTREEITEM hItem, LandsideClosure* pLandsideClosure);
	void AddTimeRangeItem(HTREEITEM hItem,LandsideTimeRange* pTimeRange);
	void DeleteTimeRangeItem(HTREEITEM hItem,LandsideTimeRange* pTimeRange);


	// land side time step range root
	void AddLandisdeTimeStep(TimeStepRange *pTimeStepRange);
	void RemoveLandsideTineStep(HTREEITEM hTimeRangItem);


	
	//Time step root
	HTREEITEM m_hTimeStepRoot;
	HTREEITEM m_hTimeStep;
	HTREEITEM m_hTimeStepRangeRoot;


	HTREEITEM m_hVehicleSim;
	HTREEITEM m_hStrecthClos;
	HTREEITEM m_hIntersectionClos;
	HTREEITEM m_hRoundClos;

	HTREEITEM m_hParkinglotClos;//new specification, add by sky.wen
	HTREEITEM m_hNameStretchClos;
	//HTREEITEM m_hParkingLot;
	std::vector<HTREEITEM> m_vTimeRange;
	std::vector<HTREEITEM> m_vParkingLot;

	LandsideSimSetting* m_pLandsideSimSetting; 
// Implementation
protected:
	CEdit	m_controlSimName;
	CSimTimeRangeDlg*	m_pSimTimeRangeDlg;
	CStatic	m_staticSimName;
	CString	m_stSimName;
	bool m_bShowSimName;

	CSimParameter* m_pSimParam;

	void LoadData();
	bool HandleNoPaxDetailFlags(HTREEITEM _hItem );
	void HandleAreaItem(HTREEITEM hItem);
	void HandlePipeItem(HTREEITEM hItem);
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	void clearMobConstraintPointer();
	// Generated message map functions
	//{{AFX_MSG(CSimEngSettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSimengCooltreePopupmenAddproc();
	afx_msg void OnSimengCooltreePopupmenAddpaxtype();
	afx_msg void OnSimengCooltreePopupmenDelproc();
	afx_msg void OnSimengCooltreePopupmenDelpaxtype();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMENGSETTINGDLG_H__F3D0A78B_6D19_4892_89C4_305F03E8EF2C__INCLUDED_)
