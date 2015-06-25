#pragma once

#include "..\AirsideGUI\ItinerantFlightListCtrl.h"
#include "DialogResize.h"
#include "Resource.h"
#include "..\InputAirside\ItinerantFlight.h"
#include "NodeViewDbClickHandler.h"
#include "..\InputAirside\InputAirside.h"
#include "..\InputAirside\ALTObject.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgItinerantTraffic dialog
class ItinerantFlightSchedule;
class CDlgItinerantTraffic : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgItinerantTraffic)

public:
	CDlgItinerantTraffic(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgItinerantTraffic();

	int m_nProjID;
	CItinerantFlightListCtrl m_wndListCtrl;
	CToolBar m_wndToolBar;
	ItinerantFlightList* m_pItinerantFlightList;
	ItinerantFlightSchedule* m_pItinerantFlightSchedule;
	
	PSelectFlightType	m_pSelectFlightType;
	InputAirside* m_pInputAirside;
	PSelectProbDistEntry    m_pSelectProbDistEntry;
	std::vector<ALTObject> m_vWaypoint;
	
	// Dialog Data
	enum { IDD = IDD_DIALOG_ITINERANTTRAFFIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnEditItem();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSave();

	afx_msg void OnViewSchedule();
	afx_msg void OnUpdateSchedule();

	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitListCtrl();
	void UpdateToolBar();
	void SetListContent();
	void GetAllStand();
	void GenerateItinerantFlightSchedule();

public:
	afx_msg void OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListContents(NMHDR *pNMHDR, LRESULT *pResult);
};
