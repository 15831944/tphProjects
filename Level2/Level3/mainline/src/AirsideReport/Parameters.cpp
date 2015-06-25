#include "StdAfx.h"
#include "parameters.h"
#include "../common/UnitsManager.h"
#include "../Common/exeption.h"
#include "../Common/fileman.h"
using namespace ADODB;

CParameters::CParameters()
:m_startTime(0L)
,m_endTime(86400L)
,m_lInterval(3600L)
,m_nProjID(-1)
,m_reportType(ASReportType_Detail)
,m_unitLength(0)
,m_pUnitManager(NULL)
,m_AirportDB(NULL)
{
	//memset(m_modelName,0,sizeof(char)*255) ;
	_tcscpy(m_modelName,_T("UnknowDef")) ;
}

CParameters::~CParameters()
{
}

void CParameters::SetModelName(char* modelName)
{
	strcpy(m_modelName,modelName);
}

char* CParameters::GetModelName()
{
	return m_modelName;
}

void CParameters::SetProjID(int nProiID)
{
	m_nProjID = nProiID ;
	return;
	//Get projName
	_variant_t RecordsAffected(0L);
	_RecordsetPtr pRecordset;
	TCHAR lpszStringSQL[1024] = {0};
	_stprintf(lpszStringSQL, _T("SELECT PROJNAME FROM ProjectInfo WHERE  PROJID = %d"), m_nProjID);
	_bstr_t stringSQL(lpszStringSQL);

	//pRecordset.CreateInstance("ADODB.Recordset");
	pRecordset =DATABASECONNECTION.GetConnection()->Execute(stringSQL,&RecordsAffected,adCmdText);
	_variant_t vValue;
	if (!pRecordset->adoEOF)
	{
		vValue = pRecordset->GetCollect(L"PROJNAME");
		if(vValue.vt != VT_NULL)
			strcpy(m_modelName, (char*)_bstr_t(vValue));
	}
	if (pRecordset)
		if(pRecordset->State == adStateOpen)
			pRecordset->Close();

}
int CParameters::GetProjID()
{
	return m_nProjID ; 
}

bool CParameters::IsFlightScheduleExist(FlightConstraint& fltCons)
{
	std::vector<FlightConstraint>::const_iterator iter = m_vFlightConstraint.begin();
	for (;iter != m_vFlightConstraint.end(); ++iter)
	{
		if((*iter).fits(fltCons)&&fltCons.fits(*iter))
			return true;
	}
	return false;
}
void CParameters::AddFlightConstraint(FlightConstraint& fltCons)
{
	m_vFlightConstraint.push_back(fltCons);
}
bool CParameters::DelFlightConstraint(size_t nIndex)
{
	if (nIndex < m_vFlightConstraint.size())
	{
		m_vFlightConstraint.erase(m_vFlightConstraint.begin()+nIndex);
		return true;
	}
	return false;
}
FlightConstraint CParameters::getFlightConstraint(size_t nIndex)
{
	FlightConstraint fltCons;

	if (nIndex < m_vFlightConstraint.size())
	{
		fltCons = m_vFlightConstraint.at(nIndex);
	}
	return fltCons;
}
size_t CParameters::getFlightConstraintCount()
{
	return m_vFlightConstraint.size();
}
void CParameters::ClearFlightConstraint()
{
	m_vFlightConstraint.clear();
}

CString CParameters::GetLengthUnitName()
{

	return "";

}
CString CParameters::GetParameterString()
{
	CString strString;
	strString += m_startTime.printTime(false);
	strString += ";";
	strString += m_endTime.printTime(false);
	strString += ";";

	for (size_t i = 0; i < m_vFlightConstraint.size(); ++ i)
	{
		CString strFltCons;
		m_vFlightConstraint[i].screenPrint(strFltCons);
		strString += strFltCons;
		strString += ";";
	}

	strString.TrimRight(";");

	return strString;
}

BOOL CParameters::ExportFile(ArctermFile& _file)
{
	_file.writeField(m_modelName) ;
	_file.writeInt(m_startTime.asSeconds()) ;
	_file.writeInt(m_endTime.asSeconds()) ;
	_file.writeInt(m_lInterval) ;
	_file.writeInt(m_nProjID) ;
	_file.writeInt((int)m_reportType) ;
	_file.writeInt(m_unitLength) ;
	_file.writeLine() ;
	int size = m_vFlightConstraint.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;

	TCHAR th[1024] = {0} ;
	for (int i = 0 ; i < size ;i++)
	{
		getFlightConstraint(i).WriteSyntaxStringWithVersion(th) ;
		_file.writeField(th) ;
		_file.writeLine() ;
	}
	return TRUE ;
}
BOOL CParameters::ImportFile(ArctermFile& _file)
{
	m_vFlightConstraint.clear();
	_file.getField(m_modelName,255) ;
	long time ;
	_file.getInteger(time) ;
	m_startTime = ElapsedTime(time) ;
	_file.getInteger(time) ;
	m_endTime = ElapsedTime(time) ;

	_file.getInteger(m_lInterval) ;
	_file.getInteger(m_nProjID) ;
	int type ;
	_file.getInteger(type) ;
	m_reportType = (enumASReportType_Detail_Summary)type ;

	_file.getInteger(m_unitLength) ;
	_file.getLine() ;

	int size = 0 ;
	if(!_file.getInteger(size) )
		return FALSE ;

	_file.getLine() ;
	TCHAR th[1024] = {0} ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getField(th,1024) ;
		FlightConstraint constrain(m_AirportDB) ;
		constrain.setConstraintWithVersion(th) ;
		m_vFlightConstraint.push_back(constrain) ;
		_file.getLine();
	}
	return TRUE ;
}

void CParameters::SetReportFileDir( const CString& strReportPath )
{
	m_strReportPath = strReportPath;
}

CString CParameters::GetReportParamName()
{
	return  _T("");
}

CString CParameters::GetReportParamPath()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\%s"),m_strReportPath,GetReportParamName());
	return strFilePath;
}

void CParameters::LoadParameterFile()
{
	CString strFilrName = GetReportParamPath();

	ArctermFile file;
	try 
	{
		file.openFile (strFilrName, READ);
	}
	catch (FileOpenError *exception)
	{
		delete exception;
		return;
	}
	file.getLine();
	ReadParameter(file);

	 file.closeIn();
}

void CParameters::SaveParameterFile()
{
	CString strFilrName = GetReportParamPath();

	int nIndex = strFilrName.ReverseFind('\\');
	if(nIndex <= 0)
		return ;


	CString strFolder = strFilrName.Left(nIndex);

	FileManager::MakePath(strFolder);

	ArctermFile arcFile;
	arcFile.openFile(strFilrName,WRITE);
	arcFile.writeField (_T("Report Parameter"));
	arcFile.writeLine();

	arcFile.writeField ("start time, end time, ...");
	arcFile.writeLine();




	WriteParameter (arcFile);

	arcFile.endFile();
}

void CParameters::LoadParameterFromFile(ArctermFile& _file)
{
	ImportFile(_file);
}

void CParameters::SaveParameterToFile(ArctermFile& _file)
{
	ExportFile(_file);
}

//write base informations, start time, end time , interval ....
void CParameters::WriteParameter( ArctermFile& _file )
{
	_file.writeInt(m_startTime.asSeconds()) ;
	_file.writeInt(m_endTime.asSeconds()) ;
	_file.writeInt(m_lInterval) ;
	_file.writeInt(m_nProjID) ;
	_file.writeInt((int)m_reportType) ;
	_file.writeInt(m_unitLength) ;
	_file.writeLine() ;
}

void CParameters::ReadParameter( ArctermFile& _file )
{
	long time ;
	_file.getInteger(time) ;
	m_startTime = ElapsedTime(time) ;
	_file.getInteger(time) ;
	m_endTime = ElapsedTime(time) ;

	_file.getInteger(m_lInterval) ;
	_file.getInteger(m_nProjID) ;
	int type ;
	_file.getInteger(type) ;
	m_reportType = (enumASReportType_Detail_Summary)type ;

	_file.getInteger(m_unitLength) ;
	_file.getLine() ;
}