#include "StdAfx.h"
#include ".\WalkwayInSim.h"
#include "..\Landside\LandsideWalkway.h"
#include "..\Common\Path2008.h"
#include "LandsideBaseBehavior.h"
#include "LandsideTrafficSystem.h"
#include "..\Common\elaptime.h"

CWalkwayInSim::CWalkwayInSim(LandsideWalkway* pWalkway)
:m_pWalkway(pWalkway)
{

}

CWalkwayInSim::~CWalkwayInSim()
{

}

//----------------------------------------------------------------------------
//Summary:
//		retrieve input walkway
//---------------------------------------------------------------------------
LandsideWalkway* CWalkwayInSim::GetInputWalkway() const
{
	return m_pWalkway;
}
//------------------------------------------------------------------------
//Summary:
//		retrieve traffic type
//------------------------------------------------------------------------
CLandsideWalkTrafficObjectInSim::TrafficObjectType CWalkwayInSim::GetTrafficType() const
{
	return CLandsideWalkTrafficObjectInSim::Walkway_Type;
}
//-----------------------------------------------------------------------
//Summary:
//		retrieve crosswalk center path
//-----------------------------------------------------------------------
CPath2008 CWalkwayInSim::GetCenterPath() const
{
	ASSERT(m_pWalkway);
	return m_pWalkway->getPath();
}
//----------------------------------------------------------------------
//Summary:
//		retrieve crosswalk right path
//----------------------------------------------------------------------
CPath2008 CWalkwayInSim::GetRightPath() const
{
	ASSERT(m_pWalkway);
	CPath2008 rightPath = m_pWalkway->m_rightPath;

	CPoint2008 ptCenter(0,0,0);
	if(m_pWalkway->getCenter(ptCenter)){
		rightPath.DoOffset(ptCenter);		 
	}
	return rightPath;
}
//----------------------------------------------------------------------
//Summary:
//		retrieve crosswalk left path
//----------------------------------------------------------------------
CPath2008 CWalkwayInSim::GetLeftPath() const
{
	ASSERT(m_pWalkway);
	CPath2008 leftPath = m_pWalkway->m_leftPath;
	CPoint2008 ptCenter(0,0,0);
	if(m_pWalkway->getCenter(ptCenter)){
		leftPath.DoOffset(ptCenter);		 
	}
	return leftPath;
}
//----------------------------------------------------------------------
//Summary:
//		step on walkway
//Parameter:
//		pLandsideBehavior[in]: person behavior
//		time[out]: passed time
//		vertex: move on vertex
//		pFreeMoveLogic[in]: person free moving logic
//---------------------------------------------------------------------
void CWalkwayInSim::stepIt( LandsideBaseBehavior* pLandsideBehavior,const LandsideTrafficGraphVertex& vertex,CFreeMovingLogic* pFreeMoveLogic,ElapsedTime& eTime )
{
	ASSERT(m_pWalkway);
	if(pLandsideBehavior->getCurrentVertex().GetTrafficInSim() != this)
	{
		pFreeMoveLogic->Step(eTime,vertex);
	}
	else
	{

		CPath2008 path = m_pWalkway->getPath();
		double dStartPos = pLandsideBehavior->getCurrentVertex().GetDistance();
		double dEndPos = vertex.GetDistance();
		CPath2008 walkPath = path.GetSubPath(dStartPos,dEndPos);

		for (int i = 1; i < walkPath.getCount(); i++)
		{
			pFreeMoveLogic->Step(eTime,walkPath.getPoint(i));
		}
		pFreeMoveLogic->Move();
	}
	pLandsideBehavior->setCurrentVertex(vertex);
}
