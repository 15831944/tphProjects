#include "StdAfx.h"
#include "Resource.h"
#include ".\airsidebagcartparkposdlg.h"
#include "..\InputAirside\AirsideBagCartParkSpot.h"

IMPLEMENT_DYNAMIC(CAirsideBagCartParkingPosDlg, CAirsideParkingPosDlg)

CAirsideBagCartParkingPosDlg::CAirsideBagCartParkingPosDlg( int nObjID,int nAirportID,int nProjID,CWnd* pParent /*= NULL*/ )
:CAirsideParkingPosDlg(nObjID, nAirportID, nProjID, pParent)
{
	m_pObject = new AirsideBagCartParkSpot();
	m_pObject->setAptID(nAirportID);
}

CString CAirsideBagCartParkingPosDlg::GetTitile( BOOL bEdit ) const
{
	if (bEdit)
	{
		return (_T("Modify BagCart Parking Spot"));
	}
	else
	{
		return (_T("Define BagCart Parking Spot"));
	}
}
