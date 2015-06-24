// SubFlow.cpp: implementation of the CSubFlow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SubFlow.h"
#include "SinglePaxTypeFlow.h"
#include "SingleProcessFlow.h"
#include "common\termfile.h"
#include "AllPaxTypeFlow.h"
#include "inputs\paxflow.h"
#include "engine\person.h" 
#include "Engine\Terminal.h"
#include "Engine\Rule.h"
#include "SimParameter.h"
#include "Bagdata.h"
#include "Procdata.h"
#include "FlowItemEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CreateSourceToDestGeometry(Processor* pSource, Processor* pDest, double* dAlt);


CSubFlow::CSubFlow( InputTerminal* _pTerm )
{
	m_pInputTerm = _pTerm;
	m_pInternalFlow = (CSinglePaxTypeFlow*)new CSingleProcessFlow( _pTerm );
	m_pTransition = NULL;
	m_pTransitionGateDetail = NULL;
    m_pStationTransition = NULL;
}
CSubFlow::CSubFlow( const CSubFlow& _another )
{
	m_pInputTerm = _another.m_pInputTerm;
	m_sProcessUnitName = _another.m_sProcessUnitName;
	m_pInternalFlow = (CSinglePaxTypeFlow*)new CSingleProcessFlow( *_another.m_pInternalFlow );

	m_pTransition = _another.m_pTransition;
	m_pTransitionGateDetail = _another.m_pTransitionGateDetail;
    m_pStationTransition = _another.m_pStationTransition;

	int iSize = _another.m_vAllPossiblePath.size();
	for( int i=0; i<iSize; ++i )
	{
		m_vAllPossiblePath.push_back( new CSinglePaxTypeFlow( * ( _another.m_vAllPossiblePath[ i ] ) ) );
	}

	PAXTYPE_TO_PAXFLOW::const_iterator iter = _another.m_mapPaxWithBagToAllFlow.begin();
	PAXTYPE_TO_PAXFLOW::const_iterator iterEnd = _another.m_mapPaxWithBagToAllFlow.end();
	for( ; iter != iterEnd ;++iter )
	{
		m_mapPaxWithBagToAllFlow.insert( PAXTYPE_TO_PAXFLOW::value_type( iter->first, new CAllPaxTypeFlow( * (iter->second) ) ) );
	}


	iter = _another.m_mapPaxWithoutBagToAllFlow.begin();
	iterEnd = _another.m_mapPaxWithoutBagToAllFlow.end();
	for( ; iter != iterEnd ;++iter )
	{
		m_mapPaxWithoutBagToAllFlow.insert( PAXTYPE_TO_PAXFLOW::value_type( iter->first, new CAllPaxTypeFlow( * (iter->second) ) ) );
	}

}
CSubFlow& CSubFlow::operator = (  const CSubFlow& _another )
{
	if( this != &_another )
	{
		m_sProcessUnitName = _another.m_sProcessUnitName;

		Clear();
		m_pInputTerm = _another.m_pInputTerm;
		m_pInternalFlow = new CSinglePaxTypeFlow (  *_another.m_pInternalFlow );
		m_pTransition = _another.m_pTransition;
		m_pTransitionGateDetail = _another.m_pTransitionGateDetail;
	    m_pStationTransition = _another.m_pStationTransition;

		int iSize = _another.m_vAllPossiblePath.size();
		for( int i=0; i<iSize; ++i )
		{
			m_vAllPossiblePath.push_back( new CSinglePaxTypeFlow( *_another.m_vAllPossiblePath[ i ] ) );
		}	

		PAXTYPE_TO_PAXFLOW::const_iterator iter = _another.m_mapPaxWithBagToAllFlow.begin();
		PAXTYPE_TO_PAXFLOW::const_iterator iterEnd = _another.m_mapPaxWithBagToAllFlow.end();
		for( ; iter != iterEnd ;++iter )
		{
			m_mapPaxWithBagToAllFlow.insert( PAXTYPE_TO_PAXFLOW::value_type( iter->first, new CAllPaxTypeFlow( * (iter->second) ) ) );
		}

		iter = _another.m_mapPaxWithoutBagToAllFlow.begin();
		iterEnd = _another.m_mapPaxWithoutBagToAllFlow.end();
		for( ; iter != iterEnd ;++iter )
		{
			m_mapPaxWithoutBagToAllFlow.insert( PAXTYPE_TO_PAXFLOW::value_type( iter->first, new CAllPaxTypeFlow( * (iter->second) ) ) );
		}
		
	}
	return *this;
}
CSubFlow::~CSubFlow()
{
	Clear();
}
void CSubFlow::Clear()
{
	delete m_pInternalFlow;
	m_pInternalFlow = NULL;

	int iSize = m_vAllPossiblePath.size();
	for( int i=0; i<iSize; ++i )
	{
		delete m_vAllPossiblePath[ i ];
	}
	m_vAllPossiblePath.clear();

	PAXTYPE_TO_PAXFLOW::iterator iter = m_mapPaxWithBagToAllFlow.begin();
	PAXTYPE_TO_PAXFLOW::iterator iterEnd = m_mapPaxWithBagToAllFlow.end();
	for( ; iter != iterEnd ;++iter )
	{
		if( iter->second )
		{
			iter->second->ClearAll();
			delete iter->second;
		}
	}
	m_mapPaxWithBagToAllFlow.clear();

	iter = m_mapPaxWithoutBagToAllFlow.begin();
	iterEnd = m_mapPaxWithoutBagToAllFlow.end();
	for( ; iter != iterEnd ;++iter )
	{
		if( iter->second )
		{
			iter->second->ClearAll();
			delete iter->second;
		}
	}
	m_mapPaxWithoutBagToAllFlow.clear();

	
}
void CSubFlow::SetInputTerm( InputTerminal* _pTerm )
{
	m_pInternalFlow->SetInputTerm( _pTerm ) ;
}

void CSubFlow::ReadData (ArctermFile& p_file)
{
	p_file.getLine();
	char tempChar[ 256 ];
	p_file.getField( tempChar,256 );
	m_sProcessUnitName = tempChar;
	int iFlowPairCount = 0;
	p_file.getInteger( iFlowPairCount );

	ProcessorID tempID;
	tempID.SetStrDict( m_pInternalFlow->GetInputTerm()->inStrDict );

	for( int i=0; i<iFlowPairCount; ++i )
	{
		CProcessorDestinationList* pSinglePair = new CProcessorDestinationList(m_pInternalFlow->GetInputTerm());
		pSinglePair->ClearAllDestProc();

		p_file.getLine();
		tempID.readProcessorID( p_file );
		pSinglePair->SetProcID( tempID );

		int iDestCount = 0;
		p_file.getLine();
		p_file.getInteger( iDestCount );

		for( int j=0; j<iDestCount; ++j )
		{
			CFlowDestination singleDest;
			p_file.getLine();

			tempID.readProcessorID( p_file );
			singleDest.SetProcID( tempID );

			int iOneToOneFlag=-1;
			p_file.getInteger( iOneToOneFlag );
			singleDest.SetOneToOneFlag( iOneToOneFlag );

			int iOneXOneFlag = 0;
			p_file.getInteger( iOneXOneFlag );
			singleDest.SetOneXOneState(iOneXOneFlag);

			int iProbability = 0;
			p_file.getInteger(iProbability);
			singleDest.SetProbality(iProbability);

			int iPipeType = 0;
			p_file.getInteger(iPipeType);
			singleDest.SetTypeOfUsingPipe(iPipeType);

			int iPipeCount = 0;
			p_file.getInteger(iPipeCount);
			for(int nCount = 0; nCount < iPipeCount; nCount++)
			{
				int iIndex = 0;
				p_file.getInteger(iIndex);
				singleDest.AddUsedPipe(iIndex);
			}

			pSinglePair->AddDestProc( singleDest ,false );
		}

		pSinglePair->Total100Percent();
		if(i == 0)//start pair
		{
			((CSingleProcessFlow*)m_pInternalFlow)->SetStartPair(pSinglePair);
		}
		else
		{
			m_pInternalFlow->AddFowPair( *pSinglePair );
		}
	}

	std::vector<ProcessorID> vRoot;
	m_pInternalFlow->GetRootProc(vRoot);
	CProcessorDestinationList* pStartPair = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();

	for(int i = 0; i < (int)vRoot.size(); i++)
	{
		bool bFind = false;
		for(int j = 0; j < pStartPair->GetDestProcCount(); j++)
		{
			if(vRoot[i] == pStartPair->GetDestProcAt(j).GetProcID())
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			m_pInternalFlow->DeleteProcNode(vRoot[i]);
		}
	}
}

void CSubFlow::readObsoleteData21(ArctermFile& p_file)
{
	p_file.getLine();
	char tempChar[ 256 ];
	p_file.getField( tempChar,256 );
	m_sProcessUnitName = tempChar;
	int iFlowPairCount = 0;
	p_file.getInteger( iFlowPairCount );

	ProcessorID tempID;
	tempID.SetStrDict( m_pInternalFlow->GetInputTerm()->inStrDict );

	CProcessorDestinationList* pSinglePair = new CProcessorDestinationList(m_pInternalFlow->GetInputTerm());

	for( int i=0; i<iFlowPairCount; ++i )
	{
		pSinglePair->ClearAllDestProc();

		p_file.getLine();
		tempID.readProcessorID( p_file );
		pSinglePair->SetProcID( tempID );

		int iDestCount = 0;
		p_file.getLine();
		p_file.getInteger( iDestCount );

		for( int j=0; j<iDestCount; ++j )
		{
			CFlowDestination singleDest;
			p_file.getLine();

			tempID.readProcessorID( p_file );
			singleDest.SetProcID( tempID );

			int iOneToOneFlag=-1;
			p_file.getInteger( iOneToOneFlag );
			singleDest.SetOneToOneFlag( iOneToOneFlag );

			//singleDest.SetProbality(100/iDestCount);
			//if(j == (iDestCount-1))
			//	singleDest.SetProbality(100/iDestCount+100%iDestCount);

			pSinglePair->AddDestProc( singleDest ,false );
		}

		pSinglePair->Total100Percent();

		m_pInternalFlow->AddFowPair( *pSinglePair );
	}

	//add by  05-11-17
	if(iFlowPairCount > 0)
	{
		pSinglePair->ClearAllDestProc();

		ProcessorID startID;
		startID.SetStrDict( m_pInternalFlow->GetInputTerm()->inStrDict );
		startID.setID("PROCESSSTART");

		pSinglePair->SetProcID( startID );

		std::vector<ProcessorID> vProcID;
		vProcID.clear();
		m_pInternalFlow->GetRootProc(vProcID);

		int nsize = (int)vProcID.size();
		if(nsize > 0)
		{
			for(int i = 0; i < nsize; i++)
			{
				CFlowDestination startDest;
				startID.setIDS(vProcID[i].getID());
				startDest.SetProcID( startID );
				startDest.SetOneToOneFlag(0);
				startDest.SetProbality(100/nsize);

				pSinglePair->AddDestProc( startDest ,false );	
			}
		}
		else
		{
			vProcID.clear();
			m_pInternalFlow->GetAllInvolvedProc( vProcID );
			if( vProcID.size() > 0 )// just have once cicle path, so it will be no root processor
			{
				CFlowDestination startDest;
				startID.setIDS(vProcID[0].getID());
				startDest.SetProcID( startID );
				startDest.SetOneToOneFlag(0);
				startDest.SetProbality(100);
				startDest.SetTypeOfUsingPipe(0);

				pSinglePair->AddDestProc( startDest ,false );	
			}
		}
		pSinglePair->Total100Percent();
		((CSingleProcessFlow*)m_pInternalFlow)->SetStartPair( pSinglePair );
	}
}
void CSubFlow::readObsoleteData24 (ArctermFile& p_file)
{
	p_file.getLine();
	char tempChar[ 256 ];
	p_file.getField( tempChar,256 );
	m_sProcessUnitName = tempChar;
	int iFlowPairCount = 0;
	p_file.getInteger( iFlowPairCount );

	ProcessorID tempID;
	tempID.SetStrDict( m_pInternalFlow->GetInputTerm()->inStrDict );

	for( int i=0; i<iFlowPairCount; ++i )
	{
		CProcessorDestinationList* pSinglePair = new CProcessorDestinationList(m_pInternalFlow->GetInputTerm());
		pSinglePair->ClearAllDestProc();

		p_file.getLine();
		tempID.readProcessorID( p_file );
		pSinglePair->SetProcID( tempID );

		int iDestCount = 0;
		p_file.getLine();
		p_file.getInteger( iDestCount );

		for( int j=0; j<iDestCount; ++j )
		{
			CFlowDestination singleDest;
			p_file.getLine();

			tempID.readProcessorID( p_file );
			singleDest.SetProcID( tempID );

			int iOneToOneFlag=-1;
			p_file.getInteger( iOneToOneFlag );
			singleDest.SetOneToOneFlag( iOneToOneFlag );

			int iProbability = 0;
			p_file.getInteger(iProbability);
			singleDest.SetProbality(iProbability);

			int iPipeType = 0;
			p_file.getInteger(iPipeType);
			singleDest.SetTypeOfUsingPipe(iPipeType);

			int iPipeCount = 0;
			p_file.getInteger(iPipeCount);
			for(int nCount = 0; nCount < iPipeCount; nCount++)
			{
				int iIndex = 0;
				p_file.getInteger(iIndex);
				singleDest.AddUsedPipe(iIndex);
			}

			pSinglePair->AddDestProc( singleDest ,false );
		}

		pSinglePair->Total100Percent();
		if(i == 0)//start pair
		{
			((CSingleProcessFlow*)m_pInternalFlow)->SetStartPair(pSinglePair);
		}
		else
		{
			m_pInternalFlow->AddFowPair( *pSinglePair );
		}
	}
}

void CSubFlow::WriteData (ArctermFile& p_file) const
{
	p_file.writeField( m_sProcessUnitName );
	int iFlowPairCount = m_pInternalFlow->GetFlowPairCount();
	if(iFlowPairCount > 0)//frank 05-11-17, add one for start pair
		iFlowPairCount++;
	p_file.writeInt( iFlowPairCount );
	p_file.writeLine();

	for( int i=0; i<iFlowPairCount; ++i )
	{
		CProcessorDestinationList* singlePair = NULL;
		if(i == 0)
		{
			singlePair = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();
			if(singlePair == NULL)
			{
				continue;				
			}
		}
		else
		{
			singlePair = m_pInternalFlow->GetFlowPairAt( i-1 );
		}
		singlePair->GetProcID().writeProcessorID( p_file );
		p_file.writeLine();

		int iDestCount = singlePair->GetDestCount();
		p_file.writeInt( iDestCount );
		p_file.writeLine();
		for( int j=0; j<iDestCount; ++j )
		{
			const CFlowDestination& singleDest = singlePair->GetDestProcAt( j );
			singleDest.GetProcID().writeProcessorID( p_file );
			p_file.writeInt( singleDest.GetOneToOneFlag() );
			p_file.writeInt( singleDest.GetOneXOneState() );
			p_file.writeInt( singleDest.GetProbality());
			p_file.writeInt( singleDest.GetTypeOfUsingPipe());
			int nCount = singleDest.GetUsedPipeCount();
			p_file.writeInt( nCount );
			for(int k = 0; k < nCount; k++)
			{
				p_file.writeInt(singleDest.GetPipeAt(k));
			}
			p_file.writeLine();			
		}		
	}
}

void CSubFlow::BuildAllPossiblePath()
{
	int iSize = m_vAllPossiblePath.size();
	for( int i=0; i<iSize; ++i )
	{
		delete m_vAllPossiblePath[ i ];
	}
	m_vAllPossiblePath.clear();

	m_pInternalFlow->BuildAllPossiblePath( m_vAllPossiblePath );

	//PrintGateInfo();
	
	PAXTYPE_TO_PAXFLOW::iterator iter = m_mapPaxWithBagToAllFlow.begin();
	PAXTYPE_TO_PAXFLOW::iterator iterEnd = m_mapPaxWithBagToAllFlow.end();
	for( ; iter != iterEnd ;++iter )
	{
		if( iter->second )
		{
			iter->second->ClearAll();
			delete iter->second;
		}
	}
	m_mapPaxWithBagToAllFlow.clear();

	iter = m_mapPaxWithoutBagToAllFlow.begin();
	iterEnd = m_mapPaxWithoutBagToAllFlow.end();
	for( ; iter != iterEnd ;++iter )
	{
		if( iter->second )
		{
			iter->second->ClearAll();
			delete iter->second;
		}
	}
	m_mapPaxWithoutBagToAllFlow.clear();

	BuildGateRelationships();
	AddLeadToGateRelationships();
	BuildBaggageRelationships();
}

// divide _tempFlow as a list of single tree ( one father - one child type tree.)
void CSubFlow::BuildAllPathOfSingleTree( const ProcessorID& _rootID , CSinglePaxTypeFlow& _tempFlow )
{
	CProcessorDestinationList* pFlowPair = m_pInternalFlow->GetFlowPairAt( _rootID );
	int iDestCount = pFlowPair->GetDestCount();
	if( iDestCount <=0 )// leaf node 
	{
		CSinglePaxTypeFlow* pFlow = new CSinglePaxTypeFlow( _tempFlow );
		m_vAllPossiblePath.push_back( pFlow );
		//pFlow->PrintAllStructure();
		_tempFlow.DeleteProcNode( _rootID );
	}
	else
	{
		for( int i=0; i<iDestCount; ++i )
		{
			const CFlowDestination& destArc = pFlowPair->GetDestProcAt( i );
			_tempFlow.AddDestProc( _rootID, destArc, false );
			BuildAllPathOfSingleTree( destArc.GetProcID(), _tempFlow );
		}
		_tempFlow.DeleteProcNode( _rootID );
	}
	
}

CAllPaxTypeFlow* CSubFlow::GetAllPossibleFlowByPaxType( const CMobileElemConstraint& _constraint , bool _bHasGate ,const ProcessorID& _gateID, bool _bHaveBag, const ProcessorID& _bagDeviceID,const CMobileElemConstraint& _leadToConstraint)
{
	CAllPaxTypeFlow* pFlow = NULL;

	if( _bHaveBag )
	{
		//int iSize = m_mapPaxWithBagToAllFlow.size();
		 // char tempChar [ 512 ]; matt
		char tempChar [2560];
		_constraint.WriteSyntaxStringWithVersion( tempChar );
		CString sKey ( tempChar );
		_leadToConstraint.WriteSyntaxStringWithVersion( tempChar );
		sKey += tempChar;
		pFlow = m_mapPaxWithBagToAllFlow[ sKey ];
		if( !pFlow )
		{
			pFlow = new CAllPaxTypeFlow();
			bool bOnlyHaveOwnBaggageDeviceFlow = false;
			int iPathCount = m_vAllPossiblePath.size();
			for( int i=0; i<iPathCount; ++i )
			{
				//
				if( m_vAllPossiblePath[ i ]->IfBaggageProcExistInFlow() )
				{
					int iPos = m_vAllPossiblePath[ i ]->IfProcessGroupExist( _bagDeviceID );
					if( iPos >= 0 )
					{
						bOnlyHaveOwnBaggageDeviceFlow =	true;
						CSinglePaxTypeFlow* pSinglePath = new CSinglePaxTypeFlow( *m_vAllPossiblePath[ i ] );
						//pSinglePath->PrintAllStructure();
						
						ReplaceProcessorInPath( pSinglePath, iPos , _bagDeviceID );
						//pSinglePath->PrintAllStructure();
						if( _bHasGate )
						{
							if( pSinglePath->IfGateProcExistInFlow() )
							{
								iPos = pSinglePath->IfProcessGroupExist( _gateID );
								if( iPos >= 0 )
								{
									ReplaceProcessorInPath( pSinglePath, iPos , _gateID );
									if( CanLeadToGate( pSinglePath, _gateID ,_constraint ) )
									{
										pFlow->AddPaxFlow( pSinglePath, false );
									}
									else
									{
										delete pSinglePath;
										pSinglePath = NULL;
									}
								}
								else
								{
									delete pSinglePath;
									pSinglePath = NULL;
								}
							}
							else
							{
								if( CanLeadToGate( pSinglePath, _gateID ,_constraint ) )
								{
									pFlow->AddPaxFlow( pSinglePath, false );
								}
								else
								{
									delete pSinglePath;
									pSinglePath = NULL;
								}
							}
						}
						else// don not have gate
						{
							pFlow->AddPaxFlow( pSinglePath, false );
						}
						
					}
					
				}
				else// baggage proc donnot exsit in path.
				{
					CSinglePaxTypeFlow* pSinglePath = new CSinglePaxTypeFlow( *m_vAllPossiblePath[ i ] );
					if( _bHasGate )
					{
						if( pSinglePath->IfGateProcExistInFlow() )
						{
							int iPos = pSinglePath->IfProcessGroupExist( _gateID );
							if( iPos >= 0 )
							{
								ReplaceProcessorInPath( pSinglePath, iPos , _gateID );
								if( CanLeadToGate( pSinglePath, _gateID ,_constraint ) )
								{
									pFlow->AddPaxFlow( pSinglePath, false );
								}
								else
								{
									delete pSinglePath;
									pSinglePath = NULL;
								}
							}
							else
							{
								delete pSinglePath;
								pSinglePath = NULL;
							}
						}
						else
						{
							if( CanLeadToGate( pSinglePath, _gateID ,_constraint ) )
							{
								pFlow->AddPaxFlow( pSinglePath, false );
							}
							else
							{
								delete pSinglePath;
								pSinglePath = NULL;
							}
						}
					}
					else// don not have gate
					{
						pFlow->AddPaxFlow( pSinglePath, false );
					}
				}				
			}

			if( bOnlyHaveOwnBaggageDeviceFlow )
			{
				iPathCount = pFlow->GetSinglePaxTypeFlowCount();
				for( int j= iPathCount -1 ; j>=0; --j )
				{
					if( !pFlow->GetSinglePaxTypeFlowAt( j )->IfBaggageProcExistInFlow() )
					{
						pFlow->DeleteSinglePaxTypeFlowAt( j, false );
					}
				}
			}

			m_mapPaxWithBagToAllFlow[ sKey ] = pFlow;
		}

		return pFlow;
	}
	else
	{
	//	pFlow = m_mapPaxWithoutBagToAllFlow[ _constraint ];
		//char tempChar [ 512 ] ; matt
		char tempChar [2560];
		_constraint.WriteSyntaxStringWithVersion( tempChar );
		CString sKey ( tempChar );
		_leadToConstraint.WriteSyntaxStringWithVersion( tempChar );
		sKey += tempChar;
		
		pFlow = m_mapPaxWithoutBagToAllFlow[ sKey ];
		if( !pFlow )
		{
			pFlow = new CAllPaxTypeFlow();

			int iPathCount = m_vAllPossiblePath.size();
			for( int i=0; i<iPathCount; ++i )
			{
				if( !m_vAllPossiblePath[ i ]->IfBaggageProcExistInFlow() )				
				{
					CSinglePaxTypeFlow* pSinglePath = new CSinglePaxTypeFlow( *m_vAllPossiblePath[ i ] );
					if( _bHasGate )
					{
						if( pSinglePath->IfGateProcExistInFlow() )
						{
							int iPos = pSinglePath->IfProcessGroupExist( _gateID );
							if( iPos >= 0 )
							{
								ReplaceProcessorInPath( pSinglePath, iPos , _gateID );
								if( CanLeadToGate( pSinglePath, _gateID ,_constraint ) )
								{
									pFlow->AddPaxFlow( pSinglePath, false );
								}
								else
								{
									delete pSinglePath;
									pSinglePath = NULL;
								}
							}
							else
							{
								delete pSinglePath;
								pSinglePath = NULL;
							}
						}
						else
						{
							if( CanLeadToGate( pSinglePath, _gateID ,_constraint ) )
							{
								pFlow->AddPaxFlow( pSinglePath, false );
							}
							else
							{
								delete pSinglePath;
								pSinglePath = NULL;
							}
						}
					}
					else// don not have gate
					{
						pFlow->AddPaxFlow( pSinglePath, false );
					}
				}				
			}

			m_mapPaxWithoutBagToAllFlow[ sKey ] = pFlow;
		}

		return pFlow;
	}
	
}

bool CSubFlow::CanLeadToGate( CSinglePaxTypeFlow* _pFlow, const ProcessorID& _gateID ,const CMobileElemConstraint& _leadToConstraint)const
{
	bool bNeedToCheckFlowLeadToGate = true;
	if( _leadToConstraint.GetTypeIndex() >= 0 || ( _leadToConstraint.isArrival() && ! _leadToConstraint.isTurround()) )// is nonpax type
	{
		bNeedToCheckFlowLeadToGate = false;
	}
	InputTerminal* pTerm = _pFlow->GetInputTerm();

	int iGateIdx = pTerm->procList->getGroupIndex( _gateID ).start;// _gateID is a single processor
	ASSERT( iGateIdx >=0 );
//	// TRACE("\n%s\n", _gateID.GetIDString() );
	std::vector<ProcessorID>vRoot;
	_pFlow->GetRootProc( vRoot );
	std::vector<ProcessorID>vPathProc;
	TravelPath( _pFlow, vRoot[0], vPathProc );// only have one root
	int iPathProcCount = vPathProc.size();

	for( int i=0; i<iPathProcCount; ++i )
	{
		GroupIndex groupIdx =  pTerm->procList->getGroupIndex( vPathProc[i] );
		ASSERT( groupIdx.start >= 0 );
		bool bAtLeastOneCanLeadTo = false;
		for( int j=groupIdx.start; j<=groupIdx.end; ++j )
		{
			Processor* pProc = pTerm->procList->getProcessor( j );
//			// TRACE("\n%s\n", pProc->getID()->GetIDString() );
			if( pProc->leadsToGate( iGateIdx ) )
			{
				if( bNeedToCheckFlowLeadToGate )
				{
					if( pProc->CanLeadTo( _leadToConstraint , _gateID ) )
					{
						bAtLeastOneCanLeadTo = true;
						break;
					}
					
				}
				else
				{
					bAtLeastOneCanLeadTo = true;
					break;
				}
				
			}
		}

		if( !bAtLeastOneCanLeadTo )
		{
			return false;
		}
	}

	return true;
}
//update _psingleFlow's specific node according to one to one relationship 
void CSubFlow::ReplaceProcessorInPath( CSinglePaxTypeFlow* _pSingleFlow, int _iStartPos ,const ProcessorID& _replacedByProcID )
{
///	_pSingleFlow->PrintAllStructure();
	CProcessorDestinationList* pPair = _pSingleFlow->GetFlowPairAt( _iStartPos );
	const ProcessorID tempID( pPair->GetProcID() );
//	// TRACE("\n tempID = %s\n", tempID.GetIDString() );
	pPair->SetProcID( _replacedByProcID );

//	// TRACE("\n _replacedByProcID = %s\n", _replacedByProcID.GetIDString() );
	pPair = GetSourcePair( _pSingleFlow, tempID );
	if( pPair )
	{
		pPair->GetDestProcArcInfo( 0 )->SetProcID( _replacedByProcID );
		if( pPair->GetDestProcArcInfo( 0 )->GetOneToOneFlag() )
		{
			CString csLeafName = _replacedByProcID.GetLeafName();
			int iIDLength = _replacedByProcID.idLength();
			//csLeafName.GetBuffer(
			if( iIDLength >=1  )
			{
				ProcessorID oneToOneID;
				if( GetOneToOneProcessorID( pPair->GetProcID(), _replacedByProcID, oneToOneID  ) )
				{
					int iPairIdx = _pSingleFlow->GetFlowPairIdx( pPair->GetProcID() );
					ASSERT( iPairIdx != -1 );
					ReplaceProcessorInPath( _pSingleFlow, iPairIdx, oneToOneID );
				}
				else
				{					
					throw new ARCError( "In PROCESS: " + m_sProcessUnitName + ", Processor : " +
						_replacedByProcID.GetIDString() + " don not have one to one Processor  ");// one to one error
				}				
			}	
			else
			{
				throw new ARCError( "In PROCESS: " + m_sProcessUnitName + ", Processor : " +
						_replacedByProcID.GetIDString() + " don not have one to one Processor  ");// one to one error
			}
		}
	}
}


//get a specific processorid in sourceid grop which has one to one relationship with dest processorid 
bool CSubFlow::GetOneToOneProcessorID( const ProcessorID& _sourceID, const ProcessorID& _destID, ProcessorID& _resutID )const
{
//	// TRACE("\n _sourceID = %s\n", _sourceID.GetIDString() );
//	// TRACE("\n _destID = %s\n", _destID.GetIDString() );

	InputTerminal* pTerm =m_pInternalFlow->GetInputTerm();
	GroupIndex group = pTerm->procList->getGroupIndex( _sourceID );
	if( group.start == -1 )
		return false;
	ProcessorArray tempArray;
	for( int i=group.start; i<=group.end; ++i )
	{
//		// TRACE("\n%s\n", pTerm->procList->getProcessor( i )->getID()->GetIDString() );
		tempArray.addItem( pTerm->procList->getProcessor( i ) );
	}

	Processor* pProcOneToOne = tempArray.GetOneToOneProcessor( &_destID, _sourceID.idLength() );
	if( pProcOneToOne )
	{
//		// TRACE("\n%s\n", pProcOneToOne->getID()->GetIDString() );
		_resutID = *pProcOneToOne->getID() ;
		return true;
	}

	return false;
}

// return _procID 's father flow pair in _pSingleFlow
CProcessorDestinationList* CSubFlow::GetSourcePair( CSinglePaxTypeFlow* _pSingleFlow, const ProcessorID& _procID )
{
	int iPairCount = _pSingleFlow->GetFlowPairCount();
	for( int i=0; i<iPairCount; ++i )
	{
		CProcessorDestinationList* pPair = _pSingleFlow->GetFlowPairAt( i );
		int iDestCount = pPair->GetDestCount();
		if( iDestCount > 0 )
		{
			//because one source just has one dest in this situation
			if( pPair->GetDestProcAt( 0 ).GetProcID() == _procID  )
			{
				return pPair;
			}
		}
	}

	return NULL;
	
}

int CSubFlow::GetDestinationIndex(CProcessorDestinationList* pProcList)
{
	std::vector<int> vPercent;
	int nCount = pProcList->GetDestProcCount();
	int i = 0;
	int iCount = 0;
	for(i = 0; i < nCount; i++)
	{
		int iPercent = pProcList->GetDestProcArcInfo(i)->GetProbality();
		iCount += iPercent;
		vPercent.push_back(iCount);
	}

	int nIndex = random(vPercent.back());

	//get index
	for(i = 0; i < (int)vPercent.size(); i++)
	{
		if(nIndex < vPercent[i])
		{
			nIndex = i;
			break;
		}
	}	

	return nIndex;
}

int CSubFlow::GetPathIndexInProcess(CAllPaxTypeFlow * pFlow, int iPathCount)
{
	//--------------------------------added by frank 05-11-17
	if(pFlow == NULL)
		return -1;
	if(iPathCount <= 0)
		return -1;

	std::vector<ProcessorID> vRooot;
	m_pInternalFlow->GetRootProc(vRooot);
	if(vRooot.size() <= 0)
		m_pInternalFlow->GetAllInvolvedProc(vRooot);
	if(vRooot.size() <= 0)
		return -1;
	CProcessorDestinationList* pFlowPair = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();
	if(pFlowPair == NULL)
		return -1;

	int i = 0;
	int iCount = 0;

	//get percent 
	std::vector<int> vPercent;
	for(i = 0; i < iPathCount; i++)
	{
		CSinglePaxTypeFlow* pSingleFlow = pFlow->GetSinglePaxTypeFlowAt(i);
		std::vector<ProcessorID> vSingleRoot;
		pSingleFlow->GetRootProc(vSingleRoot);
		if(vSingleRoot.size() <= 0)
			pSingleFlow->GetAllInvolvedProc(vSingleRoot);
		if(vSingleRoot.size() <= 0)
			return -1;
		CFlowDestination* pProc = pFlowPair->GetDestProcArcInfo(vSingleRoot[0]);
		int iSinglePercent = pProc->GetProbality();

		iCount = pSingleFlow->GetFlowPairCount();
		for(int j = 0; j < iCount; j++)
		{
			CProcessorDestinationList* pPair = pSingleFlow->GetFlowPairAt(j);
			int nDestCount = pPair->GetDestProcCount();
			if(nDestCount <= 0)
				break;
			CProcessorDestinationList* pOrgPair = m_pInternalFlow->GetFlowPairAt(pPair->GetProcID());				
			int iPercent = pOrgPair->GetDestProcArcInfo(pPair->GetDestProcAt(0).GetProcID())->GetProbality();
			iSinglePercent = (iSinglePercent*iPercent)/100;
		}
		vPercent.push_back(iSinglePercent);
	}

	iCount = 0;
	for(i = 0; i < (int)vPercent.size(); i++)
	{
		int iPercent = vPercent[i];
		iCount += iPercent;
		vPercent[i] = iCount;
	}

	int nIndex = random(vPercent.back());

	//get index
	for(i = 0; i < (int)vPercent.size(); i++)
	{
		if(nIndex < vPercent[i])
		{
			nIndex = i;
			break;
		}
	}	

	return nIndex;
}

CSinglePaxTypeFlow* CSubFlow::GetPathInProcess( const CMobileElemConstraint& _constraint , bool _bHasGate ,const ProcessorID& _gateID, bool _bHaveBag, const ProcessorID& _bagDeviceID ,const CMobileElemConstraint& _leadToConstraint)
{
	CAllPaxTypeFlow * pFlow = GetAllPossibleFlowByPaxType( _constraint,_bHasGate, _gateID, _bHaveBag ,_bagDeviceID, _leadToConstraint );
	int iPathCount = pFlow->GetSinglePaxTypeFlowCount();
	if( iPathCount <=0 )
	{
		return NULL;
	}
	else
	{
		int nIndex = GetPathIndexInProcess(pFlow, iPathCount);	
		if(nIndex < 0)
			return NULL;
		return pFlow->GetSinglePaxTypeFlowAt( nIndex );
	}
}

void CSubFlow::SetTransitionLink (const PassengerFlowDatabase& database, const PassengerFlowDatabase& _gateDetailFlowDB, const PassengerFlowDatabase& _stationPaxFlowDB)
{
	ProcessorID name;
	name.SetStrDict( m_pInternalFlow->GetInputTerm()->inStrDict );
	name.setID( m_sProcessUnitName );
    m_pTransition = (ProcessorEntry *)database.getEntryPoint (name);
	m_pTransitionGateDetail = (ProcessorEntry*)_gateDetailFlowDB.getEntryPoint( name );
    m_pStationTransition = (ProcessorEntry *)_stationPaxFlowDB.getEntryPoint (name);
}
ProcessorDistribution *CSubFlow::GetNextDestinations(const CMobileElemConstraint &p_type, int _nInGateDetailMode )
{

	if( _nInGateDetailMode == NORMAL_MODE )
	{
		if( !m_pTransition )
			return NULL;
		return (ProcessorDistribution *)m_pTransition->hierarchyLookup (p_type);
	}

    if( !m_pTransitionGateDetail )
		return NULL;

    return (ProcessorDistribution *)m_pTransitionGateDetail->hierarchyLookup (p_type);
}

void CSubFlow::DrawProcessInternalFlow( BOOL* bOn, double* dAlt )
{
	//if( m_vAllPossiblePath.size() <= 0 )
	//{
		BuildAllPossiblePath();
	//}

	InputTerminal* pTerm = m_pInternalFlow->GetInputTerm();
	int iPathCount = m_vAllPossiblePath.size();
	for( int iIdx =0; iIdx<iPathCount; ++iIdx )
	{
		CSinglePaxTypeFlow* pFlow = m_vAllPossiblePath[ iIdx ];
		int c = pFlow->GetFlowPairCount();
		for(int i=0; i<c; i++) 
		{
			//for each source processor (could be a processor group)
			CProcessorDestinationList* pFP = NULL;
			pFP = pFlow->GetFlowPairAt(i);
			GroupIndex gidx = pTerm->procList->getGroupIndex(pFP->GetProcID());
			if(gidx.start == -1 || gidx.end == -1)
				continue;
			for(int j=gidx.start; j<=gidx.end; j++)
			{
				if(j==START_PROCESSOR_INDEX || j==TO_GATE_PROCESS_INDEX
					|| j==FROM_GATE_PROCESS_INDEX || j == BAGGAGE_DEVICE_PROCEOR_INDEX )
					continue;
				//for each processor in group
				Processor* procSource = pTerm->procList->getProcessor(j);
				int nIndexInFloor=procSource->getFloor()/(int)SCALE_FACTOR;
				if(!bOn[nIndexInFloor]) continue;
				ASSERT(procSource != NULL);
				//create geometry between this processor and every destination processors in flow
				int nDestCount = pFP->GetDestCount();
				for(int k=0; k<nDestCount; k++) 
				{
					//for each destination (could be a group)
					GroupIndex gidxDest ;
					gidxDest.start = gidxDest.end = -1;
					CFlowDestination dpai = pFP->GetDestProcAt(k);
					if( dpai.GetOneToOneFlag() )
					{
						ProcessorID temp( dpai.GetProcID() );
						//// TRACE("\n%s\n", temp.GetIDString() );
						//// TRACE("\n%s\n", procSource->getID()->GetIDString() );
						if( GetOneToOneProcessorID( dpai.GetProcID(), *procSource->getID(), temp ) )
							gidxDest = pTerm->procList->getGroupIndex(temp);

					}
					else
					{
						gidxDest = pTerm->procList->getGroupIndex(dpai.GetProcID());
					}
					
					if(gidxDest.start == -1 || gidxDest.end == -1 || 
						gidxDest.start==TO_GATE_PROCESS_INDEX || 
						gidxDest.start==FROM_GATE_PROCESS_INDEX ||
						gidxDest.start==BAGGAGE_DEVICE_PROCEOR_INDEX ||
						gidxDest.end==TO_GATE_PROCESS_INDEX || 
						gidxDest.end==FROM_GATE_PROCESS_INDEX ||
						gidxDest.end==BAGGAGE_DEVICE_PROCEOR_INDEX )						
						continue;
					for(int l = gidxDest.start; l<=gidxDest.end; l++) 
					{
						if(l==END_PROCESSOR_INDEX )
							continue;
						//for each proc in dest group
						Processor* procDest = pTerm->procList->getProcessor(l);					
						//create geometry between procSource and procDest						
						CreateSourceToDestGeometry(procSource,procDest,dAlt);				
						
					}
				}

			}
		}
	}
}

const CSinglePaxTypeFlow* CSubFlow::GetPossiblePathAt( int _iIdx )
{
	ASSERT( (UINT)_iIdx < m_vAllPossiblePath.size() );
	return m_vAllPossiblePath[ _iIdx ];
}


// Find if PROCESS has path to the destination
bool CSubFlow::CanLeadTo( const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID,  InputTerminal* _pTerm )const
{
	int nSize = m_vAllPossiblePath.size();
	if( nSize == 0 )
		return false;

	bool bAtLeastOneCanLeadTo = false;
	for( int i=0; i<nSize; i++ )
	{
		CSinglePaxTypeFlow* pFlow = m_vAllPossiblePath[i];
		std::vector<ProcessorID> idList;
		pFlow->GetRootProc( idList );
		
		assert( idList.size() > 0 );
		ProcessorID procID = idList[0];
		std::vector<ProcessorID>vPathProc;
		TravelPath( pFlow, procID, vPathProc );
		int iPathProcCount = vPathProc.size();
		int iHaveTestNode = 0;
		bool bEnd = false;
		for( int k=0; k<iPathProcCount && !bEnd; ++k )
		{
			GroupIndex groupIndex = _pTerm->procList->getGroupIndex( vPathProc[ k ] );
			
			int j = groupIndex.start;
			for( ; j <= groupIndex.end; j++ )
			{
				Processor* pProc = _pTerm->procList->getProcessor(j);
//				// TRACE( "%s\n", pProc->getID()->GetIDString() );
				if( pProc->CanLeadTo( _paxType, _destProcID ) )
				{
					++iHaveTestNode;
					break;
				}				
			}

			if( j > groupIndex.end )// 
			{
				bEnd = true;
			}				
		}

		if( iHaveTestNode == iPathProcCount )
		{
			return true;
		}
	}
	return false;
}

void CSubFlow::TravelPath( CSinglePaxTypeFlow* _pFlow ,const ProcessorID& _rootID , std::vector<ProcessorID>& _result )const
{
	CProcessorDestinationList* pPair = _pFlow->GetFlowPairAt( _rootID );
	ASSERT( pPair );
	_result.push_back( _rootID );
//	// TRACE("  %s  ", _rootID.GetIDString() );
	int iDestCount = pPair->GetDestCount();
	for( int i=0; i<iDestCount; ++i )
	{
		TravelPath( _pFlow,pPair->GetDestProcAt( i ).GetProcID(), _result );
	}
}

Processor* CSubFlow::GetNextProcessorForTestFloor(Person* pPerson, Processor* pProc)
{
	const ProcessorID* pProcID = pProc->getID();

	CProcessorDestinationList* pProcList = m_pInternalFlow->GetFlowPairAt(*pProcID);

	if(pProcList == NULL || pProcList->GetDestCount() <= 0)
		return NULL;

	ProcessorID procID = pProcList->GetDestProcArcInfo(0)->GetProcID();

	GroupIndex group = pPerson->GetTerminal()->procList->getGroupIndex (procID);
	Processor* nextProc = pPerson->GetTerminal()->procList->getProcessor(group.start);

	return nextProc;
}

Processor* CSubFlow::GetNextProcessor(Person* pPerson, ElapsedTime _curTime, bool bHandleChannel, bool _bStationOnly , bool _bExcludeFlowBeltProc/* = false */)
{
	////DWORD dStepTime1 = 0L;
	////DWORD dStepTime2 = 0L;
	////DWORD dStepTime3 = 0L;
	////DWORD dStepTime4 = 0L;
	////DWORD dStepTime5 = 0L;
	///////Test code
	////DWORD dStartTime = GetTickCount();


	//CString szMobType = pPerson->getPersonErrorMsg();
	//CString strProcNameTmp = pPerson->getProcessor() ? pPerson->getProcessor()->getID()->GetIDString() : "";
	//if(pPerson->getID() == 2560 && m_sProcessUnitName == "FROM_A3_TO_SKYLINE")
	//{
	//	int i = 0;
	//}
	/////////////////////////////////////////////////////////////////
	//// 
	//pPerson->m_bInLeafProcessor = false;

	////////////////////////////////////////////////////////////////////////////
	////Processor* _pProc = NULL;

	////for(std::vector<ConvergePair*>::iterator it = pPerson->m_vpConvergePair.begin(); 
	////	it != pPerson->m_vpConvergePair.end(); it++)
	////{
	////	if((*it)->AmNowConvergeProcessor(pPerson->getID(), &_pProc, pPerson, false, _curTime))//true means want to convert,waiting
	////	{
	////		return _pProc;
	////	}
	////}

	//pPerson->m_ProcessFlowCurID.SetStrDict( pPerson->GetTerminal()->inStrDict );
	//// TRACE( "%s\n", pPerson->m_ProcessFlowCurID.GetIDString() );
	//BOOL bSubFlowStart = FALSE;
	//CProcessorDestinationList* pProcList = NULL;
	//if(pPerson->m_pSubFlow == NULL)
	//{
	//	pProcList = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();
	//	bSubFlowStart = TRUE;
	//}
	//else
	//	pProcList = m_pInternalFlow->GetFlowPairAt(pPerson->m_ProcessFlowCurID);

	//if(pProcList == NULL || pProcList->GetDestCount() <= 0)
	//{
	//	pPerson->m_ProcessFlowCurID.init();
	//	return NULL;
	//}

	////dStepTime1 = GetTickCount();
	////////////////////
	//ProcessorArray destProcs;
	//// no proc can be selected becuause of the roster
	//bool bRosterReason = FALSE;

	//const ProcessorID * nextGroup = &(pProcList->GetDestProcArcInfo(GetDestinationIndex(pProcList))->GetProcID());
	//int procCount = 0;
	//int iIndex = 0;

	//CFlowDestination* pFlowDest;
	//do 
	//{
	//	bool bNeedCheckLeadToGate = pPerson->NeedCheckLeadToGate();	// only check the gate after arrival gate for the turnaround passenger.

	//	if( bNeedCheckLeadToGate )//if need check
	//	{
	//		//get the gate processor's id
	//		const ProcessorID* procID = pPerson->GetTerminal()->procList->getProcessor(pPerson->getGateIndex())->getID();		
	//		CMobileElemConstraint type(NULL);
	//		CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(*nextGroup);
	//		
	//		if(m_Dircetion.IsEmpty())//there is not any gate after the sub flow(the sub flow can lead any gate)
	//		{
	//			//check inside the sub flow
	//			if(pList != NULL &&	!pList->FindCanLeadGate(type, *procID))
	//				continue;
	//		}
	//		else//there are some gates after the sub flow(the sub flow can lead some gates)
	//		{
	//			//find the gate which the person will lead,
	//			//if it is in the direction, the processor can server the person
	//			if(!FindCanLeadGate(type, *procID))//not find it, need check inside the sub flow
	//			{
	//				if(pList != NULL && pList->GetDirection()->IsEmpty())
	//					continue;
	//				else if(pList != NULL && !pList->FindCanLeadGate(type, *procID)) 
	//					continue;
	//			}
	//		}
	//	}
	//	bNeedCheckLeadToGate = FALSE;
	//	//  generate list of open processors
	//	pPerson->GetTerminal()->procList->buildArray (nextGroup, &destProcs,
	//		pPerson->getType(), pPerson->getGateIndex(), bNeedCheckLeadToGate, 
	//		pPerson->getBagCarosel(), FALSE, pPerson->getProcessor(), 
	//		_curTime, (Terminal*)pPerson->GetTerminal() ,bRosterReason, pPerson->GetDestFloor(),
	//		pPerson->GetPaxDestDiagnoseInfo(),pPerson,bSubFlowStart);

	//	// if none open, generate list of closed processors
	//	if (destProcs.getCount() == 0)
	//		pPerson->GetTerminal()->procList->buildArray (nextGroup, &destProcs,
	//			pPerson->getType(), pPerson->getGateIndex(), bNeedCheckLeadToGate, 
	//			pPerson->getBagCarosel(), TRUE, pPerson->getProcessor(), 
	//			_curTime, (Terminal*)pPerson->GetTerminal() ,bRosterReason, pPerson->GetDestFloor(),
	//			pPerson->GetPaxDestDiagnoseInfo(),pPerson,bSubFlowStart);

	//	// filter out the processor which can not lead to gate;
	//	FilterOutProcessorByGateCondition(pPerson, destProcs, nextGroup);
	//	FilterOutProcessorByBaggageCondition(pPerson, destProcs, nextGroup);

	//	if( _bStationOnly )
	//		pPerson->KeepOnlyStation( &destProcs );

	//	if( _bExcludeFlowBeltProc )
	//		pPerson->FilterOutFlowBeltProcs( &destProcs );

	//	//pPerson->FilterOutProcessorByFlowCondition( &destProcs ,_curTime );
	//	// get count of possible destination processors
	//	procCount = destProcs.getCount();

	//	// if any m_pProcessor can serve Person exit loop and select
	//	//bool bBaggageAllow = true;
	//	//if(pPerson->getDistribution() != NULL)
	//	//{
	//	//	bBaggageAllow = pPerson->IfBaggageProcAllowed( &destProcs, pPerson->getDistribution() );
	//	//}

	//	if (procCount > 0 && pPerson->noExceptions (&destProcs)/* && bBaggageAllow*/)		
	//		break;

	//	if(iIndex >= pProcList->GetDestProcCount())
	//		break;
	//	// if randomly selected group invalid, try each group m_in order
	//} while ( ( pFlowDest = pProcList->GetDestProcArcInfo(iIndex++)) && ( nextGroup = &(pFlowDest->GetProcID())) );

	////dStepTime2 = GetTickCount();

	//if( procCount == 0 )
	//{
	//	return NULL;
	//}

	//pPerson->m_ProcessFlowCurID = pProcList->GetDestProcArcInfo(*nextGroup)->GetProcID();
	//// is a leaf processor
	//if( pPerson->GetTerminal()->procList->getProcessor( pPerson->m_ProcessFlowCurID.GetIDString()) != NULL )	
	//	pPerson->m_bInLeafProcessor = true;

	////now 1x1 start
	//if( pProcList->GetDestProcArcInfo(*nextGroup)->GetOneXOneState() == ChannelStart && bHandleChannel)
	//{
	//	CFlowChannel* newChannel = new CFlowChannel(pPerson->getProcessor());
	//	pPerson->PushChannelToList(newChannel);
	//}
	////now 1x1 end pProcList->GetDestProcArcInfo(*nextGroup
	//if( pProcList->GetDestProcArcInfo(*nextGroup)->GetOneXOneState() == ChannelEnd && bHandleChannel)
	//{
	//	Processor* pDestProc = NULL;

	//	CFlowChannel* getChannel;
	//	getChannel = pPerson->PopChannelFromList();
	//	if (getChannel != NULL)
	//	{
	//		pDestProc = getChannel->GetOutChannelProc(destProcs, nextGroup->idLength());
	//		delete getChannel;
	//		return pDestProc;
	//	}
	//	else
	//	{
	//		pPerson->ProcessError(_curTime, szMobType, strProcNameTmp);
	//	}
	//}

	////dStepTime3 = GetTickCount();
	//
	//// if only one m_pProcessor is available to serve the Person,
	//// select it and end selection process
	//if (procCount == 1)
	//	return destProcs.getItem(0);

	//// now 1:1
	//Processor* pDestProc = HandleOneToOne(pPerson, destProcs, nextGroup);
	//if(pDestProc != NULL)
	//	return pDestProc;
	//
	////dStepTime4 = GetTickCount();

	//if( pProcList->GetDestProcArcInfo(*nextGroup)->GetOneToOneFlag() )
	//{
	//	// 1:1
	//	pPerson->m_bInLeafProcessor = true;	// don't know what for. check it later.
	//	Processor* pDestProc = destProcs.GetOneToOneProcessor( pPerson->getProcessor()->getID(),
	//									pProcList->GetDestProcArcInfo(*nextGroup)->GetProcID().idLength() );
	//	if( pDestProc == NULL )
	//		pPerson->ProcessError(_curTime, szMobType, strProcNameTmp);
	//	else
	//		return pDestProc;
	//}
	//
	////dStepTime5 = GetTickCount();
	//// first Processor is always selected randomly
	//if (pPerson->getState() == Birth)
	//	return destProcs.getItem (random(procCount));
	//
	//// choose m_pProcessor with intest queue & smallest distance
	//Processor *pProcReturn = NULL;
	//pProcReturn = pPerson->selectBestProcessor (&destProcs);

	////test code 

	//return pProcReturn;
	
	return NULL;
}

void CSubFlow::FilterOutProcessorByGateCondition(Person* pPerson, ProcessorArray& array, const ProcessorID* nextGroup)
{
	int nIndex = pPerson->getGateIndex();
	Processor* pGateProc = m_pInputTerm->procList->getProcessor(nIndex);
	if(!pGateProc)
		return ;

	MAPPROC::iterator iter = m_mapLeadToGates.find(*nextGroup);
	if(iter == m_mapLeadToGates.end())
		return ;

	MAPPROC1TO1 mapProc1To1(iter->second);
	MAPPROC1TO1::iterator iterProc1To1 = mapProc1To1.begin();
	bool bFind = false;
	for( ; iterProc1To1 != mapProc1To1.end(); iterProc1To1++)
	{
		if( iterProc1To1->first.idFits(*pGateProc->getID()) )
			bFind = true;
	}
	if(!bFind)
	{
		int nProcCount = array.getCount();
		for(int i = nProcCount-1; i >= 0; i--)
		{
			Processor* pProc = array.getItem( i );
			if( iter->first.idFits( *( pProc->getID() ) ) )
			{
				array.removeItem(i);
			}			
		}
	}
}

void CSubFlow::FilterOutProcessorByBaggageCondition(Person* pPerson, ProcessorArray& array, const ProcessorID* nextGroup)
{
	int nIndex = pPerson->getBagCarosel();
	Processor* pBagProc = m_pInputTerm->procList->getProcessor(nIndex);
	if(!pBagProc)
		return ;

	MAPPROC::iterator iter = m_mapBaggageReclaims.find(*nextGroup);
	if(iter == m_mapBaggageReclaims.end())
		return ;

	CString str = nextGroup->GetIDString();

	MAPPROC1TO1 mapProc1To1(iter->second);
	MAPPROC1TO1::iterator iterProc1To1 = mapProc1To1.begin();
	bool bFind = false;
	for( ; iterProc1To1 != mapProc1To1.end(); iterProc1To1++)
	{
		CString str1 = iterProc1To1->first.GetIDString();
		CString str2 = pBagProc->getID()->GetIDString();
		if( iterProc1To1->first.idFits(*pBagProc->getID()) )
			bFind = true;
	}
	if(!bFind)
	{
		int nProcCount = array.getCount();
		for(int i = nProcCount-1; i >= 0; i--)
		{
			Processor* pProc = array.getItem( i );
			if( iter->first.idFits( *( pProc->getID() ) ) )
			{
				array.removeItem(i);
			}			
		}
	}
}

Processor* CSubFlow::HandleOneToOne(Person* pPerson, const ProcessorArray& array, const ProcessorID* nextGroup)
{
	//MAPPROC::iterator iter = m_mapBaggageReclaims.find(*nextGroup);
	//if(iter != m_mapBaggageReclaims.end())
	//{
	//	MAPPROC1TO1 mapProc1To1(iter->second);
	//	int nIndex = pPerson->getBagCarosel();
	//	if(nIndex >= 0)
	//	{
	//		Processor* pProc = m_pInputTerm->procList->getProcessor(nIndex);
	//		MAPPROC1TO1::iterator iterProc1To1 = mapProc1To1.begin();
	//		for( ; iterProc1To1 != mapProc1To1.end(); iterProc1To1++)
	//		{
	//			if(iterProc1To1->first.idFits(*pProc->getID()) && iterProc1To1->second)
	//				break;
	//		}
	//		if(iterProc1To1 != mapProc1To1.end())
	//		{
	//			pPerson->m_bInLeafProcessor = true;
	//			Processor* pDestProc = array.GetOneToOneProcessor( pProc->getID(), nextGroup->idLength());
	//			return pDestProc;
	//		}
	//	}
	//}

	//iter = m_mapLeadToGates.find(*nextGroup);
	//if(iter != m_mapLeadToGates.end())
	//{
	//	MAPPROC1TO1 mapProc1To1(iter->second);
	//	int nIndex = pPerson->getGateIndex();
	//	if(nIndex >= 0)
	//	{
	//		Processor* pProc = m_pInputTerm->procList->getProcessor(nIndex);
	//		MAPPROC1TO1::iterator iterProc1To1 = mapProc1To1.begin();
	//		for( ; iterProc1To1 != mapProc1To1.end(); iterProc1To1++)
	//		{
	//			if(iterProc1To1->first.idFits(*pProc->getID()) && iterProc1To1->second)
	//				break;
	//		}
	//		if(iterProc1To1 != mapProc1To1.end())
	//		{
	//			pPerson->m_bInLeafProcessor = true;
	//			Processor* pDestProc = array.GetOneToOneProcessor( pProc->getID(), nextGroup->idLength());
	//			return pDestProc;
	//		}		
	//	}
	//}

	return NULL;
}

int CSubFlow::GetAllProcessorsByType(ProcessorClassList ProcType, VECTPROCID& vProcID)
{
	//if(ProcType == BaggageProc)
	//{
	//	FlightConstraintDatabase* pAssignDB = m_pInputTerm->bagData->getAssignmentDatabase();
	//	int nCount = pAssignDB->getCount();
	//	for( int i = 0; i < nCount; i++ )
	//	{
	//		const Constraint* pConstr = pAssignDB->getItem(i)->getConstraint();
	//		CString screenBuf;
	//		pConstr->screenPrint(screenBuf,0, 128);

	//		ProcessorDistribution* pDist = (ProcessorDistribution*)pAssignDB->getItem( i )->getValue();
	//		int nGroupCount = pDist->getCount();
	//		for( int j = 0; j < nGroupCount; j++ )
	//		{
	//			ProcessorID *pID = pDist->getGroup(j);
	//			char szBuf[1024];
	//			pID->printID(szBuf);

	//			VECTPROCID::iterator iter = std::find(vProcID.begin(), vProcID.end(), *pID);
	//			if(iter == vProcID.end())//not find it
	//			{
	//				vProcID.push_back(*pID);
	//			}
	//		}
	//	}
	//}
	//else
	//{				// need talk to frank to understand why need this part. rich
		int nCount = m_pInternalFlow->GetFlowPairCount();

		for(int i = 0; i < nCount; i++)
		{
			CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(i);
			const ProcessorID& procID = pList->GetProcID();

			GroupIndex group = m_pInputTerm->procList->getGroupIndex(procID);
			if(group.start < 0)
				continue;

			Processor* pProc = m_pInputTerm->procList->getProcessor(group.start);

			if( pProc->getProcessorType() == ProcType)
			{
				VECTPROCID::iterator iter = std::find(vProcID.begin(), vProcID.end(), procID);
				if(iter == vProcID.end())//not find it
					vProcID.push_back(procID);	
			}
		}	
	//}

	return (int)vProcID.size();
}

void CSubFlow::SetValueIfOneToOne(MAPPROC& mapProc, const ProcessorID& ProcID)
{
	// TRACE( "ID = %s\n", ProcID.GetIDString() );
	std::vector<ProcessorID> vProcSrc;
	m_pInternalFlow->GetSourceProc(ProcID, vProcSrc);
	int nSize = (int)vProcSrc.size();
	if(nSize <= 0)
		return ;

	MAPPROC::iterator iterCur = mapProc.find(ProcID);
	ASSERT(iterCur != mapProc.end());

	MAPPROC1TO1 mapProc1To1(iterCur->second);

	for(int i = 0; i < nSize; i++)
	{
		CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(vProcSrc[i]);
		CFlowDestination* pFlowDest = pList->GetDestProcArcInfo(ProcID);
		
		for (MAPPROC1TO1::iterator iterMapProc1To1 = mapProc1To1.begin();
			 iterMapProc1To1 != mapProc1To1.end();
			 iterMapProc1To1++)
		{
			bool bOneToOne = pFlowDest->GetOneToOneFlag() ? true : false;
			const std::pair<ProcessorID, bool> pairProc = std::make_pair(iterMapProc1To1->first, bOneToOne);

			AddProcessorIntoMap(mapProc, vProcSrc[i], pairProc);
		}

		SetValueIfOneToOne(mapProc, vProcSrc[i]);
	}
}

// Add the Gate linkage information to the GateRelationships
void CSubFlow::AddLeadToGateRelationships()
{
	for( int i = 0; i < PROC_TYPES; i++ )
	{
		MiscProcessorData* pMiscProcessorData = m_pInputTerm->miscData->getDatabase( i );
		for( int j = 0; j < pMiscProcessorData->getCount(); j++ )
		{
			ProcessorDataElement* pElement = pMiscProcessorData->getItem( j );
			const ProcessorID* pProcID = pElement->getID();

			bool bFits = m_pInternalFlow->IfFits(*pProcID);
			if (bFits)
			{
				ProcessorIDList* pGateProcIDList = ((MiscDataElement*)pElement)->getData()->getGateList();

				for( int k = 0; k < pGateProcIDList->getCount(); k++ )
				{
					const ProcessorID* pGateProcID = pGateProcIDList->getID(k);

					std::pair<ProcessorID, bool> pairProc = std::make_pair(*pGateProcID, false);
					if(((MiscProcessorIDWithOne2OneFlag*)pGateProcID)->getOne2OneFlag())
						pairProc.second = true;

					AddProcessorIntoMap(m_mapLeadToGates, *pProcID, pairProc);
				}
			}
		}
	}
}

void CSubFlow::BuildGateRelationships()
{
	VECTPROCID vProcGate;
	int nCount = GetAllProcessorsByType(GateProc, vProcGate);
	m_mapLeadToGates.clear();

	for(int i = 0; i < nCount; i++)
	{
		std::vector<ProcessorID> vProcSrc;
		m_pInternalFlow->GetSourceProc(vProcGate[i], vProcSrc);

		size_t nSourceProcCount = vProcSrc.size();
		for(size_t j = 0; j < nSourceProcCount; j++)
		{
			CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(vProcSrc[j]);
			CFlowDestination* pFlowDest = pList->GetDestProcArcInfo(vProcGate[i]);

			std::pair<ProcessorID, bool> pairProc = std::make_pair(vProcGate[i], false);
			if(pFlowDest->GetOneToOneFlag())
				pairProc.second = true;
			
			AddProcessorIntoMap(m_mapLeadToGates, vProcSrc[j], pairProc);

			SetValueIfOneToOne(m_mapLeadToGates, vProcSrc[j]);
		}
	}

	//PrintMapInfo(m_mapLeadToGates);
}

void CSubFlow::AddProcessorIntoMap(MAPPROC& mapProc, const ProcessorID& ProcID, const std::pair<ProcessorID, bool>& pairProc1To1)
{
	MAPPROC::iterator iter = mapProc.find(ProcID);

	if(iter == mapProc.end())//not find
	{
		MAPPROC1TO1 tempMapProc1To1;
		tempMapProc1To1.insert(pairProc1To1);
		mapProc.insert(std::make_pair(ProcID, tempMapProc1To1));
	}
	else//find it
	{
		iter->second.insert(pairProc1To1);
	}
}

void CSubFlow::BuildBaggageRelationships()
{
	// TRACE( "name = %s\n", m_sProcessUnitName );
	VECTPROCID vProcBag;
	int nCount = GetAllProcessorsByType(BaggageProc, vProcBag);
	m_mapBaggageReclaims.clear();
	for(int i = 0; i < nCount; i++)
	{
		std::vector<ProcessorID> vProcSrc;
		m_pInternalFlow->GetSourceProc(vProcBag[i], vProcSrc);
		size_t nSourceProcCount = vProcSrc.size();
		for(size_t j = 0; j < nSourceProcCount; j++)
		{
			CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(vProcSrc[j]);
			CFlowDestination* pFlowDest = pList->GetDestProcArcInfo(vProcBag[i]);

			std::pair<ProcessorID, bool> pairProc = std::make_pair(vProcBag[i], false);
			if(pFlowDest->GetOneToOneFlag())
				pairProc.second = true;

			AddProcessorIntoMap(m_mapBaggageReclaims, vProcSrc[j], pairProc);
		
			SetValueIfOneToOne(m_mapBaggageReclaims, vProcSrc[j]);
		}
	}

	//PrintMapInfo(m_mapBaggageReclaims);
}

void CSubFlow::PrintMapInfo(MAPPROC& mapProc)
{
	for(MAPPROC::iterator iter = mapProc.begin(); iter != mapProc.end(); iter++)
	{
		// TRACE("\n %s --------------- ", iter->first.GetIDString());
		MAPPROC1TO1 mapProc1To1 = iter->second;
		for(MAPPROC1TO1::iterator it = mapProc1To1.begin(); it != mapProc1To1.end(); it++)
		{
			// TRACE("\n          %s -- %d ", it->first.GetIDString(), it->second);
		}
	}
}


// get the branch attribute of pNextProc, where the source is curProcID
// in:
//  pNextProc:    branch processor
//  curProcID	  source proc id
// out:
//  flowItem	 result flow attribute
void CSubFlow::GetProcessorPipes(Processor* pNextProc, const ProcessorID& curProcID, CFlowItemEx& flowItem)
{

	ASSERT(pNextProc != NULL);

	const ProcessorID* pNextID = pNextProc->getID();
//	// TRACE( "curid = %s, nextid = %s\n", curProcID.GetIDString(), pNextID->GetIDString() );

	std::vector<ProcessorID> vSrcID;
	m_pInternalFlow->GetSourceProc(*pNextID, vSrcID);

	CProcessorDestinationList* pFlowPair = m_pInternalFlow->GetFlowPairAt(curProcID);
	if (pFlowPair == NULL)
		pFlowPair = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();

	
	if (pFlowPair == NULL)
	{
		return;
	}

	CFlowDestination* pDest = pFlowPair->GetDestProcArcInfo(*pNextID);

	flowItem.SetTypeOfUsingPipe(pDest->GetTypeOfUsingPipe());

	PIPES& vPipes = pDest->GetPipeVector();

	for(int i = 0; i < (int)vPipes.size(); i++)
	{
		flowItem.AddPipeIdx(vPipes[i]);
	}
}

/*******************************************************************************/
//Function: Test if root processor is in sub flow
//Input Parameters:
//		procID: 
//			type: const ProcessorID&
//			desc: the processor id which be tested
//Output Parameters:
//		NULL
//Return value:
//		true: is root processor
//		false: is not root processor
/*******************************************************************************/
bool CSubFlow::IsRootProc(const ProcessorID& procID)
{
	CProcessorDestinationList* pList = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();
	if(pList == NULL)
		return false;

	CFlowDestination* pDest = pList->GetDestProcArcInfo(procID);
	if(pDest == NULL)
		return false;

	return true;
}

void CSubFlow::AddCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID, ProcessorList *pProcList)
{
     //m_Dircetion.Insert(p_type, procID); 
	std::vector<ProcessorID> _sourceIDVector;
	((CSingleProcessFlow*)m_pInternalFlow)->GetAllInvolvedProc(_sourceIDVector);

	std::vector<ProcessorID>::iterator iterStart = _sourceIDVector.begin();


	for( ; iterStart!=_sourceIDVector.end(); ++iterStart )
	{
		GroupIndex group = pProcList->getGroupIndex ( *iterStart );
		for( int k=group.start; k<=group.end; ++k )
		{	

		//	CString strTempPaxType;
		//	p_type.screenFltPrint(strTempPaxType);

			CString strTempID = (*iterStart).GetIDString();

			pProcList->getProcessor( k )->GetDirection().Insert( p_type, procID );
		
		}	

	}



}

BOOL CSubFlow::FindCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID)
{
	return m_Dircetion.CanLeadTo(p_type, procID, m_pInputTerm);
}

BOOL CSubFlow::IfHasGateProcessor()
{
	int nCount = m_pInternalFlow->GetFlowPairCount();

	for(int i = 0; i < nCount; i++)
	{
		CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(i);
		const ProcessorID& procID = pList->GetProcID();

		GroupIndex group = m_pInputTerm->procList->getGroupIndex(procID);
		if(group.start < 0)
			continue;

		Processor* pProc = m_pInputTerm->procList->getProcessor(group.start);

		if( pProc->getProcessorType() == GateProc
			|| pProc->getProcessorType() == TO_GATE_PROCESS_INDEX
			|| pProc->getProcessorType() == FROM_GATE_PROCESS_INDEX)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSubFlow::IfHasReclaimProcessor()
{
	int nCount = m_pInternalFlow->GetFlowPairCount();

	for(int i = 0; i < nCount; i++)
	{
		CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(i);
		const ProcessorID& procID = pList->GetProcID();

		GroupIndex group = m_pInputTerm->procList->getGroupIndex(procID);
		if(group.start < 0)
			continue;

		Processor* pProc = m_pInputTerm->procList->getProcessor(group.start);

		if( pProc->getProcessorType() == BaggageProc
			|| pProc->getProcessorType() == BAGGAGE_DEVICE_PROCEOR_INDEX)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CSubFlow::GetGateProcessor(std::vector<ProcessorID>& vProcID)
{
	int nCount = m_pInternalFlow->GetFlowPairCount();

	for(int i = 0; i < nCount; i++)
	{
		CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(i);
		const ProcessorID& procID = pList->GetProcID();

		GroupIndex group = m_pInputTerm->procList->getGroupIndex(procID);
		if(group.start < 0)
			continue;

		Processor* pProc = m_pInputTerm->procList->getProcessor(group.start);

		if( pProc->getProcessorType() == GateProc
			|| pProc->getProcessorType() == TO_GATE_PROCESS_INDEX
			|| pProc->getProcessorType() == FROM_GATE_PROCESS_INDEX)
		{
			vProcID.push_back(procID);	
		}
	}
}

void CSubFlow::GetReclaimProcessor(std::vector<ProcessorID>& vProcID)
{
	int nCount = m_pInternalFlow->GetFlowPairCount();

	for(int i = 0; i < nCount; i++)
	{
		CProcessorDestinationList* pList = m_pInternalFlow->GetFlowPairAt(i);
		const ProcessorID& procID = pList->GetProcID();

		GroupIndex group = m_pInputTerm->procList->getGroupIndex(procID);
		if(group.start < 0)
			continue;

		Processor* pProc = m_pInputTerm->procList->getProcessor(group.start);

		if( pProc->getProcessorType() == BaggageProc
			|| pProc->getProcessorType() == BAGGAGE_DEVICE_PROCEOR_INDEX)
		{
			vProcID.push_back(procID);	
		}
	}
}

void CSubFlow::PrintGateInfo()
{
	//print can lead gate of processor,

}

bool CSubFlow::getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	return m_pInternalFlow->getUsedProInfo(id,_pTerm,_strVec,m_sProcessUnitName);
}

bool CSubFlow::IfUseThisPipe(int nCurPipeIndex)const
{	
	int iFlowPairCount = m_pInternalFlow->GetFlowPairCount();

	for( int i=0; i<iFlowPairCount; ++i )
	{
		CProcessorDestinationList* singlePair = NULL;
		if (i == 0)
		{
			singlePair = ((CSingleProcessFlow*)m_pInternalFlow)->GetStartPair();
		}
		else
		{
			singlePair = m_pInternalFlow->GetFlowPairAt(i);
		}
		
		return singlePair->IfUseThisPipe(nCurPipeIndex);
	}
	return false;
}

bool CSubFlow::IfSubFlowValid() const
{
	return ((CSingleProcessFlow*)m_pInternalFlow)->IfSubFlowValid(m_sProcessUnitName);
}
