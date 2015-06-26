#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\pushback.h"
#include "../InputAirside/AirsideAircraft.h"
#include <algorithm>
//#include "./../Inputs/IN_TERM.H"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"

using namespace ADODB;


/////////////////////////////////////////////////////////////
//
//CPushBackNEC
//
/////////////////////////////////////////////////////////////
CPushBackNEC::CPushBackNEC()
:m_strSpeedDistName(_T("U[10~20]"))
,m_enumSpeedProbType(UNIFORM)
,m_strSpeedPrintDist(_T("Uniform:10;20"))
,m_strTimeDistName(_T("U[3~5]"))
,m_enumTimeProbType(UNIFORM)
,m_strTimePrintDist(_T("Uniform:3;5"))
,m_pSpeedDistribution(0)
,m_pTimeDistribution(0)
,m_nProjID(-1)
,m_pAirportDB(0)
,m_bImport(false)
,m_strImportFltType(_T(""))
{
}

CPushBackNEC::~CPushBackNEC()
{
	if(m_pSpeedDistribution)
	{
		delete m_pSpeedDistribution;
		m_pSpeedDistribution = NULL;
	}
	if(m_pTimeDistribution)
	{
		delete m_pTimeDistribution;
		m_pTimeDistribution = NULL;
	}
}

void CPushBackNEC::InitFromDBRecordset(CADORecordset& recordset)
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

		recordset.GetFieldValue(_T("SPEEDDISTNAME"),m_strSpeedDistName);		
		recordset.GetFieldValue(_T("SPEEDPROBTYPE"),nProbType);
		m_enumSpeedProbType = (ProbTypes)nProbType;
		recordset.GetFieldValue(_T("SPEEDPRINTDIST"),m_strSpeedPrintDist);
		GetSpeedProbDistribution();

		recordset.GetFieldValue(_T("TIMEDISTNAME"),m_strTimeDistName);
		recordset.GetFieldValue(_T("TIMEPROBTYPE"),nProbType);
		m_enumTimeProbType = (ProbTypes)nProbType;
		recordset.GetFieldValue(_T("TIMEPRINTDIST"),m_strTimePrintDist);
		GetTimeProbDistribution();
	}
}

void CPushBackNEC::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PUSHBACKNEC WHERE (ID = %d);"),nID);
	return ;
}

void CPushBackNEC::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");
	if(m_bImport)
	{
		strFltType = m_strImportFltType;
	}
	else
	{
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltType = szFltType;
	}
	
	strSQL.Format(_T("INSERT INTO  PUSHBACKNEC (PROJID,FLTCONSTRAINT,SPEEDDISTNAME,SPEEDPROBTYPE,SPEEDPRINTDIST,TIMEDISTNAME,TIMEPROBTYPE,TIMEPRINTDIST) VALUES (%d,'%s','%s',%d,'%s','%s',%d,'%s');"),\
		nParentID,strFltType,m_strSpeedDistName,(int)m_enumSpeedProbType,m_strSpeedPrintDist,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist);
	//return ;
}

void CPushBackNEC::GetUpdateSQL(CString& strSQL) const
{
	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");
	if(m_bImport)
	{
		strFltType = m_strImportFltType;
	}
	else
	{
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltType = szFltType;
	}
	strSQL.Format(_T("UPDATE PUSHBACKNEC SET PROJID = %d,FLTCONSTRAINT = '%s',SPEEDDISTNAME = '%s',SPEEDPROBTYPE = %d,SPEEDPRINTDIST = '%s',TIMEDISTNAME = '%s',TIMEPROBTYPE = %d,TIMEPRINTDIST = '%s' WHERE (ID = %d);"),\
		m_nProjID,strFltType,m_strSpeedDistName,(int)m_enumSpeedProbType,m_strSpeedPrintDist,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist,m_nID);
	return ;
}

void CPushBackNEC::GetDeleteSQL(CString& strSQL) const
{
	strSQL = _T("");
	strSQL.Format(_T("DELETE FROM PUSHBACKNEC WHERE (ID = %d);"),m_nID);
	return ;
}

void CPushBackNEC::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);

	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType); 
	exportFile.getFile().writeInt(1024);
	exportFile.getFile().writeField(szFltType);

	exportFile.getFile().writeInt(m_strSpeedDistName.GetLength()+1);
	exportFile.getFile().writeField(m_strSpeedDistName.GetBuffer(m_strSpeedDistName.GetLength()+1));
	m_strSpeedDistName.ReleaseBuffer(m_strSpeedDistName.GetLength()+1);

	exportFile.getFile().writeInt((int)m_enumSpeedProbType);

	exportFile.getFile().writeInt(m_strSpeedPrintDist.GetLength()+1);
	exportFile.getFile().writeField(m_strSpeedPrintDist.GetBuffer(m_strSpeedPrintDist.GetLength()+1));
	m_strSpeedPrintDist.ReleaseBuffer(m_strSpeedPrintDist.GetLength()+1);

	exportFile.getFile().writeInt(m_strTimeDistName.GetLength()+1);
	exportFile.getFile().writeField(m_strTimeDistName.GetBuffer(m_strTimeDistName.GetLength()+1));
	m_strTimeDistName.ReleaseBuffer(m_strTimeDistName.GetLength()+1);

	exportFile.getFile().writeInt((int)m_enumTimeProbType);

	exportFile.getFile().writeInt(m_strTimePrintDist.GetLength()+1);
	exportFile.getFile().writeField(m_strTimePrintDist.GetBuffer(m_strTimePrintDist.GetLength()+1));
	m_strTimePrintDist.ReleaseBuffer(m_strTimePrintDist.GetLength()+1);

	if(m_pSpeedDistribution)
		m_pSpeedDistribution->writeDistribution(exportFile.getFile());
	if(m_pTimeDistribution)
		m_pTimeDistribution->writeDistribution(exportFile.getFile());

	exportFile.getFile().writeLine();
}

void CPushBackNEC::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nID);
	m_nID = -1;
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	char * pBuffer = 0;
	int nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strImportFltType = pBuffer;
	m_bImport = true;
	if(m_pAirportDB){
		m_fltType.SetAirportDB(m_pAirportDB);
		m_fltType.setConstraintWithVersion(pBuffer);
	}
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
	m_strSpeedDistName = pBuffer;
	if(pBuffer)
	{
		delete [] pBuffer;
		pBuffer = 0;
	}
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	m_enumSpeedProbType = (ProbTypes)nBufferLen;
	nBufferLen = 0;

	importFile.getFile().getInteger(nBufferLen);
	pBuffer = new char[nBufferLen+1];
	if(!pBuffer)return;
	importFile.getFile().getField(pBuffer,nBufferLen);
	m_strSpeedPrintDist = pBuffer;
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
	m_enumTimeProbType = (ProbTypes)nBufferLen;
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


	if(GetSpeedProbDistribution())
		m_pSpeedDistribution->readDistribution(importFile.getFile());
	if(GetTimeProbDistribution())
		m_pTimeDistribution->readDistribution(importFile.getFile());

	importFile.getFile().getLine();
}

CString CPushBackNEC::GetSpeedDistScreenPrint(void)
{
	CString strReturn = _T("");
	if(m_pSpeedDistribution){
		char szScreenPrintDist[1024] = {0};
		m_pSpeedDistribution->screenPrint(szScreenPrintDist);
		strReturn = szScreenPrintDist;
	}
	return (strReturn);
}

ProbabilityDistribution* CPushBackNEC::GetSpeedProbDistribution()
{
	if(m_pSpeedDistribution)
	{
		delete m_pSpeedDistribution;
		m_pSpeedDistribution = 0;
	}

	switch(m_enumSpeedProbType) 
	{
	case BERNOULLI:
		m_pSpeedDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pSpeedDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pSpeedDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pSpeedDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pSpeedDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pSpeedDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pSpeedDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pSpeedDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pSpeedDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pSpeedDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pSpeedDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pSpeedDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(!m_pSpeedDistribution)return (0);
	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strSpeedPrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pSpeedDistribution->setDistribution(szprintDist);

	return m_pSpeedDistribution;
}


CString CPushBackNEC::GetTimeDistScreenPrint(void)
{
	CString strReturn = _T("");
	if(m_pTimeDistribution){
		char szScreenPrintDist[1024] = {0};
		m_pTimeDistribution->screenPrint(szScreenPrintDist);
		strReturn = szScreenPrintDist;
	}
	return (strReturn);
}


ProbabilityDistribution* CPushBackNEC::GetTimeProbDistribution()
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
	if(!m_pTimeDistribution)return (0);
	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strTimePrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pTimeDistribution->setDistribution(szprintDist);

	return m_pTimeDistribution;
}
/////////////////////////////////////////////////////////////
//
//CPushBack
//
/////////////////////////////////////////////////////////////
CPushBack::CPushBack(CAirportDatabase* pAirportDB)
{
	m_pInputTerminal = 0;
	m_pAirportDB = pAirportDB;
}

CPushBack::~CPushBack()
{
}

void CPushBack::ExportPushBacks(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	exportFile.getFile().writeField("PushBack");
	exportFile.getFile().writeLine();

	CPushBack pushBack(pAirportDB);
	pushBack.ReadData(exportFile.GetProjectID());
	pushBack.ExportData(exportFile);

	exportFile.getFile().endFile();
}

void CPushBack::ImportPushBacks(CAirsideImportFile& importFile)
{
	if(importFile.getVersion() <= 1025)return;
	CPushBack pushBack(0);
	pushBack.ImportData(importFile);
	pushBack.SaveData(importFile.getNewProjectID());
}

void CPushBack::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PUSHBACKNEC WHERE (PROJID = %d);"),nParentID);
	return;
}

void CPushBack::ReadData(int nParentID)
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
		CPushBackNEC* element = new CPushBackNEC;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}

void CPushBack::ImportData(CAirsideImportFile& importFile)
{
	m_dataList.clear();
	while (!importFile.getFile().isBlank())
	{
		CPushBackNEC * element = new CPushBackNEC;
		if(!element)return;
		element->SetAirportDB(m_pAirportDB);
		element->ImportData(importFile);
		m_dataList.push_back(element);
		importFile.getFile().getLine();
	}

}
void CPushBack::ImportData(int nCount,CAirsideImportFile& importFile)
{
	m_dataList.clear();
	for (int i =0; i < nCount; ++ i)
	{
		CPushBackNEC * element = new CPushBackNEC;
		if(!element)return;
		element->SetAirportDB(m_pAirportDB);
		element->ImportData(importFile);
		m_dataList.push_back(element);
		importFile.getFile().getLine();	
	}
}

bool FlightTypeCompare(CPushBackNEC* fItem,CPushBackNEC* sItem)
{
	if (fItem->GetFltType().fits(sItem->GetFltType()))
		return false;
	else if(sItem->GetFltType().fits(fItem->GetFltType()))
		return true;

	return false;
}


void CPushBack::SortFlightType()
{
	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeCompare);
}

void CPushBack::DoNotCall()
{
	CPushBack cpb(NULL);
	cpb.AddNewItem(NULL);
}