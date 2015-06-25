#include "stdafx.h"
#include <Engine/proclist.h>
#include "../InputAirside/ALTObject.h"
#include "Stand2GateConstraint.h"
#include "..\inputs\in_term.h"
#include "GateAssignmentMgr.h"

CStand2GateMapping::CStand2GateMapping(void)
{
	m_enumMappingFlag = MapType_Normal;//0x0;
}

CStand2GateMapping::~CStand2GateMapping(void)
{
}

bool CStand2GateMapping::operator<( const CStand2GateMapping& gateMap ) const
{
	//return gateMap.m_StandID<m_StandID;

	if (!m_StandID.idFits(gateMap.m_StandID))//gateMap is more detail than current object
	{
		return false;
	}
	else if(!gateMap.m_StandID.idFits(m_StandID))
	{
		return true;
	}
	return false;
}

CStand2GateConstraint::CStand2GateConstraint(int nProjID)
:DataSet(Stand2GateMapping, 2.4f)
,m_nProjID(nProjID)
{
	m_ArrivalPreference = new CArrivalGateAssignPreferenceMan(NULL) ;
	m_DepPreference = new CDepGateAssignPreferenceMan(NULL) ;

    m_pArrGateAdja = new ArrivalGateAdjacencyMgr();
    m_pDepGateAdja = new DepartureGateAdjacencyMgr();
}

CStand2GateConstraint::~CStand2GateConstraint(void)
{
	clear();
	if(m_ArrivalPreference != NULL)
		delete m_ArrivalPreference ;
	if (m_DepPreference != NULL)
	    delete m_DepPreference ;
}

void CStand2GateConstraint::readData (ArctermFile& p_file)
{
	clear();

	int nCount = -1;
	p_file.getLine();
	//p_file.getLine();
	p_file.getInteger(nCount);
	for (int i = 0;i < nCount;++i)
	{
		CStand2GateMapping mappingAdd;
		mappingAdd.m_StandID.readALTObjectID(p_file);
		mappingAdd.m_ArrGateID.SetStrDict(m_pInTerm->inStrDict);
		mappingAdd.m_ArrGateID.readProcessorID(p_file);
		mappingAdd.m_DepGateID.SetStrDict(m_pInTerm->inStrDict);
		mappingAdd.m_DepGateID.readProcessorID(p_file);

		int nMapType = CStand2GateMapping::MapType_Normal;
		p_file.getInteger(nMapType);
		mappingAdd.m_enumMappingFlag = (CStand2GateMapping::MapType) nMapType;


		m_pStand2Arrgateconstraint.push_back(mappingAdd);
	}
	p_file.getLine();

	nCount = -1;
	p_file.getInteger(nCount);
	for (int i = 0; i < nCount;++i)
	{	
		CStand2GateMapping mappingAdd;
		mappingAdd.m_StandID.readALTObjectID(p_file);
		mappingAdd.m_ArrGateID.SetStrDict(m_pInTerm->inStrDict);
		mappingAdd.m_ArrGateID.readProcessorID(p_file);
		mappingAdd.m_DepGateID.SetStrDict(m_pInTerm->inStrDict);
		mappingAdd.m_DepGateID.readProcessorID(p_file);

		int nMapType = CStand2GateMapping::MapType_Normal;
		p_file.getInteger(nMapType);
		mappingAdd.m_enumMappingFlag = (CStand2GateMapping::MapType) nMapType;


		m_pStand2Depgateconstraint.push_back(mappingAdd);
	}

	//read preference data
	m_ArrivalPreference->ReadData(m_pInTerm);
	m_DepPreference->ReadData(m_pInTerm);

    m_pArrGateAdja->SetInputTerminal(m_pInTerm);
    m_pArrGateAdja->loadDataSet(GetProjPath());
    m_pDepGateAdja->SetInputTerminal(m_pInTerm);
    m_pDepGateAdja->loadDataSet(GetProjPath());

	std::sort(m_pStand2Arrgateconstraint.begin(),m_pStand2Arrgateconstraint.end());
	std::sort(m_pStand2Depgateconstraint.begin(),m_pStand2Depgateconstraint.end());
}

void CStand2GateConstraint::readObsoleteData(ArctermFile& p_file)
{
    float fVersion = p_file.getVersion();
    if(fVersion < 2.2999f)
        readObsoleteData22(p_file);
    else if(2.2999f < fVersion && fVersion <2.3001f)
        readObsoleteData23(p_file);
    else
        readData(p_file);
}

void CStand2GateConstraint::readObsoleteData22(ArctermFile& p_file)
{
    clear();
    int nCount = -1;
    p_file.getLine();
    //p_file.getLine();
    p_file.getInteger(nCount);
    for (int i = 0;i < nCount;++i) 
    {
        CStand2GateMapping mappingAdd;
        mappingAdd.m_StandID.readALTObjectID(p_file);
        mappingAdd.m_ArrGateID.SetStrDict(m_pInTerm->inStrDict);
        mappingAdd.m_ArrGateID.readProcessorID(p_file);
        mappingAdd.m_DepGateID.SetStrDict(m_pInTerm->inStrDict);
        mappingAdd.m_DepGateID.readProcessorID(p_file);

        int nMapType = CStand2GateMapping::MapType_Normal;
        p_file.getInteger(nMapType);
        mappingAdd.m_enumMappingFlag = (CStand2GateMapping::MapType) nMapType;

        push_back(mappingAdd);
    }
    iterator iter = begin();
    for (; iter != end(); ++iter)
    {
        if ((*iter).m_enumMappingFlag == CStand2GateMapping::MapType_1to1/*0x1*/)
        {
            m_pStand2Arrgateconstraint.push_back(*iter);
        }
        else if ((*iter).m_enumMappingFlag == CStand2GateMapping::MapType_Random/*0x2*/)
        {
            m_pStand2Depgateconstraint.push_back(*iter);
        }
    }

    //read preference data
    m_ArrivalPreference->ReadData(m_pInTerm);
    m_DepPreference->ReadData(m_pInTerm);

    std::sort(m_pStand2Arrgateconstraint.begin(),m_pStand2Arrgateconstraint.end());
    std::sort(m_pStand2Depgateconstraint.begin(),m_pStand2Depgateconstraint.end());
}

void CStand2GateConstraint::readObsoleteData23(ArctermFile& p_file)
{
    clear();

    int nCount = -1;
    p_file.getLine();
    //p_file.getLine();
    p_file.getInteger(nCount);
    for (int i = 0;i < nCount;++i)
    {
        CStand2GateMapping mappingAdd;
        mappingAdd.m_StandID.readALTObjectID(p_file);
        mappingAdd.m_ArrGateID.SetStrDict(m_pInTerm->inStrDict);
        mappingAdd.m_ArrGateID.readProcessorID(p_file);
        mappingAdd.m_DepGateID.SetStrDict(m_pInTerm->inStrDict);
        mappingAdd.m_DepGateID.readProcessorID(p_file);

        int nMapType = CStand2GateMapping::MapType_Normal;
        p_file.getInteger(nMapType);
        mappingAdd.m_enumMappingFlag = (CStand2GateMapping::MapType) nMapType;


        m_pStand2Arrgateconstraint.push_back(mappingAdd);
    }
    p_file.getLine();

    nCount = -1;
    p_file.getInteger(nCount);
    for (int i = 0; i < nCount;++i)
    {	
        CStand2GateMapping mappingAdd;
        mappingAdd.m_StandID.readALTObjectID(p_file);
        mappingAdd.m_ArrGateID.SetStrDict(m_pInTerm->inStrDict);
        mappingAdd.m_ArrGateID.readProcessorID(p_file);
        mappingAdd.m_DepGateID.SetStrDict(m_pInTerm->inStrDict);
        mappingAdd.m_DepGateID.readProcessorID(p_file);

        int nMapType = CStand2GateMapping::MapType_Normal;
        p_file.getInteger(nMapType);
        mappingAdd.m_enumMappingFlag = (CStand2GateMapping::MapType) nMapType;


        m_pStand2Depgateconstraint.push_back(mappingAdd);
    }

    //read preference data
    m_ArrivalPreference->ReadData(m_pInTerm);
    m_DepPreference->ReadData(m_pInTerm);

    std::sort(m_pStand2Arrgateconstraint.begin(),m_pStand2Arrgateconstraint.end());
    std::sort(m_pStand2Depgateconstraint.begin(),m_pStand2Depgateconstraint.end());
}

void CStand2GateConstraint::writeData (ArctermFile& p_file) const
{
	std::vector<CStand2GateMapping>::const_iterator itrMapping;
	p_file.writeInt((int)m_pStand2Arrgateconstraint.size());
	for (itrMapping = m_pStand2Arrgateconstraint.begin(); itrMapping != m_pStand2Arrgateconstraint.end();\
		++itrMapping)
	{
		(*itrMapping).m_StandID.writeALTObjectID(p_file);
		(*itrMapping).m_ArrGateID.writeProcessorID(p_file);
		(*itrMapping).m_DepGateID.writeProcessorID(p_file);
		p_file.writeInt((*itrMapping).m_enumMappingFlag);
	}
	p_file.writeLine();

	p_file.writeInt((int)m_pStand2Depgateconstraint.size());
	for (itrMapping = m_pStand2Depgateconstraint.begin(); itrMapping != m_pStand2Depgateconstraint.end();\
		++itrMapping)
	{
		(*itrMapping).m_StandID.writeALTObjectID(p_file);
		(*itrMapping).m_ArrGateID.writeProcessorID(p_file);
		(*itrMapping).m_DepGateID.writeProcessorID(p_file);
		p_file.writeInt((*itrMapping).m_enumMappingFlag);
	}
	p_file.writeLine();

    //write preference data
    m_ArrivalPreference->WriteData();
    m_DepPreference->WriteData();
    m_pArrGateAdja->saveDataSet(GetProjPath(), false);
    m_pDepGateAdja->saveDataSet(GetProjPath(), false);
}

void CStand2GateConstraint::clear (void)
{
	std::vector<CStand2GateMapping >::clear();
	m_pStand2Arrgateconstraint.clear();
	m_pStand2Depgateconstraint.clear();
}

ProcessorID CStand2GateConstraint::GetArrGateID(CFlightOperationForGateAssign *pFlight)
{
	ALTObjectID standID = pFlight->getFlight()->getArrStand();
	return GetGateID(pFlight, m_pStand2Arrgateconstraint, m_ArrivalPreference, m_pArrGateAdja, standID, TRUE);
}
 ProcessorID CStand2GateConstraint::GetDepGateID(CFlightOperationForGateAssign* pFlight)
{	
	ALTObjectID standID = pFlight->getFlight()->getDepStand();
	return GetGateID(pFlight, m_pStand2Depgateconstraint, m_DepPreference, m_pDepGateAdja, standID, FALSE );
}
ProcessorID CStand2GateConstraint::GetGateID( CFlightOperationForGateAssign* pFlight, 
    std::vector<CStand2GateMapping>& vStand2gateConstraint, 
    CGateAssignPreferenceMan* gatePreference,
    GateAdjacencyMan* gateAdjacency,
    const ALTObjectID& standID, BOOL bArrival)
{
	if (pFlight == NULL)
		return ProcessorID();

	//ALTObjectID standID = pFlight->getFlight()->getDepStand();

	ProcessorList* proList = m_pInTerm->GetProcessorList();

	iterator stand2GateIter = vStand2gateConstraint.begin();
///step 1, find all available processors
	std::vector<ProcessorID> vCandidateGate;//1:1 mode vector
	std::vector<ProcessorID> vRandomGate;//random and normal mode vector
	for (; stand2GateIter != vStand2gateConstraint.end(); ++stand2GateIter)
	{
		if (standID.idFits((*stand2GateIter).m_StandID))
		{
			int nidLevel = (*stand2GateIter).m_StandID.idLength();

			int nMaxidLevel = nidLevel;
			ProcessorID linkedGateIDGroup;
			
			if(bArrival)
				linkedGateIDGroup = (*stand2GateIter).m_ArrGateID;
			else
				linkedGateIDGroup = (*stand2GateIter).m_DepGateID;

			int nFlag = (*stand2GateIter).m_enumMappingFlag;

			int nArridLevel = linkedGateIDGroup.idLength();
			if (nArridLevel != 0)
			{
				 
				if (nFlag == CStand2GateMapping::MapType_1to1 ||
					nFlag == CStand2GateMapping::MapType_Normal)
				{
					GroupIndex gateGroup = proList->getGroupIndexOriginal(linkedGateIDGroup);
					ProcessorArray gateProcArray;

					for (int nGate = gateGroup.start; nGate <= gateGroup.end; ++nGate)
					{
						gateProcArray.addItem(proList->getProcessor(nGate));
					}

					ProcessorID standProcID;
					standProcID.SetStrDict(m_pInTerm->inStrDict);
					standProcID.setID(standID.GetIDString());
					Processor* procRet  = GetOneToOneProcessor(gateProcArray,&standProcID,nMaxidLevel);
					if (procRet)
					{
						ProcessorID DepGateID = *(procRet->getID());
						vCandidateGate.push_back(DepGateID);
					}
					//if find one to one processor
					ProcessorID gateProcID;
					if (GetOneToOneGateID(pFlight,gatePreference,gateAdjacency, vCandidateGate,gateProcID,bArrival))
					{
						return gateProcID;
					}
				}
				else if (nFlag == CStand2GateMapping::MapType_Random)
				{
					//DepGateID.SetStrDict(m_pInTerm->inStrDict);
					GroupIndex gateGroup = proList->getGroupIndexOriginal(linkedGateIDGroup);
					int size = gateGroup.end - gateGroup.start + 1 ;

					int random = 0 ;
					Processor* pro =  NULL ;
					ProcessorID gateID ;
					gateID.SetStrDict(m_pInTerm->inStrDict);
					
					SortedContainer<Processor> m_proc ;
					for (int i = gateGroup.start ; i <(gateGroup.start + size) ;i++)
					{
						//m_proc.Add( proList->getProcessor(i)) ;
						pro = proList->getProcessor(i);
						ProcessorID depGateID = *(pro->getID());
						vRandomGate.push_back(depGateID);
					}
				}
			}
			
		}
	}
	///step 2, filter out the item by preference
	//vStep1ProcID
 //   std::vector<ProcessorID > vPreferGate;
	std::vector<std::pair<ProcessorID,ElapsedTime>> vPreferGate;
	std::vector<ProcessorID > vNormalGate;
	std::vector<ProcessorID> vAvailableGate;
	int nProcCount = vRandomGate.size();
	bool bHasDefineFitGate = false;
	for (int nProc = 0; nProc < nProcCount; ++ nProc)
	{
		ProcessorID& gateProcID = vRandomGate[nProc];
		//check preference

		if (gatePreference->IsGateAssignmentConfilict(gateProcID, pFlight, gateAdjacency->GetGateAdjacency()) == true)//has conflict
			continue;

		bool bPrefer = false;
		bool bDefineFitGate = gatePreference->FlightAvailableByPreference(gateProcID,pFlight);
		if (gatePreference->FindItemByGateID(gateProcID))
		{
			if (bDefineFitGate == false)
				continue;
		}

		bHasDefineFitGate = (bHasDefineFitGate == false?bDefineFitGate : true);
		ElapsedTime eDurationTime;
		if(gatePreference->CheckTheGateByPreference(gateProcID,pFlight, bPrefer,eDurationTime))
		{
			vNormalGate.push_back(gateProcID);

			if(bPrefer)
			{
				//vPreferGate.push_back(gateProcID);
				vPreferGate.push_back(std::make_pair(gateProcID,eDurationTime));
			}	
			if (bDefineFitGate)
			{
				vAvailableGate.push_back(gateProcID);
			}
		}
	}

	///select the departure gate
	if (bHasDefineFitGate)
	{
		if(vPreferGate.size() > 0)
		{
			int idx = random(vPreferGate.size());
			std::pair<ProcessorID,ElapsedTime> preferData = vPreferGate[idx];
			if (bArrival)
			{
				pFlight->SetTimeRange(pFlight->GetStartTime(),pFlight->GetStartTime() + preferData.second);
			}
			else
			{
				pFlight->SetTimeRange(pFlight->GetEndTime() - preferData.second,pFlight->GetEndTime());
			}
			
			return vPreferGate.at(idx).first;
		}

		//if (vAvailableGate.size() > 0)
		//{
		//	return vAvailableGate.at(random(vAvailableGate.size()));
		//}
		return ProcessorID();
	}
	
	//if(vPreferGate.size() > 0)
	//{
	//	return vPreferGate.at(random(vPreferGate.size()));
	//}
	if(vNormalGate.size() > 0)
	{
		return vNormalGate.at(random(vNormalGate.size()));
	}

	
	return ProcessorID();
}


ProcessorID CStand2GateConstraint::GetArrGateIDInSim(Flight *pFlight, ALTObjectID &standID)
{
	ProcessorID arrGateID;

	std::vector<CStand2GateMapping>& gateConstraint = m_pStand2Arrgateconstraint;

	int nFlag;
	ProcessorList* proList = m_pInTerm->GetProcessorList();
	int nidLevel=0;
	int nArridLevel=0;
	int nMaxidLevel =0;

	ProcessorID arrGateIDGroup;
	iterator ArrGateIter = gateConstraint.begin();
	for (; ArrGateIter != gateConstraint.end(); ++ArrGateIter)
	{
		if (standID.idFits((*ArrGateIter).m_StandID))
		{
			nidLevel = (*ArrGateIter).m_StandID.idLength();
			if (nMaxidLevel < nidLevel)
			{
				nMaxidLevel = nidLevel;
				arrGateIDGroup = (*ArrGateIter).m_ArrGateID;
				nFlag = (*ArrGateIter).m_enumMappingFlag;
			}
		}
	}
	nArridLevel = arrGateIDGroup.idLength();

	if (nArridLevel != 0)
	{
		if (nFlag == CStand2GateMapping::MapType_1to1 ||
			nFlag == CStand2GateMapping::MapType_Normal )
		{

			GroupIndex gateGroup = proList->getGroupIndexOriginal(arrGateIDGroup);
			ProcessorArray gateProcArray;

			for (int nGate = gateGroup.start; nGate <= gateGroup.end; ++nGate)
			{
				gateProcArray.addItem(proList->getProcessor(nGate));
			}

			ProcessorID standProcID;
			standProcID.SetStrDict(m_pInTerm->inStrDict);
			standProcID.setID(standID.GetIDString());
			Processor* procRet  = GetOneToOneProcessor(gateProcArray,&standProcID,nMaxidLevel);
			if (procRet)
			{
				arrGateID = *(procRet->getID());	
			}

		}
		else if (nFlag == CStand2GateMapping::MapType_Random)
		{
			GroupIndex gateGroup = proList->getGroupIndexOriginal(arrGateIDGroup);
			Processor* pro = proList->getProcessor(random(gateGroup.end - gateGroup.start + 1) + gateGroup.start);
			if (pro != NULL)
				arrGateID = *(pro->getID());
		}

	}
	arrGateID.SetStrDict(m_pInTerm->inStrDict);
	return arrGateID;
}

ProcessorID CStand2GateConstraint::GetDepGateIDInSim(Flight *pFlight, ALTObjectID &standID)
{
	ProcessorID DepGateID;

	std::vector<CStand2GateMapping>& gateConstraint = m_pStand2Depgateconstraint;

	int nFlag;
	ProcessorList* proList = m_pInTerm->GetProcessorList();

	int nidLevel;
	int nArridLevel;
	int nMaxidLevel =0;
	ProcessorID depGateIDGroup;
	iterator DepGateIter = gateConstraint.begin();
	for (; DepGateIter != gateConstraint.end(); ++DepGateIter)
	{
		if (standID.idFits((*DepGateIter).m_StandID))
		{
			nidLevel = (*DepGateIter).m_StandID.idLength();
			if (nMaxidLevel < nidLevel)
			{
				nMaxidLevel = nidLevel;
				depGateIDGroup = (*DepGateIter).m_DepGateID;
				nFlag = (*DepGateIter).m_enumMappingFlag;
			}
		}
	}
	nArridLevel = depGateIDGroup.idLength();
	if (nArridLevel != 0)
	{
		if (nFlag == CStand2GateMapping::MapType_1to1||
			nFlag == CStand2GateMapping::MapType_Normal)
		{
			GroupIndex gateGroup = proList->getGroupIndexOriginal(depGateIDGroup);
			ProcessorArray gateProcArray;

			for (int nGate = gateGroup.start; nGate <= gateGroup.end; ++nGate)
			{
				gateProcArray.addItem(proList->getProcessor(nGate));
			}

			ProcessorID standProcID;
			standProcID.SetStrDict(m_pInTerm->inStrDict);
			standProcID.setID(standID.GetIDString());
			Processor* procRet  = GetOneToOneProcessor(gateProcArray,&standProcID,nMaxidLevel);
			if (procRet)
			{
				DepGateID = *(procRet->getID());	
			}


		}
		else if (nFlag == CStand2GateMapping::MapType_Random)
		{
			//if (DepGateID.idLength() < 4)
			{
				GroupIndex gateGroup = proList->getGroupIndexOriginal(depGateIDGroup);
				Processor* pro = proList->getProcessor(random(gateGroup.end - gateGroup.start + 1) + gateGroup.start);
				if (pro != NULL)
					DepGateID = *(pro->getID());
			}
		}
	}
	DepGateID.SetStrDict(m_pInTerm->inStrDict);
	return DepGateID;
}
ALTObjectID CStand2GateConstraint::GetStdGateIDbyArrGate(std::vector<CStand2GateMapping> gateConstraint,ProcessorID &arrGateID)
{	
	ALTObjectID standIDret;
	ASSERT(0);
	return standIDret;

	//ALTObjectID standID;
	//int nFlag;
	//ALTObjectIDList IDList;
	//iterator standIter = gateConstraint.begin();
	//int arridLevel;
	//int standidLevel;
	//int nMaxidLevel = 0;
	//for(; standIter != gateConstraint.end(); ++standIter)
	//{
	//	if (arrGateID.idFits((*standIter).m_ArrGateID))
	//	{
	//		arridLevel = (*standIter).m_ArrGateID.idLength();
	//		if (nMaxidLevel < arridLevel)
	//		{
	//			nMaxidLevel = arridLevel;
	//			standID = (*standIter).m_StandID;
	//			nFlag = (*standIter).m_enumMappingFlag;
	//		}
	//	}
	//}
	//standidLevel = standID.idLength();
	//if (standidLevel != 0)
	//{
	//	if (nFlag == CStand2GateMapping::MapType_1to1)
	//	{
	//		SortedStringList strDest;
	//		ALTObject::GetObjectNameList(ALT_STAND,m_nProjID,IDList);
	//		SortedStringList::iterator iter;
	//		char szLevelName[256];
	//		for (int nStandIndex = standidLevel; nStandIndex < 4; nStandIndex++)
	//		{
	//			IDList.GetStringListAtLevel(nStandIndex,strDest);
	//			iter = strDest.begin();
	//			arrGateID.getNameAtLevel(szLevelName,nStandIndex);
	//			for (int nIndex = 0; nIndex < (int)strDest.size(); nIndex++)
	//			{
	//				if (strcmp(*iter,szLevelName) == 0)
	//				{
	//					if (standID.idLength() < 5)
	//					{
	//						standID.at(nStandIndex) = szLevelName;
	//					}
	//				}
	//				++iter;
	//			}
	//		}
	//		SortedStringList strDict;
	//		for(int nID  = standID.idLength();nID < 4; nID++)
	//		{
	//			IDList.GetStringListAtLevel(standID.idLength(),strDict);
	//			iter = strDict.begin();
	//			int nCurItem = random((int)strDict.size());
	//			for (int n = 0; n < nCurItem; n++)
	//			{
	//				++iter;
	//			}
	//			//strcpy(const_cast<char*>(standID.at(nID).c_str()),(*iter).GetBuffer(1024));
	//			*iter = CString(standID.at(nID).c_str());
	//		}
	//	}
	//	else if (nFlag == CStand2GateMapping::MapType_Random)
	//	{
	//		SortedStringList strDict1;
	//		SortedStringList::iterator iter1;
	//		for(int nID  = standID.idLength();nID < 4; nID++)
	//		{
	//			IDList.GetStringListAtLevel(standID.idLength(),strDict1);
	//			iter1 = strDict1.begin();
	//			int nCurItem = random((int)strDict1.size());
	//			for (int n = 0; n < nCurItem; n++)
	//			{
	//				++iter1;
	//			}
	//			strcpy(const_cast<char*>(standID.at(nID).c_str()),(*iter1).GetBuffer(0));
	//		}
	//	}
	//}
	//return (standID);
}

ALTObjectID CStand2GateConstraint::GetStdGateIDbyDepGate(std::vector<CStand2GateMapping> gateConstraint,ProcessorID &depGateID)
{
	return (GetStdGateIDbyArrGate(gateConstraint,depGateID));
}
Processor* CStand2GateConstraint::GetOneToOneProcessor(ProcessorArray& gateArray, const ProcessorID* _pSourceID, int _nDestIdLength ) const
{
// 	TRACE( "-%s-=\n", _pSourceID->GetIDString());
	ProcessorArray arrayResult;
	ProcessorArray arrayCandidated( gateArray );
	int nDeltaFromLeaf = 0;

	while( arrayCandidated.getCount() > 0 )
	{
		// move 
		int nCount = arrayCandidated.getCount();
		if( nCount == 1 )
			// A-1  ->  G-1-2 is ok
			return arrayCandidated.getItem( 0 );

		ProcessorArray arrayTemp( arrayCandidated );
		arrayCandidated.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = _pSourceID->idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	// stop on the first level.
		_pSourceID->getNameAtLevel( szSourceLevelName, nSourceIdLength );
		nCount = arrayTemp.getCount();
		for( int i=0; i<nCount; i++ )
		{
			Processor* pProc = arrayTemp.getItem( i );

			int nThisIdLenght = pProc->getID()->idLength();
			nThisIdLenght -= nDeltaFromLeaf;

			if( nThisIdLenght <= _nDestIdLength )
			{
				// satisfy the checking. add to the result array.
// 				TRACE( "-arrayResult = %s--\n", pProc->getID()->GetIDString() );
				arrayResult.addItem( pProc );
			}
			else
			{
				char szLevelName[128];
				pProc->getID()->getNameAtLevel( szLevelName, nThisIdLenght-1 );
				if( strcmp( szLevelName, szSourceLevelName ) == 0 )
				{
					// put into the candidate list.
// 					TRACE( "-arrayCandidated = %s--\n", pProc->getID()->GetIDString() );
					arrayCandidated.addItem( pProc );
				}
			}
		}

		//if( arrayCandidated.getCount() == 0 )
		//{
		//	int nCount = arrayTemp.getCount();
		//	return arrayTemp.getItem( random( nCount ) );
		//}
		nDeltaFromLeaf++;
	}
	int nCount = arrayResult.getCount();
	if( nCount == 0 )
		return NULL;
	return arrayResult.getItem( random(nCount) );
}

void CStand2GateConstraint::setGateAssignManager( CGateAssignmentMgr* pGateAssignManager )
{
		m_pGateAssignManager = pGateAssignManager;
		m_ArrivalPreference->SetGateAssignMgr(pGateAssignManager) ;
		m_DepPreference->SetGateAssignMgr(pGateAssignManager) ;
}

bool CStand2GateConstraint::GetOneToOneGateID( CFlightOperationForGateAssign* pFlight,
    CGateAssignPreferenceMan* gatePreference,
    GateAdjacencyMan* gateAdjacency,
    const std::vector<ProcessorID>& vCandidateGate,
    ProcessorID& gateProID,BOOL bArrival )
{
//	std::vector<ProcessorID > vPreferGate;
	std::vector<std::pair<ProcessorID,ElapsedTime>> vPreferGate;
	std::vector<ProcessorID > vNormalGate;
	std::vector<ProcessorID> vAvailableGate;
	int nProcCount = vCandidateGate.size();
	bool bHasDefineFitGate = false;
	for (int nProc = 0; nProc < nProcCount; ++ nProc)
	{
		const ProcessorID& gateProcID = vCandidateGate[nProc];

		//check preference
		if (gatePreference->IsGateAssignmentConfilict(gateProcID, pFlight, gateAdjacency->GetGateAdjacency()) == true)//has conflict
			continue;
		bool bPrefer = false;
		ProcessorID gateID;
		gateID = gateProcID;
		bool bDefineFitGate = gatePreference->FlightAvailableByPreference(gateProcID,pFlight);
		if (gatePreference->FindItemByGateID(gateID))
		{
			if (bDefineFitGate == false)
				continue;
		}
		bHasDefineFitGate = (bHasDefineFitGate == false?bDefineFitGate : true);
		ElapsedTime eDurationTime;
		if(gatePreference->CheckTheGateByPreference(gateID,pFlight, bPrefer,eDurationTime))
		{
			vNormalGate.push_back(gateProcID);

			if(bPrefer)
				vPreferGate.push_back(std::make_pair(gateProcID,eDurationTime));
		
			if (bDefineFitGate)
				vAvailableGate.push_back(gateProcID);
		}
	}

	///select the departure gate
	if (bHasDefineFitGate)
	{
		if(vPreferGate.size() > 0)
		{
			int idx = random(vPreferGate.size());
			std::pair<ProcessorID,ElapsedTime> preferData = vPreferGate[idx];
			if (bArrival)
			{
				pFlight->SetTimeRange(pFlight->GetStartTime(),pFlight->GetStartTime() + preferData.second);
			}
			else
			{
				pFlight->SetTimeRange(pFlight->GetEndTime() - preferData.second,pFlight->GetEndTime());
			}
			gateProID = vPreferGate.at(idx).first;
			return true;
		}

		//if (vAvailableGate.size() > 0)
		//{
		//	gateProID = vAvailableGate.at(random(vAvailableGate.size()));
		//	return true;
		//}
		return false;
	}
	
	if(vNormalGate.size() > 0)
	{
		gateProID = vNormalGate.at(random(vNormalGate.size()));
		return true;
	}
	return false;
}

CString CStand2GateConstraint::GetProjPath() const
{
    CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
    ASSERT( pMDIActive != NULL );

    CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
    ASSERT( pDoc!= NULL );

    return pDoc->m_ProjInfo.path;
}



