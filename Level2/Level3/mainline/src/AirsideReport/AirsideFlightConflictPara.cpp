#include "StdAfx.h"
#include ".\airsideflightconflictpara.h"

AirsideFlightConflictPara::AirsideFlightConflictPara(void)
{
	m_nSubType = 0;
	m_pSummaryFilter = new SummaryDataFilter(-1,"",-1,-1,-1);
}

AirsideFlightConflictPara::~AirsideFlightConflictPara(void)
{
	delete m_pSummaryFilter;
	m_pSummaryFilter = NULL;
}

void AirsideFlightConflictPara::LoadParameter()
{

}

void AirsideFlightConflictPara::UpdateParameter()
{

}

void AirsideFlightConflictPara::ClearAreas()
{
	m_vAreaPara.clear();
}

void AirsideFlightConflictPara::AddArea(const ALTObjectID& altName)
{
	if (IsAreaFit(altName))
		return;

	m_vAreaPara.push_back(altName);

}

bool AirsideFlightConflictPara::IsAreaFit(const ALTObjectID& altName)
{
	int nCount = m_vAreaPara.size();
	for (int i = 0; i < nCount; i++)
	{
		ALTObjectID objName = m_vAreaPara.at(i);
		if (altName.idFits(objName))
			return true;
	}
	return false;
}

void AirsideFlightConflictPara::setSummaryFilterPara(int nConType, int nAreaIdx, int nLocType, int OpType, int ActType)
{
	m_pSummaryFilter->m_nConflictType = nConType;
	if (nAreaIdx == -1 || nAreaIdx >= (int)m_vAreaPara.size())
		m_pSummaryFilter->m_strArea = "";
	else
	{
		ALTObjectID objName = m_vAreaPara.at(nAreaIdx);
		m_pSummaryFilter->m_strArea = objName.GetIDString();
	}

	m_pSummaryFilter->m_nLocationType = nLocType;
	m_pSummaryFilter->m_nOperationType = OpType;
	m_pSummaryFilter->m_nActionType = ActType;
}

SummaryDataFilter* AirsideFlightConflictPara::getSummaryDataFilter() const
{
	return m_pSummaryFilter;
}

CString AirsideFlightConflictPara::GetReportParamName()
{
	return  _T("FlightConflicts\\FlightConflicts.prm");
}

void AirsideFlightConflictPara::WriteParameter( ArctermFile& _file )
{
	_file.writeInt(m_startTime.asSeconds()) ;
	_file.writeInt(m_endTime.asSeconds()) ;
	_file.writeInt(m_lInterval) ;
	_file.writeInt(m_nProjID) ;
	_file.writeInt((int)m_reportType) ;
	_file.writeInt(m_unitLength) ;
	_file.writeLine() ;

	//write reporting area
	int nCount = m_vAreaPara.size();
	_file.writeInt(nCount);
	_file.writeLine();

	std::vector<ALTObjectID>::iterator iter;
	for (iter = m_vAreaPara.begin(); iter != m_vAreaPara.end(); iter++)
	{
		_file.writeField((*iter).GetIDString());
		_file.writeLine();
	}
}

void AirsideFlightConflictPara::ReadParameter( ArctermFile& _file )
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

	int nCount;
	_file.getInteger(nCount);
	_file.getLine();
	for (int i = 0; i < nCount; ++i)
	{
		CString strName;
		_file.getField(strName.GetBuffer(1024),1024);
		strName.ReleaseBuffer();
		ALTObjectID altName(strName);
		m_vAreaPara.push_back(altName);
		_file.getLine();

	}
}

BOOL AirsideFlightConflictPara::ExportFile(ArctermFile& _file)
{
	_file.writeInt(m_startTime.asSeconds()) ;
	_file.writeInt(m_endTime.asSeconds()) ;
	_file.writeInt(m_lInterval) ;
	_file.writeInt(m_nProjID) ;
	_file.writeInt((int)m_reportType) ;
	_file.writeInt(m_unitLength) ;
	_file.writeLine() ;

	//write reporting area
	int nCount = m_vAreaPara.size();
	_file.writeInt(nCount);
	_file.writeLine();

	std::vector<ALTObjectID>::iterator iter;
	for (iter = m_vAreaPara.begin(); iter != m_vAreaPara.end(); iter++)
	{
		_file.writeField((*iter).GetIDString());
		_file.writeLine();
	}
	return TRUE;
}

BOOL AirsideFlightConflictPara::ImportFile(ArctermFile& _file)
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

	int nCount;
	_file.getInteger(nCount);
	_file.getLine();
	for (int i = 0; i < nCount; ++i)
	{
		CString strName;
		_file.getField(strName.GetBuffer(1024),1024);
		strName.ReleaseBuffer();
		ALTObjectID altName(strName);
		m_vAreaPara.push_back(altName);
		_file.getLine();
	}
	return TRUE;
}