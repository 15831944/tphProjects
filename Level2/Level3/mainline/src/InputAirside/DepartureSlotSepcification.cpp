#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "DepartureSlotSepcification.h"
#include "../Common/AirportDatabase.h"
#include "InputAirside/AirRoute.h"
#include "AirsideImportExportManager.h"

//----------------------------------------------------------------------------------
//
DepartureSlotItem::DepartureSlotItem(void)
{
	m_pAirportDB = 0;
	m_nProjID = -1;
	m_nSID = -2;

	m_FromTime.set(0);
	m_ToTime.set(86399);

	m_strFirstSlotDistName = _T("U[20~30]");
	m_enumFirstSlotProbType = UNIFORM;
	m_strFirstSlotPrintDist = _T("Uniform:20;30");
	m_pFirstSlotDistribution = 0;

	m_strFirstSlotDistName = _T("U[20~30]");
	m_enumFirstSlotProbType = UNIFORM;
	m_strFirstSlotPrintDist = _T("Uniform:20;30");
	m_pInterSlotDistribution = 0;

	m_bImport = false;
	m_strImportFltType = _T("");
}

DepartureSlotItem::~DepartureSlotItem(void)
{
}

void DepartureSlotItem::SetFromTime(ElapsedTime estFromTime)
{
	m_FromTime = estFromTime;
}
ElapsedTime DepartureSlotItem::GetFromTime(void)
{
	return m_FromTime;
}

void DepartureSlotItem::SetToTime(ElapsedTime estToTime)
{
	m_ToTime = estToTime;
}
ElapsedTime DepartureSlotItem::GetToTime(void)
{
	return m_ToTime;
}



FlightConstraint DepartureSlotItem::GetFlightConstraint()
{
	return m_fltType;
}

void DepartureSlotItem::SetFltType(FlightConstraint fltCons)
{
	m_fltType = fltCons;
}

void DepartureSlotItem::InitFromDBRecordset(CADORecordset& recordset)
{ 
	int nFromTime;
	int nToTime;
	CString strFlightType;
	recordset.GetFieldValue(_T("STARTTIME"),nFromTime);
	recordset.GetFieldValue(_T("STOPTIME"),nToTime);
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("PRJID"),m_nProjID);
	recordset.GetFieldValue(_T("FLIGHTTYPE"),strFlightType);

	int nEnumValue = -1;
	recordset.GetFieldValue(_T("FLIGHTUSESID"),m_nSID);

	recordset.GetFieldValue(_T("FIRSTSLOTDISTNAME"),m_strFirstSlotDistName);
	recordset.GetFieldValue(_T("FIRSTSLOTDISTTYPE"),nEnumValue);
	m_enumFirstSlotProbType = (ProbTypes)nEnumValue;
	nEnumValue = -1;
	recordset.GetFieldValue(_T("FIRSTSLOTDISTPRINT"),m_strFirstSlotPrintDist);
	GetFirstSlotDelayProDis();

	recordset.GetFieldValue(_T("INTERSLOTDISTNAME"),m_strInterSlotDistName);
	recordset.GetFieldValue(_T("INTERSLOTDISTTYPE"),nEnumValue);
	m_enumInterSlotProbType = (ProbTypes)nEnumValue;
	nEnumValue = -1;
	recordset.GetFieldValue(_T("INTERSLOTDISTPRINT"),m_strInterSlotPrintDist);
	GetInterSlotDelayProDis();

	m_FromTime= ElapsedTime((LONG)nFromTime);
	m_ToTime = ElapsedTime((LONG)nToTime);
	
	if (m_pAirportDB)
	{
		m_fltType.SetAirportDB(m_pAirportDB);
		m_fltType.setConstraintWithVersion(strFlightType);
	} 
}

void DepartureSlotItem::GetInsertSQL(int nProjID,CString& strSQL) const
{	
	char szFltType[1024]={0};
	CString strFltTypeSave = _T("");
	if(m_bImport)
		strFltTypeSave = m_strImportFltType;
	else
	{
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltTypeSave = szFltType;
	}

	strSQL.Format(_T("INSERT INTO IN_DEPARTURESLOTS\
					 (PRJID,FLIGHTTYPE,STARTTIME,STOPTIME,FLIGHTUSESID,\
					 FIRSTSLOTDISTNAME,FIRSTSLOTDISTTYPE,FIRSTSLOTDISTPRINT,\
					 INTERSLOTDISTNAME,INTERSLOTDISTTYPE,INTERSLOTDISTPRINT) \
					 VALUES  (%d,'%s',%d,%d,%d,'%s',%d,'%s','%s',%d,'%s')"),\
					 nProjID,strFltTypeSave,m_FromTime.asSeconds(),m_ToTime.asSeconds(),\
					 m_nSID,m_strFirstSlotDistName,(int)m_enumFirstSlotProbType,m_strFirstSlotPrintDist,\
					 m_strInterSlotDistName,(int)m_enumInterSlotProbType,m_strInterSlotPrintDist);

}

void DepartureSlotItem::GetUpdateSQL( CString& strSQL) const
{
	char szFltType[1024]={0};
	CString strFltTypeSave = _T("");
	if(m_bImport)
		strFltTypeSave = m_strImportFltType;
	else
	{
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltTypeSave = szFltType;
	}

	strSQL.Format(_T("UPDATE IN_DEPARTURESLOTS \
					 SET PRJID = %d,FLIGHTTYPE = '%s',STARTTIME = %d,STOPTIME = %d,FLIGHTUSESID = %d,\
					 FIRSTSLOTDISTNAME = '%s',FIRSTSLOTDISTTYPE = %d,FIRSTSLOTDISTPRINT = '%s',\
					 INTERSLOTDISTNAME = '%s',INTERSLOTDISTTYPE = %d,INTERSLOTDISTPRINT = '%s' WHERE (ID = %d)"),\
					 m_nProjID,strFltTypeSave,m_FromTime.asSeconds(),m_ToTime.asSeconds(),m_nSID,\
					 m_strFirstSlotDistName,(int)m_enumFirstSlotProbType,m_strFirstSlotPrintDist,\
					 m_strInterSlotDistName,(int)m_enumInterSlotProbType,m_strInterSlotPrintDist,m_nID);
		
}

void DepartureSlotItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("DELETE FROM IN_DEPARTURESLOTS \
					 WHERE (ID = %d)"),m_nID);
}

void DepartureSlotItem::GetSelectSQL(int nID,CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * FROM IN_DEPARTURESLOTS WHERE (ID = %d)"),nID);
}

void DepartureSlotItem::ExportData(CAirsideExportFile& exportFile)
{
	char szFltType[1024]={0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
 
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeField(szFltType);
	exportFile.getFile().writeInt(m_FromTime);
	exportFile.getFile().writeInt(m_ToTime);

	exportFile.getFile().writeInt(m_nSID);

	exportFile.getFile().writeInt(m_strFirstSlotDistName.GetLength());
	exportFile.getFile().writeField(m_strFirstSlotDistName.GetBuffer(m_strFirstSlotDistName.GetLength()));
	m_strFirstSlotDistName.ReleaseBuffer(m_strFirstSlotDistName.GetLength());
	exportFile.getFile().writeInt((int)m_enumFirstSlotProbType);
	exportFile.getFile().writeInt(m_strFirstSlotPrintDist.GetLength());
	exportFile.getFile().writeField(m_strFirstSlotPrintDist.GetBuffer(m_strFirstSlotPrintDist.GetLength()));
	m_strFirstSlotPrintDist.ReleaseBuffer(m_strFirstSlotPrintDist.GetLength()); 

	exportFile.getFile().writeInt(m_strInterSlotDistName.GetLength());
	exportFile.getFile().writeField(m_strInterSlotDistName.GetBuffer(m_strInterSlotDistName.GetLength()));
	m_strInterSlotDistName.ReleaseBuffer(m_strInterSlotDistName.GetLength());
	exportFile.getFile().writeInt((int)m_enumInterSlotProbType);
	exportFile.getFile().writeInt(m_strInterSlotPrintDist.GetLength());
	exportFile.getFile().writeField(m_strInterSlotPrintDist.GetBuffer(m_strInterSlotPrintDist.GetLength()));
	m_strInterSlotPrintDist.ReleaseBuffer(m_strInterSlotPrintDist.GetLength());

	if(m_pFirstSlotDistribution)
		m_pFirstSlotDistribution->writeDistribution(exportFile.getFile());
	if(m_pInterSlotDistribution)
		m_pInterSlotDistribution->writeDistribution(exportFile.getFile());

	exportFile.getFile().writeLine();
}

void DepartureSlotItem::ImportData(CAirsideImportFile& importFile)
{
	int nID = -1;
	importFile.getFile().getInteger(nID);
	int nPrjID = -1;
	importFile.getFile().getInteger(nPrjID);
	m_nProjID = importFile.getNewProjectID();
	char strFltType[1024] = {0};
	importFile.getFile().getField(strFltType,1024);
	m_strImportFltType = strFltType;
	m_bImport = true;
	if (m_pAirportDB)
	{
		m_fltType.SetAirportDB(m_pAirportDB);
		m_fltType.setConstraintWithVersion(strFltType);
	} 

	float nFrom,nTotime;
	importFile.getFile().getFloat(nFrom);
	importFile.getFile().getFloat(nTotime);
	m_FromTime= ElapsedTime((LONG)nFrom/100);
	m_ToTime = ElapsedTime((LONG)nTotime/100);
	if(importFile.getVersion() > 1028)
	{
		importFile.getFile().getInteger(m_nSID);//SID
		if(m_nSID != -2)
			m_nSID = importFile.GetAirRouteNewIndex(m_nSID);

		int nStrLen = -1;
		char * pszStr = 0;
		int nEnumValue = -1;

		//first slot delay		
		importFile.getFile().getInteger(nStrLen);
		if(nStrLen > 0)
			pszStr = new char[nStrLen + 1];
		else
			pszStr = new char[1024];
		importFile.getFile().getField(pszStr,nStrLen);
		m_strFirstSlotDistName = pszStr;
		delete []pszStr;
		pszStr = 0;
		nStrLen = -1;
		importFile.getFile().getInteger(nEnumValue);
		m_enumFirstSlotProbType = (ProbTypes)nEnumValue;
		importFile.getFile().getInteger(nStrLen);
		if(nStrLen > 0)
			pszStr = new char[nStrLen + 1];
		else
			pszStr = new char[1024];
		importFile.getFile().getField(pszStr,nStrLen);
		m_strFirstSlotPrintDist = pszStr;
		delete []pszStr;
		pszStr = 0;
		nStrLen = -1;
		GetFirstSlotDelayProDis();

		//inter slot delay
		importFile.getFile().getInteger(nStrLen);
		if(nStrLen > 0)
			pszStr = new char[nStrLen + 1];
		else
			pszStr = new char[1024];
		importFile.getFile().getField(pszStr,nStrLen);
		m_strInterSlotDistName = pszStr;
		delete []pszStr;
		pszStr = 0;
		nStrLen = -1;
		importFile.getFile().getInteger(nEnumValue);
		m_enumInterSlotProbType = (ProbTypes)nEnumValue; 
		importFile.getFile().getInteger(nStrLen);
		if(nStrLen > 0)
			pszStr = new char[nStrLen + 1];
		else
			pszStr = new char[1024];
		importFile.getFile().getField(pszStr,nStrLen);
		m_strInterSlotPrintDist = pszStr;
		delete []pszStr;
		pszStr = 0;
		nStrLen = -1;
		GetInterSlotDelayProDis();

		if(m_pFirstSlotDistribution)
			m_pFirstSlotDistribution->readDistribution(importFile.getFile());
		if(m_pInterSlotDistribution)
			m_pInterSlotDistribution->readDistribution(importFile.getFile());

		importFile.getFile().getLine();
	}	
}

int DepartureSlotItem::GetSID(void)
{
	return (m_nSID);
}

void DepartureSlotItem::SetSID(int nSID)
{
	m_nSID = nSID;
}

CString DepartureSlotItem::GetSIDName(void)
{
	if(m_nSID == -2)
		return (_T("All"));
	//read air routelist
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);

	int nAirRouteCount = airRouteList.GetAirRouteCount();

	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute *pAirRoute = 0;
		pAirRoute = airRouteList.GetAirRoute(i);

		ASSERT(pAirRoute != NULL);

		if (pAirRoute && CAirRoute::SID == pAirRoute->getRouteType() && pAirRoute->getID() == m_nSID)
				return (pAirRoute->getName());
	}

	return _T("");
}

CString DepartureSlotItem::GetFirstSlotDelayProDisName(void)
{	
	return (m_strFirstSlotDistName);
}

CString DepartureSlotItem::GetInterSlotDelayProDisName(void)
{
	return (m_strInterSlotDistName);
}

ProbabilityDistribution* DepartureSlotItem::GetFirstSlotDelayProDis(void)
{
	if(m_pFirstSlotDistribution)
	{
		delete m_pFirstSlotDistribution;
		m_pFirstSlotDistribution = 0;
	}

	switch(m_enumFirstSlotProbType) 
	{
	case BERNOULLI:
		m_pFirstSlotDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pFirstSlotDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pFirstSlotDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pFirstSlotDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pFirstSlotDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pFirstSlotDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pFirstSlotDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pFirstSlotDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pFirstSlotDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pFirstSlotDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pFirstSlotDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pFirstSlotDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(!m_pFirstSlotDistribution)return (0);
	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strFirstSlotPrintDist, ":");

	strcpy(szprintDist,endOfName + 1);

	m_pFirstSlotDistribution->setDistribution(szprintDist); 

	return (m_pFirstSlotDistribution);
}

ProbabilityDistribution* DepartureSlotItem::GetInterSlotDelayProDis(void)
{
	if(m_pInterSlotDistribution)
	{
		delete m_pInterSlotDistribution;
		m_pInterSlotDistribution = 0;
	}

	switch(m_enumInterSlotProbType) 
	{
	case BERNOULLI:
		m_pInterSlotDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pInterSlotDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pInterSlotDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pInterSlotDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pInterSlotDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pInterSlotDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pInterSlotDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pInterSlotDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pInterSlotDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pInterSlotDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pInterSlotDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pInterSlotDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(!m_pInterSlotDistribution)return (0);
	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strInterSlotPrintDist, ":");

	strcpy(szprintDist,endOfName + 1);

	m_pInterSlotDistribution->setDistribution(szprintDist);

	return (m_pInterSlotDistribution);
}

void DepartureSlotItem::SetFirstSlotDelayProDisName(CString strFirstSlotDelayProDisName)
{
	m_strFirstSlotDistName = strFirstSlotDelayProDisName;
}

void DepartureSlotItem::SetFirstSlotDelayProDisType(ProbTypes  emFirstSlotDisType)
{
	m_enumFirstSlotProbType = emFirstSlotDisType;
}

void DepartureSlotItem::SetFirstSlotDelayPrintDis(CString strFirstSlotPrintDis)
{
	m_strFirstSlotPrintDist = strFirstSlotPrintDis;
}

void DepartureSlotItem::SetInterSlotDelayProDisName(CString strInterSlotDelayProDisName)
{
	m_strInterSlotDistName = strInterSlotDelayProDisName;
}

void DepartureSlotItem::SetInterSlotDelayProDisType(ProbTypes emInterSlotDisType)
{
	m_enumInterSlotProbType = emInterSlotDisType;
}

void DepartureSlotItem::SetInterSlotDelayPrintDis(CString strInterSlotPrintDis)
{
	m_strInterSlotPrintDist = strInterSlotPrintDis;
}

//----------------------------------------------------------------------------------
//
DepartureSlotSepcifications::DepartureSlotSepcifications(CAirportDatabase* pAirportDB)
:m_pAirportDB(pAirportDB)
{
}

DepartureSlotSepcifications::~DepartureSlotSepcifications(void)
{
}


void DepartureSlotSepcifications::ReadData(int nParentID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;
	long nRecordCount  = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL,nRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{	
		//setAirportDB(m_pAirportDB);
		DepartureSlotItem * element = new DepartureSlotItem();
		element->setAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}
void DepartureSlotSepcifications::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT *\
					 FROM		IN_DEPARTURESLOTS\
					 WHERE		(PRJID = %d)"), nParentID);
}

void DepartureSlotSepcifications::ExportDepartureSlotSepc(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
  exportFile.getFile().writeField(_T("Departure Slot Sepcification"));
  exportFile.getFile().writeLine();
  DepartureSlotSepcifications slot(pAirportDB);
  slot.ReadData(exportFile.GetProjectID());
  slot.ExportData(exportFile);
  exportFile.getFile().endFile();
}

void DepartureSlotSepcifications::ImportDepartureSlotSepc(CAirsideImportFile& importFile)
{
	DepartureSlotSepcifications dep(0);
	dep.ImportData(importFile);
	dep.SaveData(importFile.getNewProjectID());
}

void DepartureSlotSepcifications::ImportData(CAirsideImportFile& importFile)
{
	m_dataList.clear();
	while (!importFile.getFile().isBlank())
	{
		DepartureSlotItem * element = new DepartureSlotItem;
		if(!element)return;
		element->setAirportDB(m_pAirportDB);
		element->ImportData(importFile);
		m_dataList.push_back(element);
		importFile.getFile().getLine();
	}

}
void DepartureSlotSepcifications::ImportData(int nCount,CAirsideImportFile& importFile)
{
	m_dataList.clear();
	for (int i =0; i < nCount; ++ i)
	{
		DepartureSlotItem * element = new DepartureSlotItem;
		if(!element)return;
		element->setAirportDB(m_pAirportDB);
		element->ImportData(importFile);
		m_dataList.push_back(element);
		importFile.getFile().getLine();	
	}
}

void DepartureSlotSepcifications::DoNotCall()
{
	DepartureSlotSepcifications sep(NULL);
	sep.AddNewItem(NULL);
}