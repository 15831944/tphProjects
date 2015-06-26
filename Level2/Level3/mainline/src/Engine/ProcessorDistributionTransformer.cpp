#include "StdAfx.h"
#include ".\processordistributiontransformer.h"
#include "inputs\SubFlowList.h"
#include "Inputs\Paxflow.h"
#include "Inputs\SubFlow.h"

#include "Inputs\ProcessorDestinationList.h"
#include "Inputs\SingleProcessFlow.h"
#include "Inputs\MobileElemConstraint.h"
#include "Inputs\Con_db.h"

#define TRACE_FLOW_TRANSFORMER
//////////////////////////////////////////////////////////////////////////
//
CFlowItemTransformer::CFlowItemTransformer()
{
	m_nPercentage = 0;
	m_procID.init();
}

CFlowItemTransformer::~CFlowItemTransformer()
{

}

ProcessorID CFlowItemTransformer::GetProcID() const
{
	return m_procID;
}

int CFlowItemTransformer::GetProbality() const
{
	return m_nPercentage;
}

void CFlowItemTransformer::SetProcID( const ProcessorID& procID )
{
	m_procID = procID;
}

void CFlowItemTransformer::SetProbality( int nProb )
{
	m_nPercentage = nProb;
}

//////////////////////////////////////////////////////////////////////////
//CProcessorDistributionTransformer
//
CProcessorDistributionTransformer::CProcessorDistributionTransformer( const CProcessorDistributionWithPipe& _otherDistribution )
:CProcessorDistributionWithPipe(_otherDistribution)
{
	//int nDestCount =  _otherDistribution.getCount();
	////convert all the data to structure CFlowItemExWithNameProb
	////CFlowItemEx + destination + prob = CFlowItemExWithNameProb
	//for (int nDest = 0; nDest < nDestCount; ++ nDest)
	//{
	//	CFlowItemTransformer newFlowItem;

	//	ProcessorID *pProcID = _otherDistribution.getGroup(nDest);
	//	newFlowItem.SetProcID(*pProcID);
	//	newFlowItem.SetProbality(_otherDistribution.getProb(nDest));
	//	newFlowItem.SetFlowItemEx(_otherDistribution.GetPipe(nDest));

	//	m_vFlowItem.push_back(newFlowItem);
	//}
}

CProcessorDistributionTransformer::CProcessorDistributionTransformer()
:CProcessorDistributionWithPipe( false )
{

}

CProcessorDistributionTransformer::CProcessorDistributionTransformer( CProcessorDestinationList* pSubFlowDesinationList )
:CProcessorDistributionWithPipe( false )
{
	int nDestCount = pSubFlowDesinationList->GetDestCount();
	for(int nDest = 0; nDest <nDestCount; ++nDest)
	{
		const CFlowDestination& flowDest = pSubFlowDesinationList->GetDestProcAt(nDest);

		CFlowItemTransformer tempFlowItem;
		tempFlowItem.SetProcID(flowDest.GetProcID());
		//split the probability
		tempFlowItem.SetProbality(flowDest.GetProbality());

		CFlowItemEx flowItemEx;
		GetFlowItemEx(flowDest,flowItemEx);

		//end
		tempFlowItem.SetFlowItemEx(flowItemEx);

		m_vFlowItem.push_back(tempFlowItem);
	}

}
CProcessorDistributionTransformer::~CProcessorDistributionTransformer()
{

}

void CProcessorDistributionTransformer::UntieSubFlows( PassengerFlowDatabase* database,const CSubFlowList& pSubFlowList ,const CMobileElemConstraint* pConstraint)
{


	int nDestCount =  getCount();
	//convert all the data to structure CFlowItemExWithNameProb
	//CFlowItemEx + destination + prob = CFlowItemExWithNameProb

	for (int nDest = 0; nDest < nDestCount; ++ nDest)
	{
		ProcessorID *pProcID = getGroup(nDest);

		ASSERT(pProcID != NULL);
		if(pProcID == NULL)
			continue;
		
		//check if sub flow or processor
		CString strProcName = pProcID->GetIDString();
		CSubFlow *pSubFlow = pSubFlowList.GetProcessUnit( strProcName);
		
		CFlowItemTransformer tempFlowItem;
		tempFlowItem.SetProcID(*pProcID);
		tempFlowItem.SetProbality((int)getNoAccumProb(nDest));
		tempFlowItem.SetFlowItemEx(GetPipe(nDest));
		
		if(pSubFlow == NULL)
		{
			m_vFlowItem.push_back(tempFlowItem);
			continue;
		}


		//untie sub flow
		UntieThisSubFlow(tempFlowItem,pSubFlow,database,pConstraint);

		//std::vector<ProcessorDataElement*> vEntryData;

		//database.getEntryPoint(pProcID, vEntryData);

		//Processor *pProcessor = NULL;
		//pProcessor->InitTransition( &m_pTransition, vEntryData,database,pSubFlowList);
	}

	//int nDestCount = static_cast<int>(m_vFlowItem.size());
	//for (int nDest = 0; nDest < nDestCount; ++ nDest)
	//{
//
	//}
}

void CProcessorDistributionTransformer::UntieThisSubFlow(const CFlowItemTransformer& subFlowItem, CSubFlow *pSubFlow ,PassengerFlowDatabase* database,const CMobileElemConstraint* pConstraint)
{
	if(pSubFlow == NULL)
		return;

	CSingleProcessFlow* pPaxFlow = (CSingleProcessFlow*)(pSubFlow->GetInternalFlow());
	if(pPaxFlow == NULL)
		return;

	{
		CProcessorDestinationList* pDesinationList =pPaxFlow->GetStartPair();
		if(pDesinationList == NULL)
			return;

		int nDestCount = pDesinationList->GetDestCount();
		for(int nDest = 0; nDest <nDestCount; ++nDest)
		{
			const CFlowDestination& flowDest = pDesinationList->GetDestProcAt(nDest);

			CFlowItemTransformer tempFlowItem;
			tempFlowItem.SetProcID(flowDest.GetProcID());
			//split the probability
			tempFlowItem.SetProbality( (int)(subFlowItem.GetProbality() * flowDest.GetProbality() *0.01));

			CFlowItemEx flowItemEx;
			GetFlowItemEx(flowDest,flowItemEx);

			flowItemEx.SetMaxSkipTime(subFlowItem.GetFlowItem().GetMaxSkipTime());

			//end
			tempFlowItem.SetFlowItemEx(flowItemEx);

			m_vFlowItem.push_back(tempFlowItem);

		}
	}

	//check the process has been expand or not
	CString strProcessName = subFlowItem.GetProcID().GetIDString();
	if(((CPassengerFlowDatabaseTransformer *)database)->HasBeenExpand(*pConstraint,strProcessName))
		return;


	int nFlowPairCount = pPaxFlow->GetFlowPairCount();
	for (int nPair = 0; nPair < nFlowPairCount; ++ nPair)
	{

		CProcessorDestinationList* pDesinationList =pPaxFlow->GetFlowPairAt(nPair);
		if(pDesinationList == NULL)
			continue;

	
		//leaf node does not need
		CProcessorDestinationList* checkLeaf =  pPaxFlow->GetEqualFlowPairAt(pDesinationList->GetProcID());
		if(checkLeaf == NULL || checkLeaf->GetDestProcCount() == 0)
			continue;



		//get new entry, in the new object
		ProcessorEntry* pNewProcEntry = NULL;
		pNewProcEntry = (ProcessorEntry*)database->FindEntry(pDesinationList->GetProcID());
		if(pNewProcEntry == NULL)
		{
			pNewProcEntry = new ProcessorEntry(pDesinationList->GetProcID(),"");
			database->addItem(pNewProcEntry);
		}
		ConstraintEntry* pNewConstraintEntry = new ConstraintEntry;
		CProcessorDistributionTransformer* pProcDistribution=new CProcessorDistributionTransformer(pDesinationList);
		pNewConstraintEntry->initialize( new CMobileElemConstraint( *pConstraint), pProcDistribution);
		//pNewProcEntry->getDatabase()->addEntry( pNewConstraintEntry,false);
		
		((CPassengerFlowDatabaseTransformer *)database)->AddConstraintEntryToProcDatabase(pNewProcEntry->getDatabase(),pNewConstraintEntry);


	}




}

void CProcessorDistributionTransformer::GetFlowItemEx( const CFlowDestination& flowDest,CFlowItemEx& flowItemEx  )
{
	//start
	//flow condition
	flowItemEx.SetMaxQueueLength(flowDest.GetMaxQueueLength());
	flowItemEx.SetMaxWaitMins(flowDest.GetMaxWaitMins());
	flowItemEx.SetMaxSkipTime(flowDest.GetMaxSkipTime());

	//channel condition
	flowItemEx.SetOneToOneFlag(flowDest.GetOneToOneFlag());

	
	//route condition
	flowItemEx.SetTypeOfUsingPipe(flowDest.GetTypeOfUsingPipe());
		
	int nPipeCount = flowDest.GetUsedPipeCount();
	for(int nPipe = 0; nPipe < nPipeCount; ++nPipe)
	{
		flowItemEx.AddPipeIdx(flowDest.GetPipeAt(nPipe));
	}

	flowItemEx.SetAvoidFixQueue(flowDest.GetAvoidFixQueue());

	flowItemEx.SetAvoidOverFlowQueue(flowDest.GetAvoidOverFlowQueue());

	flowItemEx.SetAvoidDensity(flowDest.GetAvoidDensity());
	flowItemEx.SetDensityOfArea(flowDest.GetDensityOfArea());
	flowItemEx.SetDesityArea(flowDest.GetDensityArea());
	
	//1X1
	flowItemEx.SetChannelState(flowDest.GetOneXOneState());
	flowItemEx.SetFollowState(flowDest.GetFollowState());	
}



void CProcessorDistributionTransformer::InitializeDestinations()
{
		int nDestCount = static_cast<int>(m_vFlowItem.size());
		if(nDestCount <= 0)
			return;
	
		ProcessorID* pDestList = new ProcessorID[ nDestCount ];
		double* pProbabilities = new double[ nDestCount ];
	
		m_vPipeIndex.clear();
	
		double nTotalProb = 0;
		for (int nDest = 0; nDest < nDestCount; ++nDest)
		{
			CFlowItemTransformer& flowItem = m_vFlowItem[nDest];
			
			pDestList[ nDest] = flowItem.GetProcID();
			pProbabilities[ nDest ] = flowItem.GetProbality();

			nTotalProb += pProbabilities[ nDest ];

			m_vPipeIndex.push_back(flowItem.GetFlowItem());

		}
		//convert to 100 prob
		if (nTotalProb > 0 && nDestCount > 1)
		{
			int n100ProbUsed = 0;
			for (int nProb = 0; nProb < nDestCount - 1; ++nProb)
			{
				int nTempProb = static_cast<int>(pProbabilities[ nProb ] * 100/nTotalProb);
				pProbabilities[ nProb ] = nTempProb;
				n100ProbUsed += nTempProb;
			}
			pProbabilities[ nDestCount - 1 ] = 100 - n100ProbUsed;


		}
		#ifdef TRACE_FLOW_TRANSFORMER	

		ofsstream echoFile (_T("c:\\flowTransfer.tmp"), stdios::app);
		for (int nTrace = 0; nTrace < nDestCount; ++nTrace)
		{
			CFlowItemTransformer& flowItem = m_vFlowItem[nTrace];
			//TRACE("\r\n Dest ProcID: %s , Prob: %d", flowItem.GetProcID().GetIDString(), pProbabilities[ nTrace ]);
			echoFile	
				<< "        DEST PROCID:   "<< flowItem.GetProcID().GetIDString()
				<<"   PROB, "<<(int)pProbabilities[ nTrace ]<<"\r\n";
		}
		echoFile<<"\r\n";
		echoFile.close();

		#endif

		initDestinations( pDestList, nDestCount,pProbabilities, nDestCount );

		delete[] pDestList;
		delete[] pProbabilities;
		//InitRules();
	
}

void CProcessorDistributionTransformer::MergeFlowItem( std::vector<CFlowItemTransformer>& vPassFlowItem )
{
	int nPassCount = static_cast<int>(vPassFlowItem.size());
	for (int nPass = 0; nPass < nPassCount; ++nPass)
	{
		CFlowItemTransformer& passFlowItem = vPassFlowItem[nPass];

		bool bFind = false;
		int nItemCount = static_cast<int>(m_vFlowItem.size());
		for (int nItem = 0; nItem < nItemCount; ++ nItem)
		{
			if(m_vFlowItem[nItem].GetProcID() == passFlowItem.GetProcID())
			{
				bFind = true;
				break;
			}
		}
		
		if(bFind == false)
		{
			m_vFlowItem.push_back(passFlowItem);
		}
	}
}





//////////////////////////////////////////////////////////////////////////
//CPassengerFlowDatabaseTransformer
CPassengerFlowDatabaseTransformer::CPassengerFlowDatabaseTransformer()
:PassengerFlowDatabase(0)//0 is not useful,has no means, just for compile
{

}

CPassengerFlowDatabaseTransformer::~CPassengerFlowDatabaseTransformer()
{

}

void CPassengerFlowDatabaseTransformer::loadDataSet( const CString& _pProjPath )
{
	ASSERT(0);
}

void CPassengerFlowDatabaseTransformer::saveDataSet( const CString& _pProjPath, bool _bUndo ) const
{
	ASSERT(0);
}


void CPassengerFlowDatabaseTransformer::readData( ArctermFile& p_file )
{
	ASSERT(0);
}

void CPassengerFlowDatabaseTransformer::readObsoleteData( ArctermFile& p_file )
{
	ASSERT(0);
}

void CPassengerFlowDatabaseTransformer::writeData( ArctermFile& p_file ) const
{
	ASSERT(0);
}

const char * CPassengerFlowDatabaseTransformer::getTitle( void ) const
{
	ASSERT(0);
	return NULL;
}

const char * CPassengerFlowDatabaseTransformer::getHeaders( void ) const
{
	ASSERT(0);
	return NULL;
}



void CPassengerFlowDatabaseTransformer::ExpandPaxFlow( const PassengerFlowDatabase *pPaxFlowOriginal,const CSubFlowList& pSubFlowList )
{
	#ifdef TRACE_FLOW_TRANSFORMER	

		ofsstream echoFile (_T("c:\\flowTransfer.tmp"), stdios::out);
		echoFile<<"\r\n";
		echoFile.close();

	#endif

	int nPaxFlowPairCount = pPaxFlowOriginal->getCount();

	for (int nPair = 0; nPair < nPaxFlowPairCount; ++ nPair)
	{
		ProcessorDataElement *pProcDataOrg = pPaxFlowOriginal->getItem(nPair);
		ProcessorEntry *pProcEntryOrg = (ProcessorEntry *)pProcDataOrg;

		if(pProcEntryOrg == NULL)
			continue;

		//processor or sub flow name
		const ProcessorID *pProcID = pProcEntryOrg->getID();
		ASSERT(pProcID != NULL);
		if(pProcID == NULL)
			continue;

		//check if sub flow or processor
		CString strProcName = pProcID->GetIDString();
		CSubFlow *pSubFlow = pSubFlowList.GetProcessUnit( strProcName);
		if(pSubFlow == NULL)//processor
		{	
			CMobileElemConstraintDatabase *pConstraintDBOrg = pProcEntryOrg->getDatabase();
			if(pConstraintDBOrg == NULL)
				continue;


			//get new entry, in the new object
			ProcessorEntry* pNewProcEntry = NULL;
			pNewProcEntry = (ProcessorEntry*)FindEntry(*(pProcEntryOrg->getID()));
			if(pNewProcEntry == NULL)
			{
				pNewProcEntry = new ProcessorEntry(*(pProcEntryOrg->getID()),pConstraintDBOrg->getUnits());
				addItem(pNewProcEntry);
			}

			//
			int nConstraintCount = pConstraintDBOrg->getCount();
			for (int nCons = 0; nCons < nConstraintCount; ++nCons)
			{
				ConstraintEntry *pConstraintEntry = pConstraintDBOrg->getItem(nCons);
				if(pConstraintEntry == NULL)
					continue;
				
				ConstraintEntry* pNewConstraintEntry = new ConstraintEntry;
				
		
				CProcessorDistributionTransformer* pProcDistribution=new CProcessorDistributionTransformer(*(const CProcessorDistributionWithPipe*)pConstraintEntry->getValue());

				pProcDistribution->UntieSubFlows(this,pSubFlowList,(const CMobileElemConstraint*)pConstraintEntry->getConstraint());

				pNewConstraintEntry->initialize( new CMobileElemConstraint( *(const CMobileElemConstraint*)(pConstraintEntry->getConstraint()) ), pProcDistribution);
				
				//pNewProcEntry->getDatabase()->addEntry( pNewConstraintEntry,false);
				AddConstraintEntryToProcDatabase(pNewProcEntry->getDatabase(),pNewConstraintEntry);
			}
			
		}
		else//sub flow
		{
			std::vector<ProcessorID> _leafIDVector;
			if(pSubFlow->GetInternalFlow())
				pSubFlow->GetInternalFlow()->GetLeafNodeEx(_leafIDVector);

			int nProcCount = (int)_leafIDVector.size();
			for (int nProc = 0; nProc < nProcCount; ++ nProc)
			{
				ProcessorID procID = _leafIDVector.at(nProc);

				CMobileElemConstraintDatabase *pConstraintDBOrg = pProcEntryOrg->getDatabase();
				if(pConstraintDBOrg == NULL)
					continue;


				//get new entry, in the new object
				ProcessorEntry* pNewProcEntry = NULL;
				pNewProcEntry = (ProcessorEntry*)FindEntry(procID);
				if(pNewProcEntry == NULL)
				{
					pNewProcEntry = new ProcessorEntry(procID,pConstraintDBOrg->getUnits());
					addItem(pNewProcEntry);
				}
				//
				int nConstraintCount = pConstraintDBOrg->getCount();
				for (int nCons = 0; nCons < nConstraintCount; ++nCons)
				{
					ConstraintEntry *pConstraintEntry = pConstraintDBOrg->getItem(nCons);
					if(pConstraintEntry == NULL)
						continue;
					
					ConstraintEntry* pNewConstraintEntry = new ConstraintEntry;
					
			
					CProcessorDistributionTransformer* pProcDistribution=new CProcessorDistributionTransformer(*(const CProcessorDistributionWithPipe*)pConstraintEntry->getValue());

					pProcDistribution->UntieSubFlows(this,pSubFlowList,(const CMobileElemConstraint*)pConstraintEntry->getConstraint());

					pNewConstraintEntry->initialize( new CMobileElemConstraint( *(const CMobileElemConstraint*)(pConstraintEntry->getConstraint()) ), pProcDistribution);
					
					//pNewProcEntry->getDatabase()->addEntry( pNewConstraintEntry,false);
					AddConstraintEntryToProcDatabase(pNewProcEntry->getDatabase(),pNewConstraintEntry);
				}
				

			}
		}
	}
	Initialize();
}

void CPassengerFlowDatabaseTransformer::Initialize()
{
	int nPaxFlowPairCount = getCount();

	for (int nPair = 0; nPair < nPaxFlowPairCount; ++ nPair)
	{
		ProcessorDataElement *pProcData = getItem(nPair);
		ProcessorEntry *pProcEntry = (ProcessorEntry *)pProcData;

		if(pProcEntry == NULL)
			continue;

		CMobileElemConstraintDatabase *pConstraintDB= pProcEntry->getDatabase();
		if(pConstraintDB == NULL)
			continue;


		#ifdef TRACE_FLOW_TRANSFORMER	
		ofsstream echoFile (_T("c:\\flowTransfer.tmp"), stdios::app);
		echoFile<<"\r\n" << "SOURCE PROCID:    "		<< pProcEntry->getID()->GetIDString()<<"\r\n";
		echoFile.close();
		#endif

		//TRACE("\r\n");
		//TRACE("\r\nProcID: %s", pProcEntry->getID()->GetIDString());
		int nConstraintCount = pConstraintDB->getCount();
		for (int nCons = 0; nCons < nConstraintCount; ++nCons)
		{

			ConstraintEntry *pConstraintEntry = pConstraintDB->getItem(nCons);
			if(pConstraintEntry == NULL)
				continue;

			//TRACE("\r\n PaxType: %s",strPaxType);
			#ifdef TRACE_FLOW_TRANSFORMER	
			CString strPaxType;
			((CMobileElemConstraint *)pConstraintEntry->getConstraint())->screenPrint(strPaxType);
			ofsstream echoFile (_T("c:\\flowTransfer.tmp"), stdios::app);
			echoFile<< "  PaxType:    "		<< strPaxType<<"\r\n";
			echoFile.close();
			#endif
			CProcessorDistributionTransformer* pProcDistribution = (CProcessorDistributionTransformer*)pConstraintEntry->getValue();

			pProcDistribution->InitializeDestinations();

		}
	}
}

void CPassengerFlowDatabaseTransformer::AddConstraintEntryToProcDatabase( CMobileElemConstraintDatabase* pMobElemDatabase, ConstraintEntry* pNewConstraintEntry )
{
	if(pMobElemDatabase == NULL || pNewConstraintEntry == NULL)
		return;

	int nItem = pMobElemDatabase->findItem(pNewConstraintEntry);
	if(nItem != INT_MAX)//the item has been existed
	{
		//merge the 2 constraint entry together
		ConstraintEntry* pExistEntry = pMobElemDatabase->getItem(nItem);
		if(pExistEntry)
		{
			ProbabilityDistribution *pExistProbDist = pExistEntry->getValue();
			CProcessorDistributionTransformer *pExistTransfer = (CProcessorDistributionTransformer *)pExistProbDist;
			
			CProcessorDistributionTransformer *pPassTransfer = (CProcessorDistributionTransformer *)pNewConstraintEntry->getValue();

			pExistTransfer->MergeFlowItem(pPassTransfer->m_vFlowItem);

		}
	}
	else
	{
		pMobElemDatabase->addEntry(pNewConstraintEntry,false);
	}
}

bool CPassengerFlowDatabaseTransformer::HasBeenExpand( const CMobileElemConstraint& mobConstraint,const CString& strProcessID )
{

	std::vector<pairMobConsSubFlowExpand >::iterator iterMap = m_mapConstraintProcess.begin();
	for (; iterMap != m_mapConstraintProcess.end(); ++iterMap)
	{
		if((*iterMap).m_mobConstraint == mobConstraint)
		{
			std::vector<CString>& vProcess = (*iterMap).m_vSubFlow;
			if (std::find(vProcess.begin(),vProcess.end(),strProcessID) == vProcess.end())
			{
				vProcess.push_back(strProcessID);
				return false;
			}
			else
				return true;
		}
	}
	std::vector<CString> vSubFlow;
	vSubFlow.push_back(strProcessID);
	pairMobConsSubFlowExpand pairMobSubFlow;
	pairMobSubFlow.m_mobConstraint = mobConstraint;
	pairMobSubFlow.m_vSubFlow = vSubFlow;
	m_mapConstraintProcess.push_back(pairMobSubFlow);

	return false;
}

























