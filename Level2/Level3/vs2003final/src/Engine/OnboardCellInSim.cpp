#include "stdafx.h"
#include "OnboardCellInSim.h"
#include "PaxOnboardBaseBehavior.h"



using namespace OBGeo;

OnboardCellInSim::OnboardCellInSim(OnboardDeckGroundInSim *pDeckGround,long theTileIndex, OBGeo::FRect& aTile, double deckHeight)
:FRect(aTile)
,m_pDeckGround(pDeckGround)
,mTileIndex(theTileIndex)
,mState(Idle)
,mbEntryPoint(false)
,m_pBehavior(NULL)
,m_pReservePax(NULL)
{
	mLocation.init(FRect::mX + 0.5*FRect::mWidth,  FRect::mY + 0.5*FRect::mHeight, deckHeight);
}

OnboardCellInSim::~OnboardCellInSim()
{

}

CPath2008 OnboardCellInSim::GetPath2008()const
{
	CPoint2008 ptList[4];
	ptList[0] = CPoint2008(FRect::mX,FRect::mY,0);
	ptList[1] = CPoint2008(FRect::mX + FRect::mWidth,FRect::mY,0);
	ptList[2] = CPoint2008(FRect::mX + FRect::mWidth,FRect::mY + FRect::mHeight,0);
	ptList[3] = CPoint2008(FRect::mX,FRect::mY + FRect::mHeight,0);

	CPath2008 path;
	path.init(4,ptList);
	return path;
}

Path OnboardCellInSim::GetPath()const
{
	Point ptList[4];
	ptList[0] = Point(FRect::mX,FRect::mY,0);
	ptList[1] = Point(FRect::mX + FRect::mWidth,FRect::mY,0);
	ptList[2] = Point(FRect::mX + FRect::mWidth,FRect::mY + FRect::mHeight,0);
	ptList[3] = Point(FRect::mX,FRect::mY + FRect::mHeight,0);

	Path path;
	path.init(4,ptList);
	return path;
}

bool OnboardCellInSim::IsAtSameDeck( OnboardCellInSim *pCell ) const
{
	if(pCell == NULL)
		return false;

	if(pCell->getLocation().getZ() == getLocation().getZ())
		return true;
	return false;
}

double OnboardCellInSim::GetDistanceTo( OnboardCellInSim *pCell ) const
{
	return getLocation().distance(pCell->getLocation());
	
}

void OnboardCellInSim::PaxOccupy( PaxOnboardBaseBehavior *pBehavior )
{
	ASSERT(pBehavior > 0);
	m_pBehavior = pBehavior;
}

void OnboardCellInSim::PaxLeave( PaxOnboardBaseBehavior *pBehavior, const ElapsedTime& eTime)
{
	//ASSERT(m_pBehavior == pBehavior);
	m_pBehavior = NULL;

	std::vector< OnboardAircraftElementInSim *>::iterator iterObject = m_vObjectNeedNotify.begin();
	for (; iterObject != m_vObjectNeedNotify.end(); ++iterObject)
	{
		if(*iterObject)
		{
			(*iterObject)->NotifyPaxLeave(this,eTime);
		}
	}
}

bool OnboardCellInSim::IsValid() const
{
	if(GetState() == HalfOccupied ||
		GetState() == FullOccupied)
		return false;
	return true;
}

bool OnboardCellInSim::IsAvailable( PaxOnboardBaseBehavior *pBehavior ) const
{
	if(m_pBehavior && m_pBehavior != pBehavior)//occupancy and not himself
		return false;

	return IsValid();
}



void OnboardCellInSim::AddNotifyObjectWhenPaxLeave( OnboardAircraftElementInSim *pObject )
{
	m_vObjectNeedNotify.push_back(pObject);
}




//////////////////////////////////////////////////////////////////////////
//OnboardCellPath
OnboardCellPath::OnboardCellPath()
{

}

OnboardCellPath::~OnboardCellPath()
{
	m_vPath.clear();
}

int OnboardCellPath::getCellCount() const
{
	return static_cast<int>(m_vPath.size());
}

void OnboardCellPath::addCell( OnboardCellInSim* pCell )
{
	m_vPath.push_back(pCell);
}

OnboardCellInSim* OnboardCellPath::getCell( int nIndex ) const
{
	ASSERT(nIndex >= 0 && nIndex < getCellCount());
	if(nIndex >= 0 && nIndex < getCellCount())
		return m_vPath.at(nIndex);

	return NULL;
}

bool OnboardCellPath::Contains( OnboardCellInSim *pCell )
{
	if(std::find(m_vPath.begin(), m_vPath.end(), pCell) == m_vPath.end())
		return false;

	return true;
}
//return the first cell after pCurCell
OnboardCellInSim* OnboardCellPath::GetNextCell( OnboardCellInSim *pCurCell ) const
{
	std::vector<OnboardCellInSim *>::const_iterator iter = std::find(m_vPath.begin(), m_vPath.end(), pCurCell);
	if(iter == m_vPath.end())
		return NULL;

	if((iter + 1) != m_vPath.end())
		return *(iter + 1);

	return NULL;
}
OnboardCellInSim* OnboardCellPath::GetNextCell( OnboardCellInSim *pCurCell, int nStep ) const
{
	std::vector<OnboardCellInSim *>::const_iterator iter = std::find(m_vPath.begin(), m_vPath.end(), pCurCell);
	if(iter != m_vPath.end())
	{
		int nStepLeft = m_vPath.end()-iter-1;
		if(nStepLeft >0)
		{
			return *( iter + MIN(nStepLeft,nStep) );
		}
		return NULL;
	}

	if(m_vPath.empty())
		return NULL;
	//find nearest step
	int nNearestIdx = 0;
	OnboardCellInSim* pNearCell = *m_vPath.begin();
	double dNearDist = pCurCell->GetDistanceTo(pNearCell);
	for(size_t i=1;i<m_vPath.size();i++)
	{
		OnboardCellInSim* pCell = m_vPath[i];
		double dDist = pCurCell->GetDistanceTo(pCell);
		if(dDist <= dNearDist)
		{
			dDist = dNearDist;
			pNearCell = pCurCell;
		}
	}
	
	iter = std::find(m_vPath.begin(), m_vPath.end(), pNearCell);
	if(iter != m_vPath.end())
	{
		int nStepLeft = m_vPath.end()-iter-1;
		if(nStepLeft >0)
		{
			return *( iter + MIN(nStepLeft,nStep) );
		}
		return NULL;
	}

	return NULL;

}
void OnboardCellPath::Clear()
{
	m_vPath.clear();
}

std::vector<OnboardCellInSim *>& OnboardCellPath::GetCellPath()
{
	return m_vPath;
}

void OnboardCellPath::MergeCellPath(OnboardCellPath& cellPath)
{
	m_vPath.insert(m_vPath.end(),cellPath.GetCellPath().begin(),cellPath.GetCellPath().end());
}