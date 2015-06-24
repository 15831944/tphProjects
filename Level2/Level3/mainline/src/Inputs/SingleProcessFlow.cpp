#include "StdAfx.h"
#include ".\singleprocessflow.h"
#include "IN_TERM.H"
#include "Engine/PROCLIST.H"

CSingleProcessFlow::CSingleProcessFlow(void)
{
	m_pStartPair = NULL;
}
CSingleProcessFlow::CSingleProcessFlow( InputTerminal* _pTerm )
			: CSinglePaxTypeFlow(_pTerm)
{
	m_pStartPair = NULL;
}

CSingleProcessFlow::~CSingleProcessFlow(void)
{
	if(m_pStartPair != NULL)
	{
		delete m_pStartPair;
		m_pStartPair = NULL;
	}
}
CSingleProcessFlow::CSingleProcessFlow( const CSinglePaxTypeFlow& _anotherPaxFlow )
:CSinglePaxTypeFlow(_anotherPaxFlow)
{	
	m_pStartPair = new CProcessorDestinationList(GetInputTerm());
	m_pStartPair->SetProcID(((CSingleProcessFlow*)&_anotherPaxFlow)->GetStartPair()->GetProcID());
	*m_pStartPair = *((CSingleProcessFlow*)&_anotherPaxFlow)->GetStartPair();		
}

CProcessorDestinationList* CSingleProcessFlow::GetStartPair() const
{ 
	return m_pStartPair; 
}

void CSingleProcessFlow::SetStartPair(CProcessorDestinationList* pStartPair)
{
	m_pStartPair = pStartPair;
}

// add all destnition proccessors of _oldDestProcId to _soruceProcID and remove all destproc from _oldDestProcId , then remove _oldDestProcID
void CSingleProcessFlow::TakeOverDestProc( const  ProcessorID& _sourceProcID, const ProcessorID& _oldDestProcID , bool _bEraseOldProc )
{
	PAXFLOWIDGRAPH::iterator iterFirstProc = m_vPaxFlowDigraph.end();
	PAXFLOWIDGRAPH::iterator iterSecondProc = m_vPaxFlowDigraph.end();

	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();

	if (m_pStartPair->GetProcID() == _sourceProcID)
	{
		iterFirstProc = PAXFLOWIDGRAPH::iterator(m_pStartPair, &m_vPaxFlowDigraph);
	}

	for( ; iter != iterEnd; ++iter )
	{
		if(!(m_pStartPair->GetProcID() == _sourceProcID) && iter->GetProcID() == _sourceProcID )
		{
			iterFirstProc = iter;
			if( _sourceProcID == _oldDestProcID )// remove root.
			{
				m_vPaxFlowDigraph.erase( iter );
				return;
			}
		}
		else if( iter->GetProcID() == _oldDestProcID )
		{
			iterSecondProc = iter;
		}

		if( iterFirstProc != m_vPaxFlowDigraph.end() && iterSecondProc != m_vPaxFlowDigraph.end() )
			break;
	}

	if (iterFirstProc != m_vPaxFlowDigraph.end())
		iterFirstProc->DeleteDestProc( _oldDestProcID );

	if (iterFirstProc != m_vPaxFlowDigraph.end() && iterSecondProc != m_vPaxFlowDigraph.end())
	{
		int iDestCount = iterSecondProc->GetDestCount();
		for( int i=0; i < iDestCount ; ++i )
		{
			iterSecondProc->GetDestProcArcInfo( i )->SetTypeOfOwnership( 0 );
			iterFirstProc->AddDestProc( iterSecondProc->GetDestProcAt( i ) );
		}
	}


	int iSourceCount = GetSourceProcCount( _oldDestProcID );
	if( iSourceCount >= 1 )
	{
		ClearIsolateNode();
	}
	else if (iterSecondProc != m_vPaxFlowDigraph.end())
	{
		DeleteProcNode( iterSecondProc->GetProcID() );
	}

	m_bHasChanged = true;
}

void CSingleProcessFlow::ClearIsolateNode()
{
	std::vector<int> vErasedPos;
	PAXFLOWIDGRAPH::iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::iterator iterEnd = m_vPaxFlowDigraph.end();
	if (m_pTerm == NULL)
		return;

	for(int i=0 ; iter != iterEnd; ++iter ,++i )
	{	
	//	TRACE0(iter->GetProcID().GetIDString()+ "\n");

		if (m_pTerm->procList->getGroupIndexOriginal(iter->GetProcID()).start == START_PROCESSOR_INDEX)
			continue;

		if (m_pStartPair && m_pStartPair->GetDestProcArcInfo(iter->GetProcID()))
			continue;
		
		int iSourceProcCount = GetSourceProcCount( iter->GetProcID() );
		if( iSourceProcCount <= 0 )
		{
			if( iter->GetDestProcCount() <= 0)
			{
				vErasedPos.push_back( i );
			}
		}
	}

	int iErasedPosCount = vErasedPos.size();
	for( int j=iErasedPosCount-1; j >= 0; --j )
	{
		m_vPaxFlowDigraph.erase( m_vPaxFlowDigraph.begin() + vErasedPos[ j ]  );
	}
}

bool CSingleProcessFlow::IfSubFlowValid(const CString& sProcessName) const
{
	if (m_pStartPair && !m_pStartPair->IfTotal100Percent())
	{
		CString sErrorMsg = " Process Flow : ";
		sErrorMsg += sProcessName;
		sErrorMsg += ".\n Processor: ";
		sErrorMsg += m_pStartPair->GetProcID().GetIDString();
		sErrorMsg += ".\n Total Destination Distribution Probability is not 100% !";
		AfxMessageBox( sErrorMsg);
		return false;
	}

	PAXFLOWIDGRAPH::const_iterator iter = m_vPaxFlowDigraph.begin();
	PAXFLOWIDGRAPH::const_iterator iterEnd = m_vPaxFlowDigraph.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( !iter->IfTotal100Percent() )
		{	
			CString sErrorMsg = " Process Flow : ";
			sErrorMsg += sProcessName;
			sErrorMsg += ".\n Processor: ";
			sErrorMsg += iter->GetProcID().GetIDString();
			sErrorMsg += ".\n Total Destination Distribution Probability is not 100% !";
			AfxMessageBox( sErrorMsg);
			return false;
		}			
	}
	return true;
}
