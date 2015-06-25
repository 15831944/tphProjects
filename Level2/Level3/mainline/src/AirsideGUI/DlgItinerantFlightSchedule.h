#pragma once
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\InputAirside\ALTObject.h"
#include "../MFCExControl/XTResizeDialog.h"

class ItinerantFlightList;
class ItinerantFlightSchedule;
class CDlgItinerantFlightSchedule : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgItinerantFlightSchedule)

public:
	CDlgItinerantFlightSchedule(/*ItinerantFlightList* pItinerantFltList, */ItinerantFlightSchedule* pItinerantFlightSchedule,
		std::vector<ALTObject>&vWaypoint, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgItinerantFlightSchedule();


	CListCtrlEx m_wndListCtrl;
	CToolBar m_wndToolBar;

protected:
	void InitToolBar();
	void InitListCtrl();
	void ResetListCtrl();

private:
	//ItinerantFlightList* m_pItinerantFltList;
	ItinerantFlightSchedule* m_pItinerantFlightSchedule;
	std::vector<ALTObject>& m_vectWaypoint;
public:

// Dialog Data
	enum { IDD = IDD_DIALOG_ITINERANTFLIGHT_SCHEDULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedOk();
};
