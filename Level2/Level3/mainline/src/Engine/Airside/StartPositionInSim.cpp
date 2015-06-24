#include "StdAfx.h"
#include ".\startpositioninsim.h"
#include "..\..\Results\AirsideFlightEventLog.h"

StartPositionInSim::StartPositionInSim( CStartPosition* pPosition )
:AirsideResource()
,m_pStartPosition(pPosition)
{
	m_pStartPosition->GetServicePoint(m_Position);
}
StartPositionInSim::~StartPositionInSim(void)
{
}

CPoint2008 StartPositionInSim::GetPosition()
{
	return m_Position;
}

CString StartPositionInSim::GetName()
{
	return m_pStartPosition->GetObjNameString();
}

CStartPosition* StartPositionInSim::GetStartPosition()
{
	return m_pStartPosition.get();
}

AirsideResource::ResourceType StartPositionInSim::GetType() const
{
	return AirsideResource::ResType_StartPosition;
}

CString StartPositionInSim::GetTypeName() const
{
	return "Start position";
}

CString StartPositionInSim::PrintResource() const
{
	return m_pStartPosition->GetObjNameString();
}

void StartPositionInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = m_pStartPosition->getID() ;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

CPoint2008 StartPositionInSim::GetDistancePoint( double dist ) const
{
	return m_Position;
}