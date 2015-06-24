#include "stdafx.h"
#include "OnBoardDeviceInSim.h"
#include "InputOnBoard/OnBoardBaseVerticalDevice.h"
#include "Common/Path2008.h"
#include "GroundInSim.h"
#include "OnboardFlightInSim.h"
#include "OnboardDeckGroundInSim.h"

OnboardDeviceInSim::OnboardDeviceInSim(OnBoardVerticalDevice *pDevice,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pDevice(pDevice)
{

}

OnboardDeviceInSim::~OnboardDeviceInSim()
{

}

CString OnboardDeviceInSim::GetType() const
{
	return m_pDevice->GetType();
}

bool OnboardDeviceInSim::GetRealPath(CPath2008& path)
{
	return m_pDevice->GetRealPath(path);
}

bool OnboardDeviceInSim::onDeck(const CDeck* pDeck)const
{
	std::vector<DeckPoint> deckPtList = m_pDevice->GetDeckPoint();
	for (int i = 0; i < (int)deckPtList.size(); i++)
	{
		DeckPoint deckPt = deckPtList[i];
		if (deckPt.first == pDeck)
		{
			return true;
		}
	}
	return false;
}

OnboardCellInSim* OnboardDeviceInSim::GetOnboardCell(OnboardDeckGroundInSim* pDeckGround)const
{
	std::vector<DeckPoint> deckPtList = m_pDevice->GetDeckPoint();
	for (int i = 0; i < (int)deckPtList.size(); i++)
	{
		DeckPoint deckPt = deckPtList[i];
		CPoint2008 devicePos;
		devicePos.init(deckPt.second.n[VX],deckPt.second.n[VY],deckPt.second.n[VZ]);
		if (deckPt.first == pDeckGround->GetDeck())
		{
			return pDeckGround->GetGround()->getCell(devicePos);
		}
	}
	return NULL;
}

OnboardCellInSim* OnboardDeviceInSim::GetEntryCell(OnboardDeckGroundInSim* pDeckGround)const
{
	for (int i = 0; i < (int)m_entryPointList.size(); i++)
	{
		OnboardCellInSim* pEntryCell = m_entryPointList[i];
		if (pEntryCell->GetDeckGround() == pDeckGround)
		{
			return pEntryCell;
		}
	}
	return NULL;
}

int OnboardDeviceInSim::GetResourceID() const
{
	if(m_pDevice)
		return m_pDevice->GetID();

	return -1;
}