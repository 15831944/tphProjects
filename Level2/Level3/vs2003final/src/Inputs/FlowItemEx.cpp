// FlowItemEx.cpp: implementation of the CFlowItemEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowItemEx.h"
#include "Engine\Rule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlowItemEx::CFlowItemEx(/*const ProcessorID& procID, int nPercentage*/)
//:m_procID(procID)
//,m_nPercentage(nPercentage)
{
	m_iTypeOfUsingPipe = 0;
	m_iIsOneToOne = 0;
	m_lMaxQueueLength = 0;
	m_lMaxWaitMins = 0;
	m_lMaxSkipTime=0;

	m_iAvoidFixQueue = 0;
	m_iAvoidOverFlowQueue = 0;
	m_iAvoidDensity = 0 ;
	m_dDensityOfArea = 0.0;
	m_nOneXOneState = 0;
	m_bIsFollowInFlow = false;
}

CFlowItemEx::CFlowItemEx(const CFlowItemEx& _other)
{
	//ClearRules();
	//*this = _other;
	//
	//m_vRules.clear();
	//for(int nRule = 0; nRule < (int)_other.m_vRules.size(); ++nRule)
	//{
	//	m_vRules.push_back(_other.m_vRules[nRule]->Clone());
	//}

	m_lMaxQueueLength = _other.m_lMaxQueueLength;
	m_lMaxWaitMins = _other.m_lMaxWaitMins;
	m_lMaxSkipTime= _other.m_lMaxSkipTime;
	m_iIsOneToOne = _other.m_iIsOneToOne;
	m_iTypeOfUsingPipe = _other.m_iTypeOfUsingPipe;
	m_vPipeList = _other.m_vPipeList;

	m_iAvoidFixQueue = _other.m_iAvoidFixQueue;
	m_iAvoidOverFlowQueue = _other.m_iAvoidOverFlowQueue;
	m_iAvoidDensity = _other.m_iAvoidDensity;
	m_dDensityOfArea = _other.m_dDensityOfArea;
	m_vAreas = _other.m_vAreas;
	m_nOneXOneState = _other.m_nOneXOneState;
	m_bIsFollowInFlow = _other.m_bIsFollowInFlow;

	ClearRules();

	for(std::vector<CRule*>::const_iterator iter=_other.m_vRules.begin(); iter!=_other.m_vRules.end(); iter++)
	{
		m_vRules.push_back( (*iter)->Clone() );
	}

}
CFlowItemEx::~CFlowItemEx()
{
	ClearRules();
}


int CFlowItemEx::GetSize() const
{
	return m_vPipeList.size();
}


int CFlowItemEx::GetPipeIdx( int _nIdx ) const
{
	return m_vPipeList[_nIdx]; 
}


void CFlowItemEx::AddPipeIdx( int _iPipeIdx )
{
	m_vPipeList.push_back( _iPipeIdx );
}
void CFlowItemEx::WriteData( ArctermFile& p_file) const
{
	p_file.writeInt( m_iTypeOfUsingPipe );
	if( m_iTypeOfUsingPipe > 0 )
	{
		int iSize = m_vPipeList.size();
		p_file.writeInt( iSize );
		for( int i=0; i<iSize; ++i )
		{
			p_file.writeInt( m_vPipeList[i] );
		}
	}


	p_file.writeInt( m_iIsOneToOne );
	
	p_file.writeInt( m_lMaxQueueLength );
	p_file.writeInt( m_lMaxWaitMins );
	p_file.writeInt( m_iAvoidFixQueue );
	p_file.writeInt(m_lMaxSkipTime);

	p_file.writeInt( m_iAvoidOverFlowQueue );
	p_file.writeInt( m_iAvoidDensity );
	p_file.writeDouble( m_dDensityOfArea );

	int iSize = m_vAreas.size();
	p_file.writeInt( iSize );
	for( int i=0; i<iSize; ++i )
	{
		p_file.writeField( m_vAreas[ i ] );
	}
	p_file.writeInt(m_nOneXOneState);
	if (m_bIsFollowInFlow)
	{
		p_file.writeInt(1);
	}
	else
	{
		p_file.writeInt(0);
	}
}

void CFlowItemEx::ReadData( ArctermFile& p_file)
{
	p_file.getInteger( m_iTypeOfUsingPipe );
	if( m_iTypeOfUsingPipe > 0 )
	{
		int iSize = 0;
		p_file.getInteger( iSize );
		for( int i=0; i<iSize ; ++i )
		{
			int iIdx =-1;
			p_file.getInteger( iIdx );
			ASSERT( iIdx>=0 );
			m_vPipeList.push_back( iIdx );
		}
	}
	p_file.getInteger( m_iIsOneToOne );	

	if( p_file.getVersion() >= 2.5f )
	{
		p_file.getInteger( m_lMaxQueueLength );
		p_file.getInteger( m_lMaxWaitMins );
		p_file.getInteger( m_iAvoidFixQueue );
		if( p_file.getVersion() >= 2.7f )
			p_file.getInteger(m_lMaxSkipTime);

		p_file.getInteger( m_iAvoidOverFlowQueue );
		p_file.getInteger( m_iAvoidDensity );
		p_file.getFloat( m_dDensityOfArea );
		
		int iSize = 0;
		p_file.getInteger( iSize );
		for( int i=0; i<iSize; ++i )
		{
			char name[256];
			p_file.getField( name, 256 );
			m_vAreas.push_back( name );
		}	
	}
	p_file.getInteger(m_nOneXOneState);	
	int nFollowstate;
	p_file.getInteger(nFollowstate);
	if (nFollowstate == 1) 
	{
		m_bIsFollowInFlow = true;
	}
	else
	{
		m_bIsFollowInFlow = false;
	}
}

void CFlowItemEx::DeletePipeOrAdjust( int _iPipeIdx )
{
	int iSize = m_vPipeList.size();
	for( int i=iSize-1; i>=0; --i )
	{
		if( m_vPipeList[i] > _iPipeIdx )
		{
			m_vPipeList[ i ] = m_vPipeList[ i ] - 1;
		}
		else if( m_vPipeList[i] == _iPipeIdx )
		{
			m_vPipeList.clear();
			m_iTypeOfUsingPipe = 0;
			return;
		}
	}
}
bool CFlowItemEx::IfUseThisPipe( int _iPipeIdx )const
{
	int iSize = m_vPipeList.size();
	for( int i=iSize-1; i>=0; --i )
	{
		if( m_vPipeList[i] == _iPipeIdx )
		{
			return true;
		}
	}

	return false;
}

void CFlowItemEx::SetChannelState(int _state)
{
	m_nOneXOneState = _state;
}

int CFlowItemEx::GetChannelState() const
{
	return m_nOneXOneState;
}

void CFlowItemEx::SetFollowState(bool _state)
{
	m_bIsFollowInFlow = _state ;
}

bool CFlowItemEx::GetFollowState() const
{
	return m_bIsFollowInFlow;
}

void CFlowItemEx::GetRules(std::vector<const CRule*>& _vRules) const
{
	for(std::vector<CRule*>::const_iterator iter=m_vRules.begin(); iter != m_vRules.end(); iter++)
	{
		_vRules.push_back( (*iter) );
	}
}


void CFlowItemEx::InitRules()
{
	ClearRules();
	//insert skip processor rule
	if( m_lMaxSkipTime>0 )m_vRules.push_back( new CSkipProcRule( m_lMaxSkipTime ) );
}

void CFlowItemEx::ClearRules()
{
	for(std::vector<CRule*>::iterator iter = m_vRules.begin(); iter!=m_vRules.end(); iter++)
	{
		delete (*iter);
	}
	m_vRules.clear();
}

const CFlowItemEx& CFlowItemEx::operator=(const CFlowItemEx& _other)
{
	if(&_other == this) return *this;

	//m_procID = _other.m_procID;
	//m_nPercentage = _other.m_nPercentage;

	m_lMaxQueueLength = _other.m_lMaxQueueLength;
	m_lMaxWaitMins = _other.m_lMaxWaitMins;
	m_lMaxSkipTime= _other.m_lMaxSkipTime;
	m_iIsOneToOne = _other.m_iIsOneToOne;
	m_iTypeOfUsingPipe = _other.m_iTypeOfUsingPipe;
	m_vPipeList = _other.m_vPipeList;
	
	m_iAvoidFixQueue = _other.m_iAvoidFixQueue;
	m_iAvoidOverFlowQueue = _other.m_iAvoidOverFlowQueue;
	m_iAvoidDensity = _other.m_iAvoidDensity;
	m_dDensityOfArea = _other.m_dDensityOfArea;
	m_vAreas = _other.m_vAreas;
	m_nOneXOneState = _other.m_nOneXOneState;
	m_bIsFollowInFlow = _other.m_bIsFollowInFlow;
	
	ClearRules();

	for(std::vector<CRule*>::const_iterator iter=_other.m_vRules.begin(); iter!=_other.m_vRules.end(); iter++)
	{
		m_vRules.push_back( (*iter)->Clone() );
	}

	return *this;
}













