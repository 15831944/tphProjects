#include "stdAfx.h"
#include "ApproachSeparationType.h"
#include "AirsideImportExportManager.h"
#include "..\InputAirside\ALTObject.h"
#include "InTrailSeperationEx.h"
#include "../Database/ADODatabase.h"
#include "AirSector.h"

//////////////////////////////////////////////////////////////////////
// class CInTrailSeparationData
CInTrailSeparationDataEx::CInTrailSeparationDataEx()
: m_nID(-1)
,m_nInTrailSepID(-1)
,m_nMinDistance(5)
,m_nMinTime(2)
,m_pTrailDistribution(NULL)
,m_pTimeDistribution(NULL)
,m_pSpatialDisDistribution(NULL)
{
}
CInTrailSeparationDataEx::~CInTrailSeparationDataEx()
{
}

// classification item functions
void CInTrailSeparationDataEx::setClsTrailItem(AircraftClassificationItem clsTrailItem)
{
	m_clsTrailItem = clsTrailItem;
}
AircraftClassificationItem* CInTrailSeparationDataEx::getClsTrailItem()
{
	return &m_clsTrailItem;
}
void CInTrailSeparationDataEx::setClsLeadItem(AircraftClassificationItem clsLeadItem)
{
	m_clsLeadItem = clsLeadItem;
}
AircraftClassificationItem* CInTrailSeparationDataEx::getClsLeadItem()
{
	return &m_clsLeadItem;
}

void CInTrailSeparationDataEx::setCategoryType(FlightClassificationBasisType emCategoryType)
{
	m_emFltBasisType = emCategoryType;
}

FlightClassificationBasisType CInTrailSeparationDataEx::getCategoryType()
{
	return m_emFltBasisType;
}

// minimum distance functions
void CInTrailSeparationDataEx::setMinDistance(long nMinDistance)
{
	m_nMinDistance = nMinDistance;
}
int CInTrailSeparationDataEx::getMinDistance()
{	
	return m_nMinDistance;
}

// minimum time functions
void CInTrailSeparationDataEx::setMinTime(long nMinTime)
{
	m_nMinTime = nMinTime;
}
int CInTrailSeparationDataEx::getMinTime()
{
	return m_nMinTime;
}


void CInTrailSeparationDataEx::setSpatialDistName(CString strSpatialDistName)
{
	m_strSpatialDistName = strSpatialDistName;
}

CString& CInTrailSeparationDataEx::getSpatialDistName()
{
	return m_strSpatialDistName;
}

void CInTrailSeparationDataEx::setProbSpatialType(ProbTypes emProbType)
{
	m_emProbSpatialType = emProbType;
}

ProbTypes CInTrailSeparationDataEx::getProbSpatialType()
{
	return m_emProbSpatialType;
}

void CInTrailSeparationDataEx::setSpatialPrintDist(CString strSPatialPrintDist)
{
	m_strSpatialPrintDist = strSPatialPrintDist;
}

CString& CInTrailSeparationDataEx::getSpatialPrintDist()
{
	return m_strSpatialPrintDist;
}
void CInTrailSeparationDataEx::setTimeDistName(CString strTimeDistName)
{
	m_strTimeDistName = strTimeDistName;
}

CString& CInTrailSeparationDataEx::getTimeDistName()
{
	return m_strTimeDistName;
}

void CInTrailSeparationDataEx::SetProbTimeType(ProbTypes emProbType)
{
	m_emProbTimeType = emProbType;
}

ProbTypes CInTrailSeparationDataEx::getProbTimeType()
{
	return m_emProbTimeType;
}

void CInTrailSeparationDataEx::setTimePrintDist(CString strTimePrintDist)
{
	m_strTimePrintDist = strTimePrintDist;
}

CString& CInTrailSeparationDataEx::getTimePrintDist()
{
	return m_strTimePrintDist;
}

void CInTrailSeparationDataEx::setTrailDistName(CString strTrailDistName)
{
	m_strTrailDistName = strTrailDistName;
}

CString& CInTrailSeparationDataEx::getTrailDistName()
{
	return m_strTrailDistName;
}

void CInTrailSeparationDataEx::setProbTrailType(ProbTypes emProbType)
{
	m_emProbTrailType = emProbType;
}

ProbTypes CInTrailSeparationDataEx::getProbTrailType()
{
	return m_emProbTrailType;
}

void CInTrailSeparationDataEx::setTrailPrintDist(CString strTraiPrintDist)
{
	m_strTrailPrintDist = strTraiPrintDist; 
}

CString& CInTrailSeparationDataEx::getTrailPrintDist()
{
	return m_strTrailPrintDist;
}

void CInTrailSeparationDataEx::setSpatialConverging(long nSpatialConverging)
{
	m_nSpatialConverging = nSpatialConverging;
}

long CInTrailSeparationDataEx::getSpatialConverging()
{
	return m_nSpatialConverging;
}

void CInTrailSeparationDataEx::setSpatialDiverging(long nSpatialDiverging)
{
	m_nSpatialDiverging = nSpatialDiverging;
}

long CInTrailSeparationDataEx::getSpatialDiverging()
{
	return m_nSpatialDiverging;
}

void CInTrailSeparationDataEx::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("INTRAILID"),m_nInTrailSepID);
	int nTrailType = 0;
	adoRecordset.GetFieldValue(_T("TRAILTYPE"),nTrailType);
	m_clsTrailItem.setID(nTrailType);

	int nLeadType =0;
	adoRecordset.GetFieldValue(_T("LEADTYPE"),nLeadType);
	m_clsLeadItem.setID(nLeadType);

	adoRecordset.GetFieldValue(_T("MINDIST"),m_nMinDistance);
	adoRecordset.GetFieldValue(_T("MINTIME"),m_nMinTime);

	int nCateType =0;
	adoRecordset.GetFieldValue(_T("CATETYPE"),nCateType);
	m_emFltBasisType = (FlightClassificationBasisType)nCateType;

	adoRecordset.GetFieldValue(_T("SAPDIV"),m_nSpatialDiverging);
	adoRecordset.GetFieldValue(_T("SAPCON"),m_nSpatialConverging);
	
	adoRecordset.GetFieldValue(_T("TRAILDIS"),m_strTrailDistName);
	int nTrailProbType = 0;
	adoRecordset.GetFieldValue(_T("TRAPRO"),nTrailProbType);
	m_emProbTrailType = (ProbTypes)nTrailProbType;
	adoRecordset.GetFieldValue(_T("TRAPRIN"),m_strTrailPrintDist);

	adoRecordset.GetFieldValue(_T("TIMEDIS"),m_strTimeDistName);
	int nTimeProbType = 0;
	adoRecordset.GetFieldValue(_T("TIMPRO"),nTimeProbType);
	m_emProbTimeType = (ProbTypes)nTimeProbType;
	adoRecordset.GetFieldValue(_T("TIMPRIN"),m_strTimePrintDist);

	adoRecordset.GetFieldValue(_T("SAPADIS"),m_strSpatialDistName);
	int nSpatialProbType = 0;
	adoRecordset.GetFieldValue(_T("SPAPRO"),nSpatialProbType);
	m_emProbSpatialType = (ProbTypes)nSpatialProbType;
	adoRecordset.GetFieldValue(_T("SPAPRIN"),m_strSpatialPrintDist);
}
void CInTrailSeparationDataEx::SaveData(int nInTrailSepID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	m_nInTrailSepID = nInTrailSepID;

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_INTRAILSEP_DATAEX\
					 (INTRAILID, TRAILTYPE, LEADTYPE, MINDIST, MINTIME,CATETYPE,SAPDIV,SAPCON,TRAILDIS,TRAPRO,TRAPRIN,\
					 TIMEDIS,TIMPRO,TIMPRIN,SAPADIS,SPAPRO,SPAPRIN)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,'%s','%s',%d,'%s','%s',%d,'%s')"),
					 nInTrailSepID,m_clsTrailItem.getID(),m_clsLeadItem.getID(),m_nMinDistance,m_nMinTime,\
					 (int)m_emFltBasisType,m_nSpatialDiverging,m_nSpatialConverging,m_strTrailDistName,(int)m_emProbTrailType,\
					 m_strTrailPrintDist,m_strTimeDistName,(int)m_emProbTimeType,m_strTimePrintDist,m_strSpatialDistName,\
					 (int)m_emProbSpatialType,m_strSpatialPrintDist);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}
void CInTrailSeparationDataEx::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_INTRAILSEP_DATAEX\
					 SET TRAILTYPE =%d, LEADTYPE =%d, MINDIST =%d, MINTIME =%d,CATETYPE=%d,SAPDIV=%d,SAPCON=%d,TRAILDIS='%s',TRAPRO=%d,TRAPRIN='%s',\
					 TIMEDIS='%s',TIMPRO=%d,TIMPRIN='%s',SAPADIS='%s',SPAPRO=%d,SPAPRIN='%s' \
					 WHERE (ID = %d)"),
					 m_clsTrailItem.getID(),m_clsLeadItem.getID(),m_nMinDistance,m_nMinTime,\
					 (int)m_emFltBasisType,m_nSpatialDiverging,m_nSpatialConverging,m_strTrailDistName,(int)m_emProbTrailType,\
					 m_strTrailPrintDist,m_strTimeDistName,(int)m_emProbTimeType,m_strTimePrintDist,m_strSpatialDistName,\
					 (int)m_emProbSpatialType,m_strSpatialPrintDist,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CInTrailSeparationDataEx::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_INTRAILSEP_DATAEX\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CInTrailSeparationDataEx::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt((int)m_emFltBasisType);

	exportFile.getFile().writeInt(m_nMinDistance);
	exportFile.getFile().writeInt(m_nMinTime);

	exportFile.getFile().writeInt(m_nSpatialConverging);
	exportFile.getFile().writeInt(m_nSpatialDiverging);

	exportFile.getFile().writeInt(m_clsTrailItem.m_nID);
	exportFile.getFile().writeField(m_clsTrailItem.m_strName.GetBuffer(_MAX_PATH));
	m_clsTrailItem.m_strName.ReleaseBuffer(_MAX_PATH);

	exportFile.getFile().writeInt(m_clsLeadItem.m_nID);
	exportFile.getFile().writeField(m_clsLeadItem.m_strName.GetBuffer(_MAX_PATH));
	m_clsLeadItem.m_strName.ReleaseBuffer(_MAX_PATH);	

	exportFile.getFile().writeInt(m_strTrailDistName.GetLength()+1);
	exportFile.getFile().writeField(m_strTrailDistName.GetBuffer(m_strTrailDistName.GetLength()+1));
	m_strTrailDistName.ReleaseBuffer(m_strTrailDistName.GetLength()+1);

	exportFile.getFile().writeInt(m_strTimeDistName.GetLength()+1);
	exportFile.getFile().writeField(m_strTimeDistName.GetBuffer(m_strTimeDistName.GetLength()+1));
	m_strTimeDistName.ReleaseBuffer(m_strTimeDistName.GetLength()+1);

	exportFile.getFile().writeInt(m_strSpatialDistName.GetLength()+1);
	exportFile.getFile().writeField(m_strSpatialDistName.GetBuffer(m_strSpatialDistName.GetLength()+1));
	m_strSpatialDistName.ReleaseBuffer(m_strSpatialDistName.GetLength()+1);
	
	exportFile.getFile().writeInt(m_strTrailPrintDist.GetLength()+1);
	exportFile.getFile().writeField(m_strTrailPrintDist.GetBuffer(m_strTrailPrintDist.GetLength()+1));
	m_strTrailPrintDist.ReleaseBuffer(m_strTrailPrintDist.GetLength()+1);

	exportFile.getFile().writeInt(m_strTimePrintDist.GetLength()+1);
	exportFile.getFile().writeField(m_strTimePrintDist.GetBuffer(m_strTimePrintDist.GetLength()+1));
	m_strTimePrintDist.ReleaseBuffer(m_strTimePrintDist.GetLength()+1);

	exportFile.getFile().writeInt(m_strSpatialPrintDist.GetLength()+1);
	exportFile.getFile().writeField(m_strSpatialPrintDist.GetBuffer(m_strSpatialPrintDist.GetLength()+1));
	m_strSpatialPrintDist.ReleaseBuffer(m_strSpatialPrintDist.GetLength()+1);

	exportFile.getFile().writeInt((int)m_emProbTrailType);
	exportFile.getFile().writeInt((int)m_emProbTimeType);
	exportFile.getFile().writeInt((int)m_emProbSpatialType);

	exportFile.getFile().writeLine();
}


void CInTrailSeparationDataEx::ImportData(CAirsideImportFile& importFile)
{
	char * pBuffer = 0;
	int nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_emFltBasisType = (FlightClassificationBasisType)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nMinDistance = (int)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nMinTime = (int)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nSpatialConverging = (int)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nSpatialDiverging = (int)nBufferLen;
	nBufferLen = 0;

	int nID = -1;
	importFile.getFile().getInteger(nID);
	m_clsTrailItem.m_nID = importFile.GetAircraftClassificationsNewID(nID);
	char szName[_MAX_PATH] = {0};	
	importFile.getFile().getField(szName,_MAX_PATH);
	m_clsTrailItem.m_strName = szName;

	nID = -1;
	importFile.getFile().getInteger(nID);
	m_clsLeadItem.m_nID = importFile.GetAircraftClassificationsNewID(nID);
	memset(szName,0,_MAX_PATH);
	importFile.getFile().getField(szName,_MAX_PATH);
	m_clsLeadItem.m_strName = szName;
	
	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strTrailDistName = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strTimeDistName = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strSpatialDistName = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strTrailPrintDist = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strTimePrintDist = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strSpatialPrintDist = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_emProbTrailType = (ProbTypes)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_emProbTimeType = (ProbTypes)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_emProbSpatialType = (ProbTypes)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getLine();
}

ProbabilityDistribution* CInTrailSeparationDataEx::GetTrailProbDistribution(void)
{
	if(m_pTrailDistribution)
	{
		delete m_pTrailDistribution;
		m_pTrailDistribution = NULL;
	}

	switch(m_emProbTrailType) 
	{
	case BERNOULLI:
		m_pTrailDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pTrailDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pTrailDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pTrailDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pTrailDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pTrailDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pTrailDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pTrailDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pTrailDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pTrailDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pTrailDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pTrailDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(NULL == m_pTrailDistribution)
	{
		return NULL;
	}

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strTrailPrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pTrailDistribution->setDistribution(szprintDist);

	return m_pTrailDistribution;
}

ProbabilityDistribution* CInTrailSeparationDataEx::GetTimeProbDistribution(void)
{
	if(m_pTimeDistribution)
	{
		delete m_pTimeDistribution;
		m_pTimeDistribution = NULL;
	}

	switch(m_emProbTimeType) 
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
	if(NULL == m_pTimeDistribution)
	{
		return NULL;
	}

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strTimePrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pTimeDistribution->setDistribution(szprintDist);

	return m_pTimeDistribution;
}

ProbabilityDistribution* CInTrailSeparationDataEx::GetSpatialDisProbDistribution(void)
{
	if(m_pSpatialDisDistribution)
	{
		delete m_pSpatialDisDistribution;
		m_pSpatialDisDistribution = NULL;
	}

	switch(m_emProbSpatialType) 
	{
	case BERNOULLI:
		m_pSpatialDisDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pSpatialDisDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pSpatialDisDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pSpatialDisDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pSpatialDisDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pSpatialDisDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pSpatialDisDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pSpatialDisDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pSpatialDisDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pSpatialDisDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pSpatialDisDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pSpatialDisDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(NULL == m_pSpatialDisDistribution)
	{
		return NULL;
	}

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strSpatialPrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pSpatialDisDistribution->setDistribution(szprintDist);

	return m_pSpatialDisDistribution;
}

//////////////////////////////////////////////////////////////////////
// class CInTrailSeparation
CInTrailSeparationEx::CInTrailSeparationEx()
: m_nID(-1)
,m_nProjID(-1)
,m_vInTrailSepItem(NULL)
,m_vInTrailSepItemNeedDel(NULL)
{
	m_pClasNone = NULL;
	m_pClasSurfaceBearingWeightBased = NULL;
	m_pClasWakeVortexWightBased = NULL;
	m_pClasWingspanBased = NULL;
	m_pClasApproachSpeedBased = NULL;
}
CInTrailSeparationEx::CInTrailSeparationEx(PhaseType emPahseType,int nSectorID)
: m_nID(-1)
,m_nProjID(-1)
,m_vInTrailSepItem(NULL)
,m_vInTrailSepItemNeedDel(NULL)
{
	m_pClasNone = NULL;
	m_pClasSurfaceBearingWeightBased = NULL;
	m_pClasWakeVortexWightBased = NULL;
	m_pClasWingspanBased = NULL;
	m_pClasApproachSpeedBased = NULL;

	m_emPahseType = emPahseType;
	m_nSectorID = nSectorID;
}

CInTrailSeparationEx::CInTrailSeparationEx(int nProjID)
: m_nID(-1)
,m_nProjID(-1)
,m_vInTrailSepItem(NULL)
,m_vInTrailSepItemNeedDel(NULL)
{
	m_pClasNone = new AircraftClassifications(nProjID, NoneBased);
	m_pClasNone->ReadData();

	m_pClasWakeVortexWightBased = new AircraftClassifications(nProjID, WakeVortexWightBased);
	m_pClasWakeVortexWightBased->ReadData();

	m_pClasWingspanBased = new AircraftClassifications(nProjID, WingspanBased);
	m_pClasWingspanBased->ReadData();

	m_pClasSurfaceBearingWeightBased = new AircraftClassifications(nProjID, SurfaceBearingWeightBased);
	m_pClasSurfaceBearingWeightBased->ReadData();

	m_pClasApproachSpeedBased = new AircraftClassifications(nProjID, ApproachSpeedBased);
	m_pClasApproachSpeedBased->ReadData();
}
CInTrailSeparationEx::~CInTrailSeparationEx()
{
	if ( m_pClasNone )
	{
		delete m_pClasNone;
		m_pClasNone = NULL;
	}
	if ( m_pClasWakeVortexWightBased )
	{
		delete m_pClasWakeVortexWightBased;
		m_pClasWakeVortexWightBased = NULL;
	}
	if ( m_pClasWingspanBased )
	{
		delete m_pClasWingspanBased;
		m_pClasWingspanBased = NULL;
	}
	if ( m_pClasSurfaceBearingWeightBased )
	{
		delete m_pClasSurfaceBearingWeightBased;
		m_pClasSurfaceBearingWeightBased = NULL;
	}
	if ( m_pClasApproachSpeedBased )
	{
		delete m_pClasApproachSpeedBased;
		m_pClasApproachSpeedBased = NULL;
	}

	std::vector<CInTrailSeparationDataEx *>::iterator iter = m_vInTrailSepItem.begin();
	for(; iter != m_vInTrailSepItem.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vInTrailSepItemNeedDel.begin();
	for (; iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}

BOOL CInTrailSeparationEx::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, INTRAILID, TRAILTYPE, LEADTYPE, MINDIST, MINTIME ,CATETYPE,SAPDIV,SAPCON,TRAILDIS,TRAPRO,TRAPRIN,\
					 TIMEDIS,TIMPRO,TIMPRIN,SAPADIS,SPAPRO,SPAPRIN \
					 FROM IN_INTRAILSEP_DATAEX\
					 WHERE (INTRAILID = %d)"),m_nID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CInTrailSeparationDataEx * pData = new CInTrailSeparationDataEx();

		pData->ReadData(adoRecordset);
		m_vInTrailSepItem.push_back(pData);

		adoRecordset.MoveNextData();
	}

	return TRUE;
}
BOOL CInTrailSeparationEx::SaveData(int nProjID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	m_nProjID = nProjID;

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_INTRAILSEPARATIONEX\
					 (PROJID, PHASETYPE, CATETYPE, SECTORID,CONUDER,CONOVER,RADCON)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d)"),nProjID,(int)m_emPahseType,(int)m_emCategoryType,m_nSectorID,m_nConvergentUnder,m_nDivergentOver,m_nRadiusofConcer);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	std::vector<CInTrailSeparationDataEx *>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vInTrailSepItemNeedDel.begin();
	for (;iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepItemNeedDel.clear();
	return TRUE;
}
BOOL CInTrailSeparationEx::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_INTRAILSEPARATIONEX\
					 SET PHASETYPE =%d, CATETYPE =%d,SECTORID=%d,CONUDER=%d ,CONOVER=%d,RADCON=%d\
					 WHERE (ID = %d)"),(int)m_emPahseType,(int)m_emCategoryType,m_nSectorID,m_nConvergentUnder,m_nDivergentOver,m_nRadiusofConcer,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);


	std::vector<CInTrailSeparationDataEx *>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vInTrailSepItemNeedDel.begin();
	for (;iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepItemNeedDel.clear();
	return TRUE;
}
BOOL CInTrailSeparationEx::DeleteData()
{

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_INTRAILSEPARATIONEX\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CInTrailSeparationDataEx *>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
	iter = m_vInTrailSepItemNeedDel.begin();
	for (;iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		if(*iter){
			(*iter)->DeleteData();
			delete *iter;
		}
	}
	m_vInTrailSepItemNeedDel.clear();

	return TRUE;
}

// phase type functions
void CInTrailSeparationEx::setPhaseType(PhaseType emPahseType)
{
	m_emPahseType = emPahseType;
}
PhaseType CInTrailSeparationEx::getPhaseType()
{
	return m_emPahseType;
}

void CInTrailSeparationEx::setSectorID(int nSectorID)
{
	m_nSectorID = nSectorID;
}

int CInTrailSeparationEx::getSectorID()
{
	return m_nSectorID;
}

// category type functions
void CInTrailSeparationEx::setCategoryType(FlightClassificationBasisType emCategoryType)
{
	m_emCategoryType = emCategoryType;
}
FlightClassificationBasisType CInTrailSeparationEx::getCategoryType()
{
	return m_emCategoryType;
}

void CInTrailSeparationEx::setConvergentUnder(long nConvergentUnder)
{
	m_nConvergentUnder = nConvergentUnder;
}

long CInTrailSeparationEx::getConvergentUnder()
{
	return m_nConvergentUnder;
}

void CInTrailSeparationEx::setRadiusofConcer(long nRadiusofConcer)
{
	m_nRadiusofConcer = nRadiusofConcer;
}

long CInTrailSeparationEx::getRadiusofConcer()
{
	return m_nRadiusofConcer;
}
void CInTrailSeparationEx::setDivergentOver(long nDivergentOver)
{
	m_nDivergentOver = nDivergentOver;
}

long CInTrailSeparationEx::getDivergentOver()
{
	return m_nDivergentOver;
}
int CInTrailSeparationEx::GetItemCount()const
{
	return static_cast<int> (m_vInTrailSepItem.size());
}

CInTrailSeparationDataEx* CInTrailSeparationEx::GetItem(int nIndex)const
{
	ASSERT(nIndex < (int)m_vInTrailSepItem.size());
	return m_vInTrailSepItem.at( nIndex );
}

void CInTrailSeparationEx::AddItem(CInTrailSeparationDataEx* pInTrailSepItem)
{
	m_vInTrailSepItem.push_back( pInTrailSepItem );
}

void CInTrailSeparationEx::Deltem(int nIndex)
{
	ASSERT(nIndex < (int)m_vInTrailSepItem.size());
	m_vInTrailSepItemNeedDel.push_back( m_vInTrailSepItem[nIndex] );

	m_vInTrailSepItem.erase(m_vInTrailSepItem.begin() + nIndex);
}
void CInTrailSeparationEx::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	int nPhaseType = 0;
	adoRecordset.GetFieldValue(_T("PHASETYPE"),nPhaseType);
	m_emPahseType = (PhaseType)nPhaseType;

	int nCateType = 0;
	adoRecordset.GetFieldValue(_T("CATETYPE"),nCateType);

	if (nCateType >= WingspanBased && nCateType < CategoryType_Count)
	{
		m_emCategoryType = (FlightClassificationBasisType)nCateType;
	}
	else
		m_emCategoryType = WingspanBased;

	int nIsActive = 0;
	adoRecordset.GetFieldValue(_T("SECTORID"),m_nSectorID);
	adoRecordset.GetFieldValue(_T("RADCON"),m_nRadiusofConcer);
	adoRecordset.GetFieldValue(_T("CONOVER"),m_nDivergentOver);
	adoRecordset.GetFieldValue(_T("CONUDER"),m_nConvergentUnder);

	ReadData();
}

void CInTrailSeparationEx::DelDataByClass()
{
	std::vector<CInTrailSeparationDataEx*>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end();++iter)
	{
		if(*iter)
		{
			m_vInTrailSepItemNeedDel.push_back(*iter);
		}
	}
	m_vInTrailSepItem.clear();
}
CInTrailSeparationDataEx * CInTrailSeparationEx::GetItemByACClass( const AircraftClassificationItem* trailItem, const AircraftClassificationItem* leadItem ) const
{
	ASSERT(trailItem && leadItem);

	if(! (trailItem&& leadItem) )
	{
		return NULL;
	}

	for(int i=0 ;i< GetItemCount();i++)
	{
		CInTrailSeparationDataEx * sepData = GetItem(i);
		if(sepData)
		{
			if( sepData->getClsLeadItem()->getID() == leadItem->getID() && sepData->getClsTrailItem()->getID() == trailItem->getID() )
				return sepData;
		}
	}


	return NULL;
}

void CInTrailSeparationEx::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nSectorID);
	exportFile.getFile().writeInt((int)m_emCategoryType);
	exportFile.getFile().writeInt((int)m_emPahseType);
	exportFile.getFile().writeInt(m_nConvergentUnder);
	exportFile.getFile().writeInt(m_nDivergentOver);
	exportFile.getFile().writeInt(m_nRadiusofConcer);
	exportFile.getFile().writeInt(GetItemCount());
	
	int nIndex = 0;
	for (;nIndex != m_vInTrailSepItem.size();++nIndex)
	{	

		m_vInTrailSepItem.at(nIndex)->ExportData(exportFile);
	}
}

void CInTrailSeparationEx::ImportData(CAirsideImportFile& importFile)
{
	int nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nProjID =importFile.getNewProjectID();
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nSectorID = importFile.GetObjectNewID(nBufferLen);
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_emCategoryType = (FlightClassificationBasisType)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_emPahseType = (PhaseType)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nConvergentUnder = nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nDivergentOver = nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_nRadiusofConcer = nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	int nIndex = 0;
	for (;nIndex != nBufferLen;++nIndex)
	{
		CInTrailSeparationDataEx* pData = new CInTrailSeparationDataEx();
		pData->ImportData(importFile);
		m_vInTrailSepItem.push_back(pData);
	}

}

void CInTrailSeparationEx::InitNewData(int SectorID,PhaseType emPashType,FlightClassificationBasisType emCategoryType)
{
	switch(emPashType)
	{
	case AllPhase:
		m_emCategoryType = NoneBased;
		break;
	case CruisePhase:
		m_emCategoryType = WakeVortexWightBased;
		break;
	case TerminalPhase:
		m_emCategoryType = WakeVortexWightBased;
		break;
	case ApproachPhase:
		m_emCategoryType = WakeVortexWightBased;
		break;
	default:
		break;
	}
	m_nConvergentUnder = 12;
	m_nDivergentOver = 12;
	m_nRadiusofConcer = 10;
	switch ( emCategoryType )
	{
	case NoneBased:
		{
			if (GetItemCount() <= 0)
			{
				AircraftClassificationItem classificationItem(emCategoryType);
				classificationItem.setName( "Default" );
				CInTrailSeparationDataEx* pCurInTrailSepData = new CInTrailSeparationDataEx();
				pCurInTrailSepData->setInTrailSepID( m_nID );
				pCurInTrailSepData->setClsLeadItem( classificationItem );
				pCurInTrailSepData->setClsTrailItem( classificationItem );
				pCurInTrailSepData->setMinDistance(4);
				pCurInTrailSepData->setMinTime(1);
				pCurInTrailSepData->setCategoryType(NoneBased);
				pCurInTrailSepData->setTrailDistName("U[10~20]");
				pCurInTrailSepData->setTrailPrintDist("Uniform:10;20");
				pCurInTrailSepData->setProbTrailType(UNIFORM);
				pCurInTrailSepData->setTimeDistName("U[3~5]");
				pCurInTrailSepData->setTimePrintDist("Uniform:3;5");
				pCurInTrailSepData->SetProbTimeType(UNIFORM);
				pCurInTrailSepData->setSpatialDistName("U[6~9]");
				pCurInTrailSepData->setSpatialPrintDist("Uniform:6;9");
				pCurInTrailSepData->setProbSpatialType(UNIFORM);
				pCurInTrailSepData->setSpatialConverging(4);
				pCurInTrailSepData->setSpatialDiverging(4);
				AddItem( pCurInTrailSepData );
			}
		}
		return;
	case WakeVortexWightBased:
		m_pCurClassifications = m_pClasWakeVortexWightBased;
		break;
	case WingspanBased:
		m_pCurClassifications = m_pClasWingspanBased;
		break;
	case SurfaceBearingWeightBased:
		m_pCurClassifications = m_pClasSurfaceBearingWeightBased;
		break;
	case ApproachSpeedBased:
		m_pCurClassifications = m_pClasApproachSpeedBased;
		break;
	default:
		break;

	}

	int nCount = m_pCurClassifications->GetCount();

	for (int i = 0; i < nCount; i++)
	{
		for (int j = 0; j < nCount; j++)
		{
			BOOL IsItemExist = FALSE;
			for (int k = 0; k < GetItemCount(); k++)
			{
				CInTrailSeparationDataEx* pCurInTrailSepData = GetItem(k);
				int nClsTrailID = pCurInTrailSepData->getClsTrailItem()->m_nID;
				int nClsLeadID = pCurInTrailSepData->getClsLeadItem()->m_nID;

				if ( nClsTrailID == m_pCurClassifications->GetItem(i)->m_nID &&
					nClsLeadID == m_pCurClassifications->GetItem(j)->m_nID )
				{
					IsItemExist = TRUE;
					break;
				}
			}
			if ( !IsItemExist )
			{
				AircraftClassificationItem *pClsTrailItem = m_pCurClassifications->GetItem(i);
				AircraftClassificationItem *pClsLeadItem = m_pCurClassifications->GetItem(j);
				CInTrailSeparationDataEx* pCurInTrailSepData = new CInTrailSeparationDataEx();
				pCurInTrailSepData->setInTrailSepID( getID() );
				pCurInTrailSepData->setClsTrailItem( *pClsTrailItem );
				pCurInTrailSepData->setClsLeadItem( *pClsLeadItem );
				pCurInTrailSepData->setCategoryType(m_pCurClassifications->GetBasisType());
				pCurInTrailSepData->setTrailDistName("U[10~20]");
				pCurInTrailSepData->setTrailPrintDist("Uniform:10;20");
				pCurInTrailSepData->setProbTrailType(UNIFORM);
				pCurInTrailSepData->setTimeDistName("U[3~5]");
				pCurInTrailSepData->setTimePrintDist("Uniform:3;5");
				pCurInTrailSepData->SetProbTimeType(UNIFORM);
				pCurInTrailSepData->setSpatialDistName("U[6~9]");
				pCurInTrailSepData->setSpatialPrintDist("Uniform:6;9");
				pCurInTrailSepData->setProbSpatialType(UNIFORM);
				pCurInTrailSepData->setSpatialConverging(1);
				pCurInTrailSepData->setSpatialDiverging(1);

				CString strName;
				strName.Format("%s", pClsLeadItem->m_strName);
				strName.MakeLower();
				if (!strName.CompareNoCase("light"))
				{
					pCurInTrailSepData->setMinDistance(3);
				}
				else if (!strName.CompareNoCase("medium"))
				{
					pCurInTrailSepData->setMinDistance(4);
				}
				else if (!strName.CompareNoCase("heavy"))
				{
					pCurInTrailSepData->setMinDistance(5);
				}
				else
				{
					pCurInTrailSepData->setMinDistance(5);
				}

				pCurInTrailSepData->setMinTime(1);
				AddItem( pCurInTrailSepData );
			}
		}
	}
}

void CInTrailSeparationEx::CompleteData(FlightClassificationBasisType emCategoryType )
{
	AircraftClassifications* pCurClassifications = NULL;
	switch ( emCategoryType )
	{
	case NoneBased:
		{
			if (GetItemCount() <= 0)
			{
				AircraftClassificationItem classificationItem(m_emCategoryType);
				classificationItem.setName( "Default" );
				CInTrailSeparationDataEx* pCurInTrailSepData = new CInTrailSeparationDataEx();
				pCurInTrailSepData->setInTrailSepID( m_nID );
				pCurInTrailSepData->setClsLeadItem( classificationItem );
				pCurInTrailSepData->setClsTrailItem( classificationItem );
				pCurInTrailSepData->setMinDistance(4);
				pCurInTrailSepData->setMinTime(1);
				pCurInTrailSepData->setCategoryType(NoneBased);
				pCurInTrailSepData->setTrailDistName("U[10~20]");
				pCurInTrailSepData->setTrailPrintDist("Uniform:10;20");
				pCurInTrailSepData->setProbTrailType(UNIFORM);
				pCurInTrailSepData->setTimeDistName("U[3~5]");
				pCurInTrailSepData->setTimePrintDist("Uniform:3;5");
				pCurInTrailSepData->SetProbTimeType(UNIFORM);
				pCurInTrailSepData->setSpatialDistName("U[6~9]");
				pCurInTrailSepData->setSpatialPrintDist("Uniform:6;9");
				pCurInTrailSepData->setProbSpatialType(UNIFORM);
				pCurInTrailSepData->setSpatialConverging(4);
				pCurInTrailSepData->setSpatialDiverging(4);
				AddItem( pCurInTrailSepData );
			}
		}
		return;
	case WakeVortexWightBased:
		pCurClassifications = m_pClasWakeVortexWightBased;
		break;
	case WingspanBased:
		pCurClassifications = m_pClasWingspanBased;
		break;
	case SurfaceBearingWeightBased:
		pCurClassifications = m_pClasSurfaceBearingWeightBased;
		break;
	case ApproachSpeedBased:
		pCurClassifications = m_pClasApproachSpeedBased;
		break;
	default:
		break;
	}

	if(pCurClassifications)
	{
		int nCount = pCurClassifications->GetCount();
		//add new
		for (int i = 0; i < nCount; i++)
		{
			for (int j = 0; j < nCount; j++)
			{
				BOOL IsItemExist = FALSE;
				for (int k = 0; k < GetItemCount(); k++)
				{
					CInTrailSeparationDataEx* pCurInTrailSepData = GetItem(k);
					int nClsTrailID = pCurInTrailSepData->getClsTrailItem()->m_nID;
					int nClsLeadID = pCurInTrailSepData->getClsLeadItem()->m_nID;
					if(pCurInTrailSepData->getCategoryType()!=emCategoryType)
						continue;

					if ( nClsTrailID == pCurClassifications->GetItem(i)->m_nID &&
						nClsLeadID == pCurClassifications->GetItem(j)->m_nID )
					{
						IsItemExist = TRUE;
						break;
					}
				}
				if ( !IsItemExist )
				{
					AircraftClassificationItem *pClsTrailItem = pCurClassifications->GetItem(i);
					AircraftClassificationItem *pClsLeadItem = pCurClassifications->GetItem(j);
					CInTrailSeparationDataEx* pCurInTrailSepData = new CInTrailSeparationDataEx();
					pCurInTrailSepData->setInTrailSepID( getID() );
					pCurInTrailSepData->setClsTrailItem( *pClsTrailItem );
					pCurInTrailSepData->setClsLeadItem( *pClsLeadItem );
					pCurInTrailSepData->setCategoryType(pCurClassifications->GetBasisType());
					pCurInTrailSepData->setTrailDistName("U[10~20]");
					pCurInTrailSepData->setTrailPrintDist("Uniform:10;20");
					pCurInTrailSepData->setProbTrailType(UNIFORM);
					pCurInTrailSepData->setTimeDistName("U[3~5]");
					pCurInTrailSepData->setTimePrintDist("Uniform:3;5");
					pCurInTrailSepData->SetProbTimeType(UNIFORM);
					pCurInTrailSepData->setSpatialDistName("U[6~9]");
					pCurInTrailSepData->setSpatialPrintDist("Uniform:6;9");
					pCurInTrailSepData->setProbSpatialType(UNIFORM);
					pCurInTrailSepData->setSpatialConverging(1);
					pCurInTrailSepData->setSpatialDiverging(1);

					CString strName;
					strName.Format("%s", pClsLeadItem->m_strName);
					strName.MakeLower();
					if (!strName.CompareNoCase("light"))
					{
						pCurInTrailSepData->setMinDistance(3);
					}
					else if (!strName.CompareNoCase("medium"))
					{
						pCurInTrailSepData->setMinDistance(4);
					}
					else if (!strName.CompareNoCase("heavy"))
					{
						pCurInTrailSepData->setMinDistance(5);
					}
					else
					{
						pCurInTrailSepData->setMinDistance(5);
					}

					pCurInTrailSepData->setMinTime(1);
					AddItem( pCurInTrailSepData );
				}
			}
		}
		//remove invalid
		for (int k = 0; k < GetItemCount(); k++)
		{
			CInTrailSeparationDataEx* pCurInTrailSepData = GetItem(k);
			if(pCurInTrailSepData->getCategoryType()!=emCategoryType)
				continue;

			int nClsTrailID = pCurInTrailSepData->getClsTrailItem()->m_nID;
			int nClsLeadID = pCurInTrailSepData->getClsLeadItem()->m_nID;			

			if( pCurClassifications->GetItemByID(nClsTrailID)	&& pCurClassifications->GetItemByID(nClsLeadID) )
			{
			
			}
			else
			{
				Deltem(k);
				k--;
			}	
			
		}

	}


}
//////////////////////////////////////////////////////////////////////
// class CInTrailSeparationList
CInTrailSeparationListEx::CInTrailSeparationListEx()
:m_nProjID(-1)
,m_vInTrailSeparation(NULL)
,m_vInTrailSepNeedDel(NULL)
{
	m_nRadiusofConcer = 10;
}
CInTrailSeparationListEx::~CInTrailSeparationListEx()
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vInTrailSepNeedDel.begin();
	for (;iter != m_vInTrailSepNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}


void CInTrailSeparationListEx::setRadiusofConcer(long nRadiusofConcer)
{
	m_nRadiusofConcer = nRadiusofConcer;
}

long CInTrailSeparationListEx::getRadiusofConcer()
{
	return m_nRadiusofConcer;
}

BOOL CInTrailSeparationListEx::ReadData(int nProjID)
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID,PROJID, PHASETYPE, CATETYPE, SECTORID,CONUDER,CONOVER,RADCON\
					 FROM IN_INTRAILSEPARATIONEX\
					 WHERE (PROJID = %d)"),nProjID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);


	while (!adoRecordset.IsEOF())
	{
		CInTrailSeparationEx *pData = new CInTrailSeparationEx(nProjID);
		pData->ReadData(adoRecordset);
		setRadiusofConcer(pData->getRadiusofConcer());
		pData->CompleteData(NoneBased);
		pData->CompleteData(SurfaceBearingWeightBased);
		pData->CompleteData(WakeVortexWightBased);
		pData->CompleteData(ApproachSpeedBased);
		pData->CompleteData(WingspanBased);

		m_vInTrailSeparation.push_back(pData);
		adoRecordset.MoveNextData();
		if (adoRecordset.IsEOF())
		{
			return TRUE;
		}
	}
	if (adoRecordset.IsEOF()) 
	{
// 		ALTObject altObject;
// 		std::vector<ALTObject> vObject;
// 		altObject.setObjName(_T(""));
// 		altObject.setAptID(nProjID);
// 		int nProj = altObject.SaveObject(altObject.getAptID(),ALT_SECTOR);

		CInTrailSeparationEx* pInAllData = new CInTrailSeparationEx(nProjID);
		pInAllData->InitNewData(-1,AllPhase,NoneBased);
		pInAllData->InitNewData(-1,AllPhase,SurfaceBearingWeightBased);
		pInAllData->InitNewData(-1,AllPhase,WakeVortexWightBased);
		pInAllData->InitNewData(-1,AllPhase,ApproachSpeedBased);
		pInAllData->InitNewData(-1,AllPhase,WingspanBased);
		pInAllData->setPhaseType(AllPhase);
		pInAllData->setSectorID(-1);
		pInAllData->setCategoryType(WakeVortexWightBased);
		pInAllData->setConvergentUnder(12);
		pInAllData->setDivergentOver(12);
		pInAllData->setRadiusofConcer(m_nRadiusofConcer);
		pInAllData->setProjID(nProjID);
		m_vInTrailSeparation.push_back(pInAllData);

		SaveData(nProjID);
	}
	return TRUE;
}
BOOL CInTrailSeparationListEx::SaveData(int nProjID)
{
	std::vector<CInTrailSeparationEx *>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}
	
	iter = m_vInTrailSepNeedDel.begin();
	for (; iter != m_vInTrailSepNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepNeedDel.clear();
	return TRUE;
}

int CInTrailSeparationListEx::GetItemCount()
{
	return static_cast<int> (m_vInTrailSeparation.size());
}

CInTrailSeparationEx* CInTrailSeparationListEx::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vInTrailSeparation.size());
	return m_vInTrailSeparation.at( nIndex );
}

CInTrailSeparationEx* CInTrailSeparationListEx::GetItemByType(int nSectorID, PhaseType emPahseType)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		if (emPahseType == (*iter)->getPhaseType()
			&& nSectorID == (*iter)->getSectorID())
		{
			return (*iter);
		}		
	}
	return NULL;
}

CInTrailSeparationEx* CInTrailSeparationListEx::GetSectorAllItemByPhaseType(PhaseType emPahseType)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		int nSectorID = (*iter)->getSectorID();
		if(nSectorID == -1)
			return (*iter);

		AirSector airSector;
		airSector.ReadObject(nSectorID);

		ALTObjectID objName;
		airSector.getObjName(objName);

		//all sector
		if (objName.GetIDString().IsEmpty())
		{
			if ((*iter)->getPhaseType() == emPahseType || (*iter)->getPhaseType() == AllPhase)
			{
				return (*iter);
			}
		}	
	}

	return NULL;
}

CInTrailSeparationEx* CInTrailSeparationListEx::GetItemByType(int nSectorID)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		if (nSectorID == (*iter)->getSectorID())
		{
			return (*iter);
		}		
	}
	return NULL;
}

void CInTrailSeparationListEx::ModifyRadiusofConcer(long nRadiusofConcer)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for (;iter != m_vInTrailSeparation.end();++iter)
	{
		(*iter)->setRadiusofConcer(nRadiusofConcer);
	}
}

void CInTrailSeparationListEx::AddItem(CInTrailSeparationEx* pInTrailSep)
{
	m_vInTrailSeparation.push_back(pInTrailSep);    
}

void CInTrailSeparationListEx::Deltem(int nIndex)
{
	ASSERT(nIndex < (int)m_vInTrailSeparation.size());
	m_vInTrailSepNeedDel.push_back( m_vInTrailSeparation[nIndex] );

	m_vInTrailSeparation.erase(m_vInTrailSeparation.begin() + nIndex);
}

void CInTrailSeparationListEx::DeltemBySector(int nSectorID)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	int nIndex = 0;
	for (;iter != m_vInTrailSeparation.end();++iter)
	{
		if (nSectorID == (*iter)->getSectorID())
		{
			(*iter)->DelDataByClass();
			Deltem(nIndex);
			DeltemBySector(nSectorID);
			return;
		}
		nIndex++;
	}
}

void CInTrailSeparationListEx::MoidfyBySector(int nRSectorID,int nDSectorID)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for (;iter != m_vInTrailSeparation.end();++iter)
	{
		if (nRSectorID == (*iter)->getSectorID())
		{
			(*iter)->setSectorID(nDSectorID);
		}
	}
}

BOOL CInTrailSeparationListEx::CheckBySector(int nSectorID)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for (;iter != m_vInTrailSeparation.end();++iter)
	{
		if (nSectorID == (*iter)->getSectorID())
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CInTrailSeparationListEx::CheckByPhasType(PhaseType emPahseType,int nSectorID)
{
	std::vector<CInTrailSeparationEx*>::iterator iter = m_vInTrailSeparation.begin();
	for (;iter != m_vInTrailSeparation.end();++iter)
	{
		if (nSectorID == (*iter)->getSectorID())
		{
			if (emPahseType == (*iter)->getPhaseType())
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CInTrailSeparationListEx::IsAllSectorAndAllPhaseExist(BOOL& bIsAllSectorExist, BOOL& bIsAllPhaseExist)
{
	bIsAllSectorExist = FALSE;
	bIsAllPhaseExist  = FALSE;
}

void CInTrailSeparationListEx::ExportData(CAirsideExportFile& exportFile)
{
	int nIndex = 0;
	exportFile.getFile().writeInt(GetItemCount());
	for (;nIndex !=GetItemCount() ;++nIndex)
	{
		m_vInTrailSeparation.at(nIndex)->ExportData(exportFile);
	}
	exportFile.getFile().endFile();
}

void CInTrailSeparationListEx::ImportData(CAirsideImportFile& importFile)
{
	int nBufferLen = 0;
	m_nProjID = importFile.getNewProjectID();
	importFile.getFile().getInteger(nBufferLen);
 	if (nBufferLen >= 1)
 	{
//  		ALTObject altObject;
//  		std::vector<ALTObject> vObject;
//  		altObject.setObjName(_T(""));
//  		altObject.setAptID(m_nProjID);
//  		int nProj = altObject.SaveObject(altObject.getAptID(),ALT_SECTOR);
 
 		CInTrailSeparationEx* pInAllData = new CInTrailSeparationEx(m_nProjID);
 		pInAllData->InitNewData(-1,AllPhase,NoneBased);
 		pInAllData->InitNewData(-1,AllPhase,SurfaceBearingWeightBased);
 		pInAllData->InitNewData(-1,AllPhase,WakeVortexWightBased);
 		pInAllData->InitNewData(-1,AllPhase,ApproachSpeedBased);
 		pInAllData->InitNewData(-1,AllPhase,NoneBased);
 		pInAllData->setPhaseType(AllPhase);
 		pInAllData->setSectorID(-1);
 		pInAllData->setCategoryType(WakeVortexWightBased);
 		pInAllData->setConvergentUnder(12);
 		pInAllData->setDivergentOver(12);
 		pInAllData->setRadiusofConcer(m_nRadiusofConcer);
 		pInAllData->setProjID(m_nProjID);
 		m_vInTrailSeparation.push_back(pInAllData);
 
 		SaveData(m_nProjID);
	}
	
	int nIndex = 0;
	for (;nIndex !=nBufferLen ;++nIndex)
	{
		CInTrailSeparationEx* pInTrailSep =  new CInTrailSeparationEx();
		 pInTrailSep->ImportData(importFile);
		 m_vInTrailSeparation.push_back(pInTrailSep);
	}
	importFile.getFile().getLine();
}

void CInTrailSeparationListEx::ImportInTrailSeparation(CAirsideImportFile& importFile)
{
	CInTrailSeparationListEx inTrailSeparation;
	inTrailSeparation.ImportData(importFile);
	inTrailSeparation.SaveData(importFile.getNewProjectID());
}

void CInTrailSeparationListEx::ExportInTrailSeparation(CAirsideExportFile& exportFile)
{ 
	exportFile.getFile().writeField(_T("InTrailSeparation"));
	exportFile.getFile().writeLine();
	CInTrailSeparationListEx exportInTrailSepatation;
	exportInTrailSepatation.ReadData(exportFile.GetProjectID());
	exportInTrailSepatation.ExportData(exportFile);
	exportFile.getFile().endFile();
}