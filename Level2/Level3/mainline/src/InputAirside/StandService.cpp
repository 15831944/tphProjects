#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\standservice.h"
#include "../InputAirside/AirsideAircraft.h"
#include <algorithm>
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"

using namespace ADODB;


StandService::StandService(void)
:m_strTimeDistName(_T("U[20~30]"))
,m_enumTimeProbType(UNIFORM)
,m_strTimePrintDist(_T("Uniform:20;30"))
,m_pTimeDistribution(NULL)
,m_nProjID(-1)
,m_pAirportDB(NULL)
{
}

StandService::~StandService(void)
{
	if(m_pTimeDistribution)
	{
		delete m_pTimeDistribution;
		m_pTimeDistribution = NULL;
	}
}

void StandService::InitFromDBRecordset(CADORecordset& recordset)
{
	if(!recordset.IsEOF()){
		recordset.GetFieldValue(_T("ID"),m_nID);
		recordset.GetFieldValue(_T("PROJID"),m_nProjID);

		CString strFltType;
		recordset.GetFieldValue(_T("FLTCONSTRAINT"),strFltType);
		if(m_pAirportDB){
			m_fltType.SetAirportDB(m_pAirportDB);
			m_fltType.setConstraintWithVersion(strFltType.GetBuffer(strFltType.GetLength()+1));
			strFltType.ReleaseBuffer(strFltType.GetLength()+1);
		}

		int nProbType = 0;

		recordset.GetFieldValue(_T("TIMEDISTNAME"),m_strTimeDistName);
		recordset.GetFieldValue(_T("TIMEPROBTYPE"),nProbType);
		m_enumTimeProbType = (ProbTypes)nProbType;
		recordset.GetFieldValue(_T("TIMEPRINTDIST"),m_strTimePrintDist);
		GenerateDistribution();
	}
}

void StandService::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM STANDSERVICE WHERE (ID = %d);"),nID);
	return ;
}

void StandService::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");

	{
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltType = szFltType;
	}

	strSQL.Format(_T("INSERT INTO  STANDSERVICE (PROJID,FLTCONSTRAINT,TIMEDISTNAME,TIMEPROBTYPE,TIMEPRINTDIST) VALUES (%d,'%s','%s',%d,'%s');"),\
		nParentID,strFltType,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist);
	//return ;
}

void StandService::GetUpdateSQL(CString& strSQL) const
{
	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");

	{
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltType = szFltType;
	}
	strSQL.Format(_T("UPDATE STANDSERVICE SET FLTCONSTRAINT = '%s',TIMEDISTNAME = '%s',TIMEPROBTYPE = %d,TIMEPRINTDIST = '%s' WHERE (ID = %d);"),\
		strFltType,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist,m_nID);
	return ;
}

void StandService::GetDeleteSQL(CString& strSQL) const
{
	strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STANDSERVICE WHERE (ID = %d);"),m_nID);
	return ;
}

void StandService::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeLine();
}

void StandService::ImportData(CAirsideImportFile& importFile)
{

	importFile.getFile().getLine();
}

CString StandService::GetTimeDistScreenPrint(void)
{
	CString strReturn = _T("");
	if(m_pTimeDistribution == NULL)//the m_pTimeDistribution should not be null, it has a default value
		GenerateDistribution();

	if(m_pTimeDistribution)
	{
		char szScreenPrintDist[1024] = {0};
		m_pTimeDistribution->screenPrint(szScreenPrintDist);
		strReturn = szScreenPrintDist;
	}
	return (strReturn);
}


ProbabilityDistribution* StandService::GetTimeProbDistribution()
{
	if(m_pTimeDistribution == NULL)
	{
		GenerateDistribution();
	}
	return m_pTimeDistribution;
}

void StandService::GenerateDistribution()
{
	if(m_pTimeDistribution)
	{
		delete m_pTimeDistribution;
		m_pTimeDistribution = 0;
	}

	switch(m_enumTimeProbType) 
	{
	case BERNOULLI:
		m_pTimeDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pTimeDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pTimeDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pTimeDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pTimeDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pTimeDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pTimeDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pTimeDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pTimeDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pTimeDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pTimeDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pTimeDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(!m_pTimeDistribution)
		return ;

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strTimePrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pTimeDistribution->setDistribution(szprintDist);

	
}

void StandService::SetTimeDistributionDesc( CString strDistName,ProbTypes enumProbType,CString strprintDist )
{
	m_strTimeDistName = strDistName;
	m_enumTimeProbType = enumProbType;
	m_strTimePrintDist = strprintDist;
	GenerateDistribution();
}















////////////////////////////////////////////////////////////////////////////////////////////
//StandServiceList
///////////////////////////////////////////////////////////////////////////////////////////
StandServiceList::StandServiceList(CAirportDatabase* pAirportDB)
{
	m_pAirportDB = pAirportDB;
}

StandServiceList::~StandServiceList()
{

}

void StandServiceList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM STANDSERVICE WHERE (PROJID = %d);"),nParentID);
	return;
}

void StandServiceList::ReadData(int nParentID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	m_dataList.clear();
	while (!adoRecordset.IsEOF())
	{
		StandService* element = new StandService;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}

	if(m_dataList.size()==0) //add default value
	{
		StandService * pDefault = new StandService;
		pDefault->SetAirportDB(m_pAirportDB);
		m_dataList.push_back(pDefault);
		//SaveData(nParentID);
	}
}

bool FlightTypeCompare(StandService* fItem,StandService* sItem)
{
	const FlightConstraint& fcnst = fItem->GetFltType();
	const FlightConstraint& scnst = sItem->GetFltType();

	if (fcnst.fits(scnst))
		return false;
	else if(scnst.fits(fcnst))
		return true;

	return false;
}


void StandServiceList::SortFlightType()
{
	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeCompare);
}

void StandServiceList::DoNotCall()
{
	StandServiceList ssl(NULL);
	ssl.AddNewItem(NULL);
}