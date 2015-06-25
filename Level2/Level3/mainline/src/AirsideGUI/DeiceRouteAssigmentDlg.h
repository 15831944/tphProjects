#pragma once
#include "./OutboundRouteAssignmentDlg.h"

class CDeiceRouteAssigmentDlg : public CBoundRouteAssignmentDlg
{
public:
	CDeiceRouteAssigmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent = NULL);
	virtual ~CDeiceRouteAssigmentDlg();
protected:
	virtual void NewOriginAndDestination();
	virtual void EditOriginAndDestination();

	void InitListCtrl(void);
	void SetListContent(void);

};