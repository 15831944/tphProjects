#include "StdAfx.h"
#include ".\servicingrequirement.h"
#include "AirsideImportExportManager.h"
#include "ProjectDB.h"
#include "..\Database\ADODatabase.h"
#include "..\inputs\probab.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"

CServicingRequirement::CServicingRequirement(void)
: m_nServicingRequirementNameID(-1)
, m_nNum(0)
, m_strServiceTime(_T("")) 
, m_strDistScreenPrint(_T("Uniform: [5.00, 10.00]"))
, m_enumProbType(UNIFORM)
, m_strprintDist(_T("Uniform:5.00;10.00"))
, m_subServiceTime(_T(""))
, m_subDistScreenPrint(_T("Uniform: [5.00, 10.00]"))
, m_subProbType(UNIFORM)
, m_subPrintDist(_T("Uniform:5.00;10.00"))
, m_nFltTypeID(-1)
, m_pDistribution(NULL)
, m_nConditionType(enumVehicleTypeCondition_PerVehicle)
,m_pSubDistribution(NULL)
{
}

CServicingRequirement::~CServicingRequirement(void)
{
	delete m_pDistribution;
	m_pDistribution = NULL;
}

void CServicingRequirement::SetConditionType(enumVehicleTypeCondition nConditionType)
{
	m_nConditionType = nConditionType;
}

enumVehicleTypeCondition  CServicingRequirement::GetConditionType(void)
{
	return m_nConditionType;
}

void CServicingRequirement::SetServicingRequirementNameID(int strServicingRequirementNameID)
{
	m_nServicingRequirementNameID = strServicingRequirementNameID;
}

int CServicingRequirement::GetServicingRequirementNameID()
{
	return m_nServicingRequirementNameID;
}

void CServicingRequirement::SetNum(int nNum)
{
	m_nNum = nNum;
}

int CServicingRequirement::GetNum()
{
	return m_nNum;
}

void CServicingRequirement::SetServiceTime(CString& strServiceTime)
{
	m_strServiceTime = strServiceTime;
}

CString& CServicingRequirement::GetServiceTime()
{
	return m_strServiceTime;
}

void CServicingRequirement::SetSubServiceTime(CString& strServiceTime)
{
	m_subServiceTime = strServiceTime;
}

CString& CServicingRequirement::GetSubServiceTime()
{
	return m_subServiceTime;
}

void CServicingRequirement::SetFltTypeID(int nFltTypeID)
{
	m_nFltTypeID = nFltTypeID;
}

int CServicingRequirement::GetFltTypeID()
{
	return m_nFltTypeID;
}

void CServicingRequirement::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("SERVICEREQUIREMENTNAMEID"),m_nServicingRequirementNameID);
	recordset.GetFieldValue(_T("SERVICEREQUIREMENTNUM"),m_nNum);
	recordset.GetFieldValue(_T("FLTTYPEID"),m_nFltTypeID);

	recordset.GetFieldValue(_T("SERVICETIME"),m_strServiceTime);
	recordset.GetFieldValue(_T("DISTSCREENPRINT"),m_strDistScreenPrint);
	int nProbType = 0;
	recordset.GetFieldValue(_T("PROBTYPE"),nProbType);
	m_enumProbType = (ProbTypes)nProbType;
	recordset.GetFieldValue(_T("PRINTDIST"),m_strprintDist);

	recordset.GetFieldValue(_T("SUBSERVICETIME"),m_subServiceTime);
	recordset.GetFieldValue(_T("SUBDISTSCREENPRINT"),m_subDistScreenPrint);
	nProbType = 0;
	recordset.GetFieldValue(_T("SUBPROBTYPE"),nProbType);
	m_subProbType = (ProbTypes)nProbType;
	recordset.GetFieldValue(_T("SUBPRINTDIST"),m_subPrintDist);

	int nCondition = 0;
	recordset.GetFieldValue(_T("CONDITION"), nCondition);
	if (nCondition >= enumVehicleTypeCondition_Per100Liters && nCondition < enumVehicleTypeCondition_Count)
	{
		m_nConditionType = (enumVehicleTypeCondition)nCondition;
	}
	else
	{
		m_nConditionType = enumVehicleTypeCondition_PerVehicle;
	}

	if(m_subPrintDist.IsEmpty())
	{
		m_subServiceTime= (_T(""));
		m_subDistScreenPrint=(_T("Uniform: [5.00, 10.00]"));
		m_subProbType=(UNIFORM);
		m_subPrintDist=(_T("Uniform:5.00;10.00"));
	}

}

void CServicingRequirement::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_SERVICINGREQUIREMENT\
					 (FLTTYPEID, SERVICEREQUIREMENTNAMEID, SERVICEREQUIREMENTNUM, SERVICETIME,DISTSCREENPRINT,PROBTYPE,PRINTDIST,SUBSERVICETIME,SUBDISTSCREENPRINT,SUBPROBTYPE,SUBPRINTDIST,CONDITION)\
					 VALUES (%d, %d, %d,'%s','%s',%d,'%s','%s','%s',%d,'%s',%d)"),
					 m_nFltTypeID,
					 m_nServicingRequirementNameID,
					 m_nNum,
					 m_strServiceTime,
					 m_strDistScreenPrint,
					 (int)m_enumProbType,
					 m_strprintDist,
					 m_subServiceTime,
					 m_subDistScreenPrint,
					 (int)m_subProbType,
					 m_subPrintDist,
					 (int)m_nConditionType
					 );
}

void CServicingRequirement::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_SERVICINGREQUIREMENT\
					 SET FLTTYPEID = %d, \
					 SERVICEREQUIREMENTNAMEID = %d, \
					 SERVICEREQUIREMENTNUM = %d, \
					 SERVICETIME = '%s', \
					 DISTSCREENPRINT ='%s', \
					 PROBTYPE =%d, \
					 PRINTDIST ='%s', \
					 SUBSERVICETIME = '%s', \
					 SUBDISTSCREENPRINT ='%s', \
					 SUBPROBTYPE =%d, \
					 SUBPRINTDIST ='%s', \
					 CONDITION = %d \
					 WHERE (ID =%d)"),
					 m_nFltTypeID,
					 m_nServicingRequirementNameID,
					 m_nNum,
					 m_strServiceTime,
					 m_strDistScreenPrint,
					 (int)m_enumProbType,
					 m_strprintDist,
					 m_subServiceTime,
					 m_subDistScreenPrint,
					 (int)m_subProbType,
					 m_subPrintDist,
					 m_nConditionType,
					 m_nID);
}

void CServicingRequirement::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_SERVICINGREQUIREMENT\
					 WHERE (ID = %d)"),m_nID);
}

void CServicingRequirement::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nServicingRequirementNameID);
	exportFile.getFile().writeInt(m_nNum);
	exportFile.getFile().writeInt(m_nConditionType);
	exportFile.getFile().writeField(m_strServiceTime);
	exportFile.getFile().writeField(m_subServiceTime);
//	exportFile.getFile().writeField(m_strDistScreenPrint);
	exportFile.getFile().writeInt((int)m_enumProbType);
//	exportFile.getFile().writeField(m_strprintDist);
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pAirportDB->getProbDistMan();
	CProbDistEntry* pPDEntry = NULL;
	int nCount = static_cast<int>(pProbDistMan->getCount());

	int i=0;
	for( ; i<nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if( strcmp( pPDEntry->m_csName, m_strServiceTime ) == 0 )
			break;
	}
	i=0;
	for( ; i<nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if( strcmp( pPDEntry->m_csName, m_subServiceTime ) == 0 )
			break;
	}
	if(i>=nCount)
	{
		pProbDist = new UniformDistribution;
		char szprintDist[1024] = {0};
		const char *endOfName = strstr (m_strprintDist, ":");
		strcpy_s(szprintDist,endOfName + 1);
		pProbDist->setDistribution(szprintDist);
		pProbDist->writeDistribution(exportFile.getFile());
		delete pProbDist;
	}
	else
	{
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		pProbDist->writeDistribution(exportFile.getFile());
	}
	exportFile.getFile().writeLine();
}

void CServicingRequirement::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nstrServicingRequirementNameID = -1;
	importFile.getFile().getInteger(nstrServicingRequirementNameID);
	m_nServicingRequirementNameID = importFile.GetVehicleDataSpecificationNewID(nstrServicingRequirementNameID);

	importFile.getFile().getInteger(m_nNum);
	int nCondition = 0;
	importFile.getFile().getInteger(nCondition);
	if (nCondition >= enumVehicleTypeCondition_Per100Liters && nCondition < enumVehicleTypeCondition_Count)
	{
		m_nConditionType = (enumVehicleTypeCondition)nCondition;
	}
	else
	{
		m_nConditionType = enumVehicleTypeCondition_PerVehicle;
	}

	importFile.getFile().getField(m_strServiceTime.GetBuffer(1024),1024);
	importFile.getFile().getField(m_subServiceTime.GetBuffer(1024),1024);
//	importFile.getFile().getField(m_strDistScreenPrint.GetBuffer(1024),1024);

	int nProbType = -1;
	importFile.getFile().getInteger(nProbType);
	m_enumProbType = (ProbTypes)nProbType;

//	importFile.getFile().getField(m_strprintDist.GetBuffer(1024),1024);
	ProbabilityDistribution *pProbDist;
	switch(m_enumProbType) 
	{
	case BERNOULLI:
		pProbDist = new BernoulliDistribution;
		break;
	case BETA:
		pProbDist = new BetaDistribution;
		break;
	case CONSTANT:
		pProbDist = new ConstantDistribution;
		break;
	case EMPIRICAL:
		pProbDist = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		pProbDist = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		pProbDist = new HistogramDistribution;
		break;
	case NORMAL:
		pProbDist = new NormalDistribution;
		break;
	case UNIFORM:
		pProbDist = new UniformDistribution;
		break;
	case WEIBULL:
		pProbDist = new WeibullDistribution;
		break;
	case GAMMA:
		pProbDist = new GammaDistribution;
		break;
	case ERLANG:
		pProbDist = new ErlangDistribution;
		break;
	case TRIANGLE:
		pProbDist = new TriangleDistribution;
		break;
	default:
		break;
	}
	pProbDist->readDistribution(importFile.getFile());
	char szBuffer[1024] = {0};
	pProbDist->screenPrint(szBuffer);
	m_strDistScreenPrint = szBuffer;

	pProbDist->printDistribution(szBuffer);
	m_strprintDist = szBuffer;

	delete pProbDist;


	nProbType = -1;
	importFile.getFile().getInteger(nProbType);
	m_subProbType = (ProbTypes)nProbType;
	ProbabilityDistribution *pSubProbDist;
	switch(m_subProbType) 
	{
	case BERNOULLI:
		pSubProbDist = new BernoulliDistribution;
		break;
	case BETA:
		pSubProbDist = new BetaDistribution;
		break;
	case CONSTANT:
		pSubProbDist = new ConstantDistribution;
		break;
	case EMPIRICAL:
		pSubProbDist = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		pSubProbDist = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		pSubProbDist = new HistogramDistribution;
		break;
	case NORMAL:
		pSubProbDist = new NormalDistribution;
		break;
	case UNIFORM:
		pSubProbDist = new UniformDistribution;
		break;
	case WEIBULL:
		pSubProbDist = new WeibullDistribution;
		break;
	case GAMMA:
		pSubProbDist = new GammaDistribution;
		break;
	case ERLANG:
		pSubProbDist = new ErlangDistribution;
		break;
	case TRIANGLE:
		pSubProbDist = new TriangleDistribution;
		break;
	default:
		break;
	}
	pSubProbDist->readDistribution(importFile.getFile());
	char szSubBuffer[1024] = {0};
	pSubProbDist->screenPrint(szSubBuffer);
	m_subDistScreenPrint = szSubBuffer;

	pSubProbDist->printDistribution(szSubBuffer);
	m_subPrintDist = szSubBuffer;

	delete pSubProbDist;

	SaveDataToDB();

	importFile.getFile().getLine();
}

void CServicingRequirement::DeleteDataFromDB()
{
	CString strSelectSQL;
	GetDeleteSQL(strSelectSQL);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);
}

void CServicingRequirement::SaveDataToDB()
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(strSQL);

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

void CServicingRequirement::DeleteData()
{
}

ProbabilityDistribution* CServicingRequirement::GetServiceTimeDistribution()
{
	if(m_pDistribution)
	{
		delete m_pDistribution;
	}

	switch(m_enumProbType) 
	{
	case BERNOULLI:
		m_pDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strprintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pDistribution->setDistribution(szprintDist);
	return m_pDistribution;
}

ProbabilityDistribution* CServicingRequirement::GetSubServiceTimeDistribution()
{
	if(m_pSubDistribution)
	{
		delete m_pSubDistribution;
	}
	if(m_subPrintDist.IsEmpty())
		return NULL;

	switch(m_subProbType) 
	{
	case BERNOULLI:
		m_pSubDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pSubDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pSubDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pSubDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pSubDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pSubDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pSubDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pSubDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pSubDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pSubDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pSubDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pSubDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}

	char subPrintDist[1024] = {0};
	const char *endOfName = strstr (m_subPrintDist, ":");

	strcpy_s(subPrintDist,endOfName + 1);

	m_pSubDistribution->setDistribution(subPrintDist);
	return m_pSubDistribution;
}