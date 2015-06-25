#pragma once
#include "../MFCExControl/SimpleToolTipListBox.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/ListCtrlEx.h"

#import "../../Lib/ARCFlight.tlb" no_auto_exclude


//-------------------------------------------------------------------------------------------------------
//CFltRelativeIconListCtrl
#include <map>

class CFltRelativeIconListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CFltRelativeIconListCtrl)

public:
	CFltRelativeIconListCtrl();
	virtual ~CFltRelativeIconListCtrl();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	HICON	GetIcon(CString strName);

	std::map<CString, HICON>		m_mapElementIcon;
	HICON							m_hDefaultIcon;
protected:
	HICON LoadIcon(UINT uIDRes);

	DECLARE_MESSAGE_MAP()

};


//-------------------------------------------------------------------------------------------------------
class CAirportDatabase;
class FlightTypeDetailsManager;
class FlightTypeDetailsItem;
class CVehicleSpecifications;
class CDlgFlightRelativeServiceLocations : public CDialog
{
	DECLARE_DYNAMIC(CDlgFlightRelativeServiceLocations)

public:
	CDlgFlightRelativeServiceLocations(int nProjID, PSelectFlightType pSelectFlightType,  CAirportDatabase* pAirportDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFlightRelativeServiceLocations();

public:
	CToolBar					m_wndFltToolbar;
	CSimpleToolTipListBox		m_wndListBox;
	CFltRelativeIconListCtrl		m_wndListCtrl;
	CSliderCtrl					m_wndExtXSlider;
	CSliderCtrl					m_wndExtYSlider;

	BOOL						m_bInit;
	int							m_nExtent;
	int							m_nMinorInterval;
	int							m_nRingRoadOffset;

	int							m_nProjID;
	CAirportDatabase			*m_pAirportDB;
	FlightTypeDetailsManager	*m_pFlightTypeDetailsMgr;
	PSelectFlightType			m_pSelectFlightType;
	CVehicleSpecifications		*m_pVehicleSpecifications;
	std::vector<CPoint2008>			m_vectPathPoint;

	ARCFlightLib::IFlight2DPtr m_spFlight2D;
	ARCFlightLib::IGroundGridPtr m_spGridPtr;
	ARCFlightLib::IFlightDimensionsPtr m_spFltDimensions;
	ARCFlightLib::IFlightServiceLocationsPtr m_spServiceLocations;

	//Drag source
	UINT m_DragDropFormat;

protected:
	void InitListBox();
	void InitListCtrl();
	void UpdateUIState();

	void ResetARCFlightControl(FlightTypeDetailsItem* pItem);
	void ResetListCtrl(FlightTypeDetailsItem* pItem);
	void ResetSliderCtrl();

	BOOL IsAllElementHasServiceLocation();

public:
	enum { IDD = IDD_DIALOG_FLIGHTTYPE_RELATIVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewFltType();
	afx_msg void OnDelFltType();
	afx_msg void OnSelchangeListFltType();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnEnChangeEditExtent();
	afx_msg void OnEnChangeEditMinorInterval();
	afx_msg void OnEnChangeEditRingRoadOffset();

	afx_msg void OnBnClickedOk();
	//afx_msg void OnPickRingRoad();
	afx_msg void OnLvnBegindragListRelative(NMHDR *pNMHDR, LRESULT *pResult);


	DECLARE_EVENTSINK_MAP()
	void FireElementAdd(long nTypeID, double x, double y);
	void FireElementDel(long nTypeID, double x, double y);
	void FireRingRoadPointAdd(VARIANT_BOOL bLastPt, double x, double y);
	void FireResetAllElement();
	void FireElementMove(double x1, double y1, double x2, double y2);
	afx_msg void OnBnClickedSave();
};
