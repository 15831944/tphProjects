#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../InputOnBoard/OnBoardingCall.h"
#include "../InputOnBoard/FlightLagTimeContainer.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgSectBlockLogTime dialog
class COnBoardSeatBlockList ;
class COnBoardSeatBlockItem ;
class CAirportDatabase ;
class InputTerminal ;
class COnBoardAnalysisCandidates ;
class CDlgSectBlockLogTime : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSectBlockLogTime)

public:
	CDlgSectBlockLogTime(int nProjID,InputTerminal * pInterm,CAirportDatabase* pAirportDB,COnBoardAnalysisCandidates* _pOnBoardList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSectBlockLogTime();
    
// Dialog Data
	enum { IDD = IDD_DIALOG_LAGTIME };
	enum {TY_NODE_FLIGHT ,TY_NODE_BLOCK , TY_NODE_POSITION,TY_NODE_FROM,TY_NODE_TO,TY_NODE_FACTOR,TY_NODE_PAXTYS,TY_NODE_PAXTY,TY_NODE_BASETIME,TY_NODE_BLOCKS};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	CCoolTree m_LogTimeTree ;
	CEdit m_ToolBarEdit ;
	CToolBar m_ToolBar ;
	int m_projectid ;
	CAirportDatabase* m_Airport ;
	InputTerminal* m_input ;
	COnBoardingCall m_boardingCall;
	COnBoardAnalysisCandidates* m_onboardList ;
    CFlightLagTimeData* fligtimeData ;
protected:
	BOOL OnInitDialog() ;
	void OnInitTree() ;
	void AddLagtimeItem(CSeatBlockLagTime* logTime , HTREEITEM item,COnBoardingCallFlight* _flightTy) ;
	void AddPaxDataItem(CLagTimeForPax* _paxData, HTREEITEM item) ;

	void OnInitToolBar() ;
protected:
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	afx_msg void OnSize(UINT nType, int cx, int cy) ;
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected:
	void OnOK() ;
	void OnCancel() ;
	void OnSave() ;
protected:
	afx_msg void OnToolBarAdd() ;
	afx_msg void OnToolBarDel() ;
	afx_msg void OnAddFlight() ;
	afx_msg void OnDelFlight() ;
	afx_msg void OnAddLagTimePaxData() ;
	afx_msg void OnDelLagTimePaxData() ;

	void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)  ;

	void InitComboString(CComboBox* _box,HTREEITEM Item) ;
	void HandleComboxChange(CComboBox* _box ,HTREEITEM _item) ;


	CString FormatPositionString(const CString& _val) ;
	CString FormatFromSeatString(const CString& _val) ;
	CString FormatToSeatString(const CString& _val) ;
	CString FormatLagFactor(const CString& _val) ;
	CString FormatBaseTime(const CString& _val) ;

	void HandleEditLable(const CString& _val ,HTREEITEM item) ;

	HTREEITEM AddFlightNode(COnBoardingCallFlight* _type) ;
	HTREEITEM AddFlightNode(COnBoardingCallFlight* _type,CMappingFlightID_LagTime::TY_LAGTIME_CONTAINER* _BlockData) ;
	CString   FormatFlightNodeString(COnBoardingCallFlight* _type) ;
protected:
	void OnDelBlock() ;
	void OnAddBlock() ;
	BOOL CheckBlockIfHaveDefined(HTREEITEM _item,COnBoardSeatBlockItem* blockItem);

	HTREEITEM AddBlockItem(COnBoardSeatBlockItem* _blcok ,HTREEITEM _item) ;
};
