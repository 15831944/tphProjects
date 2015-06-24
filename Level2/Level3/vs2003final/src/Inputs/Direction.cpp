// Direction.cpp: implementation of the CDirection class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Direction.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirection::CDirection()
{

}

bool CDirection::operator ==(const CDirection& direction)
{
	if (direction.m_paxDirctions.size() != m_paxDirctions.size())
	{
		return false;
	}

	for(int i = 0; i < (int)m_paxDirctions.size(); i++)
	{
		if (m_paxDirctions[i] != direction.m_paxDirctions[i])
		{
			return false;
		}
	}
	return true;
}

CDirection::~CDirection()
{

}

bool CDirection::CanLeadTo( const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID,InputTerminal* _pTerm)const
{
	if( m_paxDirctions.size() == 0 )
	{
		return true;
	}
	std::vector<CPaxDirection>::const_iterator iter = m_paxDirctions.begin();
	std::vector<CPaxDirection>::const_iterator iterEnd = m_paxDirctions.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->GetPaxType().fits( _paxType) )
		{
			return iter->IfExistInDest( _destProcID , _pTerm );
		}			
	}
	return false;
}
void CDirection::Insert( const CMobileElemConstraint& _paxType , const ProcessorID& _destProcID )
{
	std::vector<CPaxDirection>::iterator iter = m_paxDirctions.begin();
	std::vector<CPaxDirection>::iterator iterEnd = m_paxDirctions.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->IsThisPaxType( _paxType ) )
		{
			iter->AddDestProc( _destProcID );
			return;
		}			
	}

	CPaxDirection temp( _paxType );
	temp.AddDestProc( _destProcID );
	m_paxDirctions.push_back( temp );
	
}

void CDirection::Insert(CDirection& Dirctions,const CMobileElemConstraint& _paxType)
{
	if (Dirctions == *this)
		return;

	std::vector<CPaxDirection>paxDirctions = Dirctions.GetPaxDirection();
	std::vector<CPaxDirection>::iterator iter = paxDirctions.begin();
	std::vector<CPaxDirection>::iterator iterEnd = paxDirctions.end();
	for (; iter != iterEnd; ++iter)
	{
		if (iter->IsThisPaxType(_paxType))
		{
			for (size_t i = 0; i < (*iter).GetCanGoToProcessor().size(); i++)
			{
				Insert((*iter).GetPaxType(),(*iter).GetCanGoToProcessor().at(i));
			}
			for (size_t i = 0; i < (*iter).GetCanGoToObject().size(); i++)
			{
				Insert((*iter).GetPaxType(),(*iter).GetCanGoToObject().at(i));
			}


		}
	}
}

void CDirection::Sort()
{
	std::sort( m_paxDirctions.begin(), m_paxDirctions.end() );
}

//for landside object
bool CDirection::CanLeadTo( const CMobileElemConstraint& _paxType,  const LandsideObjectLinkageItemInSim& _destObjID,InputTerminal* _pTerm)const
{
	if( m_paxDirctions.size() == 0 )
	{
		return true;
	}
	std::vector<CPaxDirection>::const_iterator iter = m_paxDirctions.begin();
	std::vector<CPaxDirection>::const_iterator iterEnd = m_paxDirctions.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->GetPaxType().fits( _paxType) )
		{
			return iter->IfExistInDest( _destObjID , _pTerm );
		}			
	}
	return false;
}

bool CDirection::CanLeadTo( const CMobileElemConstraint& _paxType, const std::vector<LandsideObjectLinkageItemInSim>& _vDestProcID,InputTerminal* _pTerm ) const
{
	if( m_paxDirctions.size() == 0 )
	{
		return true;
	}
	std::vector<CPaxDirection>::const_iterator iter = m_paxDirctions.begin();
	std::vector<CPaxDirection>::const_iterator iterEnd = m_paxDirctions.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->GetPaxType().fits( _paxType) )
		{
			return iter->IfExistInDest( _vDestProcID , _pTerm );
		}			
	}
	return false;


}

void CDirection::Insert( const CMobileElemConstraint& _paxType , const LandsideObjectLinkageItemInSim& _destObjID )
{
	std::vector<CPaxDirection>::iterator iter = m_paxDirctions.begin();
	std::vector<CPaxDirection>::iterator iterEnd = m_paxDirctions.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->IsThisPaxType( _paxType ) )
		{
			iter->AddDestProc( _destObjID );
			return;
		}			
	}

	CPaxDirection temp( _paxType );
	temp.AddDestProc( _destObjID );
	m_paxDirctions.push_back( temp );

}

void CDirection::Insert( const CMobileElemConstraint& _paxType , const std::vector<LandsideObjectLinkageItemInSim>& _vDestProcID )
{
	int nCount = static_cast<int>(_vDestProcID.size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		Insert(_paxType, _vDestProcID[nItem]);
	}

}
