#pragma once

#include "AirsidePaxBusParkingPosDlg.h"

class CAirsideBagCartParkingPosDlg : public CAirsideParkingPosDlg
{
	DECLARE_DYNAMIC(CAirsideBagCartParkingPosDlg)
	
public:
	CAirsideBagCartParkingPosDlg(int nObjID,int nAirportID,int nProjID,CWnd* pParent = NULL);
	virtual CString GetTitile(BOOL bEdit)const;
};