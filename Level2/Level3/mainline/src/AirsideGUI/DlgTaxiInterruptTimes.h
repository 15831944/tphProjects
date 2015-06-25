#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../InputAirside/TaxiInterruptTimes.h"
#include "../InputAirside/HoldShortLines.h"
// CDlgTaxiInterruptTimes dialog
#include "../MFCExControl/CoolTree.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/XTResizeDialog.h"


class CTaxiLinkedRunwayData;
class CDlgTaxiInterruptTimes : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTaxiInterruptTimes)

public:
	CDlgTaxiInterruptTimes(int ProID ,PSelectFlightType selFlightTy ,CAirportDatabase* pAirportDB, PSelectProbDistEntry pSelectProbDistEntry ,InputAirside* pInputAirside ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaxiInterruptTimes();

// Dialog Data
	enum { IDD = IDD_DIALOG_INTERRUPT_TIMES };
	enum {TY_TIME_WINDOW ,TY_HOLD_TIME};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CStatic m_Sta_Tool;
	CCoolTree m_TreeCtrl;
	CToolBar m_ToolBar ;

public:
	BOOL OnInitDialog() ;
    int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void OnSize(UINT nType, int cx, int cy) ;
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;

	void InitTree() ;
	void InitToolBar() ;

	void AddFlightTyItem(CTaxiInterruptFlightData* _Pair) ;
	void DelFlightTyItem() ;
    

	void AddInterruptLineItem(CTaxiInterruptTimeItem* _InterruptItem , HTREEITEM _ParItem) ;

private:
	void OnNewButton() ;
	void OnDelButton() ;

	void OnNewFlightTy() ;
	void OnDelFlightTy() ;

	void OnNewInterruptLine() ;
	void OnDelInterruptLine() ;

	void OnNewLinkedRwy();
	void OnDelLinkedRwy();

	CTaxiInterruptLine* FindHoldShortLineNodeByID(int id ) ;
	CString FormatInterrruptNodeString(CString& _time,int direct,CString strNode1, CString strNode2 ) ;

	void OnTvnSelchangedTreeHoldShortline(NMHDR *pNMHDR, LRESULT *pResult);
	void OnOK();
	void OnSave();

	BOOL CheckFLightTyIsDefined(FlightConstraint& _constraint) ;
	BOOL CheckInterruptIsDefined(HTREEITEM _item ,CTaxiInterruptTimeItem* _interrupt) ;

    void HandleDialogShow(int ty , HTREEITEM _item) ;
	void InitComboString(CComboBox* _box,HTREEITEM Item) ;
	void HandleComboxChange(CComboBox* _box,HTREEITEM Item) ;

	void LoadLinkedRunwayData(CTaxiInterruptTimeItem* pTimeItem, HTREEITEM hParItem, HTREEITEM hGrandItem);
	void LoadRunwayOperationData(CTaxiLinkedRunwayData* pRwyData, HTREEITEM hParItem);

	CTaxiLinkedRunwayData* GetSelectedRunwayData(HTREEITEM hItem);

private:
	CTaxiInterruptTimes* m_TaxiInterruptTimes ;
	TaxiInterruptLineList m_holdShortLines;
	PSelectFlightType m_pSelectFlightType;
	InputAirside* m_pInputAirside ;
	PSelectProbDistEntry m_pSelectProbDistEntry ;
	int m_nPrjID;
};
