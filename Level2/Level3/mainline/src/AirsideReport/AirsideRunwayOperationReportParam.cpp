#include "StdAfx.h"
#include ".\airsiderunwayoperationreportparam.h"

AirsideRunwayOperationReportParam::AirsideRunwayOperationReportParam(void)
{
	m_nSubType = -1;
}

AirsideRunwayOperationReportParam::~AirsideRunwayOperationReportParam(void)
{
}

void AirsideRunwayOperationReportParam::LoadParameter()
{

}

void AirsideRunwayOperationReportParam::UpdateParameter()
{

}

void AirsideRunwayOperationReportParam::ClearRunwayMark()
{
	m_vRunway.clear();
	m_vRunwayParameter.clear();
}

void AirsideRunwayOperationReportParam::AddRunwayMark( CAirsideReportRunwayMark& reportRunwayMark )
{
	if(IsRunwayMarkExist(reportRunwayMark))//exist
		return;

	m_vRunway.push_back(reportRunwayMark);
}

bool AirsideRunwayOperationReportParam::IsRunwayMarkExist( CAirsideReportRunwayMark& reportRunwayMark )
{
	for (int nMark = 0; nMark < static_cast<int>(m_vRunway.size()); ++ nMark)
	{
		if(m_vRunway.at(nMark).m_nRunwayID == reportRunwayMark.m_nRunwayID &&
			m_vRunway.at(nMark).m_enumRunwayMark== reportRunwayMark.m_enumRunwayMark)
		{
			return true;
		}
	}
	return false;
}

bool AirsideRunwayOperationReportParam::IsRunwayMarkFit( CAirsideReportRunwayMark& reportRunwayMark )
{
	for (int nMark = 0; nMark < static_cast<int>(m_vRunway.size()); ++ nMark)
	{
		if(m_vRunway.at(nMark).m_nRunwayID == reportRunwayMark.m_nRunwayID &&
			m_vRunway.at(nMark).m_enumRunwayMark== reportRunwayMark.m_enumRunwayMark)
		{
			return true;
		}
		if(m_vRunway.at(nMark).m_nRunwayID == -1 )//all
			return true;
	}
	return false;
}

bool AirsideRunwayOperationReportParam::IsClassificationExist( int nClass )
{
	for (int nClass = 0; nClass < static_cast<int>(m_vTypes.size()); ++ nClass)
	{
		if(m_vTypes.at(nClass) == nClass)
			return true;
	}
	return false;
}

void AirsideRunwayOperationReportParam::AddClassification( int nClass )
{
	if(nClass < WingspanBased || nClass >= CategoryType_Count)
		return;

	if(IsClassificationExist(nClass))
		return;

	m_vTypes.push_back((FlightClassificationBasisType)nClass);
}

void AirsideRunwayOperationReportParam::ClearClassification()
{
	m_vTypes.clear();
}

CString AirsideRunwayOperationReportParam::GetReportParamName()
{
	return  _T("RunwayOperations\\RunwayOperations.prm");
}

BOOL AirsideRunwayOperationReportParam::ExportFile(ArctermFile& _file)
{
	CParameters::ExportFile(_file);
	//write runway information

	int nRunwayCount = m_vRunwayParameter.size();
	_file.writeInt(nRunwayCount) ;
	_file.writeLine();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		_file.writeInt(m_vRunwayParameter[nRunway].m_nRunwayID);
		_file.writeInt((int)m_vRunwayParameter[nRunway].m_enumRunwayMark);
		_file.writeField(m_vRunwayParameter[nRunway].m_strMarkName);
		_file.writeLine();
	}

	//write classification
	int nClassCount = m_vTypes.size();
	_file.writeInt(nClassCount) ;
	_file.writeLine();
	for (int nClass = 0; nClass < nClassCount; ++ nClass)
	{
		_file.writeInt(m_vTypes[nClass]);
		_file.writeLine();
	}
	return TRUE;
}

BOOL AirsideRunwayOperationReportParam::ImportFile(ArctermFile& _file)
{
	CParameters::ImportFile(_file);

	int nRunwayCount;
	_file.getInteger(nRunwayCount);
	_file.getLine();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		CAirsideReportRunwayMark runwayMark;

		_file.getInteger(runwayMark.m_nRunwayID);
		int nMark = 0;
		_file.getInteger(nMark);
		if(nMark == RUNWAYMARK_SECOND)
			runwayMark.m_enumRunwayMark = RUNWAYMARK_SECOND;

		_file.getField(runwayMark.m_strMarkName.GetBuffer(1024),1024);
		runwayMark.m_strMarkName.ReleaseBuffer();
		_file.getLine();
		m_vRunwayParameter.push_back(runwayMark);
	}

	//write classification
	int nClassCount ;
	_file.getInteger(nClassCount);
	_file.getLine();
	for (int nClass = 0; nClass < nClassCount; ++ nClass)
	{
		int nType = 0;
		_file.getInteger(nType);
		m_vTypes.push_back((FlightClassificationBasisType)nType);

		_file.getLine();
	}
	return TRUE;
}

void AirsideRunwayOperationReportParam::WriteParameter( ArctermFile& _file )
{
	CParameters::WriteParameter(_file);
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
	//write runway information

	int nRunwayCount = m_vRunwayParameter.size();
	_file.writeInt(nRunwayCount) ;
	_file.writeLine();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		_file.writeInt(m_vRunwayParameter[nRunway].m_nRunwayID);
		_file.writeInt((int)m_vRunwayParameter[nRunway].m_enumRunwayMark);
		_file.writeField(m_vRunwayParameter[nRunway].m_strMarkName);
		_file.writeLine();
	}

	//write classification
	int nClassCount = m_vTypes.size();
	_file.writeInt(nClassCount) ;
	_file.writeLine();
	for (int nClass = 0; nClass < nClassCount; ++ nClass)
	{
		_file.writeInt(m_vTypes[nClass]);
		_file.writeLine();
	}
}

void AirsideRunwayOperationReportParam::ReadParameter( ArctermFile& _file )
{
	CParameters::ReadParameter(_file);

	int size = 0 ;
	if(!_file.getInteger(size) )
		return ;

	TCHAR th[1024] = {0} ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		_file.getField(th,1024) ;
		FlightConstraint constrain(m_AirportDB) ;
		constrain.setConstraintWithVersion(th) ;
		m_vFlightConstraint.push_back(constrain) ;
	}
	_file.getLine() ;


	int nRunwayCount;
	_file.getInteger(nRunwayCount);
	_file.getLine();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		CAirsideReportRunwayMark runwayMark;

		_file.getInteger(runwayMark.m_nRunwayID);
		int nMark = 0;
		_file.getInteger(nMark);
		if(nMark == RUNWAYMARK_SECOND)
			runwayMark.m_enumRunwayMark = RUNWAYMARK_SECOND;

		_file.getField(runwayMark.m_strMarkName.GetBuffer(1024),1024);
		runwayMark.m_strMarkName.ReleaseBuffer();
		_file.getLine();
		m_vRunwayParameter.push_back(runwayMark);
	}

	//write classification
	int nClassCount ;
	_file.getInteger(nClassCount);
	_file.getLine();
	for (int nClass = 0; nClass < nClassCount; ++ nClass)
	{
		int nType = 0;
		_file.getInteger(nType);
		m_vTypes.push_back((FlightClassificationBasisType)nType);

		_file.getLine();
	}








}




















