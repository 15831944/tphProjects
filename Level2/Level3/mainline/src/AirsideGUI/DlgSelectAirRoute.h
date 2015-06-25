#pragma once
#include "AirsideGUIAPI.h"

namespace ns_FlightPlan
{
	class FlightPlanSpecific;
}
class CAirRoute;
class AIRSIDEGUI_API CDlgSelectAirRoute : public CDialog
{
public:
	CDlgSelectAirRoute(int nProjID,ns_FlightPlan::FlightPlanSpecific *pCurFltPlan ,int nSelAirRouteID = -1, CWnd* pParent = NULL);
	virtual ~CDlgSelectAirRoute();

	int GetSelectedAirRoute() const { return m_nSelAirRouteID; }

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	void SetTreeContents();
	bool IsAirportItem(HTREEITEM hItem);

	int m_nProjID;
	int m_nSelAirRouteID;
	CTreeCtrl m_wndAirRouteTree;
	
	ns_FlightPlan::FlightPlanSpecific *m_pCurFltPlan;

	afx_msg void OnSelChangedTreeAirroutes(NMHDR *pNMHDR, LRESULT *pResult);
	void AddAirRouteToTree(CAirRoute* pAirRoute,HTREEITEM hParentItem);
};
