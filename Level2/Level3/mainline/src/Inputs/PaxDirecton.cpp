// PaxDirecton.cpp: implementation of the CPaxDirecton class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PaxDirecton.h"
#include "engine\proclist.h"
#include "in_term.h"
//#include "MobileElemConstraint.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxDirection::CPaxDirection()
{

}
CPaxDirection::CPaxDirection( const CMobileElemConstraint _paxType)
:m_paxType( _paxType )
{
	
}
CPaxDirection::~CPaxDirection()
{

}

bool CPaxDirection::operator !=(const CPaxDirection& _another)const
{
	if(m_vCanGoToProcessor == _another.m_vCanGoToProcessor && 
		m_paxType == _another.m_paxType &&
		m_vCanGoToObject == _another.m_vCanGoToObject)
		return false;

	return true;
		
}

void CPaxDirection::AddDestProc( const ProcessorID& _procID )
{
	int nCount = m_vCanGoToProcessor.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vCanGoToProcessor[i] == _procID )
			return;
	}
	m_vCanGoToProcessor.push_back( _procID );
}

bool CPaxDirection::IfExistInDest( const ProcessorID& _testID ,InputTerminal* _pTerm )const
{
	
	const ProcessorID* toGateID = _pTerm->procList->getProcessor( TO_GATE_PROCESS_INDEX )->getID() ;	
	const ProcessorID* fromGateID = _pTerm->procList->getProcessor( FROM_GATE_PROCESS_INDEX )->getID();
	
	int iDestCount = m_vCanGoToProcessor.size();
	CString strTest = _testID.GetIDString();
//	// TRACE( "\n%s",_testID.GetIDString() );
	for( int i=0; i<iDestCount; ++i )
	{
//		// TRACE( "\n%s",m_vCanGoToProcessor[i].GetIDString() );
		CString strCanGo = m_vCanGoToProcessor[i].GetIDString();
		if( m_vCanGoToProcessor[i].idFits( _testID ) || m_vCanGoToProcessor[i] == *toGateID
			|| m_vCanGoToProcessor[ i] == *fromGateID )
			return true;
	}
	return false;
}

void CPaxDirection::AddDestProc( const LandsideObjectLinkageItemInSim& objID )
{
	int nCount = m_vCanGoToObject.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vCanGoToObject[i] == objID )
			return;
	}
	m_vCanGoToObject.push_back( objID );
}

bool CPaxDirection::IfExistInDest( const LandsideObjectLinkageItemInSim& _testID ,InputTerminal* _pTerm )const
{
	
	int iDestCount = m_vCanGoToObject.size();
	CString strTest = _testID.getObject().GetIDString();
//	// TRACE( "\n%s",_testID.GetIDString() );
	for( int i=0; i<iDestCount; ++i )
	{
//		// TRACE( "\n%s",m_vCanGoToProcessor[i].GetIDString() );
		//CString strCanGo = m_vCanGoToObject[i].GetIDString();
		if(m_vCanGoToObject[i].CanLeadTo(_testID))
		//if( m_vCanGoToObject[i].getObject().idFits( _testID.getObject() ) )
		{

			return true;
		}
	}
	return false;
}

bool CPaxDirection::IfExistInDest( const std::vector<LandsideObjectLinkageItemInSim>& _vDestProcID ,InputTerminal* _pTerm )const
{
	int nTestCount = static_cast<int>(_vDestProcID.size());
	for (int nTest = 0; nTest < nTestCount; ++ nTest)
	{
		if(IfExistInDest(_vDestProcID[nTest], _pTerm))
			return true;
	}

	return false;
}
