#pragma once
#include "DlgRunwayAssignment.h"

class CDlgLandingRunwayAssign :	public CDlgRunwayAssignment
{
public:
	CDlgLandingRunwayAssign(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent = NULL);
	virtual ~CDlgLandingRunwayAssign();

protected:
	virtual void ReadData();

private:
	CAirportDatabase   *m_pAirportDatabase;
public:
	virtual BOOL OnInitDialog();
};
