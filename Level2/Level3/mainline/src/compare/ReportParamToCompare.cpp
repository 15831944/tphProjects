// ReportParamToCompare.cpp: implementation of the ReportParamToCompare class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ReportParamToCompare.h"
#include "ModelToCompare.h"
#include "../Engine/terminal.h"
//////////////////////////////////////////////////////////////////////////
//class CModelParameter
void CModelParameter::InitDefaultParameter(const CString& strProjName,CModelToCompare * pModel)
{

	pModel->InitTerminal(NULL,strProjName,NULL);
	InputTerminal *pTerminal = pModel->GetTerminal();

	CMobileElemConstraint constarint(pTerminal);
	constarint.SetInputTerminal(pTerminal);
	AddPaxType(constarint);

	ProcessorID procID;
	procID.SetStrDict(pTerminal->inStrDict);

	procID.setID("START");
	m_fromToProcs.m_vFromProcs.push_back(procID);
	procID.setID("END");
	m_fromToProcs.m_vToProcs.push_back(procID);

}

void CModelParameter::AddPaxType(CMobileElemConstraint mobConst, BOOL isChecked)
{
	MobConstWithCheckedFlag mobConstWithFlag;
	mobConstWithFlag.SetChecked(isChecked);
	mobConstWithFlag.SetPaxType(mobConst);
	m_vPaxType.push_back(mobConstWithFlag);
}

BOOL CModelParameter::IsPaxTypeExist(CMobileElemConstraint paxType)
{
	int nPaxTypeCount = (int)m_vPaxType.size();
	for(int i=0; i<nPaxTypeCount; i++)
	{
		if(m_vPaxType[i].GetPaxType().isEqual(&paxType))
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CModelParameter::GetPaxTpyeList(std::vector<CMobileElemConstraint>& vPaxTpye)
{
	int nPaxTypeCount = (int)m_vPaxType.size();
	for(int i=0; i<nPaxTypeCount; i++)
	{
		vPaxTpye.push_back(m_vPaxType[i].GetPaxType());
	}
}

void CModelParameter::GetProcIDGroup(std::vector<ProcessorID>& vProcGroup)
{
	int nProcIDCount = (int)m_vProcGroup.size();
	for(int i=0; i<nProcIDCount; i++)
	{
		vProcGroup.push_back(m_vProcGroup[i].GetProcID());
	}
}

void CModelParameter::AddProcID( ProcessorID procID, BOOL isChecked /*= TRUE*/ )
{
	ProcessIDWithCheckedFlag item;
	item.SetChecked(isChecked);
	item.SetProcID(procID);
	m_vProcGroup.push_back(item);
}

BOOL CModelParameter::IsProcIDExist(ProcessorID procID)
{
	int nProcIDCount = (int)m_vProcGroup.size();
	for(int i=0; i<nProcIDCount; i++)
	{
		if(m_vProcGroup[i].GetProcID() == procID)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParamToCompare::CReportParamToCompare()
{
	m_tInterval.set(0, 0, 0);
	//m_nReportType = 0;
	//m_nThreshold = 0;
	m_nPassagerType = 0;
	//m_strReportName = "";
	m_tEnd.set(0, 0, 0);
	m_tEnd.set(0, 0, 0);
	m_lInterval = 0;
}

CReportParamToCompare::~CReportParamToCompare()
{
	
}

/*
void CReportParamToCompare::SetReportParamter( const ElapsedTime& tStart, const ElapsedTime& tEnd, 
										int nInterval, int nReportType, 
										int nThreshold, int nPassagerType, const CString& strReportName )
{
	m_tStart = tStart;
	m_tEnd = tEnd;
	m_nInterval = nInterval;
	m_nReportType = nReportType;
	m_nThreshold = nThreshold;
	m_nPassagerType = nPassagerType;
	m_strReportName = strReportName;
}


void CReportParamToCompare::GetReportParamter( ElapsedTime& tStart, ElapsedTime& tEnd, 
										int& nInterval, int& nReportType, 
										int& nThreshold, int& nPassagerType, CString& strReportName ) const
{
	tStart = m_tStart;
	tEnd = m_tEnd;
	nInterval = m_nInterval;
	nReportType = m_nReportType;
	nThreshold = m_nThreshold;
	nPassagerType = m_nPassagerType;
	strReportName = m_strReportName;
}
*/

BOOL CReportParamToCompare::LoadData(const CString& strPath)
{
	if(strPath == "")
		return FALSE;
	
	return TRUE;
}

void CReportParamToCompare::SetModelParameter(std::vector<CModelParameter>& vModelParam)
{

	m_vModelParam = vModelParam;
}
std::vector<CModelParameter>& CReportParamToCompare::GetModelParameter()
{
	return m_vModelParam;
}

int CReportParamToCompare::GetModelParameterCount()
{
	return (int)m_vModelParam.size();
}

//void CReportParamToCompare::SetPaxType(const std::vector<CMobileElemConstraint>& vPaxType)
//{
//	m_vPaxType = vPaxType;
//}
//
//void CReportParamToCompare::SetProcessorID(const std::vector<ProcessorID>& vProcGroups)
//{
//	m_vProcGroups = vProcGroups;
//}
//
//int CReportParamToCompare::GetPaxType(OUT std::vector<CMobileElemConstraint>& vPaxType)
//{
//	vPaxType = m_vPaxType;
//	return m_vPaxType.size();
//}
//
//int CReportParamToCompare::GetProcessorID(OUT std::vector<ProcessorID>& vProcGroups)
//{
//	vProcGroups = m_vProcGroups;
//	return m_vProcGroups.size();
//}
bool CReportParamToCompare::GetModelParamByModelName(const CString& strUniqueName,CModelParameter& modelParam) const
{
	std::vector<CModelParameter>::const_iterator iter  = m_vModelParam.begin();
	std::vector<CModelParameter>::const_iterator iterEnd  = m_vModelParam.end();

	for(;iter != iterEnd; ++ iter)
	{
		if (strUniqueName.CompareNoCase((*iter).GetModelUniqueName()) == 0)
		{
			modelParam = *iter;
			return true;
		}
	}
	return false;
}

void CReportParamToCompare::DeleteModelParameter(const CString& strModelUniqueName)
{
	std::vector<CModelParameter>::size_type i = 0;
	for (; i < m_vModelParam.size(); ++i)
	{
		if(strModelUniqueName.CompareNoCase(m_vModelParam.at(i).GetModelUniqueName()) == 0)
		{
			m_vModelParam.erase(m_vModelParam.begin()+ i);
			break;
		}
	} 

}