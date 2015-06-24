#include "stdafx.h"
#include "OnboardSeatGroupInSim.h"
#include "../InputOnboard/Seat.h"
#include "../Common/Path2008.h"
#include "OnboardCellInSim.h"
#include "OnboardFlightInSim.h"
#include "OnboardDeckGroundInSim.h"
#include "GroundInSim.h"
#include "OnboardSeatInSim.h"
#include "OnboardElementSpace.h"

using namespace OBGeo;
OnboardSeatGroupInSim::OnboardSeatGroupInSim(CSeatGroup* pSeatGroup,OnboardFlightInSim* pFlight)
:OnboardAircraftElementInSim(pFlight)
,m_pSeatGroup(pSeatGroup)
,m_pElementSpace(NULL)
{
	
}

OnboardSeatGroupInSim::~OnboardSeatGroupInSim()
{

}


void OnboardSeatGroupInSim::CalculateSpace()
{
	if (m_vSeatRow.empty())
		return;
	
	m_pElementSpace = new OnboardElementSpace(GetGround());
	m_pElementSpace->CreatePollygon(this);
	m_pElementSpace->CalculateSpace();
}

bool OnboardSeatGroupInSim::GetLocation(CPoint2008& location)const
{
	m_pElementSpace->GetCenterPoint(location);
	return true;
}

void OnboardSeatGroupInSim::CreatePollygon()
{
	int nRowCount = (int)m_vSeatRow.size();
	if (nRowCount == 1)
	{
		CalculateOneRowPollygon();
	}
	else
	{
		CalculateMoreThanOneRowPollygon();
	}
}

void OnboardSeatGroupInSim::CalculateOneRowPollygon()
{
	CPollygon2008 pollygon;
	OnboardSeatRowInSim* pSeatRow = m_vSeatRow.front();
	int nSeatCount = pSeatRow->GetCount();
	if (nSeatCount == 0)
		return;

	CSeat* pFrontSeat = pSeatRow->front()->m_pSeat;
	CSeat* pBackSeat = pSeatRow->back()->m_pSeat;
	CPath2008 path;
	if (nSeatCount == 1)//only one seat in seat row
	{
		pFrontSeat->GetFrontSeatLeftRightOfRowPath(path);
		pollygon.init(path.getCount(),path.getPointList());
		m_pElementSpace->setPollygon(pollygon);
		return;
	}
	
	CPath2008 frontPath,backPath;
	pFrontSeat->GetFrontSeatLeftRightOfRowPath(frontPath);
	pBackSeat->GetBackSeatLeftRightOfRowPath(backPath);

	//calculate middle seat path
	CPath2008 topPath,bottomPath;
	for (int iSeat = 1; iSeat < nSeatCount - 1; iSeat++)
	{
		CSeat* pSeat = pSeatRow->GetSeatInSim(iSeat)->m_pSeat;
		CPath2008 topPath1,bottomPath1;
		pSeat->GetTopBottomPath(topPath1,bottomPath1);

		topPath.ConnectPath(topPath1);
		bottomPath.ConnectPath(bottomPath1);
	}

	//connect front,back and middle seat to pollygon
	CPath2008 resultPath;
	resultPath.ConnectPath(frontPath);
	resultPath.ConnectPath(topPath);
	resultPath.ConnectPath(backPath);
	bottomPath.invertList();
	resultPath.ConnectPath(bottomPath);

	pollygon.init(resultPath.getCount(),resultPath.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}

bool OnboardSeatGroupInSim::CompareTwoPosition(OnboardSeatRowInSim* pOneRowInSim,OnboardSeatRowInSim* pOtherRowInSim)const
{
	//if one of seat row has no seat, return default value that pOneRowInsim is head
	if (pOneRowInSim->GetCount() == 0 || pOtherRowInSim->GetCount() == 0)
		return true;
	
	CSeat* pOneSeat = pOneRowInSim->front()->m_pSeat;
	CSeat* pOtherSeat = pOtherRowInSim->front()->m_pSeat;

	CPath2008 topPath1,bottomPath1,topPath2,bottomPath2;
	pOneSeat->GetTopBottomPath(topPath1,bottomPath1);
	pOtherSeat->GetTopBottomPath(topPath2,bottomPath2);

	bottomPath2.invertList();
	topPath1.ConnectPath(bottomPath2);

	bottomPath1.invertList();
	topPath2.ConnectPath(bottomPath1);

	CPollygon2008 pollgon1,pollgon2;
	pollgon1.init(topPath1.getCount(),topPath1.getPointList());
	pollgon2.init(topPath2.getCount(),topPath2.getPointList());

	ARCVector3 entryPt1;
	pOneRowInSim->front()->GetFrontPosition(entryPt1);
	CPoint2008 seatPt1(entryPt1.n[VX],entryPt1.n[VY],0.0);
	if (pollgon1.contains2(seatPt1) || pollgon2.contains2(seatPt1))
	{
		return true;
	}

	return false;
}

void OnboardSeatGroupInSim::CalculateMoreThanOneRowPollygon()
{
	CPollygon2008 pollygon;
	OnboardSeatRowInSim* pPlanHeadRow = m_vSeatRow.front();
	OnboardSeatRowInSim* pPlanBackRow = m_vSeatRow.back();

	OnboardSeatRowInSim* pHeadRow = pPlanHeadRow;
	OnboardSeatRowInSim* pBackRow = pPlanBackRow;
	if (!CompareTwoPosition(pPlanHeadRow,pPlanBackRow))
	{
		pHeadRow = pPlanBackRow;
		pBackRow = pPlanHeadRow;
	}
	
	int nSeatCount = pHeadRow->GetCount();
	if (nSeatCount == 0)
		return;

	if (nSeatCount == 1)
	{
		CSeat* pTop = pHeadRow->GetSeatInSim(0)->m_pSeat;
		CPath2008 topPath;
		pTop->GetFrontSeatTopBottomOfPath(topPath);

		CSeat* pBack = pBackRow->GetSeatInSim(0)->m_pSeat;
		CPath2008 bottomPath;
		pBack->GetBackSeatTopBottomOfPath(bottomPath);
		CPath2008 leftPath;
		CPath2008 rightPath;
		int iRowCount = (int)m_vSeatRow.size();
		for (int iRow = 1; iRow < iRowCount-1; iRow++)
		{
			OnboardSeatRowInSim* pSeatRow = m_vSeatRow[iRow];
			CSeat* pSeat = pSeatRow->front()->m_pSeat;

			CPath2008 leftPath1;
			CPath2008 rightPath1;
			pSeat->GetLeftRightPath(leftPath1,rightPath1);
			leftPath.ConnectPath(leftPath1);
			rightPath.ConnectPath(rightPath1);
		}

		CPath2008 resultPath;
		resultPath.ConnectPath(leftPath);
		resultPath.ConnectPath(topPath);
		rightPath.invertList();
		resultPath.ConnectPath(rightPath);
		bottomPath.invertList();
		resultPath.ConnectPath(bottomPath);

		pollygon.init(resultPath.getCount(),resultPath.getPointList());
		m_pElementSpace->setPollygon(pollygon);
		return;
	}

	//right and left path
	int iRowCount = (int)m_vSeatRow.size();
	CPath2008 leftPath,rightPath;
	for (int iRow = 0; iRow < iRowCount; iRow++)
	{
		CPath2008 leftPath1,rightPath1;
		CPath2008 frontPath,backPath;
		OnboardSeatRowInSim* pSeatRow = m_vSeatRow[iRow];
		CSeat* pFrontSeat = pSeatRow->front()->m_pSeat;
		pFrontSeat->GetLeftRightPath(frontPath,rightPath1);
		CSeat* pBackSeat = pSeatRow->back()->m_pSeat;
		pBackSeat->GetLeftRightPath(leftPath1,backPath);

		leftPath.ConnectPath(frontPath);
		rightPath.ConnectPath(backPath);
	}

	CPath2008 topPath,bottomPath;
	for (int iSeat = 0; iSeat < nSeatCount; iSeat++)
	{
		CSeat* pFrontSeat = pHeadRow->GetSeatInSim(iSeat)->m_pSeat;
		CSeat* pBackSeat = pBackRow->GetSeatInSim(iSeat)->m_pSeat;

		CPath2008 topPath1,bottomPath1;
		CPath2008 frontPath,backPath;
		pFrontSeat->GetTopBottomPath(bottomPath1,frontPath);
		pBackSeat->GetTopBottomPath(backPath,topPath1);
		
		topPath.ConnectPath(backPath);
		bottomPath.ConnectPath(frontPath);
	}

	CPath2008 resultPath;
	resultPath.ConnectPath(leftPath);
	resultPath.ConnectPath(topPath);
	rightPath.invertList();
	resultPath.ConnectPath(rightPath);
	bottomPath.invertList();
	resultPath.ConnectPath(bottomPath);

	pollygon.init(resultPath.getCount(),resultPath.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}

GroundInSim* OnboardSeatGroupInSim::GetGround()
{
	if (m_vSeatRow.empty())
		return NULL;

	OnboardSeatRowInSim* pSeatRow = m_vSeatRow.front();
	if (pSeatRow->GetCount() == 0)
		return NULL;
	
	CSeat* pSeat = pSeatRow->front()->m_pSeat;
	return m_pFlightInSim->GetGroundInSim(pSeat->GetDeck());
}

void OnboardSeatGroupInSim::InitSeatRow()
{
	for (size_t i = 0; i < m_vSeatInSim.size(); i++)
	{
		OnboardSeatInSim* pSeatInSim = m_vSeatInSim[i];
		OnboardSeatRowInSim* pSeatRowInSim = pSeatInSim->GetSeatRowInSim();
		if (std::find(m_vSeatRow.begin(),m_vSeatRow.end(),pSeatRowInSim) == m_vSeatRow.end())
		{
			pSeatRowInSim->SetSeatGroupInSim(this);
			m_vSeatRow.push_back(pSeatRowInSim);
		}
	}
}

bool OnboardSeatGroupInSim::frontSeatRow(OnboardSeatRowInSim* pSeatRowInSim)const
{
	if (m_vSeatRow.empty())
		return false;
	
	OnboardSeatInSim* pFrontSeatInSim = pSeatRowInSim->front();
	const CPollygon2008& polygon = GetPolygon();
	ARCVector3 entryPt;
	pFrontSeatInSim->GetFrontPosition(entryPt);
	CPoint2008 seatPt(entryPt.n[VX],entryPt.n[VY],0.0);

	if (polygon.contains2(seatPt))
	{
		return false;
	}

	return true;
}

const CPollygon2008& OnboardSeatGroupInSim::GetPolygon()const
{
	ASSERT(m_pElementSpace);
	return m_pElementSpace->GetPolygon();
}

int OnboardSeatGroupInSim::GetResourceID() const
{
	return -1;
}