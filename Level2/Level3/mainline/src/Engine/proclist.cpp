#include "stdafx.h"
#include <string.h>
#include "common\elaptime.h" 
#include "common\exeption.h"
#include "common\point.h"
#include "common\termfile.h"
#include "inputs\prochrch.h"
#include "results\proclog.h"
#include "results\procntry.h"
#include "results\out_term.h"
#include "engine\proclist.h"
#include "engine\process.h"
#include "engine\person.h"
#include "engine\hold.h"
#include "engine\lineproc.h"
#include "engine\baggage.h"
#include "engine\dep_srce.h"
#include "engine\dep_sink.h"
#include "engine\gate.h"
#include "engine\barrier.h"
#include "engine\floorchg.h"
#include "inputs\procdata.h"
#include "inputs\srvctime.h"
#include "inputs\paxflow.h"
#include "inputs\assigndb.h"
#include "inputs\bagdata.h"
#include "engine\terminal.h" 
#include "engine\monitor.h"
#include "engine\procq.h"
#include "engine\conveyor.h"
#include "engine\Stair.h"
#include "engine\Escalator.h"

#include "engine\Arp.h"
#include "engine\RunwayProc.h"
#include "engine\TaxiwayProc.h"
#include "engine\StandProc.h"
#include "engine\NodeProc.h"
#include "engine\DeiceBayProc.h"
#include "engine\FixProc.h"

#include "IntegratedStation.h"
#include "MovingSideWalk.h"
#include "ElevatorProc.h"
#include "ElevatorMoveEvent.h"

#include "inputs\AllPaxTypeFlow.h"
#include "inputs\PaxFlowConvertor.h"
#include "inputs\MobileElemConstraint.h"
#include "results\paxdestdiagnoseinfo.h"
#include "inputs\DirectionUtil.h"
#include "engine\anserveproclist.h"
#include "engine\sorter.h"
#include "reports\SimLevelOfService.h"
#include "inputs\SimParameter.h"
#include "engine\Tracepredef.h"
#include "engine\ApronProc.h"
#include "../Engine/ContourProc.h"
#include "../Engine/BillboardProc.h"
#include "../Engine/StretchProc.h"
#include "../Engine/IntersectionProc.h"
#include "../Engine/RoundAboutProc.h"
#include "../Engine/TurnOffProc.h"
#include "../Engine/LaneAdapterProc.h"
#include "../Engine/OverPassProc.h"
#include "../Engine/TrafficlightProc.h"
#include "../Engine/TollGateProc.h"
#include "../Engine/StopSignProc.h"
#include "../Engine/YieldSignProc.h"
#include "../Engine/LineParkingProc.h"
#include "../Engine/NoseInParkingProc.h"
#include "../Engine/HoldProc.h"
#include "../Engine/SectorProc.h"
#include "../Common/ARCTracker.h"
#include "../Engine/BridgeConnector.h"
#include "Engine/RetailProcessor.h"

#include "inputs\ProcessorFamily.h"
#include "inputs\ElevProcessorFamily.h"
#include "..\Main\DlgGateTypedefine.h"
#include "ProcessorDistributionTransformer.h"

#include "ARCportEngine.h"
#include "LandsidePaxTypeLinkageInSimManager.h"
#include "LandsideSimulation.h"
#include "LandsideBaseBehavior.h"
#include "LandsideVehicleInSim.h"
#include "LandsideParkingLotInSim.h"
#include "LandsideCellPhoneProcess.h"



// which processor's queue will be traced
const int constiIntervalMeter = 30;

ProcessorList::ProcessorList( StringDictionary* _pStrDict ) : DataSet (ProcessorLayoutFile,float(2.7))
{
	m_procCount = 0;
	startProcessor = endProcessor = NULL;
	fromGateProcessor = toGateProcessor = baggageDeviceProcessor = NULL;
	m_bDoOffset = false;
	m_pDynamicCreatedProcsByTLOS = NULL;
	m_pDynamicCreatedProcsPrototypes = NULL;
	m_bCanDynamicCreateProc = false;


	m_StartID.SetStrDict( _pStrDict );
	m_EndID.SetStrDict(_pStrDict);
	m_FromGateID.SetStrDict(_pStrDict);
	m_ToGateID.SetStrDict(_pStrDict);
	m_BaggageDeviceID.SetStrDict(_pStrDict);
	m_StartID.setID( "START" );
	m_EndID.setID( "END" );
	m_FromGateID.setID( "FROM-GATE" );
	m_ToGateID.setID( "TO-GATE" );
	m_BaggageDeviceID.setID( "BAGGAGE-DEVICE" );

}

ProcessorList::ProcessorList( StringDictionary* _pStrDict, InputFiles fileType ) : DataSet (fileType)
{
	m_procCount = 0;
	startProcessor = endProcessor = NULL;
	fromGateProcessor = toGateProcessor = baggageDeviceProcessor = NULL;
	m_bDoOffset = false;
	m_pDynamicCreatedProcsByTLOS = NULL;
	m_pDynamicCreatedProcsPrototypes = NULL;
	m_bCanDynamicCreateProc = false;


	m_StartID.SetStrDict( _pStrDict );
	m_EndID.SetStrDict(_pStrDict);
	m_FromGateID.SetStrDict(_pStrDict);
	m_ToGateID.SetStrDict(_pStrDict);
	m_BaggageDeviceID.SetStrDict(_pStrDict);
	m_StartID.setID( "START" );
	m_EndID.setID( "END" );
	m_FromGateID.setID( "FROM-GATE" );
	m_ToGateID.setID( "TO-GATE" );
	m_BaggageDeviceID.setID( "BAGGAGE-DEVICE" );

}

ProcessorList::~ProcessorList()
{
	clear();
	if (startProcessor)
		delete startProcessor;

	if (endProcessor)
		delete endProcessor;

	if( fromGateProcessor )
		delete fromGateProcessor;

	if( toGateProcessor )
		delete toGateProcessor;
	if( baggageDeviceProcessor )
		delete baggageDeviceProcessor;

	if( m_pDynamicCreatedProcsByTLOS )
	{
		delete m_pDynamicCreatedProcsByTLOS;
		m_pDynamicCreatedProcsByTLOS = NULL;
	}

	if( m_pDynamicCreatedProcsPrototypes )
	{
		delete m_pDynamicCreatedProcsPrototypes;
		m_pDynamicCreatedProcsPrototypes = NULL;
	}	
}

void ProcessorList::addProcessor (Processor *proc, bool _bAutoRebuildGroupIdxTree /*= true */)
{
	m_procCount++;
	procList.addItem (proc);
	setIndexes();
	if( _bAutoRebuildGroupIdxTree )
	{
		BuildProcGroupIdxTree();
	}	
	proc->UpdateMinMax();
}
void ProcessorList::addDynamicCreatedPrototypesProcessor(Processor *proc, bool _bAutoRebuildGroupIdxTree /*= true */)
{
	m_procCount++;
	procList.addItem (proc);
//	setIndexes();
	if( _bAutoRebuildGroupIdxTree )
	{
		BuildProcGroupIdxTree();
	}	
}
//If index is the START_PROCESSOR_INDEX, returns predefined processor "start";
//if index is the END_PROCESSOR_INDEX, returns predefined processor "end".
Processor *ProcessorList::getProcessor (int ndx) const
{
	if (ndx == START_PROCESSOR_INDEX)
		return startProcessor;
	else if (ndx == END_PROCESSOR_INDEX)
		return endProcessor;
	else if (ndx == FROM_GATE_PROCESS_INDEX)
		return fromGateProcessor;
	else if (ndx == TO_GATE_PROCESS_INDEX )
		return toGateProcessor;
	else if( ndx == BAGGAGE_DEVICE_PROCEOR_INDEX )
		return baggageDeviceProcessor;

	else if (ndx < 0 || ndx >= m_procCount)
		return NULL;

	return procList.getItem (ndx);
}


//Returns NULL if id is not valid.
Processor *ProcessorList::getProcessor (const char *procName) const
{
	if (!_stricmp (procName, "START"))
		return startProcessor;
	else if (!_stricmp (procName, "END"))
		return endProcessor;
	else if	(!_stricmp (procName,"FROM-GATE"))
		return fromGateProcessor;
	else if (!_stricmp (procName, "TO-GATE"))
		return toGateProcessor;
	else if( !_stricmp ( procName,"BAGGAGE-DEVICE"))
		return baggageDeviceProcessor;

	ProcessorID id;
	if (!id.isValidID (procName))
		return NULL;

	assert( m_pInTerm );
	id.SetStrDict(m_pInTerm->inStrDict);

	id.setID (procName);
	GroupIndex group = getGroupIndexOriginal (id);
	if (group.start != group.end)
		return NULL;

	if (group.start != -1)
		return procList.getItem (group.start);

	return NULL;
}
Processor* ProcessorList::getProcessor(const ProcessorID& procID)const
{
	assert( m_pInTerm );
	
	GroupIndex group = getGroupIndexOriginal (procID);
	if (group.start != group.end)
		return NULL;

	if (group.start != -1)
		return procList.getItem (group.start);

	return NULL;
}

int ProcessorList::getProcessorsOfType (int procType) const
{
	int count = 0;
	Processor *aProc;

	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem (i);
		if (aProc->getProcessorType() == procType)
			count++;
	}
	return count;
}


int ProcessorList::findProcessor (const ProcessorID& id) const
{
	Processor tempProc;
	tempProc.init (id);
	return procList.findItem (&tempProc);
}


void ProcessorList::clear (void)
{
	//procList.clear();
	procList.Flush( TShouldDelete::Delete );
	m_procCount = 0;
}


void ProcessorList::removeProcessor (const Processor *proc, bool _bAutoRebuildGroupIdxTree /*= true*/)
{
	int index = procList.findItem (proc);
	procList.removeItem (index);
	m_procCount--;
	setIndexes();
	if( _bAutoRebuildGroupIdxTree )
	{
		BuildProcGroupIdxTree();
	}
}

//Returns the removed processor.
Processor *ProcessorList::removeProcessor (int index, bool _bAutoRebuildGroupIdxTree /*= true*/)
{
	if (index < 0 || index >= m_procCount)
		return NULL;

	Processor *aProc = procList.getItem (index);
	removeProcessor (aProc,_bAutoRebuildGroupIdxTree);
	return aProc;
}

BOOL ProcessorList::RenameProcessor(Processor* old,const char* newName)
{
	ProcessorID newProcID;
	newProcID.SetStrDict(m_pInTerm->inStrDict);
	if(!newProcID.isValidID(newName))
		return false;
	newProcID.setID(newName);



	//Processor pNewProcessor=*old;
	//if(!pNewProcessor.setID(newName))
	//	return false;
	char tempName[100];
	strcpy(tempName,newName);
	if(old->getID()->idLength()>newProcID.idLength())
	{
		for(int i=newProcID.idLength();i<old->getID()->idLength();i++)
		{	
			char str[50];
			old->getID()->getNameAtLevel(str,i);
			strcat(tempName,"-");
			strcat(tempName,str);
		}
	}
	((ProcessorID*)old->getID())->setID(tempName);
	BuildProcGroupIdxTree();
	//removeProcessor(old);
	//addProcessor(pNewProcessor);

	return true;
}

//The group are removed from list. 
void ProcessorList::deleteProcessor (const ProcessorID& id, bool _bAutoRebuildGroupIdxTree /*= true*/)
{
	GroupIndex deleteItem = getGroupIndexOriginal (id);
	if (deleteItem.start == deleteItem.end)
	{
		procList.deleteItem (deleteItem.start);
		if( _bAutoRebuildGroupIdxTree )
		{
			BuildProcGroupIdxTree();
		}
	}		
}

void ProcessorList::setIndexes (void)
{
	for (int i = 0; i < m_procCount; i++)
		(procList.getItem (i))->setIndex (i);
}


void ProcessorList::getProcessorsOfType (int procType, ProcessorArray& _list)
{
	Processor *aProc;
	m_procCount = procList.getCount();

	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem (i);
		if (aProc->getProcessorType() == procType)
			_list.addItem (aProc);
	}
}


void ProcessorList::addStartAndEndProcessors (void)
{

	assert( m_pInTerm );
	startProcessor = new Processor;
	startProcessor->SetTerminal((Terminal*)m_pInTerm);
	startProcessor->init ( m_StartID );
	startProcessor->setIndex (START_PROCESSOR_INDEX);
	endProcessor = new Processor;
	endProcessor->init ( m_EndID );
	endProcessor->SetTerminal((Terminal*)m_pInTerm);
	endProcessor->setIndex (END_PROCESSOR_INDEX);

	Point pnt;
	startProcessor->initServiceLocation (1, &pnt);
	endProcessor->initServiceLocation (1, &pnt);

	GroupIndex gates;
	gates.start = 0;
	gates.end = procList.getCount()-1;

	startProcessor->initMiscData (false, TRUE, 1, &gates);
	endProcessor->initMiscData (false, TRUE, 1, &gates);
}

void ProcessorList::addFromAndToGateProcessors(void)
{
	assert( m_pInTerm );
	fromGateProcessor = new Processor;
	fromGateProcessor->init ( m_FromGateID );
	fromGateProcessor->SetTerminal((Terminal*)m_pInTerm);
	fromGateProcessor->setIndex (FROM_GATE_PROCESS_INDEX);
	toGateProcessor = new Processor;
	toGateProcessor->init ( m_ToGateID );
	toGateProcessor->SetTerminal((Terminal*)m_pInTerm);
	toGateProcessor->setIndex (TO_GATE_PROCESS_INDEX);

	baggageDeviceProcessor = new Processor;
	baggageDeviceProcessor->init( m_BaggageDeviceID );
	baggageDeviceProcessor->SetTerminal((Terminal*)m_pInTerm);
	baggageDeviceProcessor->setIndex( BAGGAGE_DEVICE_PROCEOR_INDEX );

	Point pnt;
	fromGateProcessor->initServiceLocation (1, &pnt);
	toGateProcessor->initServiceLocation (1, &pnt);
	//baggageDeviceProcessor->INI

	GroupIndex gates;
	gates.start = 0;
	gates.end = procList.getCount()-1;

	fromGateProcessor->initMiscData (false, TRUE, 1, &gates);
	toGateProcessor->initMiscData (false, TRUE, 1, &gates);

	baggageDeviceProcessor->initMiscData (false, TRUE, 1, &gates);
}

void ProcessorList::setServiceTimes (const ServiceTimeDatabase& serviceTimeDB)
{
	Processor* proc;
	for (int i = 0; i < m_procCount; i++)
	{
		proc = procList.getItem (i);
		if (proc->getProcessorType() != BarrierProc)
			proc->setServiceTimeLink (serviceTimeDB);
	}
}

void ProcessorList::setPassengerFlows (const PassengerFlowDatabase& paxFlowDB, 
									   const PassengerFlowDatabase& _gateDetailFlowDB, 
									   const PassengerFlowDatabase& _stationPaxFlowDB,
									   const CSubFlowList& pSubFlowList)
{
	CPassengerFlowDatabaseTransformer expandPaxFlow;
	expandPaxFlow.ExpandPaxFlow(&paxFlowDB,pSubFlowList);
	Processor* proc;
	for (int i = 0; i < m_procCount; i++)
	{
		proc = procList.getItem (i);
		if (proc->getProcessorType() != BarrierProc)
			proc->setTransitionLink (expandPaxFlow, _gateDetailFlowDB,  _stationPaxFlowDB, pSubFlowList);
			//proc->setTransitionLink (paxFlowDB, _gateDetailFlowDB,  _stationPaxFlowDB, pSubFlowList);
	}
	startProcessor->setTransitionLink (expandPaxFlow, _gateDetailFlowDB, _stationPaxFlowDB, pSubFlowList);
	toGateProcessor->setTransitionLink (expandPaxFlow, _gateDetailFlowDB, _stationPaxFlowDB, pSubFlowList);
	baggageDeviceProcessor->setTransitionLink (expandPaxFlow, _gateDetailFlowDB, _stationPaxFlowDB, pSubFlowList);

	fromGateProcessor->setTransitionLink (expandPaxFlow, _gateDetailFlowDB, _stationPaxFlowDB, pSubFlowList);
}

void ProcessorList::setProcAssignments (const ProcAssignDatabase& assignDB)
{
	Processor* proc;
	for (int i = 0; i < m_procCount; i++)
	{
		proc = procList.getItem(i);
		if (proc->getProcessorType() != BarrierProc)
			proc->setAssignmentLink (assignDB);
	}
}

//It reads miscellaneous data from CSV file
void ProcessorList::setMiscData (const MiscDatabase& miscData)
{
	Processor *aProc;
	MiscProcessorData *miscProcData;

	//////////////////////////////////////////////////////////////////////////
	// clean likange data
	for (int  j = 0; j < m_procCount; j++)
	{
		aProc = procList.getItem(j);
		aProc->SetTerminal( (Terminal*)m_pInTerm );	
		aProc->initMiscData (false, TRUE, 0, NULL);
		aProc->initSpecificMisc( NULL );
		aProc->clearLinkedDestProcs();//clear linked processor.
	}

	std::vector<ProcessorDataElement* > vProximityData;
	std::vector<ProcessorDataElement* >::iterator iterElem;

	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem(i);
		aProc->SetTerminal( (Terminal*)m_pInTerm );
		//TRACE( "ID=%s\n", aProc->getID()->GetIDString() );

		int nProcType = aProc->getProcessorType();
		if( nProcType == BarrierProc )
			continue;

		miscProcData = miscData.getDatabase (aProc->getProcessorType());
		miscProcData->getEntryPoint (*(aProc->getID()),vProximityData);
		if (vProximityData.size()>0)
		{
			//use proximity to initMisc(), and other append linked dest.
			iterElem=vProximityData.begin();
			aProc->initMisc (((MiscDataElement*)*iterElem)->getData());
			iterElem++;
			for(;iterElem!=vProximityData.end();iterElem++)
			{
				aProc->appendLinkedDestProcs( ((MiscDataElement*)*iterElem)->getData()->getLinkedDestList());
			}
		}
		vProximityData.clear();
	}
}

//because the conveyor architecture is designed in "proxy patten ", so we must init a real performer before
// engine use conveyor.
void ProcessorList::InitEveryConveyor (const MiscDatabase& miscData)
{
	PLACE_METHOD_TRACK_STRING();
	Processor *aProc;

	MiscDataElement *element;
	MiscConveyorData *elementData;

	MiscProcessorData *miscProcData = miscData.getDatabase (ConveyorProc);;

	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem(i);
		aProc->SetTerminal( (Terminal*)m_pInTerm );
		int nProcType = aProc->getProcessorType();
		if( nProcType == ConveyorProc  )
		{
			element = (MiscDataElement *)miscProcData->getEntryPoint (*(aProc->getID()));

			if( element )
			{
				elementData = (MiscConveyorData*)element->getData();			
				ASSERT( elementData->getType() == ConveyorProc );
				if (elementData)
				{
					((Conveyor*)aProc)->ConstructPerformer( elementData );
					((Conveyor*)aProc)->InitData( elementData );
				}
				else
				{
					((Conveyor*)aProc)->ConstructPerformer( NULL );
					((Conveyor*)aProc)->InitData( NULL );
				}
			}
			else
			{
				((Conveyor*)aProc)->ConstructPerformer( NULL );
				((Conveyor*)aProc)->InitData( NULL );
			}			
		}
	}
	for(int i=0;i< m_procCount; i++)
	{
		aProc = procList.getItem(i);
		if( aProc->getProcessorType()==ConveyorProc && ((Conveyor*)aProc)->GetSubConveyorType()== SORTER)
		{
			static_cast<Conveyor*>(aProc)->InitSorter();
		}
	}
}
#include "common\CodeTimeTest.h"

// out:
//	array, result dest processor
void ProcessorList::buildArray (const ProcessorID *groupID,
								ProcessorArray *pProcArray, const CMobileElemConstraint& type, int gateIndex, bool _bNeedCheckLeadGate, 
								int bagIndex,bool _bNeedCheckLeadReclaim, int openFlag, const Processor *curProc,
								ElapsedTime _currentTime, CARCportEngine* pEngine, bool& _bRosterReason  ,int _iDestFloor, CPaxDestDiagnoseInfo* _pInfo, Person* pPerson,BOOL bInSubFlow,std::vector<ALTObjectID> vAltLandsideSelectedProc) const
{

	//DWORD dStepTime1 = 0L;
	//DWORD dStepTime2 = 0L;
	//DWORD dStepTime3 = 0L;
	//DWORD dStepTime4 = 0L;
	//DWORD dStepTime5 = 0L;
	/////Test code
	//DWORD dStartTime = GetTickCount();

	ASSERT( pEngine && pEngine->getTerminal() );

	_pInfo->setDiagnoseTime( (long)_currentTime );
	_pInfo->setSrcProcessor( curProc->getID()->GetIDString() );

	CanServeProcList _canServeProcList;
	pProcArray->clear();

	int floorChange = curProc->getProcessorType() == FloorChangeProc;
	int startProc = curProc->getIndex() == START_PROCESSOR_INDEX;
	GroupIndex group = getGroupIndex (*groupID);

	if (group.start == TO_GATE_PROCESS_INDEX && group.start == group.end)
	{
		pProcArray->addItem (toGateProcessor);
		return;
	} 
	if (group.start == FROM_GATE_PROCESS_INDEX && group.start == group.end)
	{
		pProcArray->addItem (fromGateProcessor);
		return;
	} 

	if (group.start == BAGGAGE_DEVICE_PROCEOR_INDEX && group.start == group.end)
	{
		pProcArray->addItem (baggageDeviceProcessor);
		return;
	}  

	if (group.start == END_PROCESSOR_INDEX && group.start == group.end)
	{
		pProcArray->addItem (endProcessor);
		return;
	} 
	//dStepTime1 = GetTickCount();
	ProcessorArray aryLinkedDestPros;
	Processor *aProc;
	std::vector< Processor* > vVaildTLOSProcList;
	bool bLevel0Matched = false;
	for (int i = group.start; i <= group.end; i++)
	{
		
		//DWORD dtime1 = GetTickCount();
		//DWORD dtime2 = GetTickCount();
		//DWORD dtime3 = GetTickCount();
		//DWORD dtime4 = GetTickCount();
		//DWORD dtime5 = GetTickCount();
		//DWORD dtime6 = GetTickCount();
		//DWORD dtime7 = GetTickCount();
		//DWORD dtime8 = GetTickCount();
		//DWORD dtime9 = GetTickCount();
		//DWORD dtime10 = GetTickCount();

		aProc = getProcessor(i);
		CString strProcessor = aProc->getID()->GetIDString();

		CString _strGateProc = ( getProcessor( gateIndex ) ) ? getProcessor( gateIndex )->getID()->GetIDString() : "" ;
		CString _strReclaimProc = (getProcessor(bagIndex))?getProcessor(bagIndex)->getID()->GetIDString() : "";
		// processor status test
		if ( openFlag || aProc->isOpen)
		{
			//dtime2 = GetTickCount();
			CString strErrorInfo;
			if(!CanLeadToLandsideObject(aProc, pPerson, pEngine,_currentTime, strErrorInfo))
			{
				_pInfo->insertDiagnoseInfo(strProcessor,"can not lead to Landside Destination:" + strErrorInfo);
				continue;
			}
			if(aProc && aProc->getProcessorType()==BridgeConnectorProc)
			{
				BridgeConnector* bridge = (BridgeConnector*)aProc;
				if( !bridge->IsBridgeConnectToFlight( pPerson->GetCurFlightIndex() ) )
				{
					_pInfo->insertDiagnoseInfo(strProcessor,"Bridge is not connected to the flight" + strErrorInfo);
					continue;
				}
			}
			if (!CanLeadToTerminalProcessor(curProc, aProc,vAltLandsideSelectedProc, pEngine))
			{
				CString strErrorInfo;
				strErrorInfo = _T("can not lead to entry Terminal processor ");
				int nCount = static_cast<int>(vAltLandsideSelectedProc.size());
				for ( int nItem = 0; nItem < nCount; ++ nItem)
				{
					strErrorInfo += vAltLandsideSelectedProc[nItem].GetIDString();
					strErrorInfo += _T("; ");
				}
				strErrorInfo.TrimRight(_T(";"));

				_pInfo->insertDiagnoseInfo(strProcessor, strErrorInfo);
				continue;
			}

			if( !_bNeedCheckLeadGate || gateIndex == -1 
				||aProc->CanLeadTo( type, *getProcessor( gateIndex )->getID() )  
				|| *(aProc->getID()) == *(getProcessor( gateIndex )->getID())  )
			{
				if (!_bNeedCheckLeadReclaim || bagIndex == -1
					||aProc->CanLeadToReclaim(type,*getProcessor(bagIndex)->getID())
					||*(aProc->getID()) == (*getProcessor(bagIndex)->getID()))
				{
				
				//dtime3 = GetTickCount();
				// floor test
					if (aProc->getFloor() == curProc->getFloor() || startProc 
						|| aProc->getProcessorType() == Elevator || curProc->getProcessorType() == Elevator 
						|| aProc->getProcessorType() == ConveyorProc || curProc->getProcessorType() == ConveyorProc 
						|| aProc->getProcessorType() == BridgeConnectorProc || curProc->getProcessorType() == BridgeConnectorProc
						|| aProc->getProcessorType() == IntegratedStationProc
						|| (floorChange && aProc->getProcessorType() == FloorChangeProc)
						|| CheckStairProcessor( curProc, aProc )
						)
					{
						//dtime4 = GetTickCount();
						// baggage carosel test
						if (aProc->getProcessorType() != BaggageProc || i == bagIndex )
						{
							//dtime5 = GetTickCount();

							////test code by hans . log all the available dest proc information
							////code start
							//if (_pInfo->getDiagnosePaxID() ==224215 && 
							//	curProc &&
							//	curProc->getID()->GetIDString().CompareNoCase(_T("SECURITYCHECK-T1-SOUTH-REGULAR")) == 0)
							//{
							//	ofsstream echoFile ("d:\\SECURITYCHECK.log", stdios::app);
							//	echoFile<<"BUILD ARRAY"<<"\n";
							//	echoFile<<"Processor Name :"<<strProcessor<<"    "
							//		<<"Proc occupancy count:"<<aProc->getQueueLength()<<"   "
							//		<<"proc occupancy  pax count :"<<aProc->getTotalGroupCount()<<"   "
							//		<<"nFuturePersonCount : "<<nFuturePersonCount<<"   "
							//		<<"type.GetTypeIndex() > 1 :"<<  type.GetTypeIndex()<<"   "
							//		<<"\n";
							//	echoFile.close();
							//}
							////code end

							// check the processor if is vacant( can accept next person )
							if(aProc->isVacant(pPerson) || bInSubFlow)
							{
								//dtime6 = GetTickCount();
								// gate flow logic test
								if ( aProc->leadsToGate (gateIndex))
								{
									//dtime7 = GetTickCount();
									// aProc is floor change processor
									int nProcType = aProc->getProcessorType();
									if( _iDestFloor == -1 || 
										nProcType == Elevator ||
										nProcType == EscalatorProc ||
										nProcType == StairProc ||
                                        aProc->CheckIfOnTheSameFloor(_iDestFloor)  )
									{
										bool bEleCanGoto = true;
										if(curProc->getProcessorType() == Elevator && (nProcType !=Elevator && nProcType != EscalatorProc &&  nProcType != StairProc ) )
										{
											ElevatorProc *pEle = (ElevatorProc *)curProc;
											
											if( pEle->GetMinFloor() > aProc->getFloor()/SCALE_FACTOR  || pEle->GetMaxFloor() < aProc->getFloor()/SCALE_FACTOR)
												bEleCanGoto = false;
										}
										if(bEleCanGoto)
										{
											//dtime8 = GetTickCount();
											if (aProc->isPaxSkipVacant(pPerson,_currentTime))
											{
												if (aProc->CheckRecycleFrequency(pPerson))
												{
													//if one processor of the group exceed the frequency limitation
													//the whole branch is not eligible
													_pInfo->insertDiagnoseInfo(strProcessor,"Recycle frequency Problem");
													pProcArray->clear();
													return;

												}
													if( aProc->GetTLOSFlag() )
														vVaildTLOSProcList.push_back( aProc );
													_bRosterReason = true;
													// assignment test
													if (aProc->isOpened() && !aProc->closing() && aProc->canServe (type, &_canServeProcList ))
													{
														//dtime9 = GetTickCount();
														int nReturnCount = curProc->FindLinkedDestinationProcs(*aProc->getID(),aryLinkedDestPros, bLevel0Matched);
														//dtime10 = GetTickCount();
														//array->addItem (aProc);

														//test code 
														if(nReturnCount != -1 && aryLinkedDestPros.getCount() == 0)
															_pInfo->insertDiagnoseInfo( strProcessor,"The processor is not linkage processor defined in the Behavior." );

													}
													else
														_pInfo->insertDiagnoseInfo( strProcessor,"can not serve for this pax" );
												//}
												//else
												//	_pInfo->insertDiagnoseInfo(strProcessor,"Recycle frequency Problem");
											}
											else
												_pInfo->insertDiagnoseInfo(strProcessor,"Pax Skip Problem");
										}
										else
											_pInfo->insertDiagnoseInfo(strProcessor,"Elevator does not goto this floor");

									}
									else
										_pInfo->insertDiagnoseInfo( strProcessor,"DestFloor Problem!" );
								}
								else
									_pInfo->insertDiagnoseInfo( strProcessor, "can not lead to gate in behavior:" + _strGateProc );
							}
							else
								_pInfo->insertDiagnoseInfo(strProcessor, "the processor is not vacant");
						}
						else
							_pInfo->insertDiagnoseInfo(strProcessor, "AssignedBaggageProc Problem");
					}
					else
						_pInfo->insertDiagnoseInfo( strProcessor, "not in the same floor" );
				}
				else
					_pInfo->insertDiagnoseInfo(strProcessor,"can not lead to reclaim:" + _strReclaimProc);
			}
			else
				_pInfo->insertDiagnoseInfo( strProcessor, "can not lead to gate:"+ _strGateProc );
		}
		else if (aProc->getProcessorType() ==  DepSourceProc || aProc->getProcessorType() == DepSinkProc
			      || curProc->getProcessorType() == DepSourceProc || curProc->getProcessorType() == DepSinkProc)
			_pInfo->insertDiagnoseInfo(strProcessor,"Source and Sink are not linked at all");
		else
			_pInfo->insertDiagnoseInfo( strProcessor, "processor closed");
	}

	
	//dStepTime2 = GetTickCount();
	
	bool iFirstCreate = false;
	if( m_pDynamicCreatedProcsByTLOS )
	{
		iFirstCreate = true;
		group = m_pDynamicCreatedProcsByTLOS->getGroupIndex (*groupID);
		if( group.start >= 0 )
		{
			iFirstCreate = false;
			for (int i = group.start; i <= group.end; i++)
			{								
				aProc = m_pDynamicCreatedProcsByTLOS->getProcessor(i);
				CString strProcessor = aProc->getID()->GetIDString();
				//// TRACE ("\n%s\n", aProc->getID()->GetIDString() );
				if ( openFlag || aProc->isOpen)
				{
					if( !_bNeedCheckLeadGate || gateIndex == -1 
						||aProc->CanLeadTo( type, *getProcessor( gateIndex )->getID() )  
						|| *(aProc->getID()) == *(getProcessor( gateIndex )->getID())  )
					{
						if (!_bNeedCheckLeadReclaim || bagIndex == -1
							||aProc->CanLeadToReclaim(type,*getProcessor(bagIndex)->getID())
							||*(aProc->getID()) == *(getProcessor(bagIndex)->getID()))
						{
							// floor test
							if (aProc->getFloor() == curProc->getFloor() || startProc 
								|| aProc->getProcessorType() == Elevator || curProc->getProcessorType() == Elevator 
								|| aProc->getProcessorType() == ConveyorProc || curProc->getProcessorType() == ConveyorProc 
								|| aProc->getProcessorType() == BridgeConnectorProc || curProc->getProcessorType() == BridgeConnectorProc
								||(floorChange && aProc->getProcessorType() == FloorChangeProc)
								||  CheckStairProcessor( curProc, aProc )
								)
							{
								// baggage carosel test
								if (aProc->getProcessorType() != BaggageProc || i == bagIndex )
								{
									// ckeck the processor if is vacant( can accept next person )					
									if(type.GetTypeIndex() > 1 || aProc->isVacant(pPerson) )//o,pax,1,visitor,>2 nonPax, do't need check isVacant()
									{
										// gate flow logic test
										if ( aProc->leadsToGate (gateIndex))
										{
											if( _iDestFloor == -1 || aProc->getFloor() == _iDestFloor*SCALE_FACTOR )
											{
												if (aProc->isPaxSkipVacant(pPerson,_currentTime))
												{
													if (aProc->CheckRecycleFrequency(pPerson))
													{
														//if one processor of the group exceed the frequency limitation
														//the whole branch is not eligible
														_pInfo->insertDiagnoseInfo(strProcessor,"Recycle frequency Problem");
														pProcArray->clear();

														return;
													}
														if( aProc->GetTLOSFlag() )
															vVaildTLOSProcList.push_back( aProc );
														_bRosterReason = true;
														// assignment test
														if (aProc->isOpened() && !aProc->closing() && aProc->canServe (type, &_canServeProcList ))
														{
															pProcArray->addItem (aProc);
														}
														else
															_pInfo->insertDiagnoseInfo( strProcessor,"can not serve for this pax" );
													//}
													//else 
													//	_pInfo->insertDiagnoseInfo(strProcessor,"Recycle frequency Problem");
												}
												else
													_pInfo->insertDiagnoseInfo(strProcessor,"Pax Skip Problem");
											}					
										}
										else
										{
											CString _strGateProc = ( getProcessor( gateIndex ) ) ? getProcessor( gateIndex )->getID()->GetIDString() : "" ;
											_pInfo->insertDiagnoseInfo( strProcessor, "can not lead to gate:" + _strGateProc );
										}
									}
									else
										_pInfo->insertDiagnoseInfo(strProcessor, "the processor is not vacant");
								}
								else;
							}
							else
								_pInfo->insertDiagnoseInfo( strProcessor, "not in the same floor" );
						}
						else
						{
							CString _strReclaimProc = ( getProcessor( bagIndex ) ) ? getProcessor( bagIndex )->getID()->GetIDString() : "" ;
							_pInfo->insertDiagnoseInfo(strProcessor,"can not lead to reclaim:" + _strReclaimProc);
						}
					}
					else
					{
						CString _strGateProc = ( getProcessor( gateIndex ) ) ? getProcessor( gateIndex )->getID()->GetIDString() : "" ;
						_pInfo->insertDiagnoseInfo( strProcessor, "can not lead to gate:"+ _strGateProc );
					}
				}
				else if (aProc->getProcessorType() ==  DepSourceProc || aProc->getProcessorType() == DepSinkProc
					     || curProc->getProcessorType() == DepSourceProc || curProc->getProcessorType() == DepSinkProc)
					_pInfo->insertDiagnoseInfo(strProcessor,"Source and Sink  are not linked at all");
				else
					_pInfo->insertDiagnoseInfo( strProcessor, "processor closed");
			}
		}
	}

	//dStepTime3 = GetTickCount();
	
	if(aryLinkedDestPros.getCount()>0)
	{
		int nLinkageCount=aryLinkedDestPros.getCount();
		for(int nLink=0;nLink<nLinkageCount;nLink++)
		{
			pProcArray->addItem(aryLinkedDestPros.getItem(nLink));
		}
		aryLinkedDestPros.clear();
	}
	else
	{
		if(curProc->GetLinkedDestCount()==0 || bLevel0Matched==false)
		{
			_canServeProcList.getProcFromList( pProcArray );
		}
	}

	//dStepTime4 = GetTickCount();

	//////////////////////////////////////////////////////////////////////////
	// if can not find a processor can serve for the pax type
	// then ask monitor to open a processor for the pax type
	if ( pProcArray->getCount()== 0 &&  vVaildTLOSProcList.size()>0 ) 
	{
		const CSimLevelOfServiceItem* pServiceItem=NULL;
		CSimLevelOfService* pServicePara= (CSimLevelOfService*)pEngine->getTerminal()->m_pSimParam->GetSimPara();
		if(pServicePara==NULL || (pServiceItem=pServicePara->getTheNearestProc( *(*vVaildTLOSProcList.begin())->getID()))==NULL || pServiceItem->getTheNearestPaxType( type)==NULL)return;

		aProc = pEngine->getTerminal()->m_pTLOSMonitor->OpenProcessor( vVaildTLOSProcList, _currentTime, type );
		if( aProc )
		{
			pProcArray->addItem( aProc );
		}
		else if( openFlag) 
		{
			if(iFirstCreate)// if had no autocreate processor event before, and have opened processor, add to that. not to create processor.
			{
				for(UINT j=0;j<vVaildTLOSProcList.size();j++) 
				{
					if(((Processor *)vVaildTLOSProcList[j])->GetTLOSFlag() && ((Processor *)vVaildTLOSProcList[j])->GetCurrentServicePaxType().fits(type ))
						pProcArray->addItem(vVaildTLOSProcList[j]);
				}
			}
			else
			{

				Processor* pProcPrototype = vVaildTLOSProcList[ 0 ];
				Processor* pCreatedProc = DynamicCreateProcByTLOS( pProcPrototype->getID(), pEngine->getTerminal() );
				if( pCreatedProc )
				{			
					pEngine->getTerminal()->m_pTLOSMonitor->OpenProcessor( pCreatedProc, _currentTime, type );				
					pProcArray->addItem( pCreatedProc );
					pCreatedProc->setAvail();
					// to make sure that proc log has the pos for new created proc
					pCreatedProc->writeLog();
				}
			}
		}
	}
	//dStepTime5 = GetTickCount();


	////test code 

}



int ProcessorList::isUnique (const ProcessorID& id) const
{
	return (findProcessor (id) == INT_MAX);
}


void ProcessorList::getAllGroupNames (StringList& strDict, int type,BOOL bGateStandOnly) const
{
	GroupIndex group;
	group.start = 0;
	group.end = m_procCount-1;

	getGroupNames (strDict, group, 0, type,bGateStandOnly);
}


void ProcessorList::getAllFullNames (StringDictionary& p_dict, int p_type) const
{
	Processor *aProc;
	const ProcessorID *procID;
	char name[256];
	for (int i = 0; i < m_procCount; i++)
	{
		aProc = getProcessor(i);
		if ((p_type == -1 && aProc->getProcessorType() != BarrierProc)
			|| aProc->getProcessorType() == p_type)
		{
			procID = aProc->getID();
			procID->printID (name);
			p_dict.addString (name);
		}
	}
}

void ProcessorList::getGroupNames (StringList& strDict,
								   const GroupIndex& group, int level, int procType,BOOL bGateStandOnly) const
{
	Processor *aProc;
	const ProcessorID *procID;
	char name[256];

	for (int i = group.start; i <= group.end; i++)
	{
		aProc = getProcessor(i);
		if(procType==GateProc)
		{
			GateProcessor* pGP=(GateProcessor*)aProc;
			if(bGateStandOnly&&!pGP->getACStandGateFlag())
				continue;
		}
		// if ((procType == -1 )//&& aProc->getProcessorType() != BarrierProc)
		if(  procType==-1  || aProc->getProcessorType() == procType)
		{
			procID = aProc->getID();
			if (procID->getNameAtLevel (name, level))
				strDict.addString (name);
		}
	}
}


void ProcessorList::getMemberNames (const ProcessorID& pID, StringList& strDict, int type,BOOL bGateStandOnly) const
{
	GroupIndex group = getGroupIndexOriginal (pID);
	if (group.start != -1)
		getGroupNames (strDict, group, pID.idLength(), type,bGateStandOnly);
}


void ProcessorList::getProcessorID (const GroupIndex& group, char *str) const
{
	ProcessorID id;
	const ProcessorID *nextID;
	id = *(getProcessor(group.end)->getID());

	for (int i = group.start; i < group.end; i++)
	{
		nextID = getProcessor(i)->getID();

		// trim off any portion of the id that is not common to both
		id &= *nextID;
	}
	id.printID (str);
}


void ProcessorList::getNameAt (int index, char* name) const
{
	Processor* aProc = getProcessor (index);
	aProc->name.printID (name, ",");
}

static int prefixCompare (const Processor& aProc, void *id)
{
	return ((ProcessorID *)id)->idFits (*(aProc.getID()));
}


GroupIndex ProcessorList::getGroupIndex (const ProcessorID& pID) const
{
	GroupIndex groupIndex = {-1, -1};
	assert( m_pInTerm );

	// test for START processor
	if (pID == m_StartID )
	{
		groupIndex.start = groupIndex.end = START_PROCESSOR_INDEX;
		return groupIndex;
	}

	// test for END processor
	if (pID == m_EndID )
	{
		groupIndex.start = groupIndex.end = END_PROCESSOR_INDEX;
		return groupIndex;
	}

	if( pID == m_FromGateID )
	{
		groupIndex.start = groupIndex.end = FROM_GATE_PROCESS_INDEX;
		return groupIndex;
	}

	if( pID == m_ToGateID )
	{
		groupIndex.start = groupIndex.end = TO_GATE_PROCESS_INDEX;
		return groupIndex;
	}

	if( pID == m_BaggageDeviceID )
	{
		groupIndex.start = groupIndex.end = BAGGAGE_DEVICE_PROCEOR_INDEX;
		return groupIndex;
	}


	// test for ALL PROCESSORS
	if (pID.isBlank())
	{
		groupIndex.start = 0;
		groupIndex.end = m_procCount - 1;
		return groupIndex;
	}

	return m_groupIdxTree.FindGroup( pID );
}

ProcessorFamily* ProcessorList::getFamilyIndex(const ProcessorID& pID) const
{

	ProcessorFamily* pFamily = NULL;
	GroupIndex groupIndex=getGroupIndex(pID);
	if (groupIndex.start != -1 || groupIndex.end !=  - 1)
	{
		Processor* tempProcessor=getProcessor(groupIndex.start);
		int  processortype = tempProcessor->getProcessorType();
		if (processortype == Elevator)
		{
			pFamily = new ElevProcessorFamily;
		}
		else
		{
			pFamily = new ProcessorFamily;
		}

		pFamily ->SetStartIndexInProcList( getGroupIndex(pID).start );
		pFamily ->SetEndIndexInProcList( getGroupIndex(pID).end);
	}


	return pFamily;

}

GroupIndex ProcessorList::getGroupIndexOriginal (const ProcessorID& pID) const
{
	GroupIndex groupIndex = {-1, -1};
	assert( m_pInTerm );

	// test for START processor
	if (pID == m_StartID )
	{
		groupIndex.start = groupIndex.end = START_PROCESSOR_INDEX;
		return groupIndex;
	}

	// test for END processor
	if (pID == m_EndID )
	{
		groupIndex.start = groupIndex.end = END_PROCESSOR_INDEX;
		return groupIndex;
	}

	if( pID == m_FromGateID )
	{
		groupIndex.start = groupIndex.end = FROM_GATE_PROCESS_INDEX;
		return groupIndex;
	}

	if( pID == m_ToGateID )
	{
		groupIndex.start = groupIndex.end = TO_GATE_PROCESS_INDEX;
		return groupIndex;
	}

	if( pID == m_BaggageDeviceID )
	{
		groupIndex.start = groupIndex.end = BAGGAGE_DEVICE_PROCEOR_INDEX;
		return groupIndex;
	}


	// test for ALL PROCESSORS
	if (pID.isBlank())
	{
		groupIndex.start = 0;
		groupIndex.end = m_procCount - 1;
		return groupIndex;
	}

	int nFirstIndex = -1;
	int nLastIndex = -1;
	int nCount = procList.getCount();
	for( int i=0; i<nCount; i++ )
	{
		Processor *proc = procList.getItem ( i );
		if( prefixCompare ( *proc, (void*)&pID ) )
		{
			if( nFirstIndex == - 1 )
			{
				nFirstIndex = i;
			}
			nLastIndex = i;
		}
	}
	if ( nFirstIndex == -1 )
		return groupIndex;

	groupIndex.start = nFirstIndex;
	groupIndex.end = nLastIndex;    
	return groupIndex;
}
ProcessorID ProcessorList::getHighestSingleGroup (const Processor *proc) const
{
	GroupIndex group;
	ProcessorID id (*proc->getID());
	ProcessorID testID (id);

	while (testID.getSuperGroup())
	{
		group = getGroupIndexOriginal (testID);
		if (group.start != group.end)
			return id;
		id = testID;
	}
	return id;
}


//It clears waiting queue and wipes passengers from holding area.
void ProcessorList::clearPassengers (ElapsedTime curTime)
{
	Processor *aProc;

	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem (i);
		assert( aProc );

		// clear all pax in occupants list.
		aProc->clearPerson( curTime );

		// test queue for waiting passengers
		aProc->clearQueue(curTime);
		//clear RecycleFrequency
		aProc->ClearRecycleFrequency();

		//clear lead to gate and reclaim information
		aProc->ClearGateAndReclaim();
	}
}

// Writes log entries and sorts all processor events.
void ProcessorList::writeProcLog (void)
{
	PLACE_METHOD_TRACK_STRING();
	Processor *aProc;
	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem(i);
		aProc->writeLog();
	}
}


// Flushes log entries and sorts all processor events.
void ProcessorList::updateProcLog (void)
{
	PLACE_METHOD_TRACK_STRING();
	echoFlag = 1;
	Processor *aProc;
	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem(i);
		aProc->sortLog();
		aProc->flushLog();
	}
}




// Loads entire list from CSV file.
void ProcessorList::readData (ArctermFile& p_file)
{
	p_file.getLine();
	Processor *aProc=NULL;
	m_pServicePoints = new Point[MAX_POINTS];
	m_pQueue = new Point[MAX_POINTS];

	bool bFirst = true;
	int co = 0;
	while (!p_file.isBlank ())
	{
		co ++;
		try
		{
			aProc = readProcessor (p_file, bFirst);
		}catch (...) 
		{
			Sleep(0);
		}
		if(aProc==NULL)return;
		bFirst = false;
		if( aProc->getProcessorType() == Elevator )
		{
			((ElevatorProc*)aProc)->InitLayout();
		}
		else if( aProc->getProcessorType() == ConveyorProc )
		{
			((Conveyor*)aProc)->CalculateTheBisectLine();
		}
		else if( aProc->getProcessorType() == StairProc )
		{
			((Stair*)aProc)->CalculateTheBisectLine();
		}
		else if( aProc->getProcessorType() == EscalatorProc )
		{	
			((Escalator*)aProc)->CheckAndRectifyQueueInOutConstraint();
			((Escalator*)aProc)->CalculateTheBisectLine();

		}

		addProcessor (aProc,false);
	}

	BuildProcGroupIdxTree();
	if( m_bDoOffset )
		DoOffset();

	delete m_pServicePoints;
	delete m_pQueue;
	setIndexes();

// 	if (p_file.getVersion() < float(2.4) && p_file.getVersion() > float(2.2))
// 	{
// 		CDlgGateTypedefine dlg(this,m_pInTerm);
// 		dlg.DoModal();
// 	}
	//InitOldBaggageProcData();
}

// Writes processor list to CSV file.
void ProcessorList::writeData (ArctermFile& p_file) const
{
	Processor *aProc;
	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem(i);
		aProc->writeProcessor (p_file);
	}
}


// EXCEPTION: StringError
Processor *ProcessorList::readProcessor (ArctermFile& procFile, bool _bFirst /*=false*/ )
{
	Processor *proc;
	ProcessorID procID;

	assert( m_pInTerm );
	procID.SetStrDict( m_pInTerm->inStrDict );
	if (!procID.readProcessorID (procFile))
		throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");

	char string[SMALL];
	procFile.getField (string, SMALL);


	if (!_stricmp (string, "DPND_SOURCE"))
		proc = new DependentSource;
	else if (!_stricmp (string, "DPND_SINK"))
		proc = new DependentSink;
	else if (!_stricmp (string, "GATE"))
		proc = new GateProcessor;
	else if (!_stricmp (string, "LINE"))
		proc = new LineProcessor;
	else if (!_stricmp(string, "RETAIL"))
		proc = new RetailProcessor;
	else if (!_stricmp (string, "BAGGAGE"))
		proc = new BaggageProcessor;
	else if (!_stricmp (string, "HOLD_AREA"))
		proc = new HoldingArea;
	else if (!_stricmp (string, "BARRIER"))
		proc = new Barrier;
	else if (!_stricmp (string, "FLOOR")) 
		proc = new FloorChangeProcessor;
	else if (!_stricmp(string,"INTEGRATED_STATION"))
		proc = new IntegratedStation ;
	else if (!_stricmp(string,"MOVING_SIDE_WALK"))
		proc = new MovingSideWalk();
	else if(!_stricmp(string,"Elevator"))
		proc =new ElevatorProc;
	else if(!_stricmp(string,"Conveyor"))
		proc =new Conveyor;
	else if(!_stricmp(string,"Stair"))
		proc =new Stair;
	else if(!_stricmp(string,"Escalator"))
		proc =new Escalator;
	else if(!_stricmp(string,"BridgeConnector"))
		proc =new BridgeConnector;
	else if(!_stricmp(string,"ARP"))
		proc = new ArpProc();
	else if(!_stricmp(string,"RUNWAY"))
		proc = new RunwayProc();
	else if(!_stricmp(string,"TAXIWAY"))
		proc = new TaxiwayProc();
	else if(!_stricmp(string,"STAND"))
		proc = new StandProc();
	else if(!_stricmp(string,"NODE"))
		proc = new NodeProc();
	else if(!_stricmp(string,"DEICE_STATION"))
		proc = new DeiceBayProc();
	else if(!_stricmp(string,"FIX"))
		proc = new FixProc();
	else if (!_stricmp(string,"CONTOUR"))
		proc = new ContourProc;
	else if(!_stricmp(string ,"APRON"))
		proc= new ApronProc;	
	else if (!_stricmp(string,"Billboard"))
		proc = new BillboardProc;
	else if(!_stricmp(string,"Hold"))
		proc = new HoldProc;
	else if(!_stricmp(string,"Sector"))
		proc = new SectorProc;
	else if(!_stricmp(string,StretchProc::m_strTypeName))
		proc  = new StretchProc();
	else if(!_stricmp(string,IntersectionProc::m_strTypeName))
		proc = new IntersectionProc();
	else if( !_stricmp(string,RoundAboutProc::m_strTypeName) )
		proc = new RoundAboutProc();
	else if( !_stricmp(string,TurnOffProc::m_strTypeName) )
		proc = new TurnOffProc();
	else if( !_stricmp(string,LaneAdapterProc::m_strTypeName) )
		proc = new LaneAdapterProc();
	else if( !_stricmp(string,OverPassProc::m_strTypeName) )
		proc = new OverPassProc();
	else if( !_stricmp(string,TrafficlightProc::m_strTypeName))
		proc = new TrafficlightProc();
	else if( !_stricmp(string,YieldSignProc::m_strTypeName ))
		proc = new YieldSignProc;
	else if( !_stricmp(string,StopSignProc::m_strTypeName))
		proc = new StopSignProc;
	else if( !_stricmp(string,TollGateProc::m_strTypeName))
		proc = new TollGateProc;
	else if( !_stricmp(string,LineParkingProc::m_strTypeName))
		proc = new LineParkingProc;
	else if( !_stricmp(string,NoseInParkingProc::m_strTypeName))
		proc = new NoseInParkingProc;
	//else if( !stricmp(string,UnderPassProc::m_strTypeName) )
		//proc = new UnderPassProc();
	//add new proc here
	//else if(!stricmp(string,"Turnoff"))
		
	else
		proc = new Processor;


	proc->init (procID);
	proc->SetTerminal((Terminal *)m_pInTerm); // add by tutu 2002-9-26

	/*if( proc->getProcessorType() >= StretchProcessor)
	{		
		proc->readSpecialField( procFile );
		return proc;
	}*/

	//add by hans 2005-12-30 add backup flag
	int nBackup;
	procFile.getInteger(nBackup);
	if (nBackup > 0)
	{
		proc->SetBackup(true);
	}
	else
	{
		proc->SetBackup(false);
	}

	Point aPoint;
	Point inConst[100], outConst[100];
	int svcCount = 0, inCount = 0, queueCount = 0, outCount = 0;
    int index = 0 ;
	std::vector<int> entrypoint ;
	if (!procFile.getPoint (aPoint))
		throw new StringError ("All processors must have a service location");


	if( _bFirst )
	{
		m_bDoOffset = false;
		static DistanceUnit THRESHOLD_FOR_DOOFFSET = 30000000;
		DistanceUnit x = aPoint.getX();
		DistanceUnit y = aPoint.getY();
		if( x  > THRESHOLD_FOR_DOOFFSET || x < -THRESHOLD_FOR_DOOFFSET
			|| y > THRESHOLD_FOR_DOOFFSET || y < -THRESHOLD_FOR_DOOFFSET )
		{
			m_bDoOffset = true;			
			m_minX = x;
			m_maxX = x;
			m_minY = y;
			m_maxY = y;			
		}
	}

	if( m_bDoOffset )
	{
		DistanceUnit x = aPoint.getX();
		if( x < m_minX )
			m_minX = x;
		else if( x > m_maxX )
			m_maxX = x;

		DistanceUnit y = aPoint.getY();
		if( y < m_minY )
			m_minY = y;
		else if( y > m_maxY )
			m_maxY = y;		
	}

	m_pServicePoints[svcCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		inConst[inCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		m_pQueue[queueCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		outConst[outCount++] = aPoint;

	char fixedQueue = 'N';
	if (queueCount)
		procFile.getChar (fixedQueue);


	proc->readSpecialField( procFile );
	// make sure point limits are not exceeded
	for (int i = 1; procFile.getLine() && !procFile.isNewEntryLine(); i++)
	{
		if (i + 1 >= MAX_POINTS)
		{
			procID.printID (string);
			throw new TwoStringError ("maximum point count exceeded by ", string);
		}

		// skip ID and Processor Type fields
		procFile.skipField (2);

		if (procFile.getPoint (aPoint))
			m_pServicePoints[svcCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			inConst[inCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			m_pQueue[queueCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			outConst[outCount++] = aPoint;
		if(procFile.getVersion() >= 2.5 && fixedQueue=='Y')///// version changede  ,
		{
			if(procFile.getInteger(index))
				entrypoint.push_back(index) ;
		}
	}


	char c[256];
	proc->getID()->printID( c );

	proc->initInConstraint (inCount, inConst);
	proc->initServiceLocation (svcCount, m_pServicePoints);
	proc->initOutConstraint (outCount, outConst);
	proc->initQueue (fixedQueue == 'Y', queueCount, m_pQueue);
    proc->initEntryPoint(entrypoint) ;
	///proc->readSpecialProcessor(procFile);
	return proc;
}


int ProcessorList::echoProcessorNames (const char *procEchoFile) const
{
	ofsstream echoFile (procEchoFile);
	if (echoFile.bad())
		throw new FileOpenError (procEchoFile);

	Processor *aProc;
	echoFile << "Arcterm V1.2 Processor List\n\n";
	for (int i = 0; i < m_procCount; i++)
	{
		aProc = procList.getItem(i);
		char str[32];
		aProc->name.printID (str);
		echoFile << "Processor #" << i << ',' << str << '\n';
	}
	return TRUE;
}



int ProcessorList::GetNumFloors()
{
	int nFloors = 0;
	for( int i=0; i<m_procCount; i++ )
	{
		Processor* pProc = getProcessor( i );
		int n = pProc->getFloor() / (int)SCALE_FACTOR + 1;
		if( n > nFloors )
			nFloors = n;
	}
	return nFloors;
}



void ProcessorList::DoOffset()
{
	if( !m_bDoOffset )
		return;

	DistanceUnit midX = m_minX + ( m_maxX - m_minX ) / 2;
	DistanceUnit midY = m_minY + ( m_maxY - m_minY ) / 2;

	CString csMsg;
	csMsg.Format( "The project you just loaded is based on a coordinate origin %.2f (m) by %.2f (m) away. Would you like to permanently bring it closer to the origin (recommended!):", 
		midX / SCALE_FACTOR , midY / SCALE_FACTOR );
	if( MessageBox( NULL, csMsg, "Warning", MB_YESNO | MB_ICONQUESTION ) == IDNO )
		return;

	// do the offset
	for( int i=0; i<m_procCount; i++ )
	{
		//Returns processor at index ndx.
		Processor* pProc = getProcessor( i );
		pProc->DoOffset( -midX, -midY );
	}
}


BOOL ProcessorList::IsValidProcessorName(const char* name)
{
	ProcessorID newID;
	if(!newID.isValidID(name))
	{
		AfxMessageBox("name is not valid!");
		return false;
	}

	newID.SetStrDict(m_pInTerm->inStrDict);
	newID.setID(name);

	if(!isUnique(newID))
	{
		AfxMessageBox("name is not unique!");
		return false;
	}


	int iIDLength=newID.idLength();

	GroupIndex temp;

	for(int i=0;i<iIDLength;++i)
	{

		temp=getGroupIndexOriginal(newID);

		if(temp.start!=-1&&getProcessor(temp.start)->getID()->idLength()!=iIDLength)
		{
			AfxMessageBox("name is not valid!");
			return false;
		}


		newID.getSuperGroup();
	}

	return true;


}

void ProcessorList::RefreshAllStationLocatonData()
{
	int iProcessorCount = this->getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == IntegratedStationProc )
		{
			//pTemp->SetTerminal( (Terminal*) m_pInTerm );
			((IntegratedStation*)pTemp)->InitLocalData();
		}
	}
}


// Clean all the data related to sim engine
void ProcessorList::CleanData()
{
	int nCount = getProcessorCount();
	for( int i=0; i<nCount; i++ )
	{
		Processor* pProc = getProcessor( i );
		pProc->CleanData();
	}
}


// processor in the proclist is the only data the engine will interfaced. need to refreshed before
// the engine start.
void ProcessorList::RefreshRelatedDatabase()
{
	PLACE_METHOD_TRACK_STRING();
	setServiceTimes( *m_pInTerm->serviceTimeDB );	
	setPassengerFlows( *m_pInTerm->m_pPassengerFlowDB, *m_pInTerm->m_pSpecificFlightPaxFlowDB, *m_pInTerm->m_pStationPaxFlowDB, *m_pInTerm->m_pSubFlowList);
	setProcAssignments( *m_pInTerm->procAssignDB );


	setMiscData( *m_pInTerm->miscData );



	for (int i = 0; i < m_procCount; i++)
	{
		Processor *pProc = procList.getItem(i);
		pProc->RefreshRelatedData( m_pInTerm );
        //
		pProc->ResetEntryPoint() ;
		//Init ProcessorCapacityInSim
		pProc->InitProcessorCapacityInSim();
		pProc->ClearSequentialQueue();
	}
   
	// must called after setProcAssignments
	CopySinkAssignToSource();

	// following data is to avoid the same process in the sim loop
	
	m_barrierList.clear();
	m_nBarrierCount = 0;
	getProcessorsOfType (BarrierProc, m_barrierList);
	m_nBarrierCount = m_barrierList.getCount();

	m_baggageProcList.clear();
	getProcessorsOfType (BaggageProc, m_baggageProcList);
	InitAllBaggageProcData();

	// baggage device barriers
	int iBaggageProcCount = m_baggageProcList.getCount();
	for( int s=0; s<iBaggageProcCount;++s )
	{
		BaggageProcessor* pBaggage = ( BaggageProcessor* )m_baggageProcList.getItem( s );
		m_barrierList.addItem( pBaggage->GetBarrier() );
	}
	m_nBarrierCount += iBaggageProcCount;	

	m_billBoardProcList.clear();
	getProcessorsOfType (BillboardProcessor, m_billBoardProcList);
	InitAllBillBoardProcData();

}

// copy dependent sink's roster to its source
void ProcessorList::CopySinkAssignToSource()
{
	for (int i = 0; i < m_procCount; i++)
	{
		Processor *pProc = procList.getItem(i);
		if( pProc->getProcessorType() == DepSourceProc )
		{
			((DependentSource*)pProc)->CopySinkAssignToSource();
		}

	}
}
void ProcessorList::GetInbridgeOfGate( int iGateIdx , std::vector<int>& _vInbridge ) const
{

	for (int i = 0; i < m_procCount; i++)
	{
		Processor *pProc = procList.getItem(i);
		if( i != iGateIdx && pProc->AttachToGate( iGateIdx ) && pProc->getProcessorType() != HoldAreaProc )
		{
			_vInbridge.push_back( i );
		}
	}	
}
void ProcessorList::GetHoldingAraOfGate( int iGateIdx , std::vector<int>& _vHoldingArea ) const
{
	for (int i = 0; i < m_procCount; i++)
	{
		Processor *pProc = procList.getItem(i);
		if( i != iGateIdx && pProc->getProcessorType() == HoldAreaProc && pProc->AttachToGate( iGateIdx ) )
		{
			_vHoldingArea.push_back( i );
		}
	}
}
void ProcessorList::InitAllBaggageProcData()
{
	for (int i=0 ;i < m_baggageProcList.getCount(); i++)
	{
		BaggageProcessor *pBaggageProc = (BaggageProcessor *)m_baggageProcList.getItem( i );
		ASSERT( pBaggageProc != NULL );
		pBaggageProc->InitBaggageDeviceData();
	}
}

void ProcessorList::InitAllBillBoardProcData()
{
	for (int i=0 ;i < m_billBoardProcList.getCount(); i++)
	{
		BillboardProc *pBillboardProc = (BillboardProc *)m_billBoardProcList.getItem( i );
		ASSERT( pBillboardProc != NULL );

		pBillboardProc->InitWholeExposurePath();
	}

}

void ProcessorList::InitAllProcDirection()
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = this->getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		pTemp->GetDirection().Sort();	
	}
}

// init baggage proc's barrier and bag moving path if file version is old
void ProcessorList::InitOldBaggageProcData()
{
	int iProcessorCount = this->getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == BaggageProc )
		{
			BaggageProcessor* pBaggage = ( BaggageProcessor*) pTemp;

			//Pollygon temp;
			//temp.init( pBaggage->serviceLocationLength(), pBaggage->serviceLocationPath()->getPointList() );

			if( pBaggage->GetPathBagMovingOn()->getCount() < 3 )
			{
				//temp.Zoom( 0.95, 0.95 );
				//pBaggage->GetPathBagMovingOn()->init(temp);;
				pBaggage->GetPathBagMovingOn()->init(pBaggage->serviceLocationLength(), pBaggage->serviceLocationPath()->getPointList() );
			}
			if( pBaggage->GetBarrier()->serviceLocationPath()->getCount() < 2 )
			{
				//temp.Zoom( 0.9, 0.9 );
				//pBaggage->GetBarrier()->initServiceLocation( temp.getCount(), temp.getPointList() );
				pBaggage->GetBarrier()->initServiceLocation(pBaggage->serviceLocationLength(), pBaggage->serviceLocationPath()->getPointList() );
			}


		}
	}
}


void ProcessorList::GetLinkedWaitAreas( const Processor* _pProc, std::vector<Processor*>& _vWaitAreas )
{
	assert( _pProc );

	MiscProcessorData* pMiscData = m_pInTerm->miscData->getDatabase( _pProc->getProcessorType() );
	// find the nest processorID
	const ProcessorID* pid = _pProc->getID();
	int idx = pMiscData->findBestMatch( *pid );
	if( idx== -1)
		return;
	//get wait processor list
	MiscData* pData = pMiscData->getEntry( idx );
	assert(pData);
	int iWaitCount = pData->getWaitAreaList()->getCount();
	for( int i=0; i<iWaitCount; i++ )
	{
		ProcessorID* waitID = pData->getWaitAreaList()->getItem( i );
		GroupIndex group = getGroupIndexOriginal(*waitID);
		for( int g=group.start; g<=group.end; g++ )
			_vWaitAreas.push_back( getProcessor(g) );
	}
}


Processor* ProcessorList::GetWaitArea( const ProcessorID* _pProcID )
{
	// get processor type
	GroupIndex group = getGroupIndexOriginal( *_pProcID );
	if( group.start == -1 && group.end == -1 )
		return NULL;

	// find the nest processorID
	MiscProcessorData* pMiscData = m_pInTerm->miscData->getDatabase( getProcessor(group.start)->getProcessorType() );
	int idx = pMiscData->findBestMatch( *_pProcID );
	if( idx == -1 )
		return NULL;

	// get all wait processor
	std::vector<Processor*> vAllWaitProcessor;
	MiscData* pData = pMiscData->getEntry( idx );
	assert(pData);
	int iWaitCount = pData->getWaitAreaList()->getCount();
	for( int i=0; i<iWaitCount; i++ )
	{
		ProcessorID* waitID = pData->getWaitAreaList()->getItem( i );
		GroupIndex group = getGroupIndexOriginal(*waitID);
		for( int g=group.start; g<=group.end; g++ )
			vAllWaitProcessor.push_back( getProcessor(g) );
	}

	// random select a processor
	int size = vAllWaitProcessor.size();
	if( size ==0 )
		return NULL;
	return vAllWaitProcessor[ random(size) ];
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Exception : find the nearest opened proc and let person go to that proc and wait for opening
//added by Cj
//EarliestOpenTime : the earliest open time for all the processors.
//NearestOpenTime : the nearest open time for one processor.
//WaitAtProcVector : the container  includes the processors which go to process and wait for open.
//WaitAtWaitAreaProcVector : the container which include processors which should wait for open at the wait area. this because at current time, this processor is open and but severing another pax type,should go to the 
//processor's waiting Area for processor open. 
//
//
//       ---------algorithmic Graph---------
//Processor Group --> processor  ---------> GetProcessAssignSchedule --> GetNearestOpenRoster
//   |				/|\															|	
//   |				 |_______________________No nearest open Roster_____________|
//	 |				 |															|
//	 |				 |											 Compare NearestOpenTime and EarliestOpenTime
//	 |				 |															|
//	 |				 |<-----------NearestOpenTime > EarliestOpenTime -----------|
//	 |				 |													        |------------>NearestOpenTime < EarliestOpenTime
//	 |				 |														    |						|
//	 |				 |															|					   \|/
//	 |				 |															|		WaitAtProcVector.clear and WaitAtWaitAreaProcVector.clear
//	 |				 |															|						|										
//	 |				 |															|<----------------------|
//	 |				 |											check the proceesor is open or close at current time
//	 |				 |															|
//	 |				 |															|
//	 |				 |	 														|-------------------------------->is Open ------------->check if can server thie pax type
//	 |				 |															|																	|
//	 |				 |														 is close																|
//	 |				 |															|<---------------------------------CanServer------------------------|
//	 |				 |															|																	|
//	 |				 |													add processor in the WaitAtProcVector							add processor in the WaitAtWaitAreaProcVector
//	 |			     |															|																	|
//	 |				 |															|<------------------------------------------------------------------|
//	 |				 |													set EarliestOpenTime = NearestOpenTime
//   |				 |<---------------------------------------------------------|
//	 |
//   |<------------------------Out the circle-----------------------------------|
//   |
//if WaitAtProcVector.size == 0 && WaitAtWaitAreaProcVector.size == 0 
//	 |
//	 |---------TRUE------------->Return NULL
//   |
//if WaitAtProcVector.size > 0 
//   |
//   |---------TRUE------------->Return  Random processor in WaitAtProcVector ,  _resultType = TRUE
//   |
//return Random processor in WaitAtWaitAreaProcVector, _resultType = FALSE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Processor* ProcessorList::GetNearestOpenedProc(Processor *curProc, const ProcessorID* _pProcGroupID, const ElapsedTime& _curTime, Person *_person,ElapsedTime& _openTime ,bool& _resultType )//const CMobileElemConstraint& _paxType, ElapsedTime& _openTime ,bool& _resultType )
{
	ASSERT( _pProcGroupID );
	GroupIndex groupIdx = getGroupIndexOriginal( *_pProcGroupID );
	ASSERT( groupIdx.start >= 0 );
	//throw new()
	if(groupIdx.start<0 ) return NULL;
    int days = m_pInTerm->flightSchedule->GetFlightScheduleEndTime().GetDay() ;
	ElapsedTime EarliestOpenTime;
	EarliestOpenTime.set(days*WholeDay );
	
	std::vector<Processor*> WaitAtProcVector ;
	std::vector<Processor*> WaitAtWaitAreaProcVector;
	ProcessorRoster* ProcAssigment = NULL ;

	for( int i=groupIdx.start ; i<= groupIdx.end; ++i )
	{
		ElapsedTime earliestTime;
		Processor* pProc = getProcessor( i );

		ProcessorArray aryLinkedDestPros;
		bool _bLevel0Matched = false;
		if(curProc->GetLinkedDestCount() > 0 && curProc->FindLinkedDestinationProcs(*(pProc->getID()),aryLinkedDestPros, _bLevel0Matched) == 0)
			continue;

		if(pProc->getProcessorType() == DepSourceProc)
			return GetNearestOpenedDependSourceProc(curProc, _pProcGroupID,_curTime,_person,_openTime ,_resultType);

		int idx = m_pInTerm->procAssignDB->findBestMatch( *pProc->getID() );
		if( idx == -1 )
			continue;

		// TRACE ("\n%s\n",pProc->getID()->GetIDString() );
		ProcessorRosterSchedule* pSchd = m_pInTerm->procAssignDB->getDatabase( idx );
		ProcAssigment = pSchd->GetNearestOpenAssignment(_curTime,_person->getType()) ;
		if(ProcAssigment == NULL)
				continue ;
		if(ProcAssigment->getAbsOpenTime() > EarliestOpenTime)
				continue ;
		if(ProcAssigment->getAbsOpenTime() < EarliestOpenTime)
		{
				WaitAtProcVector.clear() ;
				WaitAtWaitAreaProcVector.clear() ;
		}

		ProcessorRoster* CurrentAssignment = NULL ;
		if(pSchd->CheckIsOpenAtThisTime(_curTime,&CurrentAssignment))
		{
			if(CurrentAssignment->getAssignment()->isMatched(_person->getType()))
				WaitAtProcVector.push_back(pProc) ;
			else
				WaitAtWaitAreaProcVector.push_back(pProc) ;
		}
		else
			WaitAtProcVector.push_back(pProc) ;

		EarliestOpenTime = ProcAssigment->getAbsOpenTime() ;
	}
	_openTime = EarliestOpenTime;
	if(WaitAtProcVector.size() == 0 && WaitAtWaitAreaProcVector.size() == 0)
		return NULL ;
	if(WaitAtProcVector.size() > 0)
	{
		_resultType = TRUE ;
		return WaitAtProcVector[random(WaitAtProcVector.size())] ;
	}
	_resultType = FALSE ;
	return WaitAtWaitAreaProcVector[random(WaitAtWaitAreaProcVector.size())] ;
	
}
Processor* ProcessorList::GetNearestOpenedDependSourceProc(Processor *curProc,  const ProcessorID* _pProcGroupID, const ElapsedTime& _curTime, Person *_person, ElapsedTime& _openTime ,bool& _resultType )
{

	//check depend source

	ASSERT( _pProcGroupID );
	GroupIndex groupIdx = getGroupIndexOriginal( *_pProcGroupID );
	ASSERT( groupIdx.start >= 0 );
	//throw new()
	if(groupIdx.start<0 ) return NULL;
	int days = m_pInTerm->flightSchedule->GetFlightScheduleEndTime().GetDay() ;
	ElapsedTime EarliestOpenTime;
	EarliestOpenTime.set(days*WholeDay );

	std::vector<Processor*> WaitAtProcVector ;
	std::vector<Processor*> WaitAtWaitAreaProcVector;
	ProcessorRoster* ProcAssigment = NULL ;

	for( int i=groupIdx.start ; i<= groupIdx.end; ++i )
	{
		ElapsedTime earliestTime;
		Processor* pProc = getProcessor( i );
		
		ProcessorArray aryLinkedDestPros;
		bool _bLevel0Matched = false;
		if(curProc->GetLinkedDestCount() > 0 && curProc->FindLinkedDestinationProcs(*(pProc->getID()),aryLinkedDestPros, _bLevel0Matched) == 0)
			continue;

		int idx = m_pInTerm->procAssignDB->findBestMatch( *pProc->getID() );
		if( idx == -1 )
			continue;

		// TRACE ("\n%s\n",pProc->getID()->GetIDString() );
		ProcessorRosterSchedule* pSchd = m_pInTerm->procAssignDB->getDatabase( idx );
		ProcAssigment = pSchd->GetNearestOpenAssignment(_curTime,_person->getType()) ;
		if(ProcAssigment == NULL)
			continue ;
		if(ProcAssigment->getAbsOpenTime() > EarliestOpenTime)
			continue ;
		if(ProcAssigment->getAbsOpenTime() < EarliestOpenTime)
		{
			WaitAtProcVector.clear() ;
			WaitAtWaitAreaProcVector.clear() ;
		}

		ProcessorRoster* CurrentAssignment = NULL ;
		if(pSchd->CheckIsOpenAtThisTime(_curTime,&CurrentAssignment))
		{
			if(CurrentAssignment->getAssignment()->isMatched(_person->getType()))
				WaitAtProcVector.push_back(pProc) ;
			else
				WaitAtWaitAreaProcVector.push_back(pProc) ;
		}
		else
			WaitAtProcVector.push_back(pProc) ;

		EarliestOpenTime = ProcAssigment->getAbsOpenTime() ;
	}

	_openTime = EarliestOpenTime;
	if(WaitAtProcVector.size() > 0)
	{
		_resultType = TRUE ;
		return WaitAtProcVector[random(WaitAtProcVector.size())] ;
	}
	_resultType = FALSE ;
	if(WaitAtWaitAreaProcVector.size() > 0)
		return WaitAtWaitAreaProcVector[random(WaitAtWaitAreaProcVector.size())] ;



//	if(WaitAtProcVector.size() == 0 && WaitAtWaitAreaProcVector.size() == 0)
//		return NULL ;



	EarliestOpenTime.set(days*WholeDay );
	//check sink
	for( int i=groupIdx.start ; i<= groupIdx.end; ++i )
	{
		ElapsedTime earliestTime;
		Processor* pProc = getProcessor( i );
		int idx = m_pInTerm->procAssignDB->findBestMatch( *pProc->getID() );
		if( idx == -1 )
			continue;

		DependentSource *pDependSource = (DependentSource *) pProc;
		if(pDependSource == NULL)
			continue;

		ProcessorRosterSchedule* pSchd = m_pInTerm->procAssignDB->getDatabase( idx );
		if(pSchd == NULL)
			continue;

		for (int i = 0; i < pDependSource->GetSinkCount(); i++)
		{
			for (int j = (pDependSource->GetSink())[i].start; j <= (pDependSource->GetSink())[i].end; j++)
			{

				Processor* aProc = m_pInTerm->procList->getProcessor(j);
				if(pProc == NULL)
					continue;

				ElapsedTime eSinkOpenTime;
				bool bSinkResultType = false;
				Processor* pProcAvail =GetNearestOpenedProc(curProc,  aProc->getID(),  _curTime, _person,eSinkOpenTime ,bSinkResultType );
				if(pProcAvail)
				{
					if(eSinkOpenTime > EarliestOpenTime)
						continue;
					else if(eSinkOpenTime < EarliestOpenTime)
					{
						WaitAtProcVector.clear() ;
						WaitAtWaitAreaProcVector.clear() ;
					}

					EarliestOpenTime = eSinkOpenTime;

					ProcessorRoster* CurrentAssignment = NULL ;
					if(pSchd->CheckIsOpenAtThisTime(_curTime,&CurrentAssignment))
					{
						if(CurrentAssignment == NULL || CurrentAssignment->getAssignment()->isMatched(_person->getType()))
							WaitAtProcVector.push_back(pProc) ;
						else
							WaitAtWaitAreaProcVector.push_back(pProc) ;
					}
					else
						WaitAtProcVector.push_back(pProc) ;
				}
			}
		}
	}

	if(WaitAtProcVector.size() == 0 && WaitAtWaitAreaProcVector.size() == 0)
		return NULL ;

	_openTime = EarliestOpenTime;
	if(WaitAtProcVector.size() > 0)
	{
		_resultType = TRUE ;
		return WaitAtProcVector[random(WaitAtProcVector.size())] ;
	}
	_resultType = FALSE ;
	return WaitAtWaitAreaProcVector[random(WaitAtWaitAreaProcVector.size())] ;




}
// init all processor's available = isOpen = false;
void ProcessorList::InitProcsOpenAvaiFlag()
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		//		// TRACE ("\n%s\n", pTemp->getID()->GetIDString() );
		//pTemp->available = FALSE;
		pTemp->isOpen = FALSE;
	}
}
bool ProcessorList::IfAllBaggageProcDataValid()
{
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == BaggageProc )
		{
			BaggageProcessor* pBaggage = ( BaggageProcessor*) pTemp;
			CString sError( "Invalid data in Baggage Processor " );


			if( !(pBaggage->GetPathBagMovingOn() && pBaggage->GetPathBagMovingOn()->getCount() > 2 ) )
			{
				sError += pBaggage->getID()->GetIDString();
				sError +="\nPlease pick baggage moving path  from map again.";
				AfxMessageBox( sError );
				return false;
			}
			if( !(pBaggage->GetBarrier() && pBaggage->GetBarrier()->serviceLocationPath()->getCount() > 1 ) )
			{
				sError += pBaggage->getID()->GetIDString();
				sError +="\nPlease pick barrier from map again.";
				AfxMessageBox( sError );
				return false;					
			}			
		}
	}

	return true;
}

// init elevator's necessary data, such as every floor's height....etc
bool ProcessorList::InitAllElevatorData( std::vector<double>& _vFloorsAltitude,Terminal* _pTerm )
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == Elevator )
		{
			//*************************************//
			//only for the unit test purpose. must be removed later!!!			
			//	ElevatorMoveEvent* pEvent = new ElevatorMoveEvent( (ElevatorProc*)pTemp, 0 );
			//	pEvent->setTime( ElapsedTime(0l) );
			//	pEvent->addEvent();
			//*****************************************//			
			if(!((ElevatorProc*)pTemp)->InitElevatorData( _vFloorsAltitude,_pTerm ))
				return false;
		}
	}
	return true;
}

void ProcessorList::FlushElevatorLog()
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == Elevator )
		{	
			((ElevatorProc*)pTemp)->FlushLog();		
		}
	}
}
void ProcessorList::InitQueueTraceFlag()
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		ProcessorQueue* pQueue = pTemp->GetProcessorQueue();
		if( pQueue )
		{
			if( pTemp->getID()->GetIDString() == strProcessorToBeTraced )
			{
				pQueue->SetTraceFlag( true );
			}
			else
			{
				pQueue->SetTraceFlag( false );
			}
		}
	}
}

bool ProcessorList::CheckStairProcessor( const Processor* _pCurrentProc,  const Processor* _pDestProc )const
{

	if(  _pCurrentProc->getProcessorType() == StairProc   // curProc is particular
		|| _pCurrentProc->getProcessorType()==EscalatorProc
		|| _pCurrentProc->getProcessorType()== Elevator)
	{
		if( _pDestProc->getProcessorType() == StairProc
			|| _pDestProc->getProcessorType()==EscalatorProc  // curProc and DestProc is particular
			|| _pDestProc->getProcessorType()== Elevator)
		{
			if( (int)_pCurrentProc->getServicePoint(0).getZ() == (int)_pDestProc->getServicePoint(0).getZ() 
				||(int)_pCurrentProc->getServicePoint(0).getZ() == (int)_pDestProc->getServicePoint( _pDestProc->serviceLocationLength() -1 ).getZ())
				return true;
			else if((int)_pCurrentProc->getServicePoint( _pCurrentProc->serviceLocationLength() -1 ).getZ() == (int)_pDestProc->getServicePoint(0).getZ()
				||(int)_pCurrentProc->getServicePoint( _pCurrentProc->serviceLocationLength() -1 ).getZ() == (int)_pDestProc->getServicePoint( _pDestProc->serviceLocationLength() -1 ).getZ())
				return true;
			else return false;
		}
		else 
		{
			if( (int)_pCurrentProc->getServicePoint(0).getZ() == (int)_pDestProc->getServicePoint(0).getZ() 
				||(int)_pCurrentProc->getServicePoint( _pCurrentProc->serviceLocationLength() -1).getZ() == (int)_pDestProc->getServicePoint(0).getZ())
			{
				return true;
			}
			else return false;
		}

	} 
	else  
	{
		if( _pDestProc->getProcessorType() == StairProc
			|| _pDestProc->getProcessorType()==EscalatorProc 
			|| _pDestProc->getProcessorType()== Elevator)
		{
			if( (int)_pCurrentProc->getServicePoint(0).getZ() == (int)_pDestProc->getServicePoint(0).getZ() 
				||(int)_pCurrentProc->getServicePoint(0).getZ() == (int)_pDestProc->getServicePoint( _pDestProc->serviceLocationLength() -1 ).getZ())
				return true;
			else return false;
		}
		else
		{
			if( (int)_pCurrentProc->getServicePoint(0).getZ() == (int)_pDestProc->getServicePoint(0).getZ() )
			{
				return true;
			}
			else return false;
		}

	}


	return false;

}

// build source / dest processor of every conveyor processor( or related with conveyor processor in pax flow )
void ProcessorList::InitProcessorSourceDestProcessors( CPaxFlowConvertor* _pConvertor, CAllPaxTypeFlow* _pAllFlow )
{
	PLACE_METHOD_TRACK_STRING();
	// clear all temp data related to source / dest processors
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		pTemp->ClearAllSourceProcs();
		//pTemp->ClearAllDestProcs();
	}

	// build pax flow in order to analysize source / dest processor
	_pAllFlow->FromOldToDigraphStructure( *_pConvertor );

	int iFlowCount = _pAllFlow->GetSinglePaxTypeFlowCount();
	for(int i=0; i<iFlowCount; ++i )
	{
		const CMobileElemConstraint* pConstrint = _pAllFlow->GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain();
		//		char str[256];
		//		pConstrint->screenPrint( str );
		CSinglePaxTypeFlow operaterFlow( m_pInTerm );
		operaterFlow.SetPaxConstrain( pConstrint );
		if( pConstrint->GetTypeIndex() == 0 )//pax constraint
		{
			_pAllFlow->BuildHierarchyFlow( *_pConvertor, i , operaterFlow );
		}
		else//non pax constrant
		{
			CMobileElemConstraint attachedPaxConstraint = *pConstrint;
			attachedPaxConstraint.SetTypeIndex( 0 );			
			attachedPaxConstraint.SetMobileElementType(enum_MobileElementType_PAX);
			//pax flow				
			CSinglePaxTypeFlow m_paxFlowAttachedToNonPaxFlow( m_pInTerm );			
			m_paxFlowAttachedToNonPaxFlow.SetPaxConstrain( &attachedPaxConstraint );			


			int iIdx = _pAllFlow->GetIdxIfExist( m_paxFlowAttachedToNonPaxFlow );
			if( iIdx < 0 )//not exist
			{
				CSinglePaxTypeFlow* pPaxFlowAttachedToNonPaxFlow = new CSinglePaxTypeFlow ( m_pInTerm );
				pPaxFlowAttachedToNonPaxFlow->SetPaxConstrain( &attachedPaxConstraint );
				//char str[ 64 ];
				//attachedPaxConstraint.screenPrint( str );
				////// TRACE("\n%s\n", str );
				_pAllFlow->AddPaxFlow( pPaxFlowAttachedToNonPaxFlow );
				iIdx = _pAllFlow->GetIdxIfExist( m_paxFlowAttachedToNonPaxFlow );
				ASSERT( iIdx >= 0 );
				_pAllFlow->BuildHierarchyFlow( *_pConvertor, iIdx , m_paxFlowAttachedToNonPaxFlow );
				_pAllFlow->DeleteSinglePaxTypeFlowAt( iIdx );
			}
			else
			{
				_pAllFlow->BuildHierarchyFlow( *_pConvertor, iIdx , m_paxFlowAttachedToNonPaxFlow );
			}


			//non pax flow
			CSinglePaxTypeFlow m_nonPaxFlow ( m_pInTerm  );		
			m_nonPaxFlow.SetPaxConstrain( pConstrint );
			iIdx = _pAllFlow->GetIdxIfExist( m_nonPaxFlow );
			ASSERT( iIdx >= 0 );

			_pAllFlow->BuildHierarchyFlow( *_pConvertor, iIdx , m_nonPaxFlow );
			_pConvertor->BuildAttachedNonPaxFlow( m_paxFlowAttachedToNonPaxFlow, m_nonPaxFlow, operaterFlow );
		}

		CDirectionUtil tools;
		tools.ExpandAllProcessInFlow( &operaterFlow );
		int iFlowPairCount = operaterFlow.GetFlowPairCount();
	//	for( int i=0; i<iFlowPairCount; ++i )
		for (int iFlow = 0; iFlow < iFlowPairCount; ++iFlow)
		{
			//CProcessorDestinationList* pPair = operaterFlow.GetFlowPairAt( i );
			CProcessorDestinationList* pPair = operaterFlow.GetFlowPairAt( iFlow );
			const ProcessorID& procID = pPair->GetProcID();
			GroupIndex groupIdx = getGroupIndexOriginal( procID );
			//// TRACE ("\n%s\n",procID.GetIDString() );
			if( groupIdx.start < 0 )
			{
				continue;
			}
			ASSERT( groupIdx.start >=0 );
			if( getProcessor( groupIdx.start )->getProcessorType() == ConveyorProc )
			{
				// get all conveyor processor in this pair
				std::vector<Processor*>vAllSourceConveyors;
				for( int j=groupIdx.start; j<= groupIdx.end; ++j )
				{
					vAllSourceConveyors.push_back( getProcessor( j ) );
				}

				// add all source processor into dest processor
				//std::vector<Processor*>vAllDestProcs;
				int iDestCount = pPair->GetDestCount();
				for(int j=0; j<iDestCount; ++j )
				{
					const ProcessorID& pDestProc = pPair->GetDestProcArcInfo( j )->GetProcID();
					GroupIndex destGroupIdx = getGroupIndexOriginal( pDestProc );
					ASSERT( destGroupIdx.start >= 0 );
					for( int s=destGroupIdx.start; s<=destGroupIdx.end; ++s )
					{
						Processor* pDestProc = getProcessor( s );
						for( int k=groupIdx.start; k<=groupIdx.end; ++k )
						{
							pDestProc->AddSourceProcs( vAllSourceConveyors );
							//	vAllDestProcs.push_back( pDestProc );
						}
					}
				}

				/*
				// add all dest processors into conveyor processor
				for( j=groupIdx.start; j<= groupIdx.end; ++j )
				{
				Processor* pSourceProc = getProcessor( j );
				pSourceProc->AddDestProcs( vAllDestProcs );
				}
				*/
			}
		}

	//	if (i == 38)
		//{
		//	CString strFile;
		//	strFile.Format(_T("d:\\Flow%d.log"),i);
		//	ofsstream echoFile (strFile, stdios::app);
		//		
		//	for (int j = 0; j < iFlowPairCount; j++)
		//	{
		//		CProcessorDestinationList* pPair = operaterFlow.GetFlowPairAt( j );
		//		const ProcessorID& procID = pPair->GetProcID();

		//		CString sOutStr;
		//		sOutStr = "SourceProcID :";
		//		sOutStr += procID.GetIDString();
		//		echoFile <<sOutStr <<"\n";
		//		CString strDest;
		//		int iDestCount = pPair->GetDestCount();
		//		for (int n = 0; n < iDestCount; n++)
		//		{
		//			const CFlowDestination& flowDest = pPair->GetDestProcAt(n);
		//			strDest = "			DestProceID: ";
		//			strDest += flowDest.GetProcID().GetIDString();
		//		}
		//		echoFile << strDest <<"\n\n";
		//	}
		//	echoFile.close();
		//}
	}


	/* 
	iProcessorCount = getProcessorCount();

	for( i=0 ; i<iProcessorCount; ++i )
	{
	pTemp = getProcessor( i );
	// TRACE (" Processor name:%s", pTemp->getID()->GetIDString() );
	int iSize = pTemp->m_vSourceConveyors.size();
	//// TRACE ("\n");
	for( int j=0; j<iSize; ++j )
	{			
	// TRACE (" %s,  ", pTemp->m_vSourceConveyors[j]->getID()->GetIDString() );
	}
	// TRACE ("\n");
	}
	*/
}

// init processor's resource pool data
void ProcessorList::initResourcePoolData( void )
{
	PLACE_METHOD_TRACK_STRING();
	for( int i=0; i<m_procCount; i++ )
	{
		Processor* _proc = getProcessor( i );
		_proc->initResourcePool();
	}
}

ProcessorID* ProcessorList::GetNextProcessorID(const ProcessorID* _pID )const
{
	char buf[128];
	ProcessorID* pNewID = new ProcessorID(*_pID);
	int lastLevel = pNewID->idLength()-1;
	pNewID->getNameAtLevel(buf, lastLevel);
	int i = atoi(buf);
	do {
		i++;
		_itoa(i,buf,10);
		pNewID->setNameAtLevel(buf,lastLevel);
	} while(findProcessor(*pNewID) != INT_MAX);

	return pNewID;
}
Processor* ProcessorList::CopyProcessor( Processor* _pPrototype , InputTerminal* _pTerm)const
{
	ASSERT( _pPrototype );
	ProcessorID* pNewID = GetNextProcessorID(_pPrototype->getID());

	if( m_pDynamicCreatedProcsByTLOS->getProcessor( pNewID->GetIDString() ) )
	{
		pNewID = m_pDynamicCreatedProcsByTLOS->GetNextProcessorID( pNewID );
	}

	Processor* pProcCopy = NULL;

	switch( _pPrototype->getProcessorType() )
	{
	case PointProc:
		pProcCopy = new Processor;
		break;
	case DepSourceProc:
		pProcCopy = new DependentSource;
		break;
	case DepSinkProc:
		pProcCopy = new DependentSink;
		break;
	case LineProc:
		pProcCopy = new LineProcessor;
		break;
	case BaggageProc:
		pProcCopy = new BaggageProcessor;
		break;
	case HoldAreaProc:
		pProcCopy = new HoldingArea;
		break;
	case GateProc:
		pProcCopy = new GateProcessor;
		break;
	case FloorChangeProc:
		pProcCopy = new FloorChangeProcessor;
		break;
	case MovingSideWalkProc:
		pProcCopy=new MovingSideWalk;
		break;
	case BarrierProc:
		pProcCopy = new Barrier;
		break;
	}

	if( !pProcCopy )
	{
		delete pNewID;
		return NULL;
	}

	*pProcCopy = *_pPrototype;
	//set id
	pProcCopy->init( *pNewID );

	//set Terminal
	pProcCopy->SetTerminal( _pPrototype->GetTerminal() );



	//set layout properties
	int nPSLCount=_pPrototype->serviceLocationLength() ;
	if(nPSLCount> 0)
	{
		Point* pPointSL=new Point[nPSLCount];		
		pProcCopy->initServiceLocation(nPSLCount,_pPrototype->serviceLocationPath()->getPointList());
		delete[] pPointSL;		
	}
	int nPICCount=_pPrototype->inConstraintLength();
	if( nPICCount> 0)
	{
		//Point* pPointIC=new Point[nPICCount];
		//memcpy(pPointIC,_pPrototype->inConstraint()->getPointList(),sizeof(Point)*nPICCount);
		pProcCopy->initInConstraint( nPICCount, _pPrototype->inConstraint()->getPointList());
		//delete[] pPointIC;
	}

	int nPOCCount=_pPrototype->outConstraintLength();
	if( nPOCCount > 0)
	{
		//Point* pPointOC=new Point[nPOCCount];
		//memcpy(pPointOC,_pPrototype->outConstraint()->getPointList(),sizeof(Point)*nPOCCount);
		pProcCopy->initOutConstraint( nPOCCount, _pPrototype->outConstraint()->getPointList());
		//delete[] pPointOC;
	}

	int nPQCount=_pPrototype->queueLength();
	if(nPQCount> 0)
	{
		//Point* pPointQ=new Point[nPQCount];
		//memcpy(pPointQ,_pPrototype->queuePath()->getPointList(),sizeof(Point)*nPQCount);
		pProcCopy->initQueue( _pPrototype->queueIsFixed(), nPQCount,_pPrototype->queuePath()->getPointList());
		//delete[] pPointQ;
	}

	delete pNewID;
	// set logic 

	//pProcCopy->SetDirection( _pPrototype->GetDirection() );
	return pProcCopy;
}
Processor* ProcessorList::DynamicCreateProcByTLOS( const ProcessorID* _pPrototype , InputTerminal* _pTerm)const
{
	if( !m_bCanDynamicCreateProc )
	{// this flag is true when simengine dlg set auto add processor check box
		return NULL;
	}
	ASSERT( _pPrototype );
	GroupIndex gIdx = getGroupIndexOriginal( *_pPrototype );
	ASSERT( gIdx.start >=0 );
	Processor* pProc = NULL;
	int iCount = gIdx.end - gIdx.start;
	if( iCount > 0 )
	{
		pProc = getProcessor( gIdx.start + random( iCount ) );
	}
	else
	{
		pProc = getProcessor( gIdx.start );
	}

	Processor* pDynamicProc =CopyProcessor( pProc, _pTerm);
	if( !pDynamicProc )
	{
		return NULL;
	}

	pDynamicProc->SetTLOSFlag( true );
	pDynamicProc->SetDynamicCreatedFlag( true );
	pDynamicProc->DoOffset( (random( constiIntervalMeter ) + 2 )*SCALE_FACTOR, 0.0 );
	pDynamicProc->setCloseFlag();
	pDynamicProc->GetOccupants()->clear();
	pDynamicProc->getLogEntry().clearLog();
	if( ProcessorQueue* pProcQueue = pDynamicProc->GetProcessorQueue() )
	{
		pProcQueue->ClearAllTempData();
	}
	//pDynamicProc->setIndex( pProcPrototype->getIndex() + 1 );
	//_pTerm->m_pTLOSMonitor->OpenProcessor( pDynamicProc, _currentTime, type );				
	m_pDynamicCreatedProcsByTLOS->addProcessor( pDynamicProc );
	m_pDynamicCreatedProcsByTLOS->setIndexes();
	
	m_pDynamicCreatedProcsPrototypes->addDynamicCreatedPrototypesProcessor( pProc );	
//	m_pDynamicCreatedProcsPrototypes->addProcessor( pProc );
//	m_pDynamicCreatedProcsPrototypes->setIndexes();
	return pDynamicProc;

}
void ProcessorList::InitDynamicCreateProcList( InputTerminal* _pTerm )
{
	PLACE_METHOD_TRACK_STRING();
	if( !m_pDynamicCreatedProcsByTLOS )
	{
		m_pDynamicCreatedProcsByTLOS = new ProcessorList( _pTerm->inStrDict );
		m_pDynamicCreatedProcsByTLOS->SetInputTerminal( _pTerm );
	}

	if( !m_pDynamicCreatedProcsPrototypes )
	{
		m_pDynamicCreatedProcsPrototypes = new ProcessorList( _pTerm->inStrDict );
		m_pDynamicCreatedProcsPrototypes->SetInputTerminal( _pTerm );
	}

	m_pDynamicCreatedProcsByTLOS->procList.Flush( TShouldDelete::NoDelete );
	m_pDynamicCreatedProcsByTLOS->setProcessorCount( 0 );
	m_pDynamicCreatedProcsByTLOS->m_groupIdxTree.ClearTree();

	m_pDynamicCreatedProcsPrototypes->procList.Flush( TShouldDelete::NoDelete );
	m_pDynamicCreatedProcsPrototypes->setProcessorCount( 0 );
	m_pDynamicCreatedProcsPrototypes->m_groupIdxTree.ClearTree();
}
// add all dynamic created processors (TLOS) into current proclist, and update index
void ProcessorList::AddAllDynamicCreatedProcIntoList()
{
	PLACE_METHOD_TRACK_STRING();
	m_mapProcListIdx.clear();
	m_mapDynamicCreatedProcListIdx.clear();
	if( !m_pDynamicCreatedProcsByTLOS )
	{
		return;
	}
	int iDynamicCreatedProcCount = m_pDynamicCreatedProcsByTLOS->getProcessorCount();
	if( iDynamicCreatedProcCount > 0 )
	{
		this->setIndexes();	// to make sure all proc's index is valid now
		for( int i=0; i<m_procCount; i++ )
		{			
			Processor* pProc = getProcessor( i );	
			//// TRACE ("\n%s\n",pProc->getID()->GetIDString() );
			m_mapProcListIdx[ i ] = pProc;
		}

		for(int i=0; i<iDynamicCreatedProcCount; ++i )
		{
			Processor* pProc = m_pDynamicCreatedProcsByTLOS->getProcessor( i );	
			//// TRACE ("\n%s\n",pProc->getID()->GetIDString() );
			m_mapDynamicCreatedProcListIdx[ i ] = pProc;
			addProcessor( pProc	);
		}		
	}	
}
void ProcessorList::ResetAllTLOSFlag()
{
	for( int i=0; i<m_procCount; i++ )
	{
		Processor* pProc = getProcessor( i );
		pProc->SetTLOSFlag( false );
	}
}

// put the barriers list into _list
void ProcessorList::GetBarriersList( ProcessorArray& _list )
{
	for( int i=0; i<m_nBarrierCount; i++ )
		_list.addItem( m_barrierList.getItem( i ) );
}

void ProcessorList::GetBaggateProcessorList(ProcessorArray& _list )
{
	for( int i=0; i< m_baggageProcList.getCount(); i++ )
		_list.addItem( m_baggageProcList.getItem( i ) );
}

void ProcessorList::GetBillboardProcessorList(ProcessorArray& _list )
{
	for( int i=0; i< m_billBoardProcList.getCount(); i++ )
		_list.addItem( m_billBoardProcList.getItem( i ) );
}

// build the group index tree to prepare retrieve processor's group efficiently
void ProcessorList::BuildProcGroupIdxTree()
{
	m_groupIdxTree.ClearTree();
	for( int i=0; i<m_procCount; i++ )
	{
		ProcessorID tempProcID = *(getProcessor( i )->getID());		
		m_groupIdxTree.BuildTree( &tempProcID, getGroupIndexOriginal( tempProcID ) );
		while( tempProcID.getSuperGroup() )
		{
			m_groupIdxTree.BuildTree( &tempProcID, getGroupIndexOriginal( tempProcID ) );
		}		
	}
}

int ProcessorList::FindWorthWaitedProcs(const ProcessorID *groupID, const ElapsedTime& _curTime, const CMobileElemConstraint& paxType, int gateIndex)
{

	CanServeProcList _canServeProcList;

	GroupIndex group = getGroupIndex (*groupID);

	if (group.start == TO_GATE_PROCESS_INDEX && group.start == group.end)
	{
		return 1;
	} 
	if (group.start == FROM_GATE_PROCESS_INDEX && group.start == group.end)
	{
		return 1;
	} 

	if (group.start == BAGGAGE_DEVICE_PROCEOR_INDEX && group.start == group.end)
	{
		return 1;
	}  

	if (group.start == END_PROCESSOR_INDEX && group.start == group.end)
	{
		return 1;
	} 

	Processor *aProc;
	std::vector< Processor* > vVaildTLOSProcList;
	for (int i = group.start; i <= group.end; i++)
	{
		aProc = getProcessor(i);
		CString strProcessor = aProc->getID()->GetIDString();
		// processor status test
		if (aProc->isOpen)
		{
			// ckeck the processor if is vacant( can accept next person )					
			if( aProc->isVacant()==false )
			{
				// gate flow logic test
				if ( aProc->leadsToGate (gateIndex))
				{
					// assignment test
					if (aProc->isOpened() && !aProc->closing() && aProc->canServe (paxType, &_canServeProcList ))
					{

					}

				}
			}
		}
	}


	if( m_pDynamicCreatedProcsByTLOS )
	{
		group = m_pDynamicCreatedProcsByTLOS->getGroupIndex (*groupID);
		if( group.start >= 0 )
		{
			for (int i = group.start; i <= group.end; i++)
			{								
				aProc = m_pDynamicCreatedProcsByTLOS->getProcessor(i);
				CString strProcessor = aProc->getID()->GetIDString();
				if ( aProc->isOpen)
				{
					// ckeck the processor if is vacant( can accept next person )					
					if( aProc->isVacant()==false )
					{
						// gate flow logic test
						if ( aProc->leadsToGate (gateIndex))
						{
							// assignment test
							if (aProc->isOpened() && !aProc->closing() && aProc->canServe (paxType, &_canServeProcList ))
							{
								//array->addItem (aProc);
							}

						}
					}
				}
			}
		}
	}
	ProcessorArray arrayDest;
	_canServeProcList.getProcFromList(&arrayDest);

	return arrayDest.getCount();
}

void ProcessorList::readObsoleteData ( ArctermFile& p_file )
{
	if (p_file.getVersion() <= 2.21)
	{
		p_file.getLine();
		Processor *aProc=NULL;
		m_pServicePoints = new Point[MAX_POINTS];
		m_pQueue = new Point[MAX_POINTS];

		bool bFirst = true;
		int co = 0;
		while (!p_file.isBlank ())
		{
			co ++;
			try
			{
				aProc = readOldProcessorVer22(p_file, bFirst);
			}catch (...) 
			{
				Sleep(0);
			}
			if(aProc==NULL)return;
			bFirst = false;
			if( aProc->getProcessorType() == Elevator )
			{
				((ElevatorProc*)aProc)->InitLayout();
			}
			else if( aProc->getProcessorType() == ConveyorProc )
			{
				((Conveyor*)aProc)->CalculateTheBisectLine();
			}
			else if( aProc->getProcessorType() == StairProc )
			{
				((Stair*)aProc)->CalculateTheBisectLine();
			}
			else if( aProc->getProcessorType() == EscalatorProc )
			{
				((Escalator*)aProc)->CalculateTheBisectLine();
			}

			addProcessor (aProc,false);
		}

		BuildProcGroupIdxTree();
		if( m_bDoOffset )
			DoOffset();

		delete m_pServicePoints;
		delete m_pQueue;
		setIndexes();

	}
	else if (p_file.getVersion() < float(2.7))
	{
		//readData(p_file);
		p_file.getLine();
		Processor *aProc=NULL;
		m_pServicePoints = new Point[MAX_POINTS];
		m_pQueue = new Point[MAX_POINTS];

		bool bFirst = true;
		int co = 0;
		while (!p_file.isBlank ())
		{
			co ++;
			try
			{
				aProc = readProcessor (p_file, bFirst);
			}catch (...) 
			{
				Sleep(0);
			}
			if(aProc==NULL)return;
			bFirst = false;
			if( aProc->getProcessorType() == Elevator )
			{
				((ElevatorProc*)aProc)->InitLayout();
			}
			else if( aProc->getProcessorType() == ConveyorProc )
			{
				((Conveyor*)aProc)->CalculateTheBisectLine();
			}
			else if( aProc->getProcessorType() == StairProc )
			{
				((Stair*)aProc)->CalculateTheBisectLine();
			}
			else if( aProc->getProcessorType() == EscalatorProc )
			{	
				((Escalator*)aProc)->CheckAndRectifyQueueInOutConstraint();
				((Escalator*)aProc)->CalculateTheBisectLine();

			}
			else if (aProc->getProcessorType() == BridgeConnectorProc)
			{
				BridgeConnector* pBridgeConnector = (BridgeConnector*)aProc;
				int nCountPoint = pBridgeConnector->_GetConnectPointCount();
				pBridgeConnector->SetConnectPoint(pBridgeConnector->_GetConnectPointByIdx(0));
				BridgeConvert bridgeConvert;
				bridgeConvert.SetBridgeConnector(pBridgeConnector);
				for (int i = 1; i < nCountPoint; i++)
				{
					BridgeConnector* pNewBridge = new BridgeConnector();
					*pNewBridge = *pBridgeConnector;
					CString strBridge;
					strBridge.Format(_T("%s_%d"),aProc->getID()->GetIDString(),i+1);
					ProcessorID newBridgeID;
					newBridgeID.SetStrDict(m_pInTerm->inStrDict);
					newBridgeID.setID(strBridge);
					pNewBridge->init(newBridgeID);
					pNewBridge->SetConnectPoint(pBridgeConnector->_GetConnectPointByIdx(i));
					bridgeConvert.AddBridgeConvert(pNewBridge);

					addProcessor(pNewBridge,false);
				}
				if (bridgeConvert.GetConvertCount() != 0)
				{
					m_vBridgeConvert.push_back(bridgeConvert);
				}
			}

			addProcessor (aProc,false);
		}

		BuildProcGroupIdxTree();
		if( m_bDoOffset )
			DoOffset();

		delete m_pServicePoints;
		delete m_pQueue;
		setIndexes();
	}


}

Processor *ProcessorList::readOldProcessorVer22(ArctermFile& procFile, bool _bFirst /*=false*/ )
{
	Processor *proc;
	ProcessorID procID;

	assert( m_pInTerm );
	procID.SetStrDict( m_pInTerm->inStrDict );
	if (!procID.readProcessorID (procFile))
		throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");

	char string[SMALL];
	procFile.getField (string, SMALL);


	if (!_stricmp (string, "DPND_SOURCE"))
		proc = new DependentSource;
	else if (!_stricmp (string, "DPND_SINK"))
		proc = new DependentSink;
	else if (!_stricmp (string, "GATE"))
		proc = new GateProcessor;
	else if (!_stricmp (string, "LINE"))
		proc = new LineProcessor;
	else if (!_stricmp (string, "BAGGAGE"))
		proc = new BaggageProcessor;
	else if (!_stricmp (string, "HOLD_AREA"))
		proc = new HoldingArea;
	else if (!_stricmp (string, "BARRIER"))
		proc = new Barrier;
	else if (!_stricmp (string, "FLOOR")) 
		proc = new FloorChangeProcessor;
	else if (!_stricmp(string,"INTEGRATED_STATION"))
		proc = new IntegratedStation ;
	else if (!_stricmp(string,"MOVING_SIDE_WALK"))
		proc = new MovingSideWalk();
	else if(!_stricmp(string,"Elevator"))
		proc =new ElevatorProc;
	else if(!_stricmp(string,"Conveyor"))
		proc =new Conveyor;
	else if(!_stricmp(string,"Stair"))
		proc =new Stair;
	else if(!_stricmp(string,"Escalator"))
		proc =new Escalator;
	else if(!_stricmp(string,"ARP"))
		proc = new ArpProc();
	else if(!_stricmp(string,"RUNWAY"))
		proc = new RunwayProc();
	else if(!_stricmp(string,"TAXIWAY"))
		proc = new TaxiwayProc();
	else if(!_stricmp(string,"STAND"))
		proc = new StandProc();
	else if(!_stricmp(string,"NODE"))
		proc = new NodeProc();
	else if(!_stricmp(string,"DEICE_STATION"))
		proc = new DeiceBayProc();
	else if(!_stricmp(string,"FIX"))
		proc = new FixProc();
	else if (!_stricmp(string,"CONTOUR"))
		proc = new ContourProc;
	else if(!_stricmp(string ,"APRON"))
		proc= new ApronProc;	
	else
	{		
		proc = new Processor;
	}

	proc->init (procID);
	proc->SetTerminal((Terminal *)m_pInTerm); // add by tutu 2002-9-26

	Point aPoint;
	Point inConst[100], outConst[100];
	int svcCount = 0, inCount = 0, queueCount = 0, outCount = 0;

	if (!procFile.getPoint (aPoint))
		throw new StringError ("All processors must have a service location");


	if( _bFirst )
	{
		m_bDoOffset = false;
		static DistanceUnit THRESHOLD_FOR_DOOFFSET = 30000000;
		DistanceUnit x = aPoint.getX();
		DistanceUnit y = aPoint.getY();
		if( x  > THRESHOLD_FOR_DOOFFSET || x < -THRESHOLD_FOR_DOOFFSET
			|| y > THRESHOLD_FOR_DOOFFSET || y < -THRESHOLD_FOR_DOOFFSET )
		{
			m_bDoOffset = true;			
			m_minX = x;
			m_maxX = x;
			m_minY = y;
			m_maxY = y;			
		}
	}

	if( m_bDoOffset )
	{
		DistanceUnit x = aPoint.getX();
		if( x < m_minX )
			m_minX = x;
		else if( x > m_maxX )
			m_maxX = x;

		DistanceUnit y = aPoint.getY();
		if( y < m_minY )
			m_minY = y;
		else if( y > m_maxY )
			m_maxY = y;		
	}

	m_pServicePoints[svcCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		inConst[inCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		m_pQueue[queueCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		outConst[outCount++] = aPoint;

	char fixedQueue = 'N';
	if (queueCount)
		procFile.getChar (fixedQueue);


	proc->readSpecialField( procFile );
	// make sure point limits are not exceeded
	for (int i = 1; procFile.getLine() && !procFile.isNewEntryLine(); i++)
	{
		if (i + 1 >= MAX_POINTS)
		{
			procID.printID (string);
			throw new TwoStringError ("maximum point count exceeded by ", string);
		}

		// skip ID and Processor Type fields
		procFile.skipField (2);

		if (procFile.getPoint (aPoint))
			m_pServicePoints[svcCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			inConst[inCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			m_pQueue[queueCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			outConst[outCount++] = aPoint;
	}


	char c[256];
	proc->getID()->printID( c );

	proc->initInConstraint (inCount, inConst);
	proc->initServiceLocation (svcCount, m_pServicePoints);
	proc->initOutConstraint (outCount, outConst);
	proc->initQueue (fixedQueue == 'Y', queueCount, m_pQueue);

	///proc->readSpecialProcessor(procFile);
	return proc;
}

// Get all processors by the type
// all results will stored in vProcs
// In: procType
// Out: vProcs
void ProcessorList::GetProcessorsByType(std::vector<BaseProcessor*>& vProcs, int ProcType)
{
	vProcs.clear();	
	Processor* pProc = NULL;

	for (int i = 0; i < m_procCount; i++)
	{
		pProc = procList.getItem(i);
		if (pProc->getProcessorType() == ProcType)
			vProcs.push_back((BaseProcessor*)pProc);
	}
}
//clear the dynamic created flag of the process which created by auto create process 
void ProcessorList::ClearAllProcDynamicFlag()
{
	Processor* pProc = NULL;
	for (int i = 0; i < m_procCount; i++)
	{
		pProc = procList.getItem(i);
		pProc->SetDynamicCreatedFlag(false);
	}
}

void ProcessorList::FlushBridgeLog(const ElapsedTime& t)
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == BridgeConnectorProc )
		{	
			((BridgeConnector*)pTemp)->FlushLog(t);		
		}
	}
}

void ProcessorList::FlushRetailProcess()
{
	PLACE_METHOD_TRACK_STRING();
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );
		if( pTemp->getProcessorType() == RetailProc )
		{	
			((RetailProcessor*)pTemp)->FlushLog();		
		}
	}
}

void ProcessorList::ClearStationMovingTrain()
{
	int iProcessorCount = getProcessorCount();
	Processor* pTemp=NULL;
	for( int i=0 ; i<iProcessorCount; ++i )
	{
		pTemp = getProcessor( i );

		if (pTemp->getProcessorType() == IntegratedStationProc)
		{
			((IntegratedStation*)pTemp)->ClearStationTrain(NULL);
		}
	}
}

void ProcessorList::ClearGateAndReclaimData()
{
	for (int i = 0; i < getProcessorCount(); i++)
	{
		Processor* aProc = getProcessor(i);
		assert( aProc );

		aProc->clearGateAndReclaimFlowPair();
	}
}

bool ProcessorList::CanLeadToIndividualConvergerObject( Processor *pProc,Person* pPerson, CARCportEngine* pEngine,ElapsedTime _currentTime, CString& strErrorInfo ) const
{
	LandsideSimulation* pLandsideSimulation = pEngine->GetLandsideSimulation();
	if(pLandsideSimulation == NULL)
		return true;

	CString strProc = pProc->getIDName();


	std::vector<LandsideObjectLinkageItemInSim> vAllObject;
	////////////////////////////////////////////////////from   only for vehicle parking at parking lot
	if( LandsideVehicleInSim* pVehicle = pLandsideSimulation->FindPaxVehicle(pPerson->getID()) )
	{
		if(LandsideResourceInSim* pResource = pVehicle->getLastState().getLandsideRes())
		{
			LandsideParkingSpotInSim* pParkSpot = pResource->toParkLotSpot();
			if(pParkSpot && !pParkSpot->getParkingLot()->IsCellPhone() )
			{
				std::vector<int> vLevel;
				vLevel.push_back(pParkSpot->GetParkingLotLevelID());
				vAllObject.push_back(LandsideObjectLinkageItemInSim(pParkSpot->getParkingLot()->getName(), vLevel));
			}
		}
	}
	///////////////////////////////////////////////////////to temporary code
	if(vAllObject.empty())
	//check cell phone
	{
		LandsideIndividualArrivalPaxVehicleConvergerManager* pLandsideConverge = pLandsideSimulation->GetLandsideConvergerManager();
		ALTObjectID altObj;
		LandsideObjectLinkageItemInSim objLink(altObj);
		if (pLandsideConverge->GetLinkageObject(pPerson->getID(),objLink))
		{
			vAllObject.push_back(objLink);
		}
	}

	if(pProc->GetLandsideDrection().CanLeadTo(pPerson->getType(), vAllObject, pEngine->getTerminal()))
		return true;
	else
	{
		int nCount = static_cast<int>(vAllObject.size());
		for ( int nItem = 0; nItem < nCount; ++ nItem)
		{
			strErrorInfo += vAllObject[nItem].getObject().GetIDString();

			int nLevelCount = vAllObject[nItem].getLevels().size();
			for (int nLevel = 0; nLevel < nLevelCount; ++ nLevel)
			{
				CString strLevel;
				strLevel.Format(_T(",%d"), vAllObject[nItem].getLevels().at(nLevel));
				strLevel += strLevel;
			}
			strErrorInfo += _T("; ");
		}
	}

	return false;
}

bool ProcessorList::CanLeadToLandsideObject( Processor *pProc,Person* pPerson, CARCportEngine* pEngine,ElapsedTime _currentTime, CString& strErrorInfo ) const
{
	//return true;

	if(!pEngine->IsLandSel() || pPerson == NULL)
		return true;

	if(!pPerson->getType().isArrival())
		return true;

	std::vector<LandsideObjectLinkageItemInSim> vAllObject;
	//individual
	LandsideSimulation* pLandsideSimulation = pEngine->GetLandsideSimulation();
	if(pLandsideSimulation == NULL)
		return true;

	ALTObjectID altObj;
	LandsideObjectLinkageItemInSim linkage(altObj);
	if (pLandsideSimulation->GetLandsideConvergerManager()->GetLinkageObject(pPerson->getID(),linkage))
	{
		if (!CanLeadToIndividualConvergerObject(pProc,pPerson,pEngine,_currentTime,strErrorInfo))
			return false;
		return true;
	}
	else
	{
		LandsideVehicleInSim* pVehicle = NULL;
		if(pLandsideSimulation)
		{
			pVehicle = pLandsideSimulation->FindPaxVehicle(pPerson->getID());
		}

		if(pVehicle)
		{
			LandsideResourceInSim* pResource = pVehicle->getLastState().getLandsideRes();
			if(pResource)
			{
				LandsideParkingSpotInSim* pParkSpot = pResource->toParkLotSpot();
				if(pParkSpot)
				{
					std::vector<int> vLevel;
					vLevel.push_back(pParkSpot->GetParkingLotLevelID());

					vAllObject.push_back(LandsideObjectLinkageItemInSim(pParkSpot->getParkingLot()->getLotInput()->getName(), vLevel));
				}
			}
		}
		else
		{


			LandsidePaxTypeLinkageInSimManager* pPaxTypeLinkageManager = pLandsideSimulation->GetLandsidePaxtTypeLinkageManager();
			if(pPaxTypeLinkageManager == NULL)
				return true;

			LandsidePaxTypeLinkageInSim*  pPaxTypeLinkageItem = pPaxTypeLinkageManager->getBestFitItem(pPerson->getType());
			if(pPaxTypeLinkageItem == NULL)
				return true;

			pPaxTypeLinkageItem->GetAllObjectCanLeadTo(_currentTime, vAllObject);

		}



		if(pProc->GetLandsideDrection().CanLeadTo(pPerson->getType(), vAllObject, pEngine->getTerminal()))
			return true;
		else
		{
			int nCount = static_cast<int>(vAllObject.size());
			for ( int nItem = 0; nItem < nCount; ++ nItem)
			{
				strErrorInfo += vAllObject[nItem].getObject().GetIDString();

				int nLevelCount = vAllObject[nItem].getLevels().size();
				for (int nLevel = 0; nLevel < nLevelCount; ++ nLevel)
				{
					CString strLevel;
					strLevel.Format(_T(",%d"), vAllObject[nItem].getLevels().at(nLevel));
					strLevel += strLevel;
				}
				strErrorInfo += _T("; ");
			}
		}

		return false;
	}
		
	
}

bool ProcessorList::CanLeadToTerminalProcessor(const Processor *curpProc, Processor *pNextProc,std::vector<ALTObjectID> vAltLandsideSelectedProc, CARCportEngine* pEngine )const
{
	ASSERT(pNextProc);
	if(pNextProc == NULL || curpProc == NULL)
		return true;
	if(curpProc->getIndex() != START_PROCESSOR_INDEX)
		return true;

	if(pEngine->IsLandSel() 
		&& vAltLandsideSelectedProc.size())
	{	
		int nAvailableCount = static_cast<int>(vAltLandsideSelectedProc.size());
		for (int nAva = 0; nAva < nAvailableCount; ++ nAva)
		{
			ProcessorID procLandsideConnected;
			procLandsideConnected.SetStrDict(pEngine->getTerminal()->inStrDict);
			procLandsideConnected.setID(vAltLandsideSelectedProc.at(nAva).GetIDString());;

			if(procLandsideConnected.idFits(*(pNextProc->getID())))
			{
				return true;
			}
		}
		//no fit
		return false;
	}

	//default 
	return true;
}



