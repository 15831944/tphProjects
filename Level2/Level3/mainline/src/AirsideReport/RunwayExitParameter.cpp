#include "StdAfx.h"
#include ".\runwayexitparameter.h"
#include "../InputAirside/RunwayExit.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/ALT_BIN.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../InputAirside/ALTObject.h"
CRunwayExitParameter::CRunwayExitParameter(void)
{
}

CRunwayExitParameter::~CRunwayExitParameter(void)
{
	for (int i = 0 ; i < (int)m_RunwayData.size() ;i++)
	{
		delete m_RunwayData[i] ;
	}
	m_RunwayData.clear() ;
}
void CRunwayExitParameter::ClearData()
{
	for (int i = 0 ; i < (int)m_RunwayData.size() ;i++)
	{
		delete m_RunwayData[i] ;
	}
	m_RunwayData.clear() ;
}
void CRunwayExitParameter::AddRunwayExit(int _runwayID ,int _exitID ,const CString& _runwayName ,const CString& _exitname,CRunwayExitItem::Operation_type _oper)
{
	if(!FindItemByExitID(_exitID))
	{
		CRunwayExitItem* NewItem = new CRunwayExitItem ;
		NewItem->SetExitID(_exitID) ;
		NewItem->SetExitName(_exitname) ;
		NewItem->SetRunwayID(_runwayID) ;
		NewItem->SetRunwayName(_runwayName) ;
		NewItem->SetOperationType(_oper) ;
		m_RunwayData.push_back(NewItem) ;
	}
}
CRunwayExitItem* CRunwayExitParameter::FindItemByExitID(int _exitID) 
{
	std::vector<CRunwayExitItem*>::iterator iter = m_RunwayData.begin() ;
	for ( ; iter != m_RunwayData.end() ;iter++)
	{
		if((*iter)->GetExitID() == _exitID)
			return *iter ;
	}
	return NULL ;
}
BOOL CRunwayExitParameter::ExportFile(ArctermFile& _file)
{
	CParameters::ExportFile(_file) ;
	m_ShowData.ExportFile(_file) ;
	return TRUE ;
}
BOOL CRunwayExitParameter::ImportFile(ArctermFile& _file)
{
	CParameters::ImportFile(_file) ;
	m_ShowData.ClearData() ;
	m_ShowData.ImportFile(_file) ;

	GetDataFromShowData() ;
	return TRUE ;
}

CString CRunwayExitParameter::GetReportParamName()
{
	return _T("RunwayExit\\RunwayExit.prm");
}

void CRunwayExitParameter::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file) ;
	m_ShowData.ClearData() ;
	m_ShowData.ImportFile(_file) ;
}

void CRunwayExitParameter::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file) ;
	m_ShowData.ExportFile(_file) ;
}

CRunwayExitItem* CRunwayExitParameter::GetItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < GetCount());
	return m_RunwayData[nIdx];
}

int CRunwayExitParameter::GetCount()const
{
	return (int)m_RunwayData.size();
}

void CRunwayExitParameter::GetDataFromShowData()
{
	
		for (int i = 0 ; i < (int)m_ShowData.m_data.size() ;i++)
		{
			RunwayExitRepDefineData_Runway* data = m_ShowData.m_data[i] ;
			if((int)data->m_data.size() == 0 && data->m_RunwayName.CompareNoCase("All") != 0)
			{
				std::vector<ALTObject>  objects ;
				ALTObjectGroup altGroup;
				altGroup.setName(ALTObjectID(data->m_RunwayName));
				altGroup.SetProjID(GetProjID()) ;
				altGroup.setType(ALT_RUNWAY) ;
				altGroup.GetObjects(objects) ;

				Runway runway ;
				std::vector<ALTObject>::iterator iterRunwayID = objects.begin();
				for (; iterRunwayID != objects.end(); ++iterRunwayID)
				{
					runway.ReadObject((*iterRunwayID).getID());

					RunwayExitList runwayExitList;
					int nRunwayExitCount = runway.GetExitList(RUNWAYMARK_FIRST, runwayExitList);

					//if the runway haven't exit
					if (runwayExitList.size() < 1)
					{
						continue;
					}
					std::vector<RunwayExit>::const_iterator citer;
					CString runwayExitName;
					for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
					{
						RunwayExit runwayExit = *citer;

						//runwayExitName = runwayExit.m_strName;	
						AddRunwayExit(data->m_RunwayID,runwayExit.GetID(),data->m_RunwayName,runwayExit.GetName(),(CRunwayExitItem::Operation_type)data->m_Operater) ;
					}

					 nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitList);
					if (runwayExitList.size() < 1)
					{
						continue;
					}
					for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
					{
						RunwayExit runwayExit = *citer;

						//runwayExitName = runwayExit.m_strName;	
						AddRunwayExit(data->m_RunwayID,runwayExit.GetID(),data->m_RunwayName,runwayExit.GetName(),(CRunwayExitItem::Operation_type)data->m_Operater) ;
					}
				}
			}
			if((int)data->m_data.size() == 0 && data->m_RunwayName.CompareNoCase("All") == 0)
			{
				m_AllOper = (CRunwayExitItem::Operation_type)data->m_Operater ;
			}
			for(int j = 0 ; j < (int)data->m_data.size() ;j++)
			{
				RunwayExitRepDefineData_Exit exit ;
				exit = data->m_data[j] ;
				AddRunwayExit(exit.m_RunwayID,exit.m_ExitID,exit.m_runwayName,exit.m_ExitName,(CRunwayExitItem::Operation_type)data->m_Operater) ;
			}
		}
	
}
