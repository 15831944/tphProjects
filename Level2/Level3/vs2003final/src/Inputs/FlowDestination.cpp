// FlowDestination.cpp: implementation of the CFlowDestination class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowDestination.h"
#include "Inputs\FlowItemEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlowDestination::CFlowDestination()
{
	m_iProbality = 0;
	m_iTypeOfUsingPipe = 0;
	m_iTypeOfOwnerShip = 0;
	m_bHasVisited = false;
	m_iIsOneToOne = 0;

	m_lMaxQueueLength = 0 ;
	m_lMaxWaitMins = 0;
    
	m_iAvoidFixQueue = 0 ;
	m_iAvoidOverFlowQueue = 0 ;
	m_iAvoidDensity = 0 ;
	m_dDensityOfArea = 0.0;
	m_iTypeOfOwnerShip	= 0;
	m_nOneXOneState = 0 ;
	m_bIsFollowInFlow = false;
	m_lMaxSkipTime = 0;
}

CFlowDestination::~CFlowDestination()
{

}
int CFlowDestination::GetPipeAt( int _iIdx ) const 
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vUsedPipes.size()) );
	return m_vUsedPipes.at( _iIdx );
}
bool CFlowDestination::operator != (const CFlowDestination& _anotherInstance ) const
{
	 if ( m_procID == _anotherInstance.m_procID && m_iProbality == _anotherInstance.m_iProbality )
	 {
		if( m_iIsOneToOne != _anotherInstance.m_iIsOneToOne )
			return true;

		if( m_nOneXOneState != _anotherInstance.m_nOneXOneState )
			return true;			

		if( m_iTypeOfUsingPipe != _anotherInstance.m_iTypeOfUsingPipe )
			return true;
		

		if( !( m_vUsedPipes == _anotherInstance.m_vUsedPipes ) )
		{
			return true;
		}
		
		if( m_lMaxQueueLength != _anotherInstance.m_lMaxQueueLength )
		{
			return true;
		}

		if( m_lMaxWaitMins != _anotherInstance.m_lMaxWaitMins )
		{
			return true;
		}
		if(m_lMaxSkipTime!=_anotherInstance.m_lMaxSkipTime)
		{
			return true;
		}

		if( m_iAvoidFixQueue != _anotherInstance.m_iAvoidFixQueue )
		{
			return true;
		}

		if( m_iAvoidOverFlowQueue != _anotherInstance.m_iAvoidOverFlowQueue )
		{
			return true;
		}

		if( m_iAvoidDensity != _anotherInstance.m_iAvoidDensity )
		{
			return true;
		}

		if( m_dDensityOfArea != _anotherInstance.m_dDensityOfArea )
		{
			return true;
		}

		return ! ( m_vAreas == _anotherInstance.m_vAreas );
		
	 }
	 else
	 {
		 return true;
	 }
}


int CFlowDestination::GetOneXOneState() const
{
	return m_nOneXOneState;
}

void CFlowDestination::SetOneXOneState(int _State)
{
	m_nOneXOneState = _State;
}

void CFlowDestination::SetFollowState(bool _state)
{
	m_bIsFollowInFlow = _state;
}

bool CFlowDestination::GetFollowState() const
{
	return m_bIsFollowInFlow;
}

bool CFlowDestination::IfUseThisPipe( int nCurPipeIndex )const
{
	int iSize = m_vUsedPipes.size();
	for( int i=iSize-1; i>=0; --i )
	{
		if( m_vUsedPipes[i] == nCurPipeIndex )
		{
			return true;
		}
	}

	return false;
}