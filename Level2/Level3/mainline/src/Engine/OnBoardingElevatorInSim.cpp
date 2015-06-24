#include "stdafx.h"
#include "OnBoardingElevatorInSim.h"
#include "InputOnBoard/OnBoardBaseVerticalDevice.h"
#include "Common/ARCPipe.h"
#include "OnboardFlightInSim.h"
#include "GroundInSim.h"
#include "InputOnBoard/AircaftLayOut.h"
#include "OnboardElementSpace.h"
#include "Common/Line2008.h"

OnboardElevatorInSim::OnboardElevatorInSim(COnBoardElevator *pElevator,OnboardFlightInSim* pFlightInSim)
:OnboardDeviceInSim(pElevator,pFlightInSim)
{
	m_realpath = pElevator->GetLayOut()->GetDeckManager()->GetRealPath(pElevator->GetPathService());
	CalculateSpace();
}

OnboardElevatorInSim::~OnboardElevatorInSim()
{

}

GroundInSim* OnboardElevatorInSim::GetGround(CDeck* pDeck)
{
	return m_pFlightInSim->GetGroundInSim(pDeck);
}

void OnboardElevatorInSim::ClearSpace()
{
	for (int i = 0; i < (int)m_pElementSpaceList.size(); i++)
	{
		delete m_pElementSpaceList[i];
	}
	m_pElementSpaceList.clear();
}

void OnboardElevatorInSim::CreatePollygon()
{
	if (!m_pElementSpaceList.empty())
		return;
	
	std::vector<DeckPoint> vDectPtList = m_pDevice->GetDeckPoint();
	COnBoardElevator* pElevator = (COnBoardElevator*)m_pDevice;
	int nMinDeck = pElevator->GetMinFloor();
	for (int i = 0; i < (int)vDectPtList.size(); i++)
	{
		DeckPoint dectPt = vDectPtList[i];
		GroundInSim* pGroundInSim = GetGround(dectPt.first);
		if (pGroundInSim == NULL)
			continue;
		
		DataOfEveryDeck dataDeck;
		if(!pElevator->GetDataAtFloor(dataDeck,nMinDeck + i))
			continue;

		std::vector<CPollygon2008>& pollygonList = dataDeck.GetLiftAreas();
		for (int j = 0; j < (int)pollygonList.size(); j++)
		{
			OnboardElementSpace* pElementSpace = new OnboardElementSpace(pGroundInSim);
			pElementSpace->setPollygon(pollygonList[j]);
			m_pElementSpaceList.push_back(pElementSpace);
		}
	}
}

void OnboardElevatorInSim::CreateEntryPoint()
{
	std::vector<DeckPoint> vDectPtList = m_pDevice->GetDeckPoint();
	COnBoardElevator* pElevator = (COnBoardElevator*)m_pDevice;
	int nMinDeck = pElevator->GetMinFloor();
	for (int i = 0; i < (int)vDectPtList.size(); i++)
	{
		DeckPoint dectPt = vDectPtList[i];
		GroundInSim* pGroundInSim = GetGround(dectPt.first);
		if (pGroundInSim == NULL)
			continue;

		DataOfEveryDeck dataDeck;
		if(!pElevator->GetDataAtFloor(dataDeck,nMinDeck + i))
			continue;

		CPollygon2008 pollygonList = dataDeck.GetWaitArea();
		CPoint2008 bottomLeft;
		CPoint2008 topRight;
		pollygonList.GetBoundsPoint(bottomLeft,topRight);
		CLine2008 line(bottomLeft,topRight);

		CPoint2008 ptEntryPoint;
		line.GetCenterPoint(ptEntryPoint);
		OnboardCellInSim* pEntryCell = pGroundInSim->getCell(ptEntryPoint);
		m_entryPointList.push_back(pEntryCell);
	}
}

void OnboardElevatorInSim::CalculateSpace()
{
	ClearSpace();
	CreatePollygon();
	CreateEntryPoint();
	for (int i = 0; i < (int)m_pElementSpaceList.size(); i++)
	{
		OnboardElementSpace* pElementSpace = m_pElementSpaceList[i];
		pElementSpace->CreatePollygon(this);
		pElementSpace->CalculateSpace();
	}
}

bool OnboardElevatorInSim::GetLocation( CPoint2008& location ) const
{
	if(m_realpath.getCount() == 0)
		return false;

	location = m_realpath.getPoint(0);
	return true;
}