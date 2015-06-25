#pragma once
#include "../MFCExControl/SimpleToolTipListBox.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/ListCtrlEx.h"

#import "../../Lib/ARCFlight.tlb" no_auto_exclude

class CAirportDatabase;
class FlightTypeDetailsManager;
class FlightTypeDetailsItem;
class CDlgFlightTypeDimensions : public CDialog
{
	DECLARE_DYNAMIC(CDlgFlightTypeDimensions)

public:
	CDlgFlightTypeDimensions(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFlightTypeDimensions();

public:

	void InitListCtrl();
	void InitListBox();

protected:
	void UpdateUIState();
	void ResetARCFlightControl(FlightTypeDetailsItem* pItem);
	void ResetListCtrl(FlightTypeDetailsItem* pItem);
	void ResetSliderCtrl();

protected:
	//CXTPPropertyGrid			m_wndPropertyGrid;
	CToolBar					m_wndFltToolbar;
	CSimpleToolTipListBox		m_wndListBox;	
	CListCtrlEx					m_wndListCtrl;
	CSliderCtrl					m_wndExtXSlider;
	CSliderCtrl					m_wndExtYSlider;

	BOOL						m_bInit;


	int							m_nExtent;
	int							m_nMinorInterval;

	PSelectFlightType			m_pSelectFlightType;
	FlightTypeDetailsManager	*m_pFlightTypeDetailsMgr;
	int							m_nProjID;
	CAirportDatabase			*m_pAirportDB;

	ARCFlightLib::IFlight2DPtr m_spFlight2D;
	ARCFlightLib::IGroundGridPtr m_spGridPtr;
	ARCFlightLib::IFlightDimensionsPtr m_spFltDimensions;
	ARCFlightLib::IFlightServiceLocationsPtr m_spServiceLocations;

public:
	enum { IDD = IDD_DIALOG_FLIGHTTYPE_DIMENSIONS };

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

	afx_msg void OnLvnEndlabelEditListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCsave();
};
