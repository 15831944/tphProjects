// DirectionUtil.cpp: implementation of the CDirectionUtil class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DirectionUtil.h"
#include "AllPaxTypeFlow.h"
#include "PaxFlowConvertor.h"
#include "SubFlowList.h"
#include "SubFlow.h"
#include "engine\proclist.h"
#include "../Common/ARCTracker.h"
#include "engine\ProcessorDistributionTransformer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define TRACE_SINGLEFLOW
CDirectionUtil::CDirectionUtil()
{
	m_pLTLinkageManager = NULL;

}

CDirectionUtil::~CDirectionUtil()
{

}
/************************************************************************
INPUT : _convertor, the convertor which can translate old structure to diagraph structure
                   or convent diagraph structure to old distribution structure
OUTPUT: 

/************************************************************************/

void CDirectionUtil::InitAllProcDirection( CPaxFlowConvertor& _convertor,LandsideTerminalLinkageInSimManager *pLTLinkage )
{
	PLACE_METHOD_TRACK_STRING();
	
	#ifdef TRACE_SINGLEFLOW	
	ofsstream echoFile (_T("c:\\singleflow.tmp"), stdios::out);
	echoFile<<"\r\n";
	echoFile.close();
	#endif

	InputTerminal* pTerm = _convertor.GetInputTerm();
	ASSERT(	pTerm );
	m_pLTLinkageManager = pLTLinkage;
	CAllPaxTypeFlow tempAllPaxFlow;
	//tempAllPaxFlow.FromOldToDigraphStructure( _convertor );

	CPassengerFlowDatabaseTransformer expandPaxFlow;
	expandPaxFlow.ExpandPaxFlow(pTerm->m_pPassengerFlowDB,*(pTerm->m_pSubFlowList));

	_convertor.ToDigraphStructureFromSinglePaxDB( tempAllPaxFlow,  &expandPaxFlow);
	_convertor.ToDigraphStructureFromSinglePaxDB( tempAllPaxFlow, pTerm->m_pStationPaxFlowDB );

	int iFlowCount = tempAllPaxFlow.GetSinglePaxTypeFlowCount();
	for( int i=0; i<iFlowCount; ++i )
	{
		const CMobileElemConstraint* pConstrint = tempAllPaxFlow.GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain();
		//char str[256];
		//pConstrint->screenPrint( str,0,256 );
		//// TRACE("\n%s\n",str);
		//if( pConstrint->isDeparture() || pConstrint->isTurround() || pConstrint->isDefault() )
		//{
			CSinglePaxTypeFlow operaterFlow( pTerm );
			operaterFlow.SetPaxConstrain( pConstrint );
			if( pConstrint->GetTypeIndex() == 0 )//pax constraint
			{
				tempAllPaxFlow.BuildHierarchyFlow( _convertor, i , operaterFlow );
			}
			else//non pax constraint
			{
				CMobileElemConstraint attachedPaxConstraint ( *pConstrint);
				//char strPconst [ 64 ];
				//pConstrint->screenPrint( strPconst );
				////// TRACE("\n%s\n", strPconst );
				attachedPaxConstraint.SetTypeIndex( 0 );
				attachedPaxConstraint.SetMobileElementType(enum_MobileElementType_PAX);
				//attachedPaxConstraint.screenPrint( strPconst );
				////// TRACE("\n%s\n", strPconst );


				//pax flow				
				CSinglePaxTypeFlow m_paxFlowAttachedToNonPaxFlow( pTerm );			
				m_paxFlowAttachedToNonPaxFlow.SetPaxConstrain( &attachedPaxConstraint );			
				
				//m_paxFlowAttachedToNonPaxFlow->GetPassengerConstrain()->screenPrint( strPconst );
				////// TRACE("\n%s\n", strPconst );
				int iIdx = tempAllPaxFlow.GetIdxIfExist( m_paxFlowAttachedToNonPaxFlow );
				if( iIdx < 0 )//not exist
				{
					CSinglePaxTypeFlow* pPaxFlowAttachedToNonPaxFlow = new CSinglePaxTypeFlow ( pTerm );
					pPaxFlowAttachedToNonPaxFlow->SetPaxConstrain( &attachedPaxConstraint );
					//char str[ 64 ];
					//attachedPaxConstraint.screenPrint( str );
					////// TRACE("\n%s\n", str );
					tempAllPaxFlow.AddPaxFlow( pPaxFlowAttachedToNonPaxFlow );
					iIdx = tempAllPaxFlow.GetIdxIfExist( m_paxFlowAttachedToNonPaxFlow );
					ASSERT( iIdx >= 0 );
					tempAllPaxFlow.BuildHierarchyFlow( _convertor, iIdx , m_paxFlowAttachedToNonPaxFlow );
					tempAllPaxFlow.DeleteSinglePaxTypeFlowAt( iIdx );
				}
				else
				{
					tempAllPaxFlow.BuildHierarchyFlow( _convertor, iIdx , m_paxFlowAttachedToNonPaxFlow );
				}


				//non pax flow
				CSinglePaxTypeFlow m_nonPaxFlow ( pTerm  );		
				m_nonPaxFlow.SetPaxConstrain( pConstrint );
				iIdx = tempAllPaxFlow.GetIdxIfExist( m_nonPaxFlow );
				ASSERT( iIdx >= 0 );
				
				tempAllPaxFlow.BuildHierarchyFlow( _convertor, iIdx , m_nonPaxFlow );
				_convertor.BuildAttachedNonPaxFlow( m_paxFlowAttachedToNonPaxFlow, m_nonPaxFlow, operaterFlow );
			}

			//operaterFlow.PrintAllStructure();
		//	ExpandAllProcessInFlow( &operaterFlow );
//			operaterFlow.PrintAllStructure();

			//CString strIndex;
			//pConstrint->screenPrint(strIndex);
			//CString str;
			//str.Format("Start %d --- ", i);
			//str += strIndex;
			//AfxMessageBox(str);

 			std::vector<CSinglePaxTypeFlow*> vAllPossibleFlow;
 			BuildAllPossiblePath( &operaterFlow, vAllPossibleFlow );

			//AfxMessageBox("End");

	/*		int iPathCount = vAllPossibleFlow.size();
			for( int j=0; j<iPathCount; ++j )
			{
				BuildDirctionFromEverySinglePath( vAllPossibleFlow[j], *pConstrint );
				delete vAllPossibleFlow[ j ];
			}
			*/
			
	//	}
	}
	

	//int nSize = pTerm->m_pSubFlowList->GetProcessUnitCount();
	//for(int s = 0; s < nSize; s++)
	//{
	//	CSubFlow* pFlow = pTerm->m_pSubFlowList->GetProcessUnitAt(s);
	//	pFlow->PrintGateInfo();
	//}
	//int nProcCount = pTerm->procList->getProcessorCount();
	//for (int nProc = 0; nProc < nProcCount; ++nProc)
	//{
	//	Processor *pProc = pTerm->procList->getProcessor(nProc);
	//	if(pProc)
	//	{
	//		TRACE("\r\n Proc ID: %s",pProc->getID()->GetIDString());

	//		int nCount = (int)pProc->GetDirection().GetPaxDirection().size();
	//		for (int nDir = 0; nDir < nCount; ++nDir)
	//		{
	//			CPaxDirection& dir = pProc->GetDirection().GetPaxDirection().at(nDir);
	//			CString strPaxType;
	//			dir.GetPaxType().screenPrint(strPaxType);
	//			TRACE("\r\n		 Pax Type: %s",strPaxType);
	//			int iSize = (int) dir.GetCanGoToProcessor().size();
	//			for(int i = 0; i < iSize; ++i)
	//			{
	//				ProcessorID& procID = dir.GetCanGoToProcessor().at(i);
	//				TRACE("\r\n		Gate: %s",procID.GetIDString());
	//			}

	//		}

	//		TRACE(_T("\r\n"));
	//	}

	//}



	//print the landside direction information into file.

#ifdef _DEBUG

	ofsstream echoFile (_T("C:\\Landside_ArrivalPax_LeadTo.log"), stdios::out);
	int nProcCount = pTerm->procList->getProcessorCount();
	for (int nProc = 0; nProc < nProcCount; ++nProc)
	{
		Processor *pProc = pTerm->procList->getProcessor(nProc);
		if(pProc)
		{

			echoFile<<"\r\n";
			echoFile<< pProc->getID()->GetIDString() <<"\r\n";

			echoFile<< "\t Terminal Direction" <<"\r\n";
			int nCount = (int)pProc->GetDirection().GetPaxDirection().size();
			int nDir = 0;
			for (; nDir < nCount; ++nDir)
			{
				CPaxDirection& dir = pProc->GetDirection().GetPaxDirection().at(nDir);
				CString strPaxType;
				dir.GetPaxType().screenPrint(strPaxType);
				echoFile<< "\t \t"<<strPaxType <<"\r\n";
				//TRACE("\r\n		 Pax Type: %s",strPaxType);

				int iSize = (int) dir.GetCanGoToProcessor().size();
				for(int i = 0; i < iSize; ++i)
				{
					ProcessorID& procID = dir.GetCanGoToProcessor().at(i);
					echoFile<< "\t \t \t"<<procID.GetIDString() <<"\r\n";
					//TRACE("\r\n		Gate: %s",procID.GetIDString());

				}
			}

			echoFile<< "\t Landside Direction" <<"\r\n";
			nCount = (int)pProc->GetLandsideDrection().GetPaxDirection().size();
			nDir = 0;
			for (; nDir < nCount; ++nDir)
			{
				CPaxDirection& dir = pProc->GetLandsideDrection().GetPaxDirection().at(nDir);
				CString strPaxType;
				dir.GetPaxType().screenPrint(strPaxType);
				echoFile<< "\t \t"<<strPaxType <<"\r\n";
				//TRACE("\r\n		 Pax Type: %s",strPaxType);

				int iSize = (int) dir.GetCanGoToObject().size();
				for(int i = 0; i < iSize; ++i)
				{
					LandsideObjectLinkageItemInSim& leadtoObject = dir.GetCanGoToObject().at(i);
					
					CString strLevel;
					size_t levelCount = leadtoObject.getLevels().size();
					for (size_t le = 0; le < levelCount; ++ le)
					{
						CString str;
						str.Format(_T("%d"), leadtoObject.getLevels().at(le));
						strLevel += str;
						strLevel += _T(",");
					}
					
					echoFile<< "\t \t \t"<<leadtoObject.getObject().GetIDString()<<";"
										 <<strLevel<<";"
										 <<leadtoObject.getOrigin()<<"\r\n";

				}
			}
		}
	}

	echoFile.close();
#endif // _DEBUG

}

/************************************************************************
FUNCTION :Find all the sub flow from the specified flow(_pFlow),analyse the sub flow 
		  and merge the sub flow and flow
INPUT : _pFlow , a variable stand for the flow which to be searched 
Flow:
START
	A-processor	
		SubFlow
			B-processor
				END
SubFlow:
SubFlow
 C
   D
     E

result:
START--->A-Processor --->C---->D--->E--->B-Processor --->END
/************************************************************************/
void CDirectionUtil::ExpandAllProcessInFlow( CSinglePaxTypeFlow* _pFlow )
{
	InputTerminal* pTerm = _pFlow->GetInputTerm();
	ASSERT( pTerm );
	std::vector<ProcessorID> vProcessID;
	int iPairCount = _pFlow->GetFlowPairCount();
	for( int i=iPairCount-1; i>=0; --i )
	{
		CString sStr = _pFlow->GetFlowPairAt( i )->GetProcID().GetIDString();
		//CSubFlow* pProcess = pTerm->m_pSubFlow->GetProcessUnit( sStr );
		if( pTerm->m_pSubFlowList->IfProcessUnitExist( sStr ) )
		{
			vProcessID.push_back( _pFlow->GetFlowPairAt( i )->GetProcID() );
		}
	}

	int iProcessCount = vProcessID.size();
	for( i=0; i<iProcessCount; ++i )
	{
		CSubFlow* pProcess = pTerm->m_pSubFlowList->GetProcessUnit( vProcessID[i].GetIDString() );
		ASSERT( pProcess );
		CSinglePaxTypeFlow* pFlowInProcess = pProcess->GetInternalFlow();

		std::vector<ProcessorID> vSourceID;
		_pFlow->GetSourceProc( vProcessID[i] ,vSourceID );
		
		std::vector<ProcessorID> vRootInProcess;
		pFlowInProcess->GetRootProc( vRootInProcess );

		std::vector<ProcessorID> vLeafInProcess;
		pFlowInProcess->GetLeafNode( vLeafInProcess );

		int iSourceCount = vSourceID.size();
		int iRootCount = vRootInProcess.size();
		int iLeafCount = vLeafInProcess.size();
		
		for( int j=0; j<iSourceCount; ++j )
		{
			CProcessorDestinationList* pPair = _pFlow->GetFlowPairAt( vSourceID[ j ] );
			ASSERT( pPair );
			//// TRACE("\n source id=%s\n", pPair->GetProcID().GetIDString() );
			for( int k=0; k<iRootCount; ++k )
			{
				CFlowDestination tempDest;
				tempDest.SetProcID( vRootInProcess[ k] );
				//// TRACE("\n id=%s\n", tempDest.GetProcID().GetIDString() );
				pPair->AddDestProc( tempDest, false );
			}

			CProcessorDestinationList* pProcessPair = _pFlow->GetFlowPairAt( vProcessID[ i ] );
			//// TRACE("\n id=%s\n", pProcessPair->GetProcID().GetIDString() );
			int iPairDest = pProcessPair->GetDestCount();
			for( k=0; k<iLeafCount; ++k )
			{
				//pProcessPair = _pFlow->GetFlowPairAt( vProcessID[ i ] );// can not get pProcessPair due to the STL's iteratior's implementation
				for( int s=0; s<iPairDest; ++s )
				{
					pProcessPair = _pFlow->GetFlowPairAt( vProcessID[ i ] );// must get pProcessPair here
					//// TRACE("\n%s\n", vLeafInProcess[ k ].GetIDString() );
					//// TRACE("\n%s\n", pProcessPair->GetDestProcAt( s ).GetProcID().GetIDString() );
					CFlowDestination tempDest;
					tempDest.SetProcID( pProcessPair->GetDestProcAt( s ).GetProcID() );		
					
					//_pFlow->AddDestProc( vLeafInProcess[ k ], pPair->GetDestProcAt( s ), false );
					_pFlow->AddDestProc( vLeafInProcess[ k ], tempDest, false );

					//_pFlow->SetAllPairVisitFlag(false);
					//if(_pFlow->IfHaveCircleFromProc(vLeafInProcess[ k ]))
					//{
					//	int iPairCount = _pFlow->GetFlowPairCount();
					//	for( int i=iPairCount-1; i>=0; --i )
					//	{
					//		CString sStr = _pFlow->GetFlowPairAt( i )->GetProcID().GetIDString();
					//		// TRACE(sStr+"\r\n");
					//	}
					//	CString strText;
					//	strText.Format("There is a circle!\r\n   Sub flow: %s\r\n   The processor: %s",
					//		pProcess->GetProcessUnitName(), vLeafInProcess[ k ].GetIDString()); 
					//	MessageBox(NULL, strText, "Error", MB_OK|MB_ICONWARNING );
					//	return ;			
					//}
				}			
			}

		//_pFlow->PrintAllStructure();
		}
		//_pFlow->PrintAllStructure();
		//pFlowInProcess->PrintAllStructure();
		_pFlow->MergeTwoFlow( *pFlowInProcess );
//		_pFlow->PrintAllStructure();
		_pFlow->DeleteProcNode( vProcessID[ i ] );
//		_pFlow->PrintAllStructure();

		
	}	
}
/************************************************************************
FUNCTION : analyze sub flow and find all the possible path
INPUT :	_pFlow,the sub flow need to be analyzed,
		_allPossibleFlow, a vector that store all the possible path                                                                     
/************************************************************************/

void CDirectionUtil::BuildAllPossiblePath(  CSinglePaxTypeFlow* _pFlow , std::vector<CSinglePaxTypeFlow*>& _allPossibleFlow )
{
	_pFlow->BuildAllPossiblePath( _allPossibleFlow, this );
#ifdef DEBUG
// 	int iSize = _allPossibleFlow.size();
// 	for( int i=0; i<iSize ; ++i )
// 	{
//		// TRACE("\n*******\n");
//		_allPossibleFlow[ i ]->PrintAllStructure();
//		// TRACE("\n*******\n");
// 	}
#endif
}
/************************************************************************
FUNCTION : calculate a path processor in the flow ,and make sure all the processor in the path is a Gate or not,
           if  it is a Gate , set the direction information of all the processors in the path
INPUT :_Flow ,the flow that need to analyzed
       _paxType, the constraint by which specify a pax that the processor can server
                                                                      
/************************************************************************/
void CDirectionUtil::BuildDirctionFromEverySinglePath( CSinglePaxTypeFlow* _pFlow , const CMobileElemConstraint& _paxType )
{
	InputTerminal* pTerm =_pFlow->GetInputTerm();
	ASSERT(	 pTerm );

	const ProcessorID* pToGate = pTerm->procList->getProcessor( TO_GATE_PROCESS_INDEX )->getID();
	const ProcessorID* pFromGate = pTerm->procList->getProcessor( FROM_GATE_PROCESS_INDEX )->getID();//
	const ProcessorID* pBaggageReclaim = pTerm->procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX )->getID();//
	
	std::vector<ProcessorID>vPathProc;
	std::vector<ProcessorID>vRoot;
	_pFlow->GetRootProc( vRoot );
	_pFlow->SetAllPairVisitFlag( false );
	std::vector<ProcessorID> vGateProc;
	std::vector<ProcessorID> vReclaimProc;
	TravelPath( _pFlow, vRoot[0], vPathProc );

	int iPathSize = vPathProc.size();
	for( int i=0; i<iPathSize; ++i )
	{
		GroupIndex group = pTerm->procList->getGroupIndex ( vPathProc[i] );
		if(group.start < 0)//subflow
		{
			CSubFlow* pSubFlow = pTerm->m_pSubFlowList->GetProcessUnit(vPathProc[i].GetIDString());
			if(pSubFlow)
			{
				if(pSubFlow->IfHasGateProcessor())
					vGateProc.push_back(vPathProc[i]);	

				if (pSubFlow->IfHasReclaimProcessor())
					vReclaimProc.push_back(vPathProc[i]);
			}
		}
		else
		{
			ASSERT( group.start>=0 );
			if( group.start == TO_GATE_PROCESS_INDEX )
			{
				vGateProc.push_back( *pToGate);
			}
			else if(  group.start == FROM_GATE_PROCESS_INDEX )
			{
				vGateProc.push_back( *pFromGate);
			}
			else if( pTerm->procList->getProcessor( group.start )->getProcessorType() == GateProc )
			{
				vGateProc.push_back( vPathProc[i] );
			}
			else if (group.start == BAGGAGE_DEVICE_PROCEOR_INDEX)
			{
				vReclaimProc.push_back(*pBaggageReclaim);
			}
			else if ( pTerm->procList->getProcessor( group.start )->getProcessorType() == BaggageProc)
			{
				vReclaimProc.push_back(vPathProc[i]);
			}
		}		
	}

	
	int iGateCount = vGateProc.size();
	std::vector<ProcessorID>::iterator iterStart = vPathProc.begin();

	for( int j=0; j<iGateCount; ++j )
	{			
		std::vector<ProcessorID>::iterator iterFind = std::find( iterStart, vPathProc.end(), vGateProc[ j] );
		ASSERT( iterFind != vPathProc.end() );

		std::vector<ProcessorID> vAllGateProc;
		CSubFlow* pSubFlow = pTerm->m_pSubFlowList->GetProcessUnit(vGateProc[j].GetIDString());
		if(pSubFlow)//subflow
		{
			pSubFlow->GetGateProcessor(vAllGateProc);
		}
		else
		{
			vAllGateProc.push_back(vGateProc[j]);
		}
		
		{

			#ifdef TRACE_SINGLEFLOW
			
				ofsstream echoFile (_T("c:\\singleflow.tmp"), stdios::app);	
				std::vector<ProcessorID>::iterator iterGate = vAllGateProc.begin();
				std::vector<ProcessorID>::iterator iterGateEnd = vAllGateProc.end();
				for (;iterGate != iterGateEnd; ++iterGate)
				{
					echoFile<<"Gate Proc: "<<iterGate->GetIDString()<< "\r\n";
				}
				std::vector<ProcessorID>::iterator iterPath = vPathProc.begin();
				std::vector<ProcessorID>::iterator iterPathEnd = vPathProc.end();
				for (;iterPath != iterPathEnd; ++iterPath)
				{
					echoFile<<"Proc: "<<iterPath->GetIDString()<< "\r\n";
					//TRACE("Proc: %s\n",iterPath->GetIDString());
				}
				echoFile<< "\r\n";
				echoFile.close();
			#endif

		} 

		bool bIgnorNextStation = false;
		for( ; iterStart!=iterFind; ++iterStart )
		{
			CSubFlow* pSubFlow = pTerm->m_pSubFlowList->GetProcessUnit(iterStart->GetIDString());
			if(pSubFlow)
			{
				for(int mm = 0; mm < (int)vAllGateProc.size(); mm++)
				{
					//CMobileElemConstraint type(pTerm);
					//pSubFlow->AddCanLeadGate(_paxType, vAllGateProc[mm],pTerm->procList);
				}
			}
			else
			{
				GroupIndex group = pTerm->procList->getGroupIndex ( *iterStart );
				ASSERT( group.start>=0 );
				if( pTerm->procList->getProcessor( group.start )->getProcessorType() == IntegratedStationProc )
				{
					if( bIgnorNextStation )
					{
						bIgnorNextStation = false;
						continue;
					}
					else
						bIgnorNextStation = true;
				}
				else
				{
					bIgnorNextStation = false;
				}
				CString strTemProcID = (*iterStart).GetIDString();
				for( int k=group.start; k<=group.end; ++k )
				{	
//					if (pTerm->procList->getProcessor( k )->getID()->GetIDString().CompareNoCase("APM-3-TO3A-1") == 0)
//						{
//							ofsstream echoFile ("c:\\1112121.log", stdios::app);
////							CString strTempPaxType;
////							_paxType.screenFltPrint(strTempPaxType);
////							echoFile<<strTempPaxType<<"\n";
//							std::vector<ProcessorID>::const_iterator tempIter = vAllGateProc.begin();
//							std::vector<ProcessorID>::const_iterator tempIterEnd = vAllGateProc.end();
//							for (;tempIter != tempIterEnd; ++tempIter)
//							{
//								if ((*tempIter).GetIDString().CompareNoCase("DEPGATE-3B-SW") == 0)
//								{
//									int ii=0;
//								}
//								
//								echoFile<<(*tempIter).GetIDString().GetBuffer(0)<<"      ";
//							}
//							
//							echoFile<<"\n\n";
//							echoFile.close();
//						}
					for(int mm = 0; mm < (int)vAllGateProc.size(); mm++)
					{

						pTerm->procList->getProcessor( k )->GetDirection().Insert( _paxType, vAllGateProc[mm] );
					}
				}	
			}		
		}
		//iterStart = ++iterFind;
		iterStart = vPathProc.begin();
	}	

	//reclaim
	int iReclaimCount = vReclaimProc.size();
	std::vector<ProcessorID>::iterator iterReclaimStart = vPathProc.begin();

	for(j=0; j<iReclaimCount; ++j )
	{			
		std::vector<ProcessorID>::iterator iterFind = std::find( iterReclaimStart, vPathProc.end(), vReclaimProc[ j] );
		ASSERT( iterFind != vPathProc.end() );

		std::vector<ProcessorID> vAllReclaimProc;
		CSubFlow* pSubFlow = pTerm->m_pSubFlowList->GetProcessUnit(vReclaimProc[j].GetIDString());
		if(pSubFlow)//subflow
		{
			pSubFlow->GetReclaimProcessor(vAllReclaimProc);
		}
		else
		{
			vAllReclaimProc.push_back(vReclaimProc[j]);
		}

		bool bIgnorNextStation = false;
		for( ; iterReclaimStart!=iterFind; ++iterReclaimStart )
		{
			CSubFlow* pSubFlow = pTerm->m_pSubFlowList->GetProcessUnit(iterReclaimStart->GetIDString());
			if(!pSubFlow)
			{
				GroupIndex group = pTerm->procList->getGroupIndex ( *iterReclaimStart );
				ASSERT( group.start>=0 );
				if( pTerm->procList->getProcessor( group.start )->getProcessorType() == IntegratedStationProc )
				{
					if( bIgnorNextStation )
					{
						bIgnorNextStation = false;
						continue;
					}
					else
						bIgnorNextStation = true;
				}
				else
				{
					bIgnorNextStation = false;
				}
				CString strTemProcID = (*iterReclaimStart).GetIDString();
				for( int k=group.start; k<=group.end; ++k )
				{	
					for(int mm = 0; mm < (int)vAllReclaimProc.size(); mm++)
					{
						pTerm->procList->getProcessor( k )->GetReclaimDrection().Insert( _paxType, vAllReclaimProc[mm] );
					}
				}	
			}		
		}
		iterReclaimStart = vPathProc.begin();
	}	
}
/************************************************************************
FUNCTION :Find a path start from a processor
INPUT: _rootID,start processor
		_pFlow,the flow that to be analysed
		_result,stored all the processor in the path,as a return
/************************************************************************/
void CDirectionUtil::TravelPath( CSinglePaxTypeFlow* _pFlow ,const ProcessorID& _rootID , std::vector<ProcessorID>& _result )
{
	CProcessorDestinationList* pPair = _pFlow->GetFlowPairAt( _rootID );
	ASSERT( pPair );
	_result.push_back( _rootID );
	//// TRACE("  %s  ", _rootID.GetIDString() );
	int iDestCount = pPair->GetDestCount();
	for( int i=0; i<iDestCount; ++i )
	{
		if( !pPair->GetDestProcAt( i ).GetVisitFlag() )
		{
			(const_cast<CFlowDestination &>(pPair->GetDestProcAt( i ))).SetVisitFlag( true );
			//pPair->GetDestProcAt( i ).SetVisitFlag( true );
			TravelPath( _pFlow,pPair->GetDestProcAt( i ).GetProcID(), _result );
		}
	}
}