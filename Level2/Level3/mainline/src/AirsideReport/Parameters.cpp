#include "StdAfx.h"
#include "parameters.h"
#include "../common/UnitsManager.h"
#include "../Common/exeption.h"
#include "../Common/fileman.h"
#include <algorithm>
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
,m_enableMultiRun(false)
{
	//memset(m_modelName,0,sizeof(char)*255) ;
	_tcscpy(m_modelName,_T("UnknowDef")) ;
    m_vReportRuns.clear();
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
    _file.appendValue("VERSION");
    _file.appendValue(";");
    _file.appendValue("1"); // version 1
    _file.writeLine();
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

    if(m_enableMultiRun)
        _file.writeChar('T');
    else
        _file.writeChar('F');
    _file.writeLine();

    int nCount = (int)m_vReportRuns.size();
    _file.writeInt(nCount);
    _file.writeLine();

    for(int i=0; i<nCount; i++)
    {
        _file.writeInt(m_vReportRuns.at(i));
    }
    _file.writeLine();

	return TRUE ;
}
BOOL CParameters::ImportFile(ArctermFile& _file)
{
    char buf[256] = {0};
    _file.getSubField(buf, ';');
    if(strcmp(buf, "VERSION") != 0)
    {
        m_vFlightConstraint.clear();
        strcpy(m_modelName, buf) ;
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
		m_vFlightConstraint.clear();
        for (int i = 0 ; i < size ;i++)
        {
            _file.getField(th,1024) ;
            FlightConstraint constrain(m_AirportDB) ;
            constrain.setConstraintWithVersion(th) ;
            m_vFlightConstraint.push_back(constrain) ;
            _file.getLine();
        }
    }
    else
    {
        int nVersion = -1;
        _file.getSubField(buf, ';');
        nVersion = atoi(buf);
        switch(nVersion)
        {
        case 1: // version 1
            ImportFileVersion1(_file);
            break;
        default:
            return FALSE;
            break;
        }
    }
 
    return TRUE ;
}

BOOL CParameters::ImportFileVersion1( ArctermFile& _file )
{
    _file.getLine();
    m_vFlightConstraint.clear();
    _file.getField(m_modelName, 255) ;
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
	m_vFlightConstraint.clear();
    for (int i = 0 ; i < size ;i++)
    {
        _file.getField(th,1024) ;
        FlightConstraint constrain(m_AirportDB) ;
        constrain.setConstraintWithVersion(th) ;
        m_vFlightConstraint.push_back(constrain) ;
        _file.getLine();
    }

    char isMultiRun;
    _file.getChar(isMultiRun);
    if(isMultiRun == 'T')
        m_enableMultiRun = true;
    else
        m_enableMultiRun = false;

    _file.getLine();
    _file.getInteger(size);

    _file.getLine();
    int nTemp;
	m_vReportRuns.clear();
    for(int i=0; i<size; i++)
    {
        _file.getInteger(nTemp);
        m_vReportRuns.push_back(nTemp);
    }
	   _file.getLine();
    return true;
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
    _file.appendValue("VERSION");
    _file.appendValue(";");
    _file.appendValue("1"); // version 1
    _file.writeLine();
	_file.writeInt(m_startTime.asSeconds()) ;
	_file.writeInt(m_endTime.asSeconds()) ;
	_file.writeInt(m_lInterval) ;
	_file.writeInt(m_nProjID) ;
	_file.writeInt((int)m_reportType) ;
	_file.writeInt(m_unitLength) ;
	_file.writeLine() ;

    if(m_enableMultiRun)
        _file.writeChar('T');
    else
        _file.writeChar('F');
    _file.writeLine();

    int nCount = (int)m_vReportRuns.size();
    _file.writeInt(nCount);
    _file.writeLine();

    for(int i=0; i<nCount; i++)
    {
        _file.writeInt(m_vReportRuns.at(i));
    }
    _file.writeLine();
}

void CParameters::ReadParameter( ArctermFile& _file )
{
    char buf[256] = {0};
    _file.getSubField(buf, ';');
    if(strcmp(buf, "VERSION") != 0)
    {
        long time = atoi(buf);
        m_startTime = ElapsedTime(time);
        _file.getInteger(time);
        m_endTime = ElapsedTime(time);

        _file.getInteger(m_lInterval);
        _file.getInteger(m_nProjID);
        int type;
        _file.getInteger(type);
        m_reportType = (enumASReportType_Detail_Summary)type;

        _file.getInteger(m_unitLength);
        _file.getLine();
    }
    else
    {
        int nVersion = -1;
        _file.getSubField(buf, ';');
        nVersion = atoi(buf);
        switch(nVersion)
        {
        case 1: // version 1
            ReadParameterVersion1(_file);
            break;
        default:
            return;
            break;
        }
    }
}

void CParameters::ReadParameterVersion1( ArctermFile& _file )
{
    _file.getLine();
    long time;
    _file.getInteger(time);
    m_startTime = ElapsedTime(time);
    _file.getInteger(time);
    m_endTime = ElapsedTime(time);

    _file.getInteger(m_lInterval);
    _file.getInteger(m_nProjID);
    int type;
    _file.getInteger(type);
    m_reportType = (enumASReportType_Detail_Summary)type;

    _file.getInteger(m_unitLength);
    _file.getLine();

    char isMultiRun;
    _file.getChar(isMultiRun);
    if(isMultiRun == 'T')
        m_enableMultiRun = true;
    else
        m_enableMultiRun = false;

    _file.getLine();
    int nCount = -1;
    _file.getInteger(nCount);

    _file.getLine();
    int nTemp;
	m_vReportRuns.clear();
    for(int i=0; i<nCount; i++)
    {
        _file.getInteger(nTemp);
        m_vReportRuns.push_back(nTemp);
    }
    _file.getLine();
}

bool CParameters::GetReportRuns(std::vector<int>& vReportRuns)
{
    if(!m_vReportRuns.empty())
    {
        vReportRuns = m_vReportRuns;
        return true;
    }
    else
    {
        return false;
    }
}

void CParameters::AddReportRuns(int nRun)
{
    m_vReportRuns.push_back(nRun);
    std::sort(m_vReportRuns.begin(), m_vReportRuns.end());
    m_vReportRuns.erase(std::unique(m_vReportRuns.begin(), m_vReportRuns.end()), m_vReportRuns.end());
}

bool CParameters::RemoveReportRuns(int nRun)
{
    int nCount = (int)m_vReportRuns.size();
    for(int i=0; i<nCount; i++)
    {
        if(m_vReportRuns.at(i) == nRun)
        {
            m_vReportRuns.erase(m_vReportRuns.begin() + i);
            return true;
        }
    }
    return false;
}
