#pragma once
#include "OutboundRouteAssignmentDlg.h"

class CPostDeicingRouteAssignmentDlg: public CBoundRouteAssignmentDlg
{
public:
	CPostDeicingRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent = NULL);
	virtual ~CPostDeicingRouteAssignmentDlg(void);

protected:
	virtual void NewOriginAndDestination();
	virtual void EditOriginAndDestination();

	void InitListCtrl(void);
	void SetListContent(void);
};
