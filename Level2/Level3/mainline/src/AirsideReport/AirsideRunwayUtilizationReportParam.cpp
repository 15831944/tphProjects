#include "StdAfx.h"
#include ".\airsiderunwayutilizationreportparam.h"

CAirsideRunwayUtilizationReportParam::CAirsideRunwayUtilizationReportParam(void)
{
}

CAirsideRunwayUtilizationReportParam::~CAirsideRunwayUtilizationReportParam(void)
{
}

void CAirsideRunwayUtilizationReportParam::LoadParameter()
{

}

void CAirsideRunwayUtilizationReportParam::UpdateParameter()
{

}

BOOL CAirsideRunwayUtilizationReportParam::ExportFile( ArctermFile& _file )
{
	CParameters::ExportFile(_file);

	//write runway parameter
	int nCount = (int)m_vReportMarkParam.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		m_vReportMarkParam[nItem].WriteParameter(_file);
		_file.writeLine();
	}
	return FALSE;
}

BOOL CAirsideRunwayUtilizationReportParam::ImportFile( ArctermFile& _file )
{
	CParameters::ImportFile(_file);

	//read runway parameter data
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CRunwayMarkParam runwayParam;
		runwayParam.ReadParameter(_file,m_AirportDB);
		_file.getLine();
		m_vReportMarkParam.push_back(runwayParam);
	}
	return FALSE;
}
CString CAirsideRunwayUtilizationReportParam::GetReportParamName()
{
	return  _T("RunwayUtilization\\RunwayUtilization.prm");
}
void CAirsideRunwayUtilizationReportParam::WriteParameter( ArctermFile& _file )
{
	CParameters::WriteParameter(_file);

	//write runway parameter
	int nCount = (int)m_vReportMarkParam.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		m_vReportMarkParam[nItem].WriteParameter(_file);
		_file.writeLine();
	}

}

void CAirsideRunwayUtilizationReportParam::ReadParameter( ArctermFile& _file )
{
	m_vReportMarkParam.clear();
	CParameters::ReadParameter(_file);

	//read runway parameter data
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CRunwayMarkParam runwayParam;
		runwayParam.ReadParameter(_file,m_AirportDB);
		_file.getLine();
		m_vReportMarkParam.push_back(runwayParam);
	}

}

bool CAirsideRunwayUtilizationReportParam::IsRunwayParamDefault()
{
	if(m_vReportMarkParam.size() == 0)
		return true;

	for (int nMarkParam = 0; nMarkParam < (int)m_vReportMarkParam.size(); ++ nMarkParam)
	{
		if(m_vReportMarkParam[nMarkParam].IsDefault())
			return true;
	}

	return false;
}
CString CAirsideRunwayUtilizationReportParam::CRunwayMarkParam::GetRunwayNames()
{
	CString strRunwayParamName = _T("All");
	if(m_vRunwayMark.size() > 0)
		strRunwayParamName = _T("");

	for (int nRunway = 0; nRunway < (int)m_vRunwayMark.size(); ++nRunway)
	{
		strRunwayParamName += m_vRunwayMark[nRunway].m_strMarkName;
		strRunwayParamName +=_T(", ");
	}
	strRunwayParamName.Trim();
	strRunwayParamName.TrimRight(_T(","));
	
	return strRunwayParamName;
}

bool CAirsideRunwayUtilizationReportParam::CRunwayMarkParam::IsRunwayMarkFit( CAirsideReportRunwayMark& reportRunwayMark )
{
	for (int nMark = 0; nMark < static_cast<int>(m_vRunwayMark.size()); ++ nMark)
	{
		if(m_vRunwayMark.at(nMark).m_nRunwayID == reportRunwayMark.m_nRunwayID &&
			m_vRunwayMark.at(nMark).m_enumRunwayMark== reportRunwayMark.m_enumRunwayMark)
		{
			return true;
		}
		if(m_vRunwayMark.at(nMark).m_nRunwayID == -1 )//all
			return true;
	}
	return false;
}

void CAirsideRunwayUtilizationReportParam::CRunwayMarkParam::WriteParameter( ArctermFile& _file )
{

	//runway mark data
	int nMarkCount = (int)m_vRunwayMark.size();
	_file.writeInt(nMarkCount);
	for (int nMark = 0; nMark < nMarkCount; ++nMark)//for each mark
	{
		CAirsideReportRunwayMark& runwayMark = m_vRunwayMark[nMark];
		_file.writeInt(runwayMark.m_nRunwayID);
		_file.writeInt((int)runwayMark.m_enumRunwayMark);
		_file.writeField(runwayMark.m_strMarkName);
	}
	_file.writeLine();

	//write operation data
	int nOpCount = (int)m_vOperationParam.size();
	_file.writeInt(nOpCount);
	_file.writeLine();
	for (int nOp = 0; nOp < nOpCount; ++nOp)
	{
		CRunwayOperationParam& runwayOp =m_vOperationParam[nOp];
		runwayOp.WriteParameter(_file);
		_file.writeLine();
	}
}

void CAirsideRunwayUtilizationReportParam::CRunwayMarkParam::ReadParameter( ArctermFile& _file,CAirportDatabase *pAirportDatabase  )
{
	//runway mark data
	int nMarkCount = 0;
	_file.getInteger(nMarkCount);
	for (int nMark = 0; nMark < nMarkCount; ++nMark)//for each mark
	{
		CAirsideReportRunwayMark runwayMark;
		_file.getInteger(runwayMark.m_nRunwayID);
		_file.getInteger((int&)runwayMark.m_enumRunwayMark);
		_file.getField(runwayMark.m_strMarkName.GetBuffer(1024),1024);
		runwayMark.m_strMarkName.ReleaseBuffer();
		m_vRunwayMark.push_back(runwayMark);
	}
	_file.getLine();

	//write operation data
	int nOpCount = 0;
	_file.getInteger(nOpCount);
	_file.getLine();
	for (int nOp = 0; nOp < nOpCount; ++nOp)
	{
		CRunwayOperationParam runwayOp;
		runwayOp.ReadParameter(_file,pAirportDatabase);
		_file.getLine();
		m_vOperationParam.push_back(runwayOp);
	}
}

bool CAirsideRunwayUtilizationReportParam::CRunwayMarkParam::IsDefault()
{
	std::vector<CAirsideReportRunwayMark>::iterator iterMark = m_vRunwayMark.begin();
	for (;iterMark != m_vRunwayMark.end(); ++iterMark)
	{
		if((*iterMark).m_nRunwayID == -1)
			break;

	}
	if(iterMark == m_vRunwayMark.end())//no runway mark is default
		return false;


	std::vector<CRunwayOperationParam>::iterator iterOp = m_vOperationParam.begin();
	for(; iterOp != m_vOperationParam.end(); ++iterOp)
	{
		if((*iterOp).IsDefault())
			return true;
	}

	return false;
}

CString CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::GetOperationName()
{
	if(m_enumOperation == Both)
		return _T("All");

	if(m_enumOperation == Landing)
		return _T("Landing");

	if(m_enumOperation == TakeOff)
		return _T("Take Off");

	return _T("All");
}

bool CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::fitAirRoute( const CAirRouteParam& airRoutePassed )
{
    int nAirRouteCount = (int)m_vAirRoute.size();
	for (int nAirRoute = 0; nAirRoute < nAirRouteCount; ++nAirRoute)
	{
		CAirRouteParam& curAirRoute = m_vAirRoute[nAirRoute];
		if(curAirRoute.m_nRouteID == -1 || curAirRoute.m_nRouteID == airRoutePassed.m_nRouteID)
			return true;
	}

	return false;

}

bool CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::fitFltCons( const AirsideFlightDescStruct& fltConsPassed, char mode)
{
	if(IsDefault())
		return true;

	FlightConstraint fltcost;
	fltcost.initFlightType(fltConsPassed,mode);

	size_t nFltConsCount = m_vFlightConstraint.size();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(m_vFlightConstraint[nfltCons].fits(fltcost))
		{
			return true;
		}
	}

	return false;
}

void CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::WriteParameter( ArctermFile& _file )
{
	_file.writeInt((int)m_enumOperation);
	
	int nAirRouteCount = (int)m_vAirRoute.size();
	_file.writeInt(nAirRouteCount);

	for (int nAirRoute = 0; nAirRoute < nAirRouteCount; ++nAirRoute)
	{
		CAirRouteParam& airRouteParam = m_vAirRoute[nAirRoute];
		_file.writeInt(airRouteParam.m_nRouteID);
		_file.writeField(airRouteParam.m_nRouteName);
	}

	int nFltConsCount = (int)m_vFlightConstraint.size();
	_file.writeInt(nFltConsCount);

	for (int nFlt = 0; nFlt < nFltConsCount; ++nFlt)
	{
		TCHAR th[1024] = {0} ;
		m_vFlightConstraint[nFlt].WriteSyntaxStringWithVersion(th) ;
		_file.writeField(th) ;

	}

}

void CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::ReadParameter( ArctermFile& _file,CAirportDatabase *pAirportDatabase )
{
	_file.getInteger((int&)m_enumOperation);

	int nAirRouteCount = 0;
	_file.getInteger(nAirRouteCount);

	for (int nAirRoute = 0; nAirRoute < nAirRouteCount; ++nAirRoute)
	{
		CAirRouteParam airRouteParam;
		_file.getInteger(airRouteParam.m_nRouteID);
		_file.getField(airRouteParam.m_nRouteName.GetBuffer(1024),1024);
		airRouteParam.m_nRouteName.ReleaseBuffer();
		m_vAirRoute.push_back(airRouteParam);

	}

	int nFltConsCount = 0;
	_file.getInteger(nFltConsCount);

	for (int nFlt = 0; nFlt < nFltConsCount; ++nFlt)
	{
		TCHAR th[1024] = {0} ;
		_file.getField(th,1024) ;
		FlightConstraint constrain(pAirportDatabase) ;
		constrain.setConstraintWithVersion(th) ;
		m_vFlightConstraint.push_back(constrain) ;
	}

}

bool CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::IsDefault()
{
	return IsAirRouteDefault() && IsFltConsDefault();
}

bool CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::IsAirRouteDefault()
{
	if(m_vAirRoute.size() == 0)//default is all
		return true;

	int nAirRouteCount = (int)m_vAirRoute.size();


	for (int nAirRoute = 0; nAirRoute < nAirRouteCount; ++nAirRoute)
	{
		CAirRouteParam& airRouteParam = m_vAirRoute[nAirRoute];
		if(airRouteParam.m_nRouteID == -1)
			return true;
	}
	return false;
}

bool CAirsideRunwayUtilizationReportParam::CRunwayOperationParam::IsFltConsDefault()
{
	if(m_vFlightConstraint.size() == 0)
		return true;

	FlightConstraint fltDefault;
	int nFltConsCount = (int)m_vFlightConstraint.size();
	for (int nFlt = 0; nFlt < nFltConsCount; ++nFlt)
	{
		if(m_vFlightConstraint[nFlt].isDefault())
			return true;
	}

	return false;
}

