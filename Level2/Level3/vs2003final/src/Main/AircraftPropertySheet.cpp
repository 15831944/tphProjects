// AircraftPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircraftPropertySheet.h"
#include "DlgACTypeProperties.h"
#include "DlgAircraftDoors.h"
#include "../Common/Aircraft.h"


// CAircraftPropertySheet

IMPLEMENT_DYNAMIC(CAircraftPropertySheet, CPropertySheet)
CAircraftPropertySheet::CAircraftPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CAircraftPropertySheet::CAircraftPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd,UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	CDlgACTypeProperties* pACTypeDlg = new CDlgACTypeProperties();
	CDlgAircraftDoors* pAircraftDoorsDlg = new CDlgAircraftDoors();

	AddPage(pACTypeDlg);
	AddPage(pAircraftDoorsDlg);

	m_PropertyPageArr[0]= pACTypeDlg;
	m_PropertyPageArr[1]= pAircraftDoorsDlg;
}

CAircraftPropertySheet::~CAircraftPropertySheet()
{
	for( int i=0; i< 2; i++ )
	{
		if( m_PropertyPageArr[i] )
			delete m_PropertyPageArr[i];
	}
}


BEGIN_MESSAGE_MAP(CAircraftPropertySheet, CPropertySheet)
END_MESSAGE_MAP()


// CAircraftPropertySheet message handlers
BOOL CAircraftPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	return bResult;
}

void CAircraftPropertySheet::setACType(CACType* pACType)
{
	CDlgACTypeProperties* pACTypeDlg = (CDlgACTypeProperties*)m_PropertyPageArr[0];
	pACTypeDlg->setACType(pACType);
	CDlgAircraftDoors* pAircraftDoorsDlg = (CDlgAircraftDoors*)m_PropertyPageArr[1];
	pAircraftDoorsDlg->setACType(pACType);
}