#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\weatherimpact.h"
#include "ProjectDB.h"
#include "AirsideImportExportManager.h"

CFlightPerformance::CFlightPerformance()
 :DBElement()
 ,m_FlightType("")
 ,m_LandingDisPerf(0.0)
 ,m_TaxiSpedPerf(0.0)
 ,m_HoldShortLineOffset(0.0)
 ,m_TakeoffDisPerf(0.0)
 ,m_DeicingTimeChange(0.0)
{
}

CFlightPerformance::~CFlightPerformance()
{
}

void CFlightPerformance::SetFlightType(CString FlightType)
{
	m_FlightType = FlightType;
}

CString CFlightPerformance::GetFlightType()
{
	return m_FlightType;
}

void CFlightPerformance::SetLandingDisPerf(double LandingDisperf)
{
	m_LandingDisPerf = LandingDisperf;
}

double CFlightPerformance::GetLandingDisPerf()
{
	return m_LandingDisPerf;
}

void CFlightPerformance::SetTaxiSpedPerf(double TaxiSpedPerf)
{
	m_TaxiSpedPerf = TaxiSpedPerf;
}

double CFlightPerformance::GetTaxiSpedPerf()
{
	return m_TaxiSpedPerf;
}

void CFlightPerformance::SetHoldShortLineOffset(double HoldShortLineOffset)
{
	m_HoldShortLineOffset = HoldShortLineOffset;
}

double CFlightPerformance::GetHoldShortLineOffset()
{
	return m_HoldShortLineOffset;
}

void CFlightPerformance::SetTakeoffDisPerf(double TakeoffDisPerf)
{
	m_TakeoffDisPerf = TakeoffDisPerf;
}

double CFlightPerformance::GetTakeoffDisPerf()
{
	return m_TakeoffDisPerf;
}

void CFlightPerformance::SetDeicingTimeChange(double DeicingTimeChange)
{
	m_DeicingTimeChange = DeicingTimeChange;
}

double CFlightPerformance::GetDeicingTimeChange()
{
	return m_DeicingTimeChange;
}

void CFlightPerformance::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("FLTTYPE"),m_FlightType);
	recordset.GetFieldValue(_T("LANDINGDIST"),m_LandingDisPerf);
	recordset.GetFieldValue(_T("TAXISPEED"),m_TaxiSpedPerf);
	recordset.GetFieldValue(_T("HOLDOFFSET"),m_HoldShortLineOffset);
	recordset.GetFieldValue(_T("TAKEOFFDIST"),m_TakeoffDisPerf);
	recordset.GetFieldValue(_T("DEICETIME"),m_DeicingTimeChange);

}
void CFlightPerformance::Save(int nParentID,WEATHERCONDITION WeatherCondition)
{

	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID,WeatherCondition,strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}
void CFlightPerformance::GetInsertSQL(int nProjID,WEATHERCONDITION WeatherCondition ,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_WEATHERIMPACT\
		(PROJID, WITYPE, FLTTYPE, LANDINGDIST, TAXISPEED, HOLDOFFSET, \
		TAKEOFFDIST, DEICETIME)\
		VALUES (%d,%d,'%s',%f,%f,%f,%f,%f)"),
		nProjID,
		(int)WeatherCondition,
		m_FlightType,
		m_LandingDisPerf,
		m_TaxiSpedPerf,
		m_HoldShortLineOffset,
		m_TakeoffDisPerf,
		m_DeicingTimeChange);
}

void CFlightPerformance::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_WEATHERIMPACT\
		SET FLTTYPE ='%s', LANDINGDIST =%f, TAXISPEED =%f, HOLDOFFSET =%f, \
		TAKEOFFDIST =%f, DEICETIME =%f\
		WHERE (ID =%d)"),
		m_FlightType,
		m_LandingDisPerf,
		m_TaxiSpedPerf,
		m_HoldShortLineOffset,
		m_TakeoffDisPerf,
		m_DeicingTimeChange,
		m_nID);
}

void CFlightPerformance::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_WEATHERIMPACT\
		WHERE (ID = %d)"),m_nID);
}
//void CFlightPerformance::ExportData(CAirsideExportFile& exportFile,WEATHERCONDITION WeatherCondition)
//{
//	exportFile.getFile().writeInt(m_nID);
//	exportFile.getFile().writeInt(exportFile.GetProjectID());
//	exportFile.getFile().writeInt((int)WeatherCondition);
//
//	exportFile.getFile().writeField(m_FlightType);
//	exportFile.getFile().writeDouble(m_LandingDisPerf);
//	exportFile.getFile().writeDouble(m_TaxiSpedPerf);
//	exportFile.getFile().writeDouble(m_HoldShortLineOffset);
//	exportFile.getFile().writeDouble(m_TakeoffDisPerf);
//	exportFile.getFile().writeDouble(m_DeicingTimeChange);
//	exportFile.getFile().writeLine();
//
//}
void CFlightPerformance::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	int nOldProjID = -1;
	int nWeatherCondition = 0;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getInteger(nOldProjID);
	importFile.getFile().getInteger(nWeatherCondition);
	importFile.getFile().getField(m_FlightType.GetBuffer(1024),1024);
	importFile.getFile().getFloat(m_LandingDisPerf);
	importFile.getFile().getFloat(m_TaxiSpedPerf);
	importFile.getFile().getFloat(m_HoldShortLineOffset);
	importFile.getFile().getFloat(m_TakeoffDisPerf);
	importFile.getFile().getFloat(m_DeicingTimeChange);
	importFile.getFile().getLine();

	if (nWeatherCondition > DEFAULT_WEATHERCONDITION && nWeatherCondition < WEATHERCONDITION_COUNT)
	{
	
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO IN_WEATHERIMPACT\
						(PROJID, WITYPE, FLTTYPE, LANDINGDIST, TAXISPEED, HOLDOFFSET, \
						TAKEOFFDIST, DEICETIME)\
						VALUES (%d,%d,'%s',%f,%f,%f,%f,%f)"),
						importFile.getNewProjectID(),
						nWeatherCondition,
						m_FlightType,
						m_LandingDisPerf,
						m_TaxiSpedPerf,
						m_HoldShortLineOffset,
						m_TakeoffDisPerf,
						m_DeicingTimeChange);

		CADODatabase::ExecuteSQLStatement(strSQL);
	
	}
}

////////////////////////////////////////////////////////////////////////////
CWeatherImpact::CWeatherImpact()
 :BaseClass()
{
	m_nID = DEFAULT_BASEDBELEMENT_ID;
}

CWeatherImpact::~CWeatherImpact(void)
{
}

void CWeatherImpact::SetWeatherCondition(WEATHERCONDITION WeatherCondition)
{
	m_WeatherCondition = WeatherCondition; 
}

WEATHERCONDITION CWeatherImpact::GetWeatherCondition()
{
	return m_WeatherCondition;
}

void CWeatherImpact::GetItemByFlightType(CString strFlightType, CFlightPerformance ** ppFlightPerf)
{
	if (NULL == ppFlightPerf)
	{
		return;
	}

	size_t flightCount = GetElementCount();
	for (int i=0; i<(int)flightCount; i++)
	{
		CFlightPerformance *flightPerf = GetItem(i); 
		if(!strFlightType.Compare(flightPerf->GetFlightType()))
		{
			*ppFlightPerf = flightPerf;
			return;
		}
	}
}

void CWeatherImpact::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++ i)
	{
//		GetItem(i)->ExportData(exportFile,m_WeatherCondition);
	}
}
//void CWeatherImpact::ReadData(int nParentID)
//{
//
//}
void CWeatherImpact::InitFromDBRecordset(CADORecordset& recordset)
{
}
void CWeatherImpact::SaveData(int nParentID)
{
	for (std::vector<CFlightPerformance *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->Save(nParentID,m_WeatherCondition);
	}

	for (std::vector<CFlightPerformance *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();
}
////////////////////////////////////////////////////////////////////////////
CWeatherImpactList::CWeatherImpactList()
: BaseClass()
{

}

void CWeatherImpactList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{

	strSQL.Format(_T("SELECT ID, WITYPE, FLTTYPE, LANDINGDIST, TAXISPEED, HOLDOFFSET, TAKEOFFDIST, \
		DEICETIME\
		FROM IN_WEATHERIMPACT\
		WHERE (PROJID = %d)"),nProjectID);

}
void CWeatherImpactList::ReadData(int nProjectID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nProjectID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		InitFromDBRecordset(adoRecordset);
		adoRecordset.MoveNextData();
	}
}
void CWeatherImpactList::InitFromDBRecordset(CADORecordset& recordset)
{
	int nWeatherImpactType = 0;
	recordset.GetFieldValue(_T("WITYPE"),nWeatherImpactType);
	
	if (nWeatherImpactType > DEFAULT_WEATHERCONDITION && nWeatherImpactType < WEATHERCONDITION_COUNT)
	{
		CWeatherImpact *pWeatherImpact = NULL;
		GetItemByWeatherCondition((WEATHERCONDITION)nWeatherImpactType,&pWeatherImpact);
		if (pWeatherImpact == NULL)
		{
			pWeatherImpact = new CWeatherImpact();
			pWeatherImpact->SetWeatherCondition((WEATHERCONDITION)nWeatherImpactType);
			AddNewItem(pWeatherImpact);
			if (pWeatherImpact == NULL)
			{
				return;
			}
		}

		CFlightPerformance *flightPerformance = new CFlightPerformance() ;
		flightPerformance->InitFromDBRecordset(recordset);

		pWeatherImpact->AddNewItem(flightPerformance);
	}
}

void CWeatherImpactList::GetItemByWeatherCondition(WEATHERCONDITION WeatherCondition, CWeatherImpact **WeathImpact)
{
	if (NULL == WeathImpact)
	{
		return;
	}

	*WeathImpact = NULL;

	size_t weatherConditonCount = GetElementCount();
	for (int i=0; i<(int)weatherConditonCount; i++)
	{
		CWeatherImpact *weathImpct = GetItem(i); 
		if(WeatherCondition ==weathImpct->GetWeatherCondition())
		{
			*WeathImpact = weathImpct;
			return;
		}
	}
}
void CWeatherImpactList::ExportWeatherImpact(CAirsideExportFile& exportFile)
{

	CWeatherImpactList weatherImpactList;
	weatherImpactList.ReadData(exportFile.GetProjectID());
	weatherImpactList.ExportData(exportFile);

}
void CWeatherImpactList::ImportWeatherImpcat(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CFlightPerformance fltPerformance;
		fltPerformance.ImportData(importFile);

	}
}
void CWeatherImpactList::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}

}
void CWeatherImpactList::ImportData(CAirsideImportFile& importFile)
{


}