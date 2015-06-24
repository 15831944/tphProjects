// PaxFlowConvertor.cpp: implementation of the CPaxFlowConvertor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PaxFlowConvertor.h"
#include "SinglePaxTypeFlow.h"
#include "inputs\pax_db.h"
#include "inputs\paxflow.h"
#include "AllPaxTypeFlow.h"
#include "inputs\procdist.h"
#include "engine\proclist.h"
#include "inputs\schedule.h"
#include "inputs\procdata.h"
#include "engine\gate.h"
#include "inputs\flight.h"
#include "ProcessorDistributionWithPipe.h"
#include "HandleSingleFlowLogic.h"

//#define  _TRACE_
//#include<vector>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxFlowConvertor::CPaxFlowConvertor()
{
	m_pTempNonPaxFlow = new CSinglePaxTypeFlow();
}

CPaxFlowConvertor::~CPaxFlowConvertor()
{
	if (m_pTempNonPaxFlow)
	{
		delete m_pTempNonPaxFlow;
		m_pTempNonPaxFlow = NULL;
	}
}
void CPaxFlowConvertor::ToDigraphStructure( CAllPaxTypeFlow& _allPaxTypeFlow )const
{
	ASSERT( m_pTerm );
	ToDigraphStructureFromSinglePaxDB( _allPaxTypeFlow, m_pTerm->m_pPassengerFlowDB );
	ToDigraphStructureFromSinglePaxDB( _allPaxTypeFlow, m_pTerm->m_pStationPaxFlowDB );
}

CSinglePaxTypeFlow* CPaxFlowConvertor::GetRelatedPaxTypeFlow( CAllPaxTypeFlow& _allPaxTypeFlow ,const Constraint*  _pConstrain )const
{
	int iSingleFlowCount = _allPaxTypeFlow.GetSinglePaxTypeFlowCount();
	//char name[256];
	//((CMobileElemConstraint*) _pConstrain)->screenPrint( name );
	//// TRACE("\n _pConstrainname = %s\n",name);
	for( int i=0; i<iSingleFlowCount; ++i )
	{
	//	_allPaxTypeFlow.GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain()->screenPrint( name );
	//	// TRACE("\n allpax in  = %s\n",name);
		if( _pConstrain->isEqual( _allPaxTypeFlow.GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain() ) )
			return _allPaxTypeFlow.GetSinglePaxTypeFlowAt( i );
	}

	CSinglePaxTypeFlow* pSingleFlow = new CSinglePaxTypeFlow( m_pTerm );
	pSingleFlow->SetPaxConstrain( (const CMobileElemConstraint*) _pConstrain );
	_allPaxTypeFlow.AddPaxFlow( pSingleFlow ,false );
	return pSingleFlow;
}
void CPaxFlowConvertor::ToOldStructure(const CAllPaxTypeFlow& _allPaxTypeFlow )const
{
	ASSERT( m_pTerm );
	PassengerFlowDatabase* m_pPaxFlowDB = m_pTerm->m_pPassengerFlowDB;
	PassengerFlowDatabase* m_pStationFlowDB = m_pTerm->m_pStationPaxFlowDB;
	int iSingleFlowCount = _allPaxTypeFlow.GetSinglePaxTypeFlowCount();
	for(int i=0; i<iSingleFlowCount; ++i )
	{
		const CSinglePaxTypeFlow* pFlow = _allPaxTypeFlow.GetSinglePaxTypeFlowAt( i );
		if( pFlow->IfFlowBeChanged() )
		{
			m_pPaxFlowDB->deletePaxType( pFlow->GetPassengerConstrain() );
			m_pStationFlowDB->deletePaxType( pFlow->GetPassengerConstrain() );
			int iFlowPairCount = pFlow->GetFlowPairCount();
			for( int j=0; j<iFlowPairCount; ++j )
			{				
				const CProcessorDestinationList& pFlowPair = pFlow->GetFlowPairAt( j );
				int iDestCount  = pFlowPair.GetDestCount();
				/*  [7/5/2004]
				int iProbalilityCount = pFlowPair.GetAppointedDestCount();
				if(iProbalilityCount < iDestCount)iProbalilityCount++;//the last probability is a total of every not-appointedDest's probability.
				*/
//				// TRACE("%s\n",pFlowPair.GetProcID().GetIDString());
				if( iDestCount > 0 )
				{
					if( !IsStation( pFlowPair.GetProcID() ) )
					{
						CMobileElemConstraint* pPaxCon = new CMobileElemConstraint(m_pTerm);
						*pPaxCon = *( pFlow->GetPassengerConstrain() );
						CProcessorDistributionWithPipe* pProcDist = new CProcessorDistributionWithPipe( false );
						ProcessorID* pDestList = new ProcessorID[ iDestCount ];				
						/*[7/5/2004]
						unsigned char* pProbabilities = new unsigned char[ iProbalilityCount ];
						memset(pProbabilities,0,sizeof(unsigned char)* iProbalilityCount);
						*/
						double* pProbabilities = new double[ iDestCount ];
						
						for( int k=0; k<iDestCount; ++k )
						{
							const CFlowDestination& pDestInfo = pFlowPair.GetDestProcAt( k );
							pDestList[ k ] = pDestInfo.GetProcID();
							/*[7/5/2004]
							if(k< iProbalilityCount-1)
							{
								pProbabilities[ k ] = pDestInfo.GetProbality();							
							}
							else
							{
								pProbabilities[ iProbalilityCount-1 ]+=pDestInfo.GetProbality();
							}
							*/
							pProbabilities[ k ] = pDestInfo.GetProbality();							

//							// TRACE("%s\n",pDestInfo.GetProcID().GetIDString());
							//// TRACE("%d\n",pProbabilities[k] );
							CFlowItemEx tempPipe;
							tempPipe.GetPipeVector() = ((CFlowDestination&)pDestInfo).GetPipeVector();
							tempPipe.SetTypeOfUsingPipe( pDestInfo.GetTypeOfUsingPipe() );
							tempPipe.SetOneToOneFlag( pDestInfo.GetOneToOneFlag() );
							
							tempPipe.SetAvoidDensity( pDestInfo.GetAvoidDensity() );
							tempPipe.SetAvoidFixQueue( pDestInfo.GetAvoidFixQueue() );
							tempPipe.SetAvoidOverFlowQueue( pDestInfo.GetAvoidOverFlowQueue() );
							tempPipe.SetDensityOfArea( pDestInfo.GetDensityOfArea() );
							tempPipe.SetMaxQueueLength( pDestInfo.GetMaxQueueLength() );
							tempPipe.SetMaxWaitMins( pDestInfo.GetMaxWaitMins() );
							tempPipe.SetChannelState(pDestInfo.GetOneXOneState());
							tempPipe.SetFollowState(pDestInfo.GetFollowState());
							tempPipe.SetMaxSkipTime( pDestInfo.GetMaxSkipTime());

							tempPipe.GetDensityArea() = ((CFlowDestination&)pDestInfo).GetDensityArea();

							pProcDist->GetPipeVector().push_back( tempPipe );
							
							
						}
						
						//  [7/5/2004]pProcDist->initDestinations( pDestList, iDestCount,pProbabilities, iProbalilityCount );
						pProcDist->initDestinations( pDestList, iDestCount,pProbabilities, iDestCount );
						//pProcDist->
						ConstraintEntry* pConEntry = new ConstraintEntry();
						pConEntry->initialize( pPaxCon, pProcDist );
						m_pPaxFlowDB->insert(  pFlowPair.GetProcID(), pConEntry, true );
					}
					else
					{
						int iStationCount = 0;
						for( int k=0; k<iDestCount; ++k )
						{
							if( IsStation( pFlowPair.GetDestProcAt( k ).GetProcID() ) )
							{
								++iStationCount;
							}
						}						
						ProcessorID* pDestStation = new ProcessorID[ iStationCount ];
						double* pStationProbabilities = new double[ iStationCount ];
						
						ProcessorID* pDestCommon = new ProcessorID[ iDestCount-iStationCount ];
						double* pCommonProbabilities = new double[ iDestCount-iStationCount ];
						int iStation =0;
						int iCommon =0;
						std::vector<CFlowItemEx> m_vPipe;
						std::vector<CFlowItemEx> m_vPipeStation;
						for( int c =0; c<iDestCount; ++c )
						{
							const CFlowDestination& pDestInfo = pFlowPair.GetDestProcAt( c );
							CFlowItemEx tempPipe;
							tempPipe.SetTypeOfUsingPipe( pDestInfo.GetTypeOfUsingPipe() );
							tempPipe.SetOneToOneFlag( pDestInfo.GetOneToOneFlag() );
							tempPipe.GetPipeVector() = ((CFlowDestination&)pDestInfo).GetPipeVector();

							tempPipe.SetAvoidDensity( pDestInfo.GetAvoidDensity() );
							tempPipe.SetAvoidFixQueue( pDestInfo.GetAvoidFixQueue() );
							tempPipe.SetAvoidOverFlowQueue( pDestInfo.GetAvoidOverFlowQueue() );
							tempPipe.SetDensityOfArea( pDestInfo.GetDensityOfArea() );
							tempPipe.SetMaxQueueLength( pDestInfo.GetMaxQueueLength() );
							tempPipe.SetMaxWaitMins( pDestInfo.GetMaxWaitMins() );
							tempPipe.SetChannelState(pDestInfo.GetOneXOneState());
							tempPipe.SetFollowState(pDestInfo.GetFollowState());
							tempPipe.SetMaxSkipTime(pDestInfo.GetMaxSkipTime());

							tempPipe.GetDensityArea() = ((CFlowDestination&)pDestInfo).GetDensityArea();


//							// TRACE("%s\n",pDestInfo.GetProcID().GetIDString());
							if( IsStation( pDestInfo.GetProcID() ) )
							{
								m_vPipeStation.push_back( tempPipe );								
								pDestStation[ iStation ] = pDestInfo.GetProcID();
								pStationProbabilities[ iStation ] = pDestInfo.GetProbality();
								++iStation;
							}
							else
							{
								m_vPipe.push_back( tempPipe );
								pDestCommon[ iCommon ] = pDestInfo.GetProcID();
								pCommonProbabilities[ iCommon ] = pDestInfo.GetProbality();
								++iCommon;
							}
						}

						if( iStationCount > 0 )
						{
							CMobileElemConstraint* pStationCon = new CMobileElemConstraint(m_pTerm);
							*pStationCon = *( pFlow->GetPassengerConstrain() );
							CProcessorDistributionWithPipe* pStationDist = new CProcessorDistributionWithPipe( false );
							pStationDist->initDestinations( pDestStation, pStationProbabilities, iStationCount );
							pStationDist->GetPipeVector() = m_vPipeStation;
							
							ConstraintEntry* pConEntry = new ConstraintEntry();
							pConEntry->initialize( pStationCon, pStationDist );
							m_pStationFlowDB->insert(  pFlowPair.GetProcID(), pConEntry, true );							
						}

						if( iDestCount-iStationCount > 0 )
						{
							CMobileElemConstraint* pCommonCon = new CMobileElemConstraint(m_pTerm);
							*pCommonCon = *( pFlow->GetPassengerConstrain() );
							CProcessorDistributionWithPipe* pCommonDist = new CProcessorDistributionWithPipe( false );
							pCommonDist->initDestinations( pDestCommon, pCommonProbabilities, iDestCount-iStationCount );
							pCommonDist->GetPipeVector() = m_vPipe;
							ConstraintEntry* pConEntry = new ConstraintEntry();
							pConEntry->initialize( pCommonCon, pCommonDist );
							m_pPaxFlowDB->insert(  pFlowPair.GetProcID(), pConEntry, true );						
						}
						

						
					}

					
				}
				
			}
		}
	}
	
	
}


void CPaxFlowConvertor::BuildHierarchyFlow( const CSinglePaxTypeFlow& _sourceFlow ,CSinglePaxTypeFlow& _resultFlow ) const
{
	_resultFlow.ClearAllPair();	
	std::vector<ProcessorID> vAllRootProc;	
	_sourceFlow.GetRootProcInHierarchy( vAllRootProc );
	std::vector<ProcessorID>::const_iterator iter = vAllRootProc.begin();
	std::vector<ProcessorID>::const_iterator iterEnd = vAllRootProc.end();
	
	
	std::vector<const CProcessorDestinationList* > vVisitedPair;
	for(; iter != iterEnd; ++iter )
	{	
#ifdef _TRACE_
		// TRACE("_sourceFlow in BuildHierarchyFlow\n");
//		_sourceFlow.PrintAllStructure();		
#endif		
		MergeHierarchyFlow( _sourceFlow,_resultFlow, *iter,vVisitedPair );
#ifdef _TRACE_
		// TRACE("\n root = %s\n", iter->GetIDString() );
		// TRACE("_sourceFlow in BuildHierarchyFlow\n");
//		_sourceFlow.PrintAllStructure();
		// TRACE("_resultFlow in BuildHierarchyFlow\n");
//		_resultFlow.PrintAllStructure();		
#endif

	}	
}

/************************************************************************
 _sourceFlow's m_vPaxFlowDigraph and _sourceFlow Merge into _resultFlow

   OutPut : _resultFlow
 see also : GetHierarchyFlowPair                                      
************************************************************************/ 
void CPaxFlowConvertor::MergeHierarchyFlow( const CSinglePaxTypeFlow& _sourceFlow,CSinglePaxTypeFlow& _resultFlow, const ProcessorID& _sourceProc,std::vector<const CProcessorDestinationList* >& vVisitedPair ) const
{
	bool bIsOwend = false;
	CString strTemp=_sourceProc.GetIDString();
//	TRACE("%s\n",strTemp);
//	// TRACE("MergeHierarchyFlow's _sourceProc  "+strTemp+"\n");
	const CProcessorDestinationList* pPair = _sourceFlow.GetHierarchyFlowPair( _sourceProc, bIsOwend );
	if( !pPair || (std::find(vVisitedPair.begin(),vVisitedPair.end(),pPair) != vVisitedPair.end()))
		return;

	vVisitedPair.push_back(pPair);

	_resultFlow.SetAllPairVisitFlag( false );
	//only combined pair will be seted as true , because now we only display the combined flow pair with green color
	if( bIsOwend )
	{
		((CProcessorDestinationList* )pPair)->SetAllDestInherigeFlag( 0 );
	}
	else
	{
		((CProcessorDestinationList* )pPair)->SetAllDestInherigeFlag( 1 );
	}
	
	const ProcessorID& sourceProce = pPair->GetProcID();
	int iCount = pPair->GetDestCount();
	for( int i=0; i<iCount; ++i )
	{
		_resultFlow.AddDestProc( sourceProce, pPair->GetDestProcAt( i ) ,false );
		_resultFlow.ResetDesitinationFlag(false);
		if( !HandleSingleFlowLogic::IfHaveCircleFromProc(&_resultFlow,pPair->GetDestProcAt( i ).GetProcID()))
		{
			MergeHierarchyFlow( _sourceFlow, _resultFlow, pPair->GetDestProcAt( i ).GetProcID(),vVisitedPair );
		}
	}

	return;	
}
void CPaxFlowConvertor::BuildSinglePaxFlow( const CSinglePaxTypeFlow& _oldFlow ,const CSinglePaxTypeFlow& _compareFlow ,CSinglePaxTypeFlow& _resultFlow ) const
{
	_resultFlow.ClearAllPair();
	int iFlowPairCount = _compareFlow.GetFlowPairCount();
	for( int i=0; i<iFlowPairCount; ++i )
	{
//		// TRACE("%s\n", _compareFlow.GetFlowPairAt( i ).GetProcID().GetIDString() );
		bool bIsOwned = true;
		const CProcessorDestinationList* pPair = _oldFlow.GetHierarchyFlowPair( _compareFlow.GetFlowPairAt( i ).GetProcID() ,bIsOwned );
		
		if( bIsOwned || !pPair )
		{			
			_resultFlow.AddFowPair( _compareFlow.GetFlowPairAt( i ) );
		}
		else if ( pPair && !( *pPair == _compareFlow.GetFlowPairAt( i ) ) )
		{
			_resultFlow.AddFowPair( _compareFlow.GetFlowPairAt( i ) );
		}

	}
	
}

bool CPaxFlowConvertor::IsStation( const ProcessorID& _procID )const 
{ 
	ASSERT( m_pTerm );
	GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( _procID );
	ASSERT( groupIdx.start >= 0 );	
	Processor* pProc = m_pTerm->procList->getProcessor( groupIdx.start );
	if( pProc == NULL )
		return false;
	return pProc->getProcessorType() == IntegratedStationProc;
}
bool CPaxFlowConvertor::HandTurroundFlight( Flight* _pTurrountFlight, CAllPaxTypeFlow& _allPaxTypeFlow,int _iGateIdx )const
{
	FlightConstraint tempFlightConstraint = _pTurrountFlight->getType('A');
	CMobileElemConstraint tempConstrain(m_pTerm) ;
	//tempConstrain.SetInputTerminal( m_pTerm );
		
	char airline[AIRLINE_LEN];
	char airport[AIRPORT_LEN];
	char acType[AC_TYPE_LEN];
	char sStr[ 256 ];
	tempFlightConstraint.getAirline( airline);
	tempConstrain.setAirline( airline );

	tempFlightConstraint.getAirport( airport );
	tempConstrain.setAirport( airport );
	tempFlightConstraint.getStopoverAirport( airport );
	tempConstrain.setStopoverAirport( airport );

	tempFlightConstraint.getACType( acType);
	tempConstrain.setACType( acType );

	tempFlightConstraint.getAirlineGroup( sStr );
	tempConstrain.setAirlineGroup( sStr );

	tempFlightConstraint.getFlightGroup(sStr);
	tempConstrain.setFlightGroup(sStr); 

	tempConstrain.setFlightID (tempFlightConstraint.getFlightID() );
	tempConstrain.SetFltConstraintMode( tempFlightConstraint.GetFltConstraintMode() );

	tempFlightConstraint.getSector( sStr );
	tempConstrain.setSector( sStr );
	tempFlightConstraint.getStopoverSector( sStr );
	tempConstrain.setStopoverSector( sStr );

	tempFlightConstraint.getCategory( sStr );
	tempConstrain.setCategory( sStr );


	GateProcessor* pGateProc = ( GateProcessor* ) m_pTerm->procList->getProcessor( _iGateIdx );

	// handle arriving
	CSinglePaxTypeFlow* pNewPaxFlow = new CSinglePaxTypeFlow( m_pTerm );		
	// @gate proc and its dest proc			
	const Processor* pProcFromGate = m_pTerm->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	std::vector<int> vInbridge;
	m_pTerm->procList->GetInbridgeOfGate( _iGateIdx ,vInbridge);
	int iInbridgeCount = vInbridge.size();
	if( iInbridgeCount > 0 )
	{
	
		for( int iInbridge = 0; iInbridge < iInbridgeCount-1; ++iInbridge )
		{
			const Processor* pInbridgeProc = m_pTerm->procList->getProcessor( vInbridge[ iInbridge ] );

			// FromGate-->inbridge
			CFlowDestination inbridgeDest;
			inbridgeDest.SetProcID( *pInbridgeProc->getID() );
			pNewPaxFlow->AddDestProc( *pProcFromGate->getID() , inbridgeDest,false );

			// inbridge-->gate
		//	inbridgeDest.SetProcID( *pGateProc->getID() );
		//	pNewPaxFlow->AddDestProc( *pInbridgeProc->getID(), inbridgeDest );
		}
		
		const Processor* pInbridgeProc = m_pTerm->procList->getProcessor( vInbridge[ iInbridgeCount-1 ] );

		// FromGate-->inbridge
		CFlowDestination inbridgeDest;
		inbridgeDest.SetProcID( *pInbridgeProc->getID() );
		pNewPaxFlow->AddDestProc( *pProcFromGate->getID() , inbridgeDest);

		// inbridge-->gate
		//inbridgeDest.SetProcID( *pGateProc->getID() );
		//pNewPaxFlow->AddDestProc( *pInbridgeProc->getID(), inbridgeDest);

	}
	/*
	else
	{
		CFlowDestination fromGateDestArc;
		fromGateDestArc.SetProcID( *pGateProc->getID() );
		pNewPaxFlow->AddDestProc( *pProcFromGate->getID(), fromGateDestArc );
	}
	*/

	pNewPaxFlow->SetPaxConstrain( &tempConstrain );
	pNewPaxFlow->SetChangedFlag( true );
	//pNewPaxFlow->PrintAllStructure();
	_allPaxTypeFlow.AddPaxFlow( pNewPaxFlow , false );



	FlightConstraint tempDepFlight = _pTurrountFlight->getType('D');
	CMobileElemConstraint tempDepConstrain(m_pTerm) ;
	//tempDepConstrain.SetInputTerminal( m_pTerm );
		
	tempDepFlight.getAirline( airline);
	tempDepConstrain.setAirline( airline );

	tempDepFlight.getAirport( airport );
	tempDepConstrain.setAirport( airport );
	tempDepFlight.getStopoverAirport( airport );
	tempDepConstrain.setStopoverAirport( airport );

	tempDepFlight.getACType( acType);
	tempDepConstrain.setACType( acType );

	tempDepFlight.getAirlineGroup( sStr );
	tempDepConstrain.setAirlineGroup( sStr );

	tempDepConstrain.setFlightID (tempDepFlight.getFlightID() );
	tempDepConstrain.SetFltConstraintMode( tempDepFlight.GetFltConstraintMode() );

	tempDepFlight.getSector( sStr );
	tempDepConstrain.setSector( sStr );
	tempDepFlight.getStopoverSector( sStr );
	tempDepConstrain.setStopoverSector( sStr );

	tempDepFlight.getCategory( sStr );
	tempDepConstrain.setCategory( sStr );
	// handle departure
	pNewPaxFlow = new CSinglePaxTypeFlow( m_pTerm );		
		

	const Processor* pProcToGate = m_pTerm->procList->getProcessor( TO_GATE_PROCESS_INDEX );

	std::vector<int> vHoldingArea;
	m_pTerm->procList->GetHoldingAraOfGate( _iGateIdx, vHoldingArea );
	
	int iWaitAreaCount = vHoldingArea.size();
	if( iWaitAreaCount <= 0 )
	{
		const Processor* pEnd = m_pTerm->procList->getProcessor( END_PROCESSOR_INDEX );
		CFlowDestination tempDestArc;
		// To_Gate-->End
		tempDestArc.SetProcID( *pEnd->getID() );
		pNewPaxFlow->AddDestProc( *pProcToGate->getID(), tempDestArc );	
		pNewPaxFlow->SetPaxConstrain( &tempDepConstrain );
		pNewPaxFlow->SetChangedFlag( true );		
		_allPaxTypeFlow.AddPaxFlow( pNewPaxFlow , false );
		//pNewPaxFlow->PrintAllStructure();
		return true;
	}

	for( int i=0 ; i<iWaitAreaCount-1; ++i )
	{				
		Processor* pProc = m_pTerm->procList->getProcessor( vHoldingArea[ i ] );
		ASSERT( pProc );
		CFlowDestination tempDestArc;
		// To_Gate-->HoldingArea
		tempDestArc.SetProcID( *pProc->getID() );
		pNewPaxFlow->AddDestProc( *pProcToGate->getID(), tempDestArc,false );						
		//HoldingArea-->GateProcessor
		tempDestArc.SetProcID( *pGateProc->getID() );
		pNewPaxFlow->AddDestProc( *pProc->getID(), tempDestArc);
		
	}	
	Processor* pProc = m_pTerm->procList->getProcessor( vHoldingArea[ iWaitAreaCount-1 ] );
	ASSERT( pProc );
	//// TRACE("\n%s\n", pProc->getID()->GetIDString() );
	//// TRACE("\n%s\n", pGateProc->getID()->GetIDString() );
	CFlowDestination tempDestArc;
	// To_Gate-->HoldingArea
	tempDestArc.SetProcID( *pProc->getID() );
	pNewPaxFlow->AddDestProc( *pProcToGate->getID(), tempDestArc);						
	//HoldingArea-->GateProcessor
	tempDestArc.SetProcID( *pGateProc->getID() );
	pNewPaxFlow->AddDestProc( *pProc->getID(), tempDestArc );									

	pNewPaxFlow->SetPaxConstrain( &tempDepConstrain );
	pNewPaxFlow->SetChangedFlag( true );
	//pNewPaxFlow->PrintAllStructure();
	_allPaxTypeFlow.AddPaxFlow( pNewPaxFlow , false );

	return true;
}
bool CPaxFlowConvertor::BuildSpecFlightFlowFromGateAssign( CAllPaxTypeFlow& _allPaxTypeFlow )const
{
	ASSERT( m_pTerm );
	int iFlightCount = m_pTerm->flightSchedule->getCount();
	//m_pTerm->procList->RefreshRelatedDatabase();
	m_pTerm->procList->setMiscData( *(m_pTerm->miscData) );

	PassengerFlowDatabase* m_pFlightPaxFlowDB = m_pTerm->m_pSpecificFlightPaxFlowDB;
	ASSERT( m_pFlightPaxFlowDB );
	m_pFlightPaxFlowDB->clear();
	
	
	const Processor* pProcFromGate = m_pTerm->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	const Processor* pProcToGate = m_pTerm->procList->getProcessor( TO_GATE_PROCESS_INDEX );
	const Processor* pEnd = m_pTerm->procList->getProcessor( END_PROCESSOR_INDEX );
	for( int i=0; i<iFlightCount; ++i )
	{
		Flight* pFlight = m_pTerm->flightSchedule->getItem( i );
		ASSERT( pFlight );
		//int iGateIdx = m_pTerm->procList->findProcessor( pFlight->getGate() );
		int iGateIdx = -1; // 
		//ASSERT( iGateIdx>=0 && iGateIdx != INT_MAX );
		if( iGateIdx<0 || iGateIdx == INT_MAX  )
		{
			continue;
		}
		if( pFlight->isTurnaround() )
		{
			if( !HandTurroundFlight( pFlight,  _allPaxTypeFlow, iGateIdx  ) )
			{
				return false;
			}
			else
			{
				continue;
			}
			
		}
		//FlightConstraint tempFlight = pFlight->getType( pFlight->getMode() );
		FlightConstraint tempFlightConstraint = pFlight->getType(pFlight->getFlightMode());
		CMobileElemConstraint tempConstrain(m_pTerm) ;
		//tempConstrain.SetInputTerminal( m_pTerm );
		
		char airline[AIRLINE_LEN];
	    char airport[AIRPORT_LEN];
	    char acType[AC_TYPE_LEN];
		char sStr[ 256 ];
		tempFlightConstraint.getAirline( airline);
		tempConstrain.setAirline( airline );

		tempFlightConstraint.getAirport( airport );
		tempConstrain.setAirport( airport );
		tempFlightConstraint.getStopoverAirport( airport );
		tempConstrain.setStopoverAirport( airport );

		tempFlightConstraint.getACType( acType);
		tempConstrain.setACType( acType );

		tempFlightConstraint.getAirlineGroup( sStr );
		tempConstrain.setAirlineGroup( sStr );

		tempFlightConstraint.getFlightGroup( sStr );
		tempConstrain.setFlightGroup(sStr);

	    tempConstrain.setFlightID (tempFlightConstraint.getFlightID() );
	    tempConstrain.SetFltConstraintMode( tempFlightConstraint.GetFltConstraintMode() );

		tempFlightConstraint.getSector( sStr );
		tempConstrain.setSector( sStr );
		tempFlightConstraint.getStopoverSector( sStr );
		tempConstrain.setStopoverSector( sStr );

		tempFlightConstraint.getCategory( sStr );
		tempConstrain.setCategory( sStr );

		
		GateProcessor* pGateProc = ( GateProcessor* ) m_pTerm->procList->getProcessor( iGateIdx );
//		// TRACE("gate id =%s\n",pGateProc->getID()->GetIDString() );

		
		
		CSinglePaxTypeFlow* pNewPaxFlow = new CSinglePaxTypeFlow( m_pTerm );		
		
		if( pFlight->isArriving() )
		{
			// @gate proc and its dest proc			
			
			std::vector<int> vInbridge;
			m_pTerm->procList->GetInbridgeOfGate( iGateIdx ,vInbridge);
			int iInbridgeCount = vInbridge.size();
			if( iInbridgeCount > 0 )
			{
			
				for( int iInbridge = 0; iInbridge < iInbridgeCount-1; ++iInbridge )
				{
					const Processor* pInbridgeProc = m_pTerm->procList->getProcessor( vInbridge[ iInbridge ] );

					// FromGate-->inbridge
					CFlowDestination inbridgeDest;
					inbridgeDest.SetProcID( *pInbridgeProc->getID() );
					pNewPaxFlow->AddDestProc( *pProcFromGate->getID() , inbridgeDest,false );

					// inbridge-->gate
					//inbridgeDest.SetProcID( *pGateProc->getID() );
					//pNewPaxFlow->AddDestProc( *pInbridgeProc->getID(), inbridgeDest );
				}
				
				const Processor* pInbridgeProc = m_pTerm->procList->getProcessor( vInbridge[ iInbridgeCount-1 ] );

				// FromGate-->inbridge
				CFlowDestination inbridgeDest;
				inbridgeDest.SetProcID( *pInbridgeProc->getID() );
				pNewPaxFlow->AddDestProc( *pProcFromGate->getID() , inbridgeDest);

				// inbridge-->gate
				//inbridgeDest.SetProcID( *pGateProc->getID() );
				//pNewPaxFlow->AddDestProc( *pInbridgeProc->getID(), inbridgeDest);

			}
			/*
			else
			{
				CFlowDestination fromGateDestArc;
				fromGateDestArc.SetProcID( *pGateProc->getID() );
				pNewPaxFlow->AddDestProc( *pProcFromGate->getID(), fromGateDestArc );
			}
			*/
		}

		if( pFlight->isDeparting() )
		{
			//tempConstrain.setFlightID( pFlight->getDepID() );

			

			std::vector<int> vHoldingArea;
			m_pTerm->procList->GetHoldingAraOfGate( iGateIdx, vHoldingArea );
			
			int iWaitAreaCount = vHoldingArea.size();
			if( iWaitAreaCount <= 0 )
			{
				CFlowDestination tempDestArc;
				// To_Gate-->End
				tempDestArc.SetProcID( *pEnd->getID() );
				pNewPaxFlow->AddDestProc( *pProcToGate->getID(), tempDestArc );						
				pNewPaxFlow->SetPaxConstrain( &tempConstrain );
				pNewPaxFlow->SetChangedFlag( true );		
				_allPaxTypeFlow.AddPaxFlow( pNewPaxFlow , false );
				continue;
			}
	
			for( int i=0 ; i<iWaitAreaCount-1; ++i )
			{				
				Processor* pProc = m_pTerm->procList->getProcessor( vHoldingArea[ i ] );
				ASSERT( pProc );
				CFlowDestination tempDestArc;
				// To_Gate-->HoldingArea
				tempDestArc.SetProcID( *pProc->getID() );
				pNewPaxFlow->AddDestProc( *pProcToGate->getID(), tempDestArc,false );						
				//HoldingArea-->GateProcessor
				tempDestArc.SetProcID( *pGateProc->getID() );
				pNewPaxFlow->AddDestProc( *pProc->getID(), tempDestArc);									
			}	
			Processor* pProc = m_pTerm->procList->getProcessor( vHoldingArea[ iWaitAreaCount-1 ] );
			ASSERT( pProc );
			//// TRACE("\n%s\n", pProc->getID()->GetIDString() );
			//// TRACE("\n%s\n", pGateProc->getID()->GetIDString() );
			CFlowDestination tempDestArc;
			// To_Gate-->HoldingArea
			tempDestArc.SetProcID( *pProc->getID() );
			pNewPaxFlow->AddDestProc( *pProcToGate->getID(), tempDestArc);						
			//HoldingArea-->GateProcessor
			tempDestArc.SetProcID( *pGateProc->getID() );
			pNewPaxFlow->AddDestProc( *pProc->getID(), tempDestArc );									

		}

		pNewPaxFlow->SetPaxConstrain( &tempConstrain );
		pNewPaxFlow->SetChangedFlag( true );
		//pNewPaxFlow->PrintAllStructure();
		_allPaxTypeFlow.AddPaxFlow( pNewPaxFlow , false );
	}

	_allPaxTypeFlow.Sort();
	_allPaxTypeFlow.InitHierachy();
	//_allPaxTypeFlow.PrintAllStructure();
	return true;

}
void CPaxFlowConvertor::ToDigraphStructureFromSinglePaxDB( CAllPaxTypeFlow& _allPaxTypeFlow,PassengerFlowDatabase* _pPaxFlowDB ) const
{
	ASSERT( m_pTerm );
	ASSERT( _pPaxFlowDB );
	PassengerFlowDatabase* m_pPaxFlowDB = _pPaxFlowDB;
	int nCount = m_pPaxFlowDB->getCount();
	for( int i = 0; i < nCount; i++ )
	{
		CMobileElemConstraintDatabase* pPaxConDB = m_pPaxFlowDB->getDatabase( i );

		if( pPaxConDB == NULL )
			continue;
		
		int nPaxConCount = pPaxConDB->getCount();
		if( nPaxConCount <= 0 )
			continue;

		const ProcessorID* pProcID =  m_pPaxFlowDB->getItem( i )->getID();
		for (int j = 0; j < nPaxConCount; j++)
		{
			
			ConstraintEntry* pEntry = pPaxConDB->getItem( j );
			const Constraint* pCon = pEntry->getConstraint();
			CSinglePaxTypeFlow* pSinglePaxFlow = GetRelatedPaxTypeFlow( _allPaxTypeFlow, pCon );

			CProcessorDestinationList tempPair( m_pTerm );
			tempPair.SetProcID( *pProcID );
//			// TRACE("%s\n",pProcID->GetIDString());
			
			const CProcessorDistributionWithPipe* pProcDist = (const CProcessorDistributionWithPipe*) pEntry->getValue();
			int iProcCount = pProcDist->GetDestCount();
			//  [7/5/2004]int iProbabilityCount = pProcDist->getCount();
			
			int nCum = 0;
			for( int s=0; s< iProcCount ; ++s )
			{
				CProcessorDistributionWithPipe* pProcDistWithPipe = (CProcessorDistributionWithPipe*)pProcDist;
				CFlowDestination tempDestProcInfo;
				tempDestProcInfo.SetProcID( *pProcDist->getGroup( s ) );
//				// TRACE("%s\n",tempDestProcInfo.GetProcID().GetIDString());
				/*[7/5/2004]
				if(s < iProbabilityCount)
				{
					int nProb = pProcDist->getProb( s );
					tempDestProcInfo.SetAppointedPercentFlag(true);
					tempDestProcInfo.SetProbality( nProb - nCum );
					nCum = nProb;					
				}
				*/
				int nProb = static_cast<int>(pProcDist->getProb( s ));
				tempDestProcInfo.SetProbality( nProb - nCum );
				nCum = nProb;
				
//				tempDestProcInfo.SetInheritedFlag( false );
				tempDestProcInfo.SetTypeOfOwnership( 0 );
				tempDestProcInfo.GetPipeVector() = pProcDistWithPipe->GetPipeVector().at( s ).GetPipeVector();
				tempDestProcInfo.SetTypeOfUsingPipe( pProcDistWithPipe->GetPipeVector().at( s ).GetTypeOfUsingPipe() );
				tempDestProcInfo.SetOneToOneFlag( pProcDistWithPipe->GetPipeVector().at( s ).GetOneToOneFlag() );
				tempDestProcInfo.SetOneXOneState( pProcDistWithPipe->GetPipeVector().at( s ).GetChannelState() );
				tempDestProcInfo.SetFollowState( pProcDistWithPipe->GetPipeVector().at( s ).GetFollowState() );
				
				
				tempDestProcInfo.SetAvoidDensity( pProcDistWithPipe->GetPipeVector().at( s ).GetAvoidDensity() );
				tempDestProcInfo.SetAvoidFixQueue( pProcDistWithPipe->GetPipeVector().at( s ).GetAvoidFixQueue() );
				tempDestProcInfo.SetAvoidOverFlowQueue( pProcDistWithPipe->GetPipeVector().at( s ).GetAvoidOverFlowQueue() );
				tempDestProcInfo.SetDensityOfArea( pProcDistWithPipe->GetPipeVector().at( s ).GetDensityOfArea() );
				tempDestProcInfo.SetMaxQueueLength( pProcDistWithPipe->GetPipeVector().at( s ).GetMaxQueueLength() );
				tempDestProcInfo.SetMaxWaitMins( pProcDistWithPipe->GetPipeVector().at( s ).GetMaxWaitMins() );
				tempDestProcInfo.SetMaxSkipTime( pProcDistWithPipe->GetPipeVector().at( s).GetMaxSkipTime());

				tempDestProcInfo.GetDensityArea() = pProcDistWithPipe->GetPipeVector().at( s ).GetDensityArea();
				/*[7/5/2004]
				if(pProcDistWithPipe->GetDestCount() > pProcDistWithPipe->getCount() && s>= pProcDistWithPipe->getCount()-1)
				{
					if(s==pProcDistWithPipe->getCount()-1)
					{
						nCum=tempDestProcInfo.GetProbality();
					}
					tempDestProcInfo.SetAppointedPercentFlag(false);
					int nProb=nCum/(pProcDistWithPipe->GetDestCount() - s);
					tempDestProcInfo.SetProbality(nProb);
					nCum -= nProb;
				}
				*/
				
				// some pipes should be added here
				tempPair.AddDestProc( tempDestProcInfo,false );	
			}
			pSinglePaxFlow->AddFowPair( tempPair );
			
		}
	}

	//very important
	_allPaxTypeFlow.Sort();

	_allPaxTypeFlow.InitHierachy();	
}

void CPaxFlowConvertor::ToOldFlightFlow( const CAllPaxTypeFlow& _allPaxTypeFlow  )const
{
	ASSERT( m_pTerm );
	PassengerFlowDatabase* m_pFlightPaxFlowDB = m_pTerm->m_pSpecificFlightPaxFlowDB;
	int iSingleFlowCount = _allPaxTypeFlow.GetSinglePaxTypeFlowCount();
	for(int i=0; i<iSingleFlowCount; ++i )
	{
		const CSinglePaxTypeFlow* pFlow = _allPaxTypeFlow.GetSinglePaxTypeFlowAt( i );
		if( pFlow->IfFlowBeChanged() )
		{
			m_pFlightPaxFlowDB->deletePaxType( pFlow->GetPassengerConstrain() );
			int iFlowPairCount = pFlow->GetFlowPairCount();
			for( int j=0; j<iFlowPairCount; ++j )
			{				
				const CProcessorDestinationList& pFlowPair = pFlow->GetFlowPairAt( j );
				int iDestCount  = pFlowPair.GetDestCount();
//				// TRACE("%s\n",pFlowPair.GetProcID().GetIDString());
				if( iDestCount > 0 )
				{
					
					CMobileElemConstraint* pPaxCon = new CMobileElemConstraint(m_pTerm);
					*pPaxCon = *( pFlow->GetPassengerConstrain() );
					CProcessorDistributionWithPipe* pProcDist = new CProcessorDistributionWithPipe( false );
					ProcessorID* pDestList = new ProcessorID[ iDestCount ];				
					double* pProbabilities = new double[ iDestCount ];
					
					for( int k=0; k<iDestCount; ++k )
					{
						const CFlowDestination& pDestInfo = pFlowPair.GetDestProcAt( k );
						pDestList[ k ] = pDestInfo.GetProcID();
						pProbabilities[ k ] = pDestInfo.GetProbality();							
//						// TRACE("%s\n",pDestInfo.GetProcID().GetIDString());
						//// TRACE("%d\n",pProbabilities[k] );
						CFlowItemEx tempPipe;
						tempPipe.GetPipeVector() = ((CFlowDestination&)pDestInfo).GetPipeVector();
						tempPipe.SetTypeOfUsingPipe( pDestInfo.GetTypeOfUsingPipe() );
						tempPipe.SetOneToOneFlag( pDestInfo.GetOneToOneFlag() );
						tempPipe.SetChannelState( pDestInfo.GetOneXOneState() );
						tempPipe.SetFollowState( pDestInfo.GetFollowState() );

						tempPipe.SetAvoidDensity( pDestInfo.GetAvoidDensity() );
						tempPipe.SetAvoidFixQueue( pDestInfo.GetAvoidFixQueue() );
						tempPipe.SetAvoidOverFlowQueue( pDestInfo.GetAvoidOverFlowQueue() );
						tempPipe.SetDensityOfArea( pDestInfo.GetDensityOfArea() );
						tempPipe.SetMaxQueueLength( pDestInfo.GetMaxQueueLength() );
						tempPipe.SetMaxWaitMins( pDestInfo.GetMaxWaitMins() );
						tempPipe.SetMaxSkipTime( pDestInfo.GetMaxSkipTime());

						tempPipe.GetDensityArea() = ((CFlowDestination&)pDestInfo).GetDensityArea();

						pProcDist->GetPipeVector().push_back( tempPipe );					
						
					}
					
					pProcDist->initDestinations( pDestList, pProbabilities, iDestCount );
					ConstraintEntry* pConEntry = new ConstraintEntry();
					pConEntry->initialize( pPaxCon, pProcDist );
					m_pFlightPaxFlowDB->insert(  pFlowPair.GetProcID(), pConEntry, true );
				}
			}
		}
	}			
}

/************************************************************************
 Rebuild _resultFlow depend on _paxFlow ,current _nonPaxFlow

 INPUT: _paxFlow , _nonPaxFlow , 
		_paxFlow is nonPax owner's flow
 OUTPUT:_resultFlow , _nonPaxFlow
 For example:    

 input:
   _paxFlow: A,B,C,D,E  (A->B->C->D->E)
   _nonPaxFlow: NULL
 output:
   _resultFlow: A,B,C,D,E  (A->B->C->D->E)
   _nonPaxFlow: NULL
 
 input:
   _paxFlow: A,B,C,D,E  (A->B->C->D->E)
   _nonPaxFlow: C,H     (C->H->D)
 output:
   _resultFlow: A,B,C,H,D,E (A->B->C->H->D->E)
   _nonPaxFlow: C,H		(C->H->D) 

  input:
  _paxFlow: A,B,C,D,E  (A->B->C->D->E)    
  _nonPaxFlow: C       (C->D->E)
  output:
  _resultFlow: A,B,C,D,E (A->B->C->D->E)
  _nonPaxFlow: NULL	       earse Flowfair from nonPaxFlow if nonPaxFlow's Flowfair as same as PaxFlow's Flowfair
  
  input:
   _paxFlow: A,B,C,D,E  (A->B->C->D->E)
   _nonPaxFlow: A, C, H (A->B->C,C->H->D)
  output:
   _resultFlow: A,B,C,D,E (A->B->C->H->D->E)
   _nonPaxFlow: C,H 	 (C->H->D)	    
	

 _resultFow is the new operatedFlow  
 See also: BuildNonPaxFlowToFile ,CombineNonPaxFlow
************************************************************************/
void CPaxFlowConvertor::BuildAttachedNonPaxFlow( const CSinglePaxTypeFlow& _paxFlow,  CSinglePaxTypeFlow& _nonPaxFlow, CSinglePaxTypeFlow& _resultFlow ) const
{
	_resultFlow.ClearAllPair();
	std::vector<ProcessorID > allRoot;
	_paxFlow.GetRootProc( allRoot );
	int iRootCount = allRoot.size();
	for( int i=0; i<iRootCount; ++i )
	{
		*m_pTempNonPaxFlow = _nonPaxFlow;
		CombineNonPaxFlow( _paxFlow, _nonPaxFlow, _resultFlow, allRoot.at( i ) );
	}
}

/*
 Attention Function BuildAttachedNonPaxFlow
 */
void CPaxFlowConvertor::CombineNonPaxFlow( const CSinglePaxTypeFlow& _paxFlow,  CSinglePaxTypeFlow& _nonPaxFlow, CSinglePaxTypeFlow& _resultFlow , const ProcessorID& _fromProc ) const
{
// 	 TRACE("\n%s\n", _fromProc.GetIDString() );
 	if( _resultFlow.IfExist( _fromProc ) )
 	{
 		return ;
 	}
	if( _nonPaxFlow.IfExist( _fromProc ) )
	{
		CProcessorDestinationList tempPair =*(_nonPaxFlow.GetEqualFlowPairAt( _fromProc ));
		CProcessorDestinationList _tempPair = *(m_pTempNonPaxFlow->GetEqualFlowPairAt( _fromProc ));

		CProcessorDestinationList comparePair(m_pTerm);
		if (_paxFlow.IfExist(_fromProc))
		{
			_paxFlow.GetEqualFlowPairAt( _fromProc, comparePair );
		}
		else
		{
			comparePair.ClearAllDestProc() ;
		}

		if (tempPair == comparePair || _tempPair == comparePair) // earse Flowfair from nonPaxFlow if nonPaxFlow's Flowfair as same as PaxFlow's Flowfair
		{
			_nonPaxFlow.DeleteProcNode(_fromProc);  //changed nonPaxFlow 
			_nonPaxFlow.SetChangedFlag( true );
			((CProcessorDestinationList& )tempPair).SetAllDestInherigeFlag( 2 );
			_resultFlow.AddFowPair( _tempPair );
			int iDestCount = _tempPair.GetDestCount();
			for( int i=0; i<iDestCount; ++i )
			{
				CombineNonPaxFlow( _paxFlow, _nonPaxFlow, _resultFlow, _tempPair.GetDestProcAt(i).GetProcID() );
			}
		}
		else
		{
			int iDestCount = tempPair.GetDestCount();
			if( iDestCount > 0 )
			{
				_resultFlow.AddFowPair( tempPair );
				for( int i=0; i<iDestCount; ++i )
				{
					CombineNonPaxFlow( _paxFlow, _nonPaxFlow, _resultFlow, tempPair.GetDestProcAt(i).GetProcID() );
				}
			}
			else
			{
				if( _paxFlow.IfExist( _fromProc ) )
				{
					CProcessorDestinationList tempPair( m_pTerm );
					_paxFlow.GetEqualFlowPairAt( _fromProc, tempPair );
					//((CProcessorDestinationList& )tempPair).SetAllDestInherigeFlag( false );
					((CProcessorDestinationList& )tempPair).SetAllDestInherigeFlag( 2 );
					_resultFlow.AddFowPair( tempPair );
					int iDestCount = tempPair.GetDestCount();
					for( int i=0; i<iDestCount; ++i )
					{
						CombineNonPaxFlow( _paxFlow, _nonPaxFlow, _resultFlow, tempPair.GetDestProcAt(i).GetProcID() );
					}
				}
				else
				{
					_resultFlow.AddFowPair( _tempPair );
				}				
			}
		}
	}
	else if( _paxFlow.IfExist( _fromProc ) )
	{
		CProcessorDestinationList tempPair( m_pTerm );
		_paxFlow.GetEqualFlowPairAt( _fromProc, tempPair );
		//((CProcessorDestinationList& )tempPair).SetAllDestInherigeFlag( false );
		((CProcessorDestinationList& )tempPair).SetAllDestInherigeFlag( 2 );
		_resultFlow.AddFowPair( tempPair );
		int iDestCount = tempPair.GetDestCount();
		for( int i=0; i<iDestCount; ++i )
		{
			CombineNonPaxFlow( _paxFlow, _nonPaxFlow, _resultFlow, tempPair.GetDestProcAt(i).GetProcID() );
		}
	}
	
}

/************************************************************************
  build _resultFlow from _paxFlow,_nonPaxFlow,_operatePaxFlow in order to store it into file
  see also :  BuildAttachedNonPaxFlow , DividNonPaxFlow                                                                      
************************************************************************/
void CPaxFlowConvertor::BuildNonPaxFlowToFile( const CSinglePaxTypeFlow& _paxFlow, const CSinglePaxTypeFlow& _nonPaxFlow, const CSinglePaxTypeFlow& _operatePaxFlow, CSinglePaxTypeFlow& _resultFlow ) const
{
//	_paxFlow.PrintAllStructure();
//	_nonPaxFlow.PrintAllStructure();
//	_operatePaxFlow.PrintAllStructure();
	
	_resultFlow.ClearAllPair();

	std::vector<ProcessorID > allRoot;
/*
	const Processor* pProcStart = m_pTerm->procList->getProcessor( START_PROCESSOR_INDEX );
	if( _nonPaxFlow.IfExist( *pProcStart->getID() ))
	{
		DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow,_resultFlow, _nonPaxFlow.GetFlowPairAt( *pProcStart->getID() ).GetProcID() );
	}
	else
	{
*/
		_operatePaxFlow.GetRootProc( allRoot );	
		int iRootCount = allRoot.size();
		for( int i=0; i<iRootCount; ++i )
		{
			DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow,_resultFlow, allRoot.at( i ) );
		}
//	}	
}

/*******************************************************************************
  create _resultFlow depend on _paxFlow ,current _nonPaxFlow,_operatePaxFlow
  INPUT: _paxFlow ,_nonPaxFlow , _operatePaxFlow , _fromProc
         _operatePaxFlow is current flow that show on tree
         _paxFlow is nonPax owner's flow
  OUTPUT:_resultFlow ,
           _resultFow is the new nonPaxFlow that we wanted. 

  For example:   _paxFlow: A,B,C,D,E      (A->B->C->D->E)
              _nonPaxFlow: NULL 
		  _operatePaxFlow: A,B,C,D,H,D,E   (A->B->C->H->D->E)      H is new added node from User Interface (CTreeCtrl)

	   return _resultFlow: C,H       (C->H->D)

See Also : BuildHierarchyFlow
********************************************************************************/
void CPaxFlowConvertor::DividNonPaxFlow( const CSinglePaxTypeFlow& _paxFlow, const CSinglePaxTypeFlow& _nonPaxFlow, const CSinglePaxTypeFlow& _operatePaxFlow, CSinglePaxTypeFlow& _resultFlow , const ProcessorID& _fromProc ) const
{
	 TRACE("\n%s\n", _fromProc.GetIDString() );

//	const CProcessorDestinationList& operatorFlowPair = _operatePaxFlow.GetFlowPairAt( _fromProc );
	
	CProcessorDestinationList *pOperatorFlowPair = ((CSinglePaxTypeFlow *)(&_operatePaxFlow))->GetEqualFlowPairAt( _fromProc );
	if(pOperatorFlowPair == NULL)
		pOperatorFlowPair = ((CSinglePaxTypeFlow *)(&_operatePaxFlow))->GetFlowPairAt( _fromProc );

    if(pOperatorFlowPair == NULL) 
		return;

	CProcessorDestinationList& operatorFlowPair = *pOperatorFlowPair;

//	_nonPaxFlow.PrintAllStructure();
//	_paxFlow.PrintAllStructure();
	bool bExistInNoPaxFlow = _nonPaxFlow.IfExist( _fromProc );
	bool bExistInPaxFlow = _paxFlow.IfExist( _fromProc );
	if( !bExistInNoPaxFlow && !bExistInPaxFlow )// new pair
	{
		_resultFlow.AddFowPair( operatorFlowPair );
		int iDestCount = operatorFlowPair.GetDestCount();
		for( int i=0; i<iDestCount; ++i )
		{
			if (!HandleSingleFlowLogic::IfHaveCircleFromProc((CSinglePaxTypeFlow *)(&_operatePaxFlow),operatorFlowPair.GetDestProcAt( i ).GetProcID()))
			{
				DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow, _resultFlow,operatorFlowPair.GetDestProcAt( i ).GetProcID() );
			}
		}
	}
	else if( !bExistInNoPaxFlow && bExistInPaxFlow )
	{
		CProcessorDestinationList tempPaxPair( m_pTerm );
		_paxFlow.GetFlowPairAt( _fromProc, tempPaxPair );
		if( tempPaxPair == operatorFlowPair )
		{
			int iDestCount = tempPaxPair.GetDestCount();
			for( int i=0; i<iDestCount; ++i )
			{
				if (!HandleSingleFlowLogic::IfHaveCircleFromProc((CSinglePaxTypeFlow *)(&_operatePaxFlow),operatorFlowPair.GetDestProcAt( i ).GetProcID()))
				{
					DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow, _resultFlow,tempPaxPair.GetDestProcAt( i ).GetProcID() );
				}
			}
		}
		else
		{
			_resultFlow.AddFowPair( operatorFlowPair );
			int iDestCount = operatorFlowPair.GetDestCount();
			for( int i=0; i<iDestCount; ++i )
			{
				if (!HandleSingleFlowLogic::IfHaveCircleFromProc((CSinglePaxTypeFlow *)(&_operatePaxFlow),operatorFlowPair.GetDestProcAt( i ).GetProcID()))
				{
					DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow, _resultFlow,operatorFlowPair.GetDestProcAt( i ).GetProcID() );
				}
			}
			return;
		}
	}
	else // both paxflow and nonpaxflow have the pair is same to nopaxflow have but paxflow donn't have.
	{
		//const CProcessorDestinationList& tempNonPair = _nonPaxFlow.GetFlowPairAt( _fromProc );
		//if( tempNonPair == operatorFlowPair )
		//{
		_resultFlow.AddFowPair( operatorFlowPair );
		int iDestCount = operatorFlowPair.GetDestCount();
		for( int i=0; i<iDestCount; ++i )
		{
			if (!HandleSingleFlowLogic::IfHaveCircleFromProc((CSinglePaxTypeFlow *)(&_operatePaxFlow),operatorFlowPair.GetDestProcAt( i ).GetProcID()))
			{
				DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow, _resultFlow,operatorFlowPair.GetDestProcAt( i ).GetProcID() );
			}
		}
		/*
		else
		{
			//tempNonPair = _operatePaxFlow.GetFlowPairAt( _fromProc );
			const CProcessorDestinationList& pairInPax = _paxFlow.GetFlowPairAt( _fromProc );
			if( !(pairInPax == operatorFlowPair ) )
			{
				_resultFlow.AddFowPair( operatorFlowPair );

			}
			iDestCount = operatorFlowPair.GetDestCount();
			for( int i=0; i<iDestCount; ++i )
			{
				DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow, _resultFlow,operatorFlowPair.GetDestProcAt( i ).GetProcID() );
			}
		}
		*/

	
	//	}
	/*	else
		{
			_resultFlow.AddFowPair( operatorFlowPair );
			int iDestCount = operatorFlowPair.GetDestCount();
			for( int i=0; i<iDestCount; ++i )
			{
				DividNonPaxFlow( _paxFlow, _nonPaxFlow, _operatePaxFlow, _resultFlow,operatorFlowPair.GetDestProcAt( i ).GetProcID() );
			}
			return;
		}
	*/
	}

}