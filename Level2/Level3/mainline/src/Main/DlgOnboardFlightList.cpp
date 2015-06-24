// DlgOnboardFlightList.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgOnboardFlightList.h"
#include "../InputOnboard/AircaftLayOut.h"


// CDlgOnboardFlightList dialog

BEGIN_MESSAGE_MAP(CDlgOnboardFlightList, CXTResizeDialog)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LISTBOX_FLIGHT,OnSelChangeFlightList)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDlgOnboardFlightList, CXTResizeDialog)
CDlgOnboardFlightList::CDlgOnboardFlightList(Terminal* _pTerm,FlightListType eFlightListType/*=ALL_FLIGHT_LIST*/,CWnd* pParent/* = NULL*/)
	: CXTResizeDialog(CDlgOnboardFlightList::IDD, pParent)
{
	m_pTerminal=_pTerm;
	m_nCurSel=-1;
	m_pOnboardFlight=NULL;
	m_strFlight=_T("");
	m_eFlightListType=eFlightListType;
}

CDlgOnboardFlightList::~CDlgOnboardFlightList()
{
}

int CDlgOnboardFlightList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDlgOnboardFlightList::OnSelChangeFlightList()
{
	m_nCurSel=m_listboxFlight.GetCurSel();
	m_pOnboardFlight=(COnboardFlight*)m_listboxFlight.GetItemData(m_nCurSel);
	m_listboxFlight.GetText(m_nCurSel,m_strFlight);
}

void CDlgOnboardFlightList::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LISTBOX_FLIGHT,m_listboxFlight);
}
BOOL CDlgOnboardFlightList::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetResize(IDC_LISTBOX_FLIGHT,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
	SetResize(IDCANCEL,SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
	OnInitFlightList();
	return true;
}
void CDlgOnboardFlightList::OnInitFlightList()
{
//  	m_listFlight.InsertColumn(0,"Flight");
	int nCount = m_pTerminal->flightSchedule->getCount();
	int flightNum=0;
	for (int i = 0; i < nCount; i++)
	{
		Flight* pFlight = m_pTerminal->flightSchedule->getItem(i);
		if (m_eFlightListType == ARR_FLIGHT_LIST && pFlight->isDeparting())
		{
			continue;
		}
		if (m_eFlightListType == DEP_FLIGHT_LIST && pFlight->isArriving())
		{
			continue;
		}
		char szID[1024] = {0};
		char szACType[1024] = {0};
		CString strFlight = _T("");
		COnboardFlight* pFlightData = new COnboardFlight();
		if (pFlight->isArriving())
		{
			pFlight->getFullID(szID,'A');
			pFlightData->setFlightType(ArrFlight);
		}
		if (pFlight->isDeparting())
		{
			pFlight->getFullID(szID,'D');
			pFlightData->setFlightType(DepFlight);
		}
		
		pFlight->getACType(szACType);
		int nDay = pFlight->getArrTime().GetDay();
		

		pFlightData->setACType(szACType);
		pFlightData->setFlightID(szID);
		pFlightData->setDay(nDay);
		CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID()) ;
		CString strLayout;
		if(_layout != NULL)
			strLayout = _layout->GetName() ;

		strFlight = FormatFlightIdNodeForCandidate(strLayout,pFlightData) ;
 		m_listboxFlight.InsertString(flightNum,strFlight);
		m_listboxFlight.SetItemData(flightNum,(DWORD)pFlightData);
		flightNum++;
	}
}
CString CDlgOnboardFlightList::FormatFlightIdNodeForCandidate(const CString& _configName,COnboardFlight* pFlight)
{
	CString strFlight ;
	if (pFlight->getFlightType() == ArrFlight)
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Arr Day%d"),pFlight->getFlightID(),pFlight->getACType(), pFlight->getDay());
	}
	else
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Dep Day%d"),pFlight->getFlightID(),pFlight->getACType(), pFlight->getDay());
	}    
	if(!_configName.IsEmpty())
	{
		CString config;
		config.Format("(%s)",_configName) ;
		strFlight.Append(config) ;
	}
	return strFlight ;
}
COnboardFlight* CDlgOnboardFlightList::getOnboardFlight()
{
	if (m_nCurSel<0)
	{
		return NULL;
	}
	return m_pOnboardFlight;
}
CString CDlgOnboardFlightList::getFlightString()
{	
	if (m_nCurSel<0)
	{
		return _T("");
	}

	
	return m_strFlight;
}



// CDlgOnboardFlightList message handlers
