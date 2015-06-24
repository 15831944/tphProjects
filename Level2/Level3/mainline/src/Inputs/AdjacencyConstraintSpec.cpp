#include "StdAfx.h"
#include "AdjacencyConstraintSpec.h"
#include "Common\ARCTracker.h"
#include "Common\ProjectManager.h"
#include "Common\exeption.h"
#include "Common\ARCFlight.h"
#include "Inputs\IN_TERM.H"
#include "../Common/ACTypesManager.h"
#include "AssignedGate.h"


AdjacencyConstraintSpecList::AdjacencyConstraintSpecList()
:DataSet(ConstraintSpecFile,2.3f)
{
	 
}

AdjacencyConstraintSpecList::~AdjacencyConstraintSpecList()
{

}

void AdjacencyConstraintSpecList::clear()
{
	m_adjacencyConstraintSpec.clear();
}

void AdjacencyConstraintSpecList::readData( ArctermFile& p_file )
{
	while (p_file.getLine())
	{
		AdjacencyConstraintSpecDate ConstraintSpec;
		
		ConstraintSpec.m_FirstGate.readALTObjectID(p_file);				
		ConstraintSpec.m_SecondGate.readALTObjectID(p_file);
				

		if (p_file.isBlankField())
		{
			p_file.skipField(1);
		}
		else
		{
			ConstraintSpec.m_ThirdGate.readALTObjectID(p_file);
		}		

		int conType;
		p_file.getInteger(conType);
		ConstraintSpec.SetConType(ConditionOfUseType(conType));
	
		float value;
		p_file.getFloat(value);
		ConstraintSpec.m_Width = value;
	
		
		p_file.getFloat(value);
		ConstraintSpec.m_Clearance = value;
		
		m_adjacencyConstraintSpec.push_back(ConstraintSpec);
	}
}

void AdjacencyConstraintSpecList::writeData( ArctermFile& p_file ) const
{
	size_t nCount = m_adjacencyConstraintSpec.size();
	for(size_t i = 0; i < nCount; i++)
	{
		AdjacencyConstraintSpecDate ConstraintSpec = m_adjacencyConstraintSpec.at(i);
		
 		ConstraintSpec.GetFirstGate().writeALTObjectID(p_file);	
		ConstraintSpec.GetSecondGate().writeALTObjectID(p_file);
		ConstraintSpec.GetThirdGate().writeALTObjectID(p_file);
				
		p_file.writeInt((int)ConstraintSpec.GetConType());
		p_file.writeDouble(ConstraintSpec.m_Width);
		p_file.writeDouble(ConstraintSpec.m_Clearance);

		p_file.writeLine();
	}
}

int AdjacencyConstraintSpecList::GetCount()
{
	return (int)m_adjacencyConstraintSpec.size();
}

void AdjacencyConstraintSpecList::AddItem( const AdjacencyConstraintSpecDate& gateAssignCon )
{
	m_adjacencyConstraintSpec.push_back(gateAssignCon);
}

void AdjacencyConstraintSpecList::DeleteItem( int nIndex )
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	m_adjacencyConstraintSpec.erase(m_adjacencyConstraintSpec.begin() + nIndex);
}

AdjacencyConstraintSpecDate& AdjacencyConstraintSpecList::GetItem( int nIndex )
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	return m_adjacencyConstraintSpec[nIndex];
}

void AdjacencyConstraintSpecList::loadDataSet( const CString& _pProjPath )
{
	PLACE_METHOD_TRACK_STRING();
	clear();

	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename);

	ArctermFile file;
	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet(_pProjPath, false);
		return;
	}

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		readObsoleteData( file );
		file.closeIn();
		saveDataSet(_pProjPath, false);
	}
	else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		file.closeIn();
		throw new FileVersionTooNewError( filename );		
	}
	else
	{
		readData (file);
		file.closeIn();
	}
}

bool AdjacencyConstraintSpecList::IsFlightAndGateFit( const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList )
{
	int nSize = (int)vGateList.size();
	if (nGateIdx < 0 || nGateIdx >= nSize)
		return false;
	
	std::vector<int> vStandList;
	for (std::vector<AdjacencyConstraintSpecDate>::iterator iter = m_adjacencyConstraintSpec.begin(); iter != m_adjacencyConstraintSpec.end(); ++iter)
	{
		vStandList.clear();
		if(!iter->GetAdjancenyStandList(nGateIdx,vStandList))
			continue;

		for (unsigned i = 0; i != vStandList.size(); i++)
		{
			int nAdjGateIdx = vStandList[i];
			std::vector<ARCFlight*> vAdjFlights;
			vAdjFlights.clear();

			vGateList[nAdjGateIdx]->GetFlightsInTimeRange(tStart,tEnd,vAdjFlights);
		
			if (!iter->IsFlightFitContraint(flight,vAdjFlights,nGateIdx,nAdjGateIdx,m_pInTerm))
				return false;
		}
	}


	return true;
}

bool AdjacencyConstraintSpecList::FlightAndGateFitErrorMessage( const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList,CString& strError )
{
	int nSize = (int)vGateList.size();
	if (nGateIdx < 0 || nGateIdx >= nSize)
		return false;

	std::vector<int> vStandList;
	for (std::vector<AdjacencyConstraintSpecDate>::iterator iter = m_adjacencyConstraintSpec.begin(); iter != m_adjacencyConstraintSpec.end(); ++iter)
	{
		vStandList.clear();
		if(!iter->GetAdjancenyStandList(nGateIdx,vStandList))
			continue;

		for (unsigned i = 0; i != vStandList.size(); i++)
		{
			int nAdjGateIdx = vStandList[i];
			std::vector<ARCFlight*> vAdjFlights;
			vAdjFlights.clear();

			vGateList[nAdjGateIdx]->GetFlightsInTimeRange(tStart,tEnd,vAdjFlights);

			if (!iter->IsFlightFitConstraint(flight,vAdjFlights,nGateIdx,nAdjGateIdx,m_pInTerm,strError))
				return false;
		}
	}


	return true;
}

void AdjacencyConstraintSpecList::InitConstraintGateIdx( std::vector<ref_ptr_bee<CAssignGate> >& vGateList )
{
	int nConstraint = GetCount();
	int nGateNum = vGateList.size();

	for (int i = 0; i < nConstraint; i++)
	{
		AdjacencyConstraintSpecDate& constraint = GetItem(i);
		for (int j = 0; j < nGateNum; j++)
		{
			if (constraint.GetFirstGateIdx() <0 && vGateList[j]->GetName() == constraint.GetFirstGate().GetIDString())
				constraint.SetFirstGateIdx(j);

			if (constraint.GetSecondGateIdx() <0 && vGateList[j]->GetName() == constraint.GetSecondGate().GetIDString())
				constraint.SetSecondGateIdx(j);

			if (constraint.getThirdGateIdx() <0 && vGateList[j]->GetName() == constraint.GetThirdGate().GetIDString())
				constraint.SetThirdGateIdx(j);

			if (constraint.GetFirstGateIdx() >=0 && constraint.GetSecondGateIdx() >=0 && constraint.getThirdGateIdx() >= 0)
				break;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

AdjacencyConstraintSpecDate::AdjacencyConstraintSpecDate()
:m_ConditionType(ConditionType_OneStandOnly)
,m_Width(0.00)
,m_Clearance(0.00)
,m_nFirstStandIdx(-1)
,m_nSecondStandIdx(-1)
,m_nThirdStandIdx(-1)
{

}

AdjacencyConstraintSpecDate::~AdjacencyConstraintSpecDate()
{

}

float AdjacencyConstraintSpecDate::GetFlightSpan( const ARCFlight* pFlight, InputTerminal* pInTerm)
{
	char strFlight[AC_TYPE_LEN];
	pFlight->getACType(strFlight);

	CACType acType, *anAircraft;
	acType.setIATACode(strFlight);
	int ndx = _g_GetActiveACMan(pInTerm->m_pAirportDB)->findACTypeItem(&acType);
	anAircraft = _g_GetActiveACMan(pInTerm->m_pAirportDB)->getACTypeItem(ndx);
	if (anAircraft == NULL)
		return 0.0;
	return anAircraft->m_fSpan*100;
}

bool AdjacencyConstraintSpecDate::ConcurrentValid(double fData, const ARCFlight* pFlight, InputTerminal* pInTerm)
{
	float fAdjSpan = GetFlightSpan(pFlight,pInTerm);
	TwoStandFlightOperationCase SideStandTwoCase(fData,fAdjSpan);
	return SideStandTwoCase.Available(m_Width,m_Clearance);
}

bool AdjacencyConstraintSpecDate::IsFlightFitContraint( const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx,int nAdjGateIdx, InputTerminal* pInTerm )
{
	if (vFlights.empty())		//no adjacent flights
		return true;

	if (m_ConditionType == ConditionType_OneStandOnly)
		return false;
	
	float fCompareValue = GetFlightSpan(pFlight,pInTerm);
	int nCount = (int)vFlights.size();
	if (m_ConditionType == ConditionType_ConcurrentUse)
	{
		for (int i = 0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
			 if(ConcurrentValid(fCompareValue,pAdjFlight,pInTerm) == false)
				 return false;
		}
	}
	
	if (m_ConditionType == ConditionType_MideOrSides)
	{
		if (m_nSecondStandIdx == nGateIdx)
		{
			return false;
		}
		else if(nAdjGateIdx == m_nSecondStandIdx)
		{
			return false;
		}
		else
		{
			for (int i = 0; i < nCount; i++)
			{
				ARCFlight* pAdjFlight = vFlights.at(i);
				if(ConcurrentValid(fCompareValue,pAdjFlight,pInTerm) == false)
					return false;
			}
		}
	}

	return true;
}

bool AdjacencyConstraintSpecDate::IsFlightFitContraint( const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, const ALTObjectID& standID,const ALTObjectID& adjStandID, InputTerminal* pInTerm )
{
	if (vFlights.empty())		//no adjacent flights
		return true;

	if (m_ConditionType == ConditionType_OneStandOnly)
		return false;

	float fCompareValue = GetFlightSpan(pFlight,pInTerm);
	int nCount = (int)vFlights.size();
	if (m_ConditionType == ConditionType_ConcurrentUse)
	{
		for (int i = 0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
			if(ConcurrentValid(fCompareValue,pAdjFlight,pInTerm) == false)
				return false;
		}
	}

	if (m_ConditionType == ConditionType_MideOrSides)
	{
		if (m_SecondGate == standID)
		{
			return false;
		}
		else if(adjStandID == m_SecondGate)
		{
			return false;
		}
		else
		{
			for (int i = 0; i < nCount; i++)
			{
				ARCFlight* pAdjFlight = vFlights.at(i);
				if(ConcurrentValid(fCompareValue,pAdjFlight,pInTerm) == false)
					return false;
			}
		}
	}

	return true;
}

bool AdjacencyConstraintSpecDate::IsFlightFitConstraint( const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx,int nAdjGateIdx, InputTerminal* pInTerm,CString& strError )
{
	if (vFlights.empty())		//no adjacent flights
		return true;

	if (m_ConditionType == ConditionType_OneStandOnly)
	{
		char flightBuff[255];
		char adjBuff[255];
		pFlight->getFlightIDString(flightBuff);
		ARCFlight* pAdjFlight = vFlights.front();
		pAdjFlight->getFlightIDString(adjBuff);
		CString strFlightStand;
		strFlightStand = GetStandString(nGateIdx);
		CString strConflictStand;
		strConflictStand = GetStandString(nAdjGateIdx);

		strError.Format(_T("Condition of use is One stand only. Flight: %s can't park at stand: %s, \n because stand: %s is occupied by adjacency flight: %s"),flightBuff,strFlightStand,strConflictStand,adjBuff);
		return false;
	}

	float fCompareValue = GetFlightSpan(pFlight,pInTerm);
	int nCount = (int)vFlights.size();
	if (m_ConditionType == ConditionType_ConcurrentUse)
	{
		for (int i = 0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
			if(ConcurrentValid(fCompareValue,pAdjFlight,pInTerm) == false)
			{
				CString strFlightStand;
				strFlightStand = GetStandString(nGateIdx);
				CString strConflictStand;
				strConflictStand = GetStandString(nAdjGateIdx);
				char flightBuff[255];
				char adjBuff[255];
				pFlight->getFlightIDString(flightBuff);
				pAdjFlight->getFlightIDString(adjBuff);
				strError.Format(_T("Condition of use is Concurrent use. Flight: %s and  Adjacency flight: %s at side stand: %s and stand: %s  \n doesn't satisfy adjacency defintiion"),flightBuff,\
					adjBuff,strFlightStand,strConflictStand);
				return false;
			}
		}
	}

	if (m_ConditionType == ConditionType_MideOrSides)
	{
		if (m_nSecondStandIdx == nGateIdx)
		{
			CString strSecondStand;
			strSecondStand = m_SecondGate.GetIDString();
			CString strConflictStand;
			strConflictStand = GetStandString(nAdjGateIdx);
			char flightBuff[255];
			char adjBuff[255];
			pFlight->getFlightIDString(flightBuff);
			ARCFlight* pAdjFlight = vFlights.front();
			pAdjFlight->getFlightIDString(adjBuff);
			strError.Format(_T("Condition of use is mide or side. Flight: %s cann't park at middle stand: %s,\n because the side stand: %s is occupied by adjacency flight: %s"),flightBuff,strSecondStand,strConflictStand,adjBuff);
			return false;
		}
		else if(nAdjGateIdx == m_nSecondStandIdx)
		{
			char flightBuff[255];
			char adjBuff[255];
			CString strSecondStand;
			pFlight->getFlightIDString(flightBuff);
			ARCFlight* pAdjFlight = vFlights.front();
			pAdjFlight->getFlightIDString(adjBuff);
			strSecondStand = m_SecondGate.GetIDString();
			CString strcConflictStand = GetStandString(nGateIdx);
			strError.Format(_T("Condition of use is mide or side.The middel stand: %s is occupied by adjacency flight: %s, \n so flight: %s cann't park at side stand: %s"),strSecondStand,adjBuff,flightBuff,strcConflictStand);
			return false;
		}
		else
		{
			for (int i = 0; i < nCount; i++)
			{
				ARCFlight* pAdjFlight = vFlights.at(i);
				if(ConcurrentValid(fCompareValue,pAdjFlight,pInTerm) == false)
				{
					CString strFirstStand;
					strFirstStand = m_FirstGate.GetIDString();
					CString strThrideStand;
					strThrideStand = m_ThirdGate.GetIDString();
					char fristBuff[255];
					char thirdBuff[255];
					pFlight->getFlightIDString(fristBuff);
					pAdjFlight->getFlightIDString(thirdBuff);
					strError.Format(_T("Condition of use is mide or side. Flight: %s and  Adjacency flight: %s at side stand: %s and stand: %s  \n doesn't satisfy adjacency defintiion"),fristBuff,\
						thirdBuff,strFirstStand,strThrideStand);
					return false;
				}
			}
		}
	}

	return true;
}

bool AdjacencyConstraintSpecDate::GetAdjancenyStandList( int nGateIdx, std::vector<int>& vStandList)
{
	if (m_nFirstStandIdx >= 0)
		vStandList.push_back(m_nFirstStandIdx);

	if (m_nSecondStandIdx >= 0)
		vStandList.push_back(m_nSecondStandIdx);

	if (m_nThirdStandIdx >= 0)
		vStandList.push_back(m_nThirdStandIdx);

	std::vector<int>::iterator standIter = std::find(vStandList.begin(),vStandList.end(),nGateIdx);
	if (standIter == vStandList.end())
		return false;

	vStandList.erase(standIter);
	return true;
}

bool AdjacencyConstraintSpecDate::GetAdjacencyStandNameList( const ALTObjectID& standID,std::vector<ALTObjectID>& vStandList )
{
	if (!m_FirstGate.IsBlank())
		vStandList.push_back(m_FirstGate);

	if (!m_SecondGate.IsBlank())
		vStandList.push_back(m_SecondGate);

	if (!m_ThirdGate.IsBlank())
		vStandList.push_back(m_ThirdGate);

	std::vector<ALTObjectID>::iterator standIter = std::find(vStandList.begin(),vStandList.end(),standID);
	if (standIter == vStandList.end())
		return false;

	vStandList.erase(standIter);
	return true;
}

CString AdjacencyConstraintSpecDate::GetStandString( int nStandIdx ) const
{
	CString strStand;
	if (nStandIdx == m_nFirstStandIdx)
	{
		strStand = m_FirstGate.GetIDString();
	}
	else if (nStandIdx == m_nSecondStandIdx)
	{
		strStand = m_SecondGate.GetIDString();
	}
	else if (nStandIdx == m_nThirdStandIdx)
	{
		strStand = m_ThirdGate.GetIDString();
	}
	return strStand;
}

//-----------------------------------------------------------------------------------------------------------------------------------
TwoStandFlightOperationCase::TwoStandFlightOperationCase(double dFlightPlan1,double dFlightPlan2 )
:m_dFlightSpan1(dFlightPlan1)
,m_dFlightSpan2(dFlightPlan2)
{

}

TwoStandFlightOperationCase::~TwoStandFlightOperationCase()
{

}

//Function code for check adjacency case
bool TwoStandFlightOperationCase::Available( double dWidth,double dWingTip )const
{
	double dTotal = m_dFlightSpan1/2 + m_dFlightSpan2/2 + dWingTip;
	if (dTotal > dWidth)
		return false;

	return true;
}


ThreeStandFlightOperationCase::ThreeStandFlightOperationCase(double dFlightPlan1,double dFlightPlan2,double dFlightPlan3)
:TwoStandFlightOperationCase(dFlightPlan1,dFlightPlan2)
,m_dFlightSpan3(dFlightPlan3)
{

}

ThreeStandFlightOperationCase::~ThreeStandFlightOperationCase()
{

}

bool ThreeStandFlightOperationCase::Available( double dWidth,double dWingTip ) const
{
	//Make stand1 and stand2 together
	if (TwoStandFlightOperationCase::Available(dWidth,dWingTip) == false)
		return false;

	//Make stand1 and stand3 together
	TwoStandFlightOperationCase SideStandTwoCase(m_dFlightSpan1,m_dFlightSpan3);
	if(SideStandTwoCase.Available(dWidth,dWingTip) == false)
		return false;

	//Make Stand2 and Stand3 together
	TwoStandFlightOperationCase RightStandTwoCase(m_dFlightSpan2,m_dFlightSpan3);
	if (RightStandTwoCase.Available(dWidth,dWingTip) == false)
		return false;

	return true;
}
