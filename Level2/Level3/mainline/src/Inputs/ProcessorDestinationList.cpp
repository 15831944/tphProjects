// FlowPair.cpp: implementation of the CProcessorDestinationList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessorDestinationList.h"
#include "engine\proclist.h"
#include "in_term.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessorDestinationList::CProcessorDestinationList( InputTerminal* _pTerm )
{
	m_pTerm = _pTerm;
	m_bVisitFlag = false;
}

CProcessorDestinationList::~CProcessorDestinationList()
{

}

void CProcessorDestinationList::AddCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID)
{
	m_Dircetion.Insert(p_type, procID);
}

void CProcessorDestinationList::AddCanLeadGateList(const CMobileElemConstraint &p_type, std::vector<ProcessorID>& vProcID)
{
	std::vector<ProcessorID>::iterator iter = vProcID.begin();
	std::vector<ProcessorID>::iterator iterEnd = vProcID.end();
    for( ; iter != iterEnd; iter++)
        AddCanLeadGate(p_type, *iter);
}

BOOL CProcessorDestinationList::FindCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID)
{
	return m_Dircetion.CanLeadTo(p_type, procID, m_pTerm);
}

void CProcessorDestinationList::AddCanLeadReclaim(const CMobileElemConstraint &p_type, const ProcessorID& procID)
{
	m_ReclaimDircetion.Insert(p_type,procID);
}

void CProcessorDestinationList::AddCanLeadReclaimList(const CMobileElemConstraint &p_type, std::vector<ProcessorID>& vProcID)
{
	std::vector<ProcessorID>::iterator iter = vProcID.begin();
	std::vector<ProcessorID>::iterator iterEnd = vProcID.end();
	for( ; iter != iterEnd; iter++)
		AddCanLeadReclaim(p_type, *iter);
}

BOOL CProcessorDestinationList::FindCanLeadReclaim(const CMobileElemConstraint &p_type, const ProcessorID& procID)
{
	return m_ReclaimDircetion.CanLeadTo(p_type, procID, m_pTerm);
}
/*
CProcessorDestinationList::CProcessorDestinationList( const CProcessorDestinationList& _anotherFlowPair )
{
	m_procID = _anotherFlowPair.m_procID;	
	m_vDestProcs = _anotherFlowPair.m_vDestProcs;
}

CProcessorDestinationList& CProcessorDestinationList::operator = ( const CProcessorDestinationList& _anotherFlowPair )
{
	if( this != &_anotherFlowPair )
	{
		m_procID = _anotherFlowPair.m_procID;	
		m_vDestProcs.clear();
		m_vDestProcs = _anotherFlowPair.m_vDestProcs;
	}
	return *this;
}
*/
void CProcessorDestinationList::AddDestProc( const CFlowDestination& _destProc, bool _bMustEvenPercent )
{
	FLOWDESTVECT::iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::iterator iterEnd = m_vDestProcs.end();
	for(; iter != iterEnd; ++iter )
	{
		if( *iter == _destProc )
		{
			*iter = _destProc;
			if( _bMustEvenPercent )
			{
				//NormalizeDest(); [7/5/2004]
				EvenPercent();
			}
			return;
		}
	}
	m_vDestProcs.push_back( _destProc ) ;
	if( _bMustEvenPercent )
	{
		//NormalizeDest(); [7/5/2004]
		EvenPercent();
	}
	

}

bool CProcessorDestinationList::IfUseThisPipe( int nCurPipeIndex )const
{
	int nSize = (int)m_vDestProcs.size();
	for (int i = 0; i < nSize; i++)
	{
		if(m_vDestProcs[i].IfUseThisPipe(nCurPipeIndex))
			return true;
	}
	return false;
}

void CProcessorDestinationList::Total100Percent()
{
	int nSize = (int)m_vDestProcs.size();
	if(nSize <= 0)
		return ;
	int i = 0;
	std::vector<int> vPercent;
	int iCount = 0;
	for(i = 0; i < nSize; i++)
	{
		iCount += m_vDestProcs[i].GetProbality();
	}
	if(iCount <= 0)
	{
		EvenPercent();
		return ;
	}
	int n = 0;
	for(i = 0; i < nSize-1; i++)
	{
		int iPercent = m_vDestProcs[i].GetProbality();
		iPercent = iPercent*100/iCount;
		n += iPercent;
		m_vDestProcs[i].SetProbality(iPercent);
	}
	m_vDestProcs[nSize-1].SetProbality(100-n);
}

void CProcessorDestinationList::EvenPercent()
{
	int iSize = m_vDestProcs.size();
	if( iSize > 0 )
	{
		if( !IsStation( m_procID ) )
		{
			int iPercent = 100 / iSize ;
			if( 100 % iSize == 0 )
			{
				for( int i=0; i< iSize; ++i )
				{
					m_vDestProcs[ i ].SetProbality( iPercent );
				}
			}
			else
			{
				for( int i=0; i< iSize-1; ++i )
				{
					m_vDestProcs[ i ].SetProbality( iPercent );
				}
			
				m_vDestProcs[ iSize-1 ].SetProbality( 100- iPercent*(iSize-1) );
			}
		}
		else
		{
			std::vector<int> vStationIdx;
			std::vector<int> vNonStationIdx;
			
			for( int i=0; i<iSize ; ++i )
			{
				if( IsStation( m_vDestProcs[i].GetProcID() ) )
				{
					vStationIdx.push_back( i );
				}
				else
				{
					vNonStationIdx.push_back( i );
				}
			}

			int iStationCount = vStationIdx.size();
			if( iStationCount > 0 )
			{
				int iPercent = 100 / iStationCount ;
				if( 100 % iStationCount == 0 )
				{
					for( int i=0; i< iStationCount; ++i )
					{
						m_vDestProcs[ vStationIdx[i] ].SetProbality( iPercent );
					}
				}
				else
				{				
					for( int i=0; i< iStationCount-1 ; ++i )
					{
						m_vDestProcs[ vStationIdx[i] ].SetProbality( iPercent );
					}
					
					m_vDestProcs[ vStationIdx[ iStationCount-1 ] ].SetProbality( 100- iPercent*(iStationCount-1) );
				}
			}

			int iNonStationCount = vNonStationIdx.size();	
			if( iNonStationCount > 0 )
			{
				int iPercent = 100 / iNonStationCount ;
				if( 100 % iNonStationCount == 0 )
				{
					for( int i=0; i< iNonStationCount; ++i )
					{
						m_vDestProcs[ vNonStationIdx[i] ].SetProbality( iPercent );
					}
				}
				else
				{				
					for( int i=0; i< iNonStationCount-1 ; ++i )
					{
						m_vDestProcs[ vNonStationIdx[i] ].SetProbality( iPercent );
					}
					
					m_vDestProcs[ vNonStationIdx[ iNonStationCount-1 ] ].SetProbality( 100- iPercent*(iNonStationCount-1) );
				}
			}
		}
	}

}
void CProcessorDestinationList::EvenRemainPercent( const ProcessorID& _destProc )
{
	FLOWDESTVECT::iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::iterator iterEnd = m_vDestProcs.end();


	for(int nSelItem=0; iter != iterEnd; ++iter ,++nSelItem)
	{
		if( iter->GetProcID() == _destProc )
		{
			int iRemainPercent = 100 - iter->GetProbality();
			ASSERT( iRemainPercent >=0  );
			int iRemainDestCount = m_vDestProcs.size() - 1;
			if( iRemainDestCount > 0 )
			{
				int iPercent = iRemainPercent / iRemainDestCount ;
				if( iRemainPercent % iRemainDestCount == 0 )
				{
					for( int i=0; i<= iRemainDestCount; ++i )
					{
						if( i != nSelItem )
						{
							m_vDestProcs[ i ].SetProbality( iPercent );
						}
					}
				}
				else
				{
					for( int i=0; i<= iRemainDestCount-1; ++i )
					{
						if( i != nSelItem )
						{
							m_vDestProcs[ i ].SetProbality( iPercent );
						}
					}
					if( nSelItem != iRemainDestCount )
					{
						m_vDestProcs[ iRemainDestCount ].SetProbality( iRemainPercent- iPercent*(iRemainDestCount-1) );
					}
					else
					{
						m_vDestProcs[iRemainDestCount - 1].SetProbality( iRemainPercent- iPercent*(iRemainDestCount-1) );
					}
					
				}

			}
			return;
		}
	}
}
/*
void CProcessorDestinationList::EvenRemainPercent( const std::vector<ProcessorID>& _vDestProc )
{
	FLOWDESTVECT::iterator iter0 ;
	std::vector<ProcessorID>::const_iterator iter1 = _vDestProc.begin();
	int nUesedPercent=0;
	int nAppointedCount=0;

	//set correct AppointedPercentFlag into each CDestProcArcinfo.
	for(iter0 = m_vDestProcs.begin();iter0!=m_vDestProcs.end();iter0++)
	{
		(*iter0).SetAppointedPercentFlag(false);
		for(iter1=_vDestProc.begin();iter1!=_vDestProc.end();iter1++)
		{
			if((*iter0).GetProcID() == (*iter1))
			{
				(*iter0).SetAppointedPercentFlag(true);
				nUesedPercent+=(*iter0).GetProbality();
				nAppointedCount++;
				break;
			}
		}
	}

	ASSERT(nUesedPercent<=100 && (100 - nUesedPercent)>= (m_vDestProcs.size()-nAppointedCount));
	//all of dest procs have AppointedPercentFlag.
	if(nAppointedCount==m_vDestProcs.size())
	{
		if(nAppointedCount>0)
		{
			m_vDestProcs[nAppointedCount-1].SetProbality( m_vDestProcs[nAppointedCount-1].GetProbality()+( 100- nUesedPercent)  );
		}
	}
	else
	{//nAppointedCount<m_vDestProcs.size()
		if(nAppointedCount==0)
		{
			EvenPercent();//no one dest Procs.
		}
		else
		{
			int nEvenPercent= (100 - nUesedPercent)/(m_vDestProcs.size()-nAppointedCount);
			FLOWDESTVECT::iterator iterLastDisappointed = NULL;
			
			for(iter0=m_vDestProcs.begin();iter0!=m_vDestProcs.end();iter0++)
			{
				if((*iter0).GetAppointedPercentFlag()== false )
				{
					(*iter0).SetProbality(nEvenPercent);
					iterLastDisappointed = iter0;
				}
			}
			//ensure all of dest procs percent's total == 100
			if(iterLastDisappointed !=NULL)
			{
				(*iterLastDisappointed).SetProbality( 100 -nUesedPercent - nEvenPercent * (m_vDestProcs.size()- nAppointedCount -1) );
			}
			
		}
	}
}

int CProcessorDestinationList::GetAppointedDestCount() const
{
	int nAppointedDestCount=0;
	FLOWDESTVECT::const_iterator iter;
	for(iter=m_vDestProcs.begin(); iter!=m_vDestProcs.end();iter++)
	{
		if( iter->GetAppointedPercentFlag() ==true)nAppointedDestCount++;
	}
	return nAppointedDestCount;
}

void CProcessorDestinationList::NormalizeDest()
{
	int nUsedPercent=0;
	FLOWDESTVECT vNotAppointedDestProcs;
	FLOWDESTVECT::iterator iter=m_vDestProcs.begin();
	//erase all not appointedDest from m_vDestProcs and insert them into vNotAppointedDest.
	while(iter!=m_vDestProcs.end())
	{
		if( (*iter).GetAppointedPercentFlag() ==false )
		{
			vNotAppointedDestProcs.push_back(*iter);
			iter=m_vDestProcs.erase(iter);
			continue;
		}

		nUsedPercent+=iter->GetProbality();
		iter++;
	}

	if(nUsedPercent>100)
	{
		//temporary solution.
		for(iter=vNotAppointedDestProcs.begin();iter!=vNotAppointedDestProcs.end();iter++)
		{
			m_vDestProcs.push_back(*iter);
		}
		EvenPercent();
		return;
	}
//	ASSERT(nUsedPercent<=100);
	int nRemainPercent=100 - nUsedPercent;
	int nEvenPercent=0;
	if(vNotAppointedDestProcs.size()>0)
	{
		nEvenPercent = nRemainPercent/vNotAppointedDestProcs.size();
		//append not appointedDest into m_vDestProcs.
		for(iter=vNotAppointedDestProcs.begin();iter!=vNotAppointedDestProcs.end();iter++)
		{
			iter->SetProbality(nEvenPercent);
			m_vDestProcs.push_back(*iter);
			nRemainPercent -= nEvenPercent;
		}
		FLOWDESTVECT::reverse_iterator iterLast=m_vDestProcs.rbegin();
		iterLast->SetProbality( iterLast->GetProbality() + nRemainPercent );
		vNotAppointedDestProcs.clear();
	}
}
bool CProcessorDestinationList::IsNormalized()
{
	bool bReturn=true;
	bool bPreviousAppointed=false;
	FLOWDESTVECT::iterator iter=m_vDestProcs.begin();
	if(iter!=m_vDestProcs.end())
	{
		bPreviousAppointed=iter->GetAppointedPercentFlag();
	}
	int nNotAppointedCount=0;
	for(iter=m_vDestProcs.begin();iter!=m_vDestProcs.end();iter++)
	{
		if(bPreviousAppointed !=iter->GetAppointedPercentFlag() && bPreviousAppointed==false)
		{
			bReturn=false;
			break;
		}
		if(iter->GetAppointedPercentFlag()==false)nNotAppointedCount++;

		bPreviousAppointed=iter->GetAppointedPercentFlag();
		
	}
	if(nNotAppointedCount==m_vDestProcs.size())EvenPercent();

	return bReturn;
}
*/

void CProcessorDestinationList::DeleteDestProc( const ProcessorID & _destProc )
{
	FLOWDESTVECT::iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::iterator iterEnd = m_vDestProcs.end();
	for(; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _destProc )
		{
			m_vDestProcs.erase( iter );
		//	NormalizeDest(); [7/5/2004]
			EvenPercent();
			return;
		}
	}

	//ASSERT( 0 );
}

void CProcessorDestinationList::DeleteDestProc( int _iIndex )
{
	ASSERT( _iIndex >= 0 && _iIndex < static_cast<int>(m_vDestProcs.size()) );
	m_vDestProcs.erase( m_vDestProcs.begin() + _iIndex );
	//NormalizeDest(); [7/5/2004]
	EvenPercent();
}

const CFlowDestination& CProcessorDestinationList::GetDestProcAt( int _iIndex ) const 
{
	ASSERT( _iIndex >= 0 && _iIndex < static_cast<int>(m_vDestProcs.size()) );
	return m_vDestProcs.at( _iIndex );
}
/*
const CFlowDestination& CProcessorDestinationList::GetDestProcAt( const ProcessorID & _destProc ) const 
{
	FLOWDESTVECT::const_iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::const_iterator iterEnd = m_vDestProcs.end();
	for(; iter != iterEnd; ++iter )
	{
		if( iter->GetProcID() == _destProc )
		{
			return *iter;
		}
	}

	ASSERT(0);
}
*/
CFlowDestination* CProcessorDestinationList::GetDestProcArcInfo( const ProcessorID &  _destProc )
{
	FLOWDESTVECT::iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::iterator iterEnd = m_vDestProcs.end();
	for(; iter != iterEnd; ++iter )
	{
//		// TRACE( "dest=%s\n", iter->GetProcID().GetIDString() );
		if( iter->GetProcID().idFits( _destProc ) )
		{
			return &(*iter);
		}
	}

	return NULL;
}

CFlowDestination* CProcessorDestinationList::GetDestProcArcInfo( int _iIndex )
{
	if (_iIndex < 0 || _iIndex >= static_cast<int>(m_vDestProcs.size()))
		return NULL;

	return & m_vDestProcs[ _iIndex ];	
}

bool CProcessorDestinationList::operator == ( const CProcessorDestinationList& _another ) const
{
	if( m_procID == _another.m_procID )
	{
		if( m_vDestProcs.size() != _another.m_vDestProcs.size() )
			return false;

		int iDestCount = m_vDestProcs.size();
		for( int i=0; i<iDestCount; ++i )
		{
			if( m_vDestProcs[i] != _another.m_vDestProcs[i] )
				return false;
		}

		return true;
		
	}
	else
	{
		return false;
	}
}
void CProcessorDestinationList::SetAllDestVisitFlag( bool _bVisit )
{
	FLOWDESTVECT::iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::iterator iterEnd = m_vDestProcs.end();
	for(; iter != iterEnd; ++iter )
	{
		iter->SetVisitFlag( _bVisit );
	}
}
void CProcessorDestinationList::SetAllDestInherigeFlag( int _iType )
{
	FLOWDESTVECT::iterator iter = m_vDestProcs.begin();
	FLOWDESTVECT::iterator iterEnd = m_vDestProcs.end();
	for(; iter != iterEnd; ++iter )
	{
		iter->SetTypeOfOwnership( _iType );
	}
}
bool CProcessorDestinationList::IfTotal100Percent() const
{
	if( m_vDestProcs.size() <= 0 )
		return true;
	if( !IsStation( m_procID ) )
	{
		int iTotalPercent = 0 ;
		FLOWDESTVECT::const_iterator iter = m_vDestProcs.begin();
		FLOWDESTVECT::const_iterator iterEnd = m_vDestProcs.end();
		for(; iter != iterEnd; ++iter )
		{
			iTotalPercent += iter->GetProbality();
		}

		return iTotalPercent == 100;
	}
	else
	{
		int iStationTotal = 0;
		int iNonStationTotal = 0;
		bool bHasStation = false;
		bool bHasNonStation = false;
		FLOWDESTVECT::const_iterator iter = m_vDestProcs.begin();
		FLOWDESTVECT::const_iterator iterEnd = m_vDestProcs.end();
		for(; iter != iterEnd; ++iter )
		{
			if( IsStation(iter->GetProcID() ) )
			{
				iStationTotal += iter->GetProbality();
				bHasStation = true;
			}
			else
			{
				iNonStationTotal += iter->GetProbality();
				bHasNonStation = true;
			}
			 
		}
		if( bHasStation )
		{
			if( iStationTotal != 100 )
				return false;
		}			

		if( bHasNonStation )
		{
			if( iNonStationTotal != 100 )
				return false;
		} 

		return true;	
	}	
}
bool CProcessorDestinationList::IsStation( const ProcessorID& _procID ) const 
{
	ASSERT( m_pTerm );
	GroupIndex gIndex = m_pTerm->procList->getGroupIndex( _procID );
	Processor* pProc = m_pTerm->procList->getProcessor( gIndex.start );
	if( pProc == NULL )
		return false;
	return pProc->getProcessorType() == IntegratedStationProc;
}

bool CProcessorDestinationList::FlowDistinationUnVisit()const
{
	int iDestCount = GetDestCount();
	for( int i=0; i<iDestCount; ++i )
	{
		const CFlowDestination& destArc = GetDestProcAt( i );
		if (!destArc.GetVisitFlag())
		{
			return true;
		}
	}
	return false;
}