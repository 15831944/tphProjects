#pragma once
#include "GateAssignDlg.h"

class ParkingStandBufferList;
class CDlgStandAssignment : public CGateAssignDlg
{
	DECLARE_DYNAMIC(CDlgStandAssignment)
public:
	CDlgStandAssignment(int nProjID, CAirportDatabase* pAirportDatabase,CWnd* pParent = NULL);
	~CDlgStandAssignment();

	virtual void SetGate();
	virtual void SetUnassignedFlight();
	virtual void SetAssignedFlight();
	virtual GateDefineType GetGateDefineType(){return StandType;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CGateAssignDlg)
	virtual BOOL OnInitDialog();

	afx_msg void OnButtonPriorityGate();
	afx_msg void OnButtonPriorityFlight();

	afx_msg void OnButtonSummary();
	afx_msg void OnButtonUnassignall();
	afx_msg void OnBtnCheckLinkLine();

	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void InitFlightListCtrl();


private:
	void GatePriorityAssign();
	void FlightPriorityAssign();
	void AssignFlightAccordingToGatePriority();		// according to gate priority assign flight
	void AssignFlightAccordingToFlightPriority();	// according to flight priority assign flight
	void SetGateAssignedFlight(int nGateIdx);

private:
	ParkingStandBufferList* m_pStandBufferList;
};
