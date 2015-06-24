4// SubFlowList.cpp: implementation of the CSubFlowList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SubFlowList.h"
#include "SubFlow.h"
#include "../Common/ARCTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubFlowList::CSubFlowList(): DataSet( SubFlowList, float(2.5) )
								
{

}

CSubFlowList::~CSubFlowList()
{
	clear();
}

void CSubFlowList::clear (void)
{
	int iUnitCount = m_vAllProcessUnit.size();
	for( int i=0; i<iUnitCount; ++i )
	{
		delete m_vAllProcessUnit[ i ];
	}
	m_vAllProcessUnit.clear();
}

bool CSubFlowList::IfProcessUnitExist( const CString _sProcessUnitName ) const
{
	int iUnitCount = m_vAllProcessUnit.size();
	for( int i=0; i<iUnitCount; ++i )
	{
		if( m_vAllProcessUnit[i]->GetProcessUnitName() == _sProcessUnitName )
		{
			return true;
		}
	}
	return false;
}

CSubFlow* CSubFlowList::GetProcessUnit( const CString _sProcessUnitName ) const
{
	int iUnitCount = m_vAllProcessUnit.size();
	for( int i=0; i<iUnitCount; ++i )
	{
		if( m_vAllProcessUnit[i]->GetProcessUnitName() == _sProcessUnitName )
		{
			return m_vAllProcessUnit[ i ];
		}
	}
	return NULL;
}
void CSubFlowList::AddProcessUnit( const CSubFlow& _processUnit )
{
	CSubFlow* pNewProcessUnit = new CSubFlow( _processUnit );
	m_vAllProcessUnit.push_back( pNewProcessUnit );
}

CSubFlow* CSubFlowList::GetProcessUnitAt( int _iIdx )
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vAllProcessUnit.size()) );
	return m_vAllProcessUnit[ _iIdx ];
}

void CSubFlowList::DeleteProcessUnitAt( int _iIdx )
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vAllProcessUnit.size()) );
	delete m_vAllProcessUnit[ _iIdx ];
	m_vAllProcessUnit.erase( m_vAllProcessUnit.begin() + _iIdx );
}

void CSubFlowList::readData (ArctermFile& p_file)	
{
	int iSize=0;
	p_file.getLine();
	p_file.getInteger( iSize );
	for( int i=0; i<iSize ;++i )
	{
		CSubFlow* pNewProcessUnit = new CSubFlow( m_pInTerm );
		pNewProcessUnit->ReadData( p_file );
		m_vAllProcessUnit.push_back( pNewProcessUnit );
	}
}

void CSubFlowList::readObsoleteData ( ArctermFile& p_file )
{
	if(p_file.getVersion() <= 2.21)
	{
		int iSize=0;
		p_file.getLine();
		p_file.getInteger( iSize );
		for( int i=0; i<iSize ;++i )
		{
			CSubFlow* pNewProcessUnit = new CSubFlow( m_pInTerm );
			pNewProcessUnit->readObsoleteData21( p_file );
			m_vAllProcessUnit.push_back( pNewProcessUnit );
		}
	}
	else if (p_file.getVersion() <= 2.41)
	{
		int iSize=0;
		p_file.getLine();
		p_file.getInteger( iSize );
		for( int i=0; i<iSize ;++i )
		{
			CSubFlow* pNewProcessUnit = new CSubFlow( m_pInTerm );
			pNewProcessUnit->readObsoleteData24( p_file );
			m_vAllProcessUnit.push_back( pNewProcessUnit );
		}
	}
}

void CSubFlowList::writeData (ArctermFile& p_file) const
{
	int iSize = m_vAllProcessUnit.size();
	p_file.writeInt( iSize  );
	p_file.writeLine();

	for( int i=0; i<iSize ;++i )
	{
		m_vAllProcessUnit[ i ]->WriteData( p_file );
	}

}
void CSubFlowList::BuildAllProcessPossiblePath()
{
	PLACE_METHOD_TRACK_STRING();
	int iSize = m_vAllProcessUnit.size();	
	for( int i=0; i<iSize ;++i )
	{
		m_vAllProcessUnit[ i ]->BuildAllPossiblePath();
	}
}
void CSubFlowList::SetTransitionLink (const PassengerFlowDatabase& database, const PassengerFlowDatabase& _gateDetailFlowDB, const PassengerFlowDatabase& _stationPaxFlowDB)
{
	PLACE_METHOD_TRACK_STRING();
	int iSize = m_vAllProcessUnit.size();	
	for( int i=0; i<iSize ;++i )
	{
		m_vAllProcessUnit[ i ]->SetTransitionLink( database, _gateDetailFlowDB, _stationPaxFlowDB );
	}
}

bool CSubFlowList::getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	int iSize = m_vAllProcessUnit.size();
	for (int i = 0; i < iSize; i++)
	{
		if(m_vAllProcessUnit[i]->getUsedProInfo(id,_pTerm,_strVec))
			return true;
	}
	return false;
}

bool CSubFlowList::IfUseThisPipe(int nCurPipeIndex,CString& strSubFlowName)const
{
	int iSize = m_vAllProcessUnit.size();
	for (int i = 0; i < iSize; i++)
	{
		if(m_vAllProcessUnit[i]->IfUseThisPipe(nCurPipeIndex))
		{
			strSubFlowName = m_vAllProcessUnit[i]->GetProcessUnitName();
			return true;
		}
	}
	return false;
}

bool CSubFlowList::IfAllFlowValid() const
{
	int iSize = m_vAllProcessUnit.size();
	for (int i = 0; i < iSize; i++)
	{
		if(!m_vAllProcessUnit[i]->IfSubFlowValid())
		{
			return false;
		}
	}
	return true;
}
