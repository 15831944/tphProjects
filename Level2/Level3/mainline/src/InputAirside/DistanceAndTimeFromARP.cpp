#include "StdAfx.h"
#include ".\distanceandtimefromarp.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/ProbDistManager.h"

CDistanceAndTimeFromARP::CDistanceAndTimeFromARP(void)
: m_enumProbType(UNIFORM)
{
	m_nFltTypeID = 0;
	m_nDistanceFromARP = 30;
	m_strDistScreenPrint.Format(_T("Uniform: [10.00, 15.00]"));
	m_strprintDist.Format(_T("Uniform:10.00;15.00"));
	m_pDistribution = NULL;
}

CDistanceAndTimeFromARP::~CDistanceAndTimeFromARP(void)
{
	if (NULL != m_pDistribution)
	{
		delete m_pDistribution;
		m_pDistribution = NULL;
	}
}

void CDistanceAndTimeFromARP::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("FLTTYPEID"),m_nFltTypeID);
	recordset.GetFieldValue(_T("DISTANCEFROMARP"),m_nDistanceFromARP);
	recordset.GetFieldValue(_T("DISTSCREENPRINT"),m_strDistScreenPrint);
	int nProbType = 0;
	recordset.GetFieldValue(_T("PROBTYPE"),nProbType);
	m_enumProbType = (ProbTypes)nProbType;
	recordset.GetFieldValue(_T("PRINTDIST"),m_strprintDist);
	recordset.GetFieldValue(_T("TIME"),m_strTime);
}

void CDistanceAndTimeFromARP::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_DISTANCEANDTIMEFROMARP\
					 (FLTTYPEID, DISTANCEFROMARP, DISTSCREENPRINT, PROBTYPE, PRINTDIST,[TIME])\
					 VALUES (%d, %d, '%s', %d, '%s', '%s')"),\
					 m_nFltTypeID,
					 m_nDistanceFromARP,
					 m_strDistScreenPrint,
					 (int)m_enumProbType,
					 m_strprintDist,
					 m_strTime);
}

void CDistanceAndTimeFromARP::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_DISTANCEANDTIMEFROMARP\
					 SET FLTTYPEID = %d, \
					 DISTANCEFROMARP = %d, \
					 DISTSCREENPRINT ='%s', \
					 PROBTYPE =%d, \
					 PRINTDIST ='%s',\
					 [TIME]='%s' \
					 WHERE (ID =%d)"),\
					 m_nFltTypeID,
					 m_nDistanceFromARP,
					 m_strDistScreenPrint,
					 (int)m_enumProbType,
					 m_strprintDist,
					 m_strTime,
					 m_nID);
}

void CDistanceAndTimeFromARP::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_DISTANCEANDTIMEFROMARP\
					 WHERE (ID = %d)"),m_nID);
}

void CDistanceAndTimeFromARP::SaveData(int nID)
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

void CDistanceAndTimeFromARP::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nFltTypeID);
	exportFile.getFile().writeInt(m_nDistanceFromARP);

	exportFile.getFile().writeField(m_strTime);
	exportFile.getFile().writeInt((int)m_enumProbType);
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pAirportDatabase->getProbDistMan();
	CProbDistEntry* pPDEntry = NULL;
	int nCount = static_cast<int>(pProbDistMan->getCount());

	int i=0; 
	for(; i<nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if( strcmp( pPDEntry->m_csName, m_strTime ) == 0 )
			break;
	}
	if(i>=nCount)
	{
		pProbDist = new UniformDistribution;
		char szprintDist[1024] = {0};
		const char *endOfName = strstr (m_strprintDist, ":");
		strcpy(szprintDist,endOfName + 1);
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

void CDistanceAndTimeFromARP::ImportData(CAirsideImportFile& importFile)
{
	int nOldID;
	importFile.getFile().getInteger(nOldID);

	int nOldFlightTypeID;
	importFile.getFile().getInteger(nOldFlightTypeID);

	importFile.getFile().getInteger(m_nDistanceFromARP);

	importFile.getFile().getField(m_strTime.GetBuffer(1024),1024);

	int nProbType = -1;
	importFile.getFile().getInteger(nProbType);
	m_enumProbType = (ProbTypes)nProbType;

	//	importFile.getFile().getField(m_strprintDist.GetBuffer(1024),1024);
	ProbabilityDistribution* pProbDist;
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

	importFile.getFile().getLine();

	CString strSQL = _T("");
	GetInsertSQL(strSQL);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CDistanceAndTimeFromARP::SetTime(CString& strTime)
{
	m_strTime = strTime;
}

CString& CDistanceAndTimeFromARP::GetTime()
{
	return m_strTime;
}

void CDistanceAndTimeFromARP::SetProbTypes(ProbTypes enumProbType)
{
	m_enumProbType = enumProbType;
}

void CDistanceAndTimeFromARP::SetPrintDist(CString strprintDist)
{
	m_strprintDist = strprintDist;
}

ProbabilityDistribution* CDistanceAndTimeFromARP::GetServiceTimeDistribution()
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

	strcpy(szprintDist,endOfName + 1);

	m_pDistribution->setDistribution(szprintDist);
	return m_pDistribution;
}

void CDistanceAndTimeFromARP::SetDistScreenPrint(CString strScreenPrint)                
{
	m_strDistScreenPrint = strScreenPrint;
}

CString CDistanceAndTimeFromARP::GetDistScreenPrint() 
{
	return m_strDistScreenPrint;
}