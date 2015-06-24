#include "StdAfx.h"
#include ".\PollutantSourceEmissionSheet.h"
// #include "ProcDataPageDepSink.h"
#include "SourceFlightTypePage.h"
#include "SourceVehiclePage.h"
#include "SourceOtherSourcePage.h"


IMPLEMENT_DYNAMIC(CPollutantSourceEmissionSheet, CResizableSheet)

BEGIN_MESSAGE_MAP(CPollutantSourceEmissionSheet, CResizableSheet)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CPollutantSourceEmissionSheet::CPollutantSourceEmissionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
	:CResizableSheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pParentWnd=pParentWnd;
	m_pSourceEmissionManager=new CSourceEmissionManager(GetAirportDatabase());
	m_pFuelProperties=new FuelProperties();
	m_pAirPollutantList=new CAirPollutantList();

	m_pFuelProperties->ReadData(-1);
	m_pAirPollutantList->ReadData(-1);

	AddPage(new CSourceFlightTypePage(m_pSourceEmissionManager,m_pFuelProperties,m_pAirPollutantList,m_pParentWnd));
	AddPage(new CSourceVehiclePage(m_pSourceEmissionManager,m_pFuelProperties,m_pAirPollutantList,m_pParentWnd,IDS_POLLUTANTSOURCE_VEHICLE_GSE));
	AddPage(new CSourceVehiclePage(m_pSourceEmissionManager,m_pFuelProperties,m_pAirPollutantList,m_pParentWnd,IDS_POLLUTANTSOURCE_VEHICLE_AME));
	AddPage(new CSourceVehiclePage(m_pSourceEmissionManager,m_pFuelProperties,m_pAirPollutantList,m_pParentWnd,IDS_POLLUTANTSOURCE_VEHICLE_LANDSIDE));
	AddPage(new CSourceOtherSourcePage(m_pSourceEmissionManager,m_pFuelProperties,m_pAirPollutantList));
}

CPollutantSourceEmissionSheet::~CPollutantSourceEmissionSheet(void)
{
}
BOOL CPollutantSourceEmissionSheet::OnInitDialog() 
{
	CResizableSheet::OnInitDialog();
	return true;
}

// void CPollutantSourceEmissionSheet::InitPollutantMap()
// {
// 	CAirpollutant *curPollutant;
// 	int nID;
// 	CString strSymbol;
// 	for (int i=0;i<m_pAirPollutantList->GetItemCount();i++)
// 	{
// 		curPollutant=(CAirpollutant *)m_pAirPollutantList->GetItem(i);
// 		nID=curPollutant->GetID();
// 		strSymbol=curPollutant->getSymbol();
// 		m_mapPollutantIDSymbol.insert(std::make_pair(nID,strSymbol));
// 	}
// }
// InputTerminal* CPollutantSourceEmissionSheet::GetInputTerminal()
// {
// 	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
// 	return (InputTerminal*)&pDoc->GetTerminal();
// }
// 
// CString CPollutantSourceEmissionSheet::GetProjPath()
// {
// 	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
// 	return pDoc->m_ProjInfo.path;
// }
CAirportDatabase *CPollutantSourceEmissionSheet::GetAirportDatabase()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->GetTerminal().m_pAirportDB;
}
/*
int CPollutantSourceEmissionSheet::GetProjectID()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return pDoc->GetProjectID();
}*/
