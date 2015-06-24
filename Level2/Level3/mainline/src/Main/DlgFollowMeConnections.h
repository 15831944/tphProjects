#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../InputAirside/FollowMeConnectionData.h"

// CDlgFollowMeConnections dialog
class CAirportDatabase ;
class CDlgFollowMeConnections : public CDialog
{
	DECLARE_DYNAMIC(CDlgFollowMeConnections)

public:
	CDlgFollowMeConnections(int _proID ,CAirportDatabase* _AirportDB ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFollowMeConnections();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONNECTION };
	enum TY_NODE { TY_FLIGHT_ROOT ,TY_FLIGHT , TY_RANGETIME ,TY_RUNWAY,TY_STAND};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_TreeCtrl;
	CStatic m_StaticTool;
	CStatic m_StaticMeeting;
	CListBox m_MeetingListBox;
	CComboBox m_ComboType;
	CComboBox m_SettingCombo;
	int m_ProID ;
protected:
	BOOL OnInitDialog() ;
	void OnOK() ;
	void OnCancel() ;

	void InitTreeCtrl() ;

	void AddFlightItem(CFollowMeConnectionItem* _item,HTREEITEM _rootItem) ;
	void EditFlightItem(CFollowMeConnectionItem* _item,HTREEITEM _fltItem) ;

	void AddFlightRangeItem(CFollowMeConnectionTimeRangeItem* _rangerItem,HTREEITEM _fltItem) ;
	void EditRangeItem(CFollowMeConnectionTimeRangeItem* _rangerItem,HTREEITEM _RangeItem) ;

	void AddRunwayItem(CFollowMeConnectionRunway* _runway ,HTREEITEM _RangeItem) ;
	void EditRunwayItem(CFollowMeConnectionRunway* _runway ,HTREEITEM _runwayItem) ;

	void AddStandItem(CFollowMeConnectionStand* _stand,HTREEITEM _RunwayItem) ;
	void EditStandItem(CFollowMeConnectionStand* _stand,HTREEITEM _standItem) ;

protected:
	void InitMeetingPoint(CFollowMeConnectionStand* _stand) ;

protected:
	void InitTypeComboBox() ;
	void InitBeforeComboBox() ;
protected:
	void OnFlightToolBarAdd() ;
	void OnFlightToolBarEdit() ;
	void OnFlightToolBarDel() ;


	void OnAddFlightType() ;
	void OnEditFlightType() ;
	void OnDeleteFlightType() ;


	void OnAddFlightRangeItem() ;
	void OnEditFlightRangeItem() ;
	void OnDeleteRangeItem() ;

	void OnAddRunwayItem() ;
	void OnEditRunwayItem() ;
	void OnDeleteRunwayItem() ;

	void OnAddStandItem() ;
	void OnEditStandItem() ;
	void OnDeleteStandItem() ;

protected:

	void OnAddMeetingPoint() ;
	void OnDeleteMeetingPoint() ;

protected:
	CToolBar m_FlightTool ;
	CToolBar m_MeetingTool ;
protected:
	CFollowMeConnectionData m_Data ;
protected:
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void InitFlightToolBar() ;
	void InitMeetingToolBar() ;
	CString FormatTimeRanger(CFollowMeConnectionTimeRangeItem* _item);
public:
	afx_msg void OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	void EnableRightCtrl(BOOL _EnableOrDis) ;
	void InitRightAllCtrlData(CFollowMeConnectionStand* _standItem) ;
	CEdit m_EditDIS;

	void InitListBox(CFollowMeConnectionStand* _standItem) ;
public:
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnCbnSelchangeComboBefore();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	CDlgFollowMeConnections::TY_NODE GetNodeType(HTREEITEM _item);
	afx_msg void OnLbnSelchangeListMeeting();
	afx_msg void OnEnUpdateEditVal();
};
