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

void CSingleProcessFlow::InsertBetwwen(const ProcessorID & _firstProcID, const ProcessorID & _secondProcID, const CFlowDestination& _insertProcID)
{
    if(_firstProcID == _insertProcID.GetProcID() ||
        _secondProcID == _insertProcID.GetProcID() ||
        _firstProcID == _secondProcID)
        return;

    if(!IfExist(_insertProcID.GetProcID()))
    {
        AddIsolatedProc(_insertProcID.GetProcID());
    }

    CProcessorDestinationList* pFirstProcDestList = NULL;
    if(_firstProcID.GetIDString().CompareNoCase(m_pStartPair->GetProcID().GetIDString()) == 0)
    {
        pFirstProcDestList = m_pStartPair;
    }
    else
    {
        pFirstProcDestList = FindProcDestinationListByProcID(_firstProcID);
    }
    ASSERT(pFirstProcDestList);

    CProcessorDestinationList* pSecondProcDestList = FindProcDestinationListByProcID(_secondProcID);
    ASSERT(pSecondProcDestList);

    CProcessorDestinationList* pInsertProcDestList = FindProcDestinationListByProcID(_insertProcID.GetProcID());
    ASSERT(pInsertProcDestList);

    // _secondProcID must in the destination list of _firstProcID
    CFlowDestination* pDestInfo = pFirstProcDestList->GetDestProcArcInfo(_secondProcID);
    ASSERT(pDestInfo);

    // now we will add _secondProcID to the destination list of _insertProcID
    CFlowDestination tempInfo = *pDestInfo;
    tempInfo.SetTypeOfOwnership(0);
    pInsertProcDestList->AddDestProc(tempInfo);

    // and then remove _secondProcID from the destination list of _firstProcID
    pFirstProcDestList->DeleteDestProc(_secondProcID);

    // and then add _insertProcID to the destination list of _firstProcID
    CFlowDestination insertFlowDest;
    insertFlowDest.SetProcID(_insertProcID.GetProcID());
    pFirstProcDestList->AddDestProc(insertFlowDest);

    m_bHasChanged = true;
    return;
}

void CSingleProcessFlow::InsertProceoosorAfter(const ProcessorID& _sourceProcID, const ProcessorID& _insertProcID)
{
    if(_sourceProcID == _insertProcID)
        return;

    if(!IfExist(_insertProcID))
    {
        AddIsolatedProc(_insertProcID);
    }

    CProcessorDestinationList* pSourceProcDestList = NULL;
    if(_sourceProcID.GetIDString().CompareNoCase(m_pStartPair->GetProcID().GetIDString()) == 0)
    {
        pSourceProcDestList = m_pStartPair;
    }
    else
    {
        pSourceProcDestList = FindProcDestinationListByProcID(_sourceProcID);
    }
    ASSERT(pSourceProcDestList);

    CProcessorDestinationList* pNewProcDestList = FindProcDestinationListByProcID(_insertProcID);
    ASSERT(pNewProcDestList);

    int iDestCount = pSourceProcDestList->GetDestProcCount();
    for(int i=0; i<iDestCount; i++)
    {
        //iterSource->GetDestProcArcInfo(i)->SetInheritedFlag(false);
        CFlowDestination* pDest = pSourceProcDestList->GetDestProcArcInfo(i);
        pDest->SetTypeOfOwnership(0);
        pNewProcDestList->AddDestProc(*pDest, true);
    }

    pSourceProcDestList->ClearAllDestProc();
    CFlowDestination temp;
    temp.SetProcID(_insertProcID);
    pSourceProcDestList->AddDestProc(temp, true);
    m_bHasChanged = true;
}

CProcessorDestinationList* CSingleProcessFlow::FindProcDestinationListByProcID(const ProcessorID& procID)
{
    PAXFLOWIDGRAPH::iterator iter;
    for(iter=m_vPaxFlowDigraph.begin(); iter!=m_vPaxFlowDigraph.end(); ++iter)
    {
        if(iter->GetProcID() == procID)
        {
            return &(*iter);
        }
    }
    return NULL;
}

