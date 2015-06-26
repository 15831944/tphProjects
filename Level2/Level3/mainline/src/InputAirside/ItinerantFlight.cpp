#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "ItinerantFlight.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectGroup.h"
#include "ALTObject.h"

#include "..\inputs\probab.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"


ItinerantFlight::ItinerantFlight()
:DBElement()
,m_strCode(_T(""))
,m_bEnRoute(FALSE)
,m_nEntryID(-1)
,m_nEntryFlag(0)
,m_nExitID(-1)
,m_nExitFlag(0)
,m_nStandID(-1)
,m_startArrTime(0L)
,m_endArrTime(0L)
,m_startDepTime(0L)
,m_endDepTime(0L)
,m_nFlightCount(1)
,m_strArrDistName(_T(""))
,m_strDepDistName(_T(""))
,m_strArrDistScreenPrint(_T(""))
,m_strDepDistScreenPrint(_T(""))
,m_enumArrProbType(CONSTANT)
,m_enumDepProbType(CONSTANT)
,m_strArrprintDist(_T("Constant:0.00"))
,m_strDepprintDist(_T("Constant:0.00"))
,m_strOrig(_T(""))
,m_strDest(_T(""))
,m_strACType(_T(""))
{
	m_pDistribution = NULL;
}

ItinerantFlight::~ItinerantFlight()
{
	delete m_pDistribution;
	m_pDistribution = NULL;
}

void ItinerantFlight::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("NAME"),m_strCode);
	int bEnRoute = 0;
	recordset.GetFieldValue(_T("ENROUTE"),bEnRoute);
	m_bEnRoute = (BOOL)bEnRoute;

// 	CString strFltType;
// 	recordset.GetFieldValue(_T("FLTTYPE"),strFltType);
// 	m_fltType.SetAirportDB(m_pAirportDB);
// 	m_fltType.setConstraint(strFltType,VERSION_CONSTRAINT_CURRENT);

	recordset.GetFieldValue(_T("ENTRYID"),m_nEntryID);
	recordset.GetFieldValue(_T("ENTRYFLAG"),m_nEntryFlag);
	recordset.GetFieldValue(_T("EXITID"),m_nExitID);
	recordset.GetFieldValue(_T("EXITFLAG"),m_nExitFlag);
	//recordset.GetFieldValue(_T("STANDID"),m_nStandID);
	
	long nStartArrTime = 0;
	recordset.GetFieldValue(_T("STARTARRTIME"),nStartArrTime);
	m_startArrTime.set(nStartArrTime);
	
	long nStartDepTime = 0;
	recordset.GetFieldValue(_T("STARTDEPTIME"),nStartDepTime);
	m_startDepTime.set(nStartDepTime);

	long nEndArrTime = 0;
	recordset.GetFieldValue(_T("ENDARRTIME"),nEndArrTime);
	m_endArrTime.set(nEndArrTime);

	long nEndDepTime = 0;
	recordset.GetFieldValue(_T("ENDDEPTIME"),nEndDepTime);
	m_endDepTime.set(nEndDepTime);	

	recordset.GetFieldValue(_T("FLTCOUNT"),m_nFlightCount);

	recordset.GetFieldValue(_T("ARRDISTNAME"),m_strArrDistName);
	recordset.GetFieldValue(_T("DEPDISTNAME"),m_strDepDistName);

	recordset.GetFieldValue(_T("ARRDISTSCREENPRINT"),m_strArrDistScreenPrint);
	recordset.GetFieldValue(_T("DEPDISTSCREENPRINT"),m_strDepDistScreenPrint);
	
	int nProbType = 0;
	recordset.GetFieldValue(_T("ARRPROBTYPE"),nProbType);
	m_enumArrProbType = (ProbTypes)nProbType;

	nProbType = 0;
	recordset.GetFieldValue(_T("DEPPROBTYPE"),nProbType);
	m_enumDepProbType = (ProbTypes)nProbType;

	recordset.GetFieldValue(_T("ARRPRINTDIST"),m_strArrprintDist);
	recordset.GetFieldValue(_T("DEPPRINTDIST"),m_strDepprintDist);
	recordset.GetFieldValue(_T("ACTYPE"),m_strACType);
	recordset.GetFieldValue(_T("ORIGIN"),m_strOrig);
	recordset.GetFieldValue(_T("DEST"),m_strDest);
}

void ItinerantFlight::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_ITINERANTFLIGHT\
		(PROJID, NAME, ENROUTE, ENTRYID,ENTRYFLAG, EXITID, EXITFLAG, STARTARRTIME,STARTDEPTIME,\
		ENDARRTIME,ENDDEPTIME, FLTCOUNT, \
		ARRDISTNAME,DEPDISTNAME,ARRDISTSCREENPRINT,DEPDISTSCREENPRINT,ARRPROBTYPE,DEPPROBTYPE,ARRPRINTDIST,\
		DEPPRINTDIST,ACTYPE,ORIGIN,DEST)\
		VALUES (%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s',%d,%d,'%s','%s','%s','%s','%s')"),
		nParentID,m_strCode,(int)m_bEnRoute,m_nEntryID,m_nEntryFlag,m_nExitID,m_nExitFlag
		,m_startArrTime.asSeconds(),m_startDepTime.asSeconds(),m_endArrTime.asSeconds(),m_endDepTime.asSeconds(),\
		m_nFlightCount,m_strArrDistName,m_strDepDistName,m_strArrDistScreenPrint,m_strDepDistScreenPrint,\
		(int)m_enumArrProbType,(int)m_enumDepProbType,m_strArrprintDist,m_strDepprintDist,m_strACType,m_strOrig,m_strDest);
}

void ItinerantFlight::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_ITINERANTFLIGHT\
		SET NAME ='%s',ENROUTE = %d, ENTRYID =%d, ENTRYFLAG =%d, EXITID =%d, EXITFLAG =%d, STARTARRTIME =%d,\
		STARTDEPTIME =%d,ENDARRTIME =%d,ENDDEPTIME =%d, FLTCOUNT =%d, \
		ARRDISTNAME ='%s',DEPDISTNAME = '%s', ARRDISTSCREENPRINT ='%s',DEPDISTSCREENPRINT = '%s',ARRPROBTYPE =%d,\
		DEPPROBTYPE =%d, ARRPRINTDIST ='%s',DEPPRINTDIST = '%s',ACTYPE = '%s',ORIGIN ='%s',DEST = '%s'\
		WHERE (ID = %d)"),m_strCode,(int)m_bEnRoute,m_nEntryID,m_nEntryFlag,m_nExitID,m_nExitFlag\
		,m_startArrTime.asSeconds(),m_startDepTime.asSeconds(),m_endArrTime.asSeconds(),m_endDepTime.asSeconds(),\
		m_nFlightCount,m_strArrDistName,m_strDepDistName,m_strArrDistScreenPrint,m_strDepDistScreenPrint,\
		(int)m_enumArrProbType,(int)m_enumDepProbType,m_strArrprintDist,m_strDepprintDist,m_strACType,m_strOrig,m_strDest,m_nID);

}

void ItinerantFlight::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_ITINERANTFLIGHT\
		WHERE (ID = %d)"),m_nID);
}

void ItinerantFlight::ExportData(CAirsideExportFile& exportFile)
{
/*	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeField(m_strName);
//	exportFile.getFile().writeField(m_strFlightType);
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxString(szFltType);
	exportFile.getFile().writeField(szFltType);

	exportFile.getFile().writeInt(m_nEntryFlag);

	if(m_nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
	{
		exportFile.getFile().writeInt(m_nEntryID);
	}
	else
	{
		ALTObjectGroup altObjGroup;
		altObjGroup.ReadData(m_nEntryID);	
		ALTObjectID objName = altObjGroup.getName();
		objName.writeALTObjectID(exportFile.getFile());
	}
	
	exportFile.getFile().writeInt(m_nExitFlag);
	if(m_nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
	{
		exportFile.getFile().writeInt(m_nExitID);
	}
	else
	{
		ALTObjectGroup altObjGroup;
		altObjGroup.ReadData(m_nExitID);	
		ALTObjectID objName = altObjGroup.getName();
		objName.writeALTObjectID(exportFile.getFile());
	}
	
	exportFile.getFile().writeInt(m_startTime.asSeconds());
	exportFile.getFile().writeInt(m_endTime.asSeconds());
	exportFile.getFile().writeInt(m_nFlightCount);
	exportFile.getFile().writeField(m_strDistName);
	exportFile.getFile().writeInt((int)m_enumProbType);
	ProbabilityDistribution* pProbDist = NULL;
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
	char szprintDist[1024] = {0};
	char *endOfName = strstr (m_strprintDist, ":");
	strcpy(szprintDist,endOfName + 1);
	pProbDist->setDistribution(szprintDist);
	pProbDist->writeDistribution(exportFile.getFile());
	delete pProbDist;

	exportFile.getFile().writeLine();
*/
}
void ItinerantFlight::ImportData(CAirsideImportFile& importFile)
{
/*	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	importFile.getFile().getField(m_strName.GetBuffer(1024),1024);
//	importFile.getFile().getField(m_strFlightType.GetBuffer(1024),1024);
	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	importFile.getFile().getInteger(m_nEntryFlag);
	if(m_nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
	{
		int nOldEntryID = -1;
		importFile.getFile().getInteger(nOldEntryID);
		m_nEntryID = importFile.GetObjectNewID(nOldEntryID);
	}
	else
	{
		ALTObjectGroup altObjGroup;	
		altObjGroup.setType(ALT_STAND);
		ALTObjectID objName;
		objName.readALTObjectID(importFile.getFile());
		altObjGroup.setName(objName);

		try
		{
			CADODatabase::BeginTransaction();
			m_nEntryID = altObjGroup.InsertData(importFile.getNewProjectID());
			CADODatabase::CommitTransaction();

		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
		}
	}


	importFile.getFile().getInteger(m_nExitFlag);
	if(m_nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
	{
		int nOldExitID = -1; 
		importFile.getFile().getInteger(nOldExitID);
		m_nExitID = importFile.GetObjectNewID(nOldExitID);
	}
	else
	{
		ALTObjectGroup altObjGroup;	
		altObjGroup.setType(ALT_STAND);
		ALTObjectID objName;
		objName.readALTObjectID(importFile.getFile());
		altObjGroup.setName(objName);

		try
		{
			CADODatabase::BeginTransaction();
			m_nExitID = altObjGroup.InsertData(importFile.getNewProjectID());
			CADODatabase::CommitTransaction();

		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
		}
	}
	

	long lstartTime = 0L;
	importFile.getFile().getInteger(lstartTime);
	m_startTime.set(lstartTime);


	long lendtime = 0L;
	importFile.getFile().getInteger(lendtime);
	m_endTime.set(lendtime);

	importFile.getFile().getInteger(m_nFlightCount);

	importFile.getFile().getField(m_strDistName.GetBuffer(1024),1024);


	int nProbType = 0;
	importFile.getFile().getInteger(nProbType);
	m_enumProbType = (ProbTypes)nProbType;

	ProbabilityDistribution* pProbDist = NULL;
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

    ImportSaveData(importFile.getNewProjectID(),strFltType);
*/
}

void ItinerantFlight::ImportSaveData(int nParentID,CString strFltType)
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO IN_ITINERANTFLIGHT\
					 (PROJID, NAME, ENROUTE, ENTRYID,ENTRYFLAG, EXITID, EXITFLAG,STANDID, STARTARRTIME,STARTDEPTIME,\
					 ENDARRTIME,ENDDEPTIME, FLTCOUNT, \
					 ARRDISTNAME,DEPDISTNAME,ARRDISTSCREENPRINT,DEPDISTSCREENPRINT,ARRPROBTYPE,DEPPROBTYPE,ARRPRINTDIST,\
					 DEPPRINTDIST,ACTYPE,ORIGIN,DEST)\
					 VALUES (%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s',%d,%d,'%s','%s','%s','%s','%s')"),
					 nParentID,m_strCode,(int)m_bEnRoute,m_nEntryID,m_nEntryFlag,m_nExitID,m_nExitFlag
					 ,m_nStandID,m_startArrTime.asSeconds(),m_startDepTime.asSeconds(),m_endArrTime.asSeconds(),m_endDepTime.asSeconds(),\
					 m_nFlightCount,m_strArrDistName,m_strDepDistName,m_strArrDistScreenPrint,m_strDepDistScreenPrint,\
					 (int)m_enumArrProbType,(int)m_enumDepProbType,m_strArrprintDist,m_strDepprintDist,m_strACType,m_strOrig,m_strDest);
	if (strSQL.IsEmpty())
		return;

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	
}

ProbabilityDistribution* ItinerantFlight::GetArrProbDistribution()
{
	if(m_pDistribution)
	{
		delete m_pDistribution;
	}

	switch(m_enumArrProbType) 
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
	const char *endOfName = strstr (m_strArrprintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pDistribution->setDistribution(szprintDist);
	return m_pDistribution;
}
ProbabilityDistribution* ItinerantFlight::GetDepProbDistribution()
{
	if(m_pDistribution)
	{
		delete m_pDistribution;
	}

	switch(m_enumDepProbType) 
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
	const char *endOfName = strstr (m_strDepprintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pDistribution->setDistribution(szprintDist);
	return m_pDistribution;
}
/////////////////////////////////////////////////////////////////////////////////////////
//
ItinerantFlightList::ItinerantFlightList()
:BaseClass()
{

}
ItinerantFlightList::~ItinerantFlightList()
{

}

void ItinerantFlightList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT *\
		FROM IN_ITINERANTFLIGHT\
		WHERE (PROJID = %d)"), nParentID);

}
void ItinerantFlightList::ReadData(int nParentID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		ItinerantFlight* element = new ItinerantFlight;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}
void ItinerantFlightList::ExportItinerantFlights(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	ItinerantFlightList itinerantFlightList;
	itinerantFlightList.SetAirportDB(pAirportDB);
	itinerantFlightList.ReadData(exportFile.GetProjectID());

	exportFile.getFile().writeField("ItinerantFlightList");
	exportFile.getFile().writeLine();

	size_t itemCount = itinerantFlightList.GetElementCount();	
	for (size_t i =0; i < itemCount; ++i)
	{
		itinerantFlightList.GetItem(i)->ExportData(exportFile);	
	}
	exportFile.getFile().endFile();
}
void ItinerantFlightList::ImportItinerantFlights(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		ItinerantFlight  itinerantFlight;
		itinerantFlight.ImportData(importFile);
	}
}

void ItinerantFlightList::DoNotCall()
{
	ItinerantFlightList list;
	list.AddNewItem(NULL);
}
