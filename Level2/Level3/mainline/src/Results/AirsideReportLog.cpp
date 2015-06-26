#include "StdAfx.h"
#include ".\airsidereportlog.h"
#include "../Engine/terminal.h"
#include "common\SimAndReportManager.h"
#include ".\ARCBaseLog.hpp"



AirsideReportLog::~AirsideReportLog(void){}

void AirsideReportLog::OpenRead( Terminal* pTerminal,const CString& _csProjPath )
{
	m_pSimAndReportManager = pTerminal->GetSimReportManager();

	
	int iResult = GetSimReportManager()->getSubSimResultCout();
	if (iResult < 1) //no simulation result
	{
		return;
	}
	
	//load flight log
	{
		CString sDescFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideFlightDescFileReport, _csProjPath );
		CString sEventFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideFlightEventFileReport,_csProjPath);

		mFlightLogData.OpenInput(sDescFileName.GetString(), sEventFileName.GetString());
	}

	//load vehicle log
	{
		CString sDescFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideVehicleDescFileReport, _csProjPath );
		CString sEventFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideVehicleEventFileReport,_csProjPath);
		
		mVehicleLogData.OpenInput(sDescFileName.GetString(),sEventFileName.GetString());
		
	}

	//load altobject log
	{
		CString sAltObjectFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideALTObjectListFile, _csProjPath );
		mAltObjectLogData.OpenObjectInput(sAltObjectFileName.GetString());
	}
}

void AirsideReportLog::OpenWrite( Terminal* pTerminal,const CString& _csProjPath )
{
	m_pSimAndReportManager = pTerminal->GetSimReportManager();


	int iResult = GetSimReportManager()->getSubSimResultCout();
	if (iResult < 1) //no simulation result
	{
		return;
	}

	//load flight log
	{
		CString sEventFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideFlightEventFileReport,_csProjPath);

		mFlightLogData.OpenOutput(sEventFileName.GetString());
	}

	//load vehicle log
	{
		CString sEventFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideVehicleEventFileReport,_csProjPath);

		mVehicleLogData.OpenOutput(sEventFileName.GetString());

	}

	//load altobject log
	{
		CString sAltObjectFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideALTObjectListFile, _csProjPath );
		mAltObjectLogData.Clean();
	}
}

void AirsideReportLog::CloseSave( const CString& _csProjPath )
{
	ASSERT(GetSimReportManager());
	int iResult = GetSimReportManager()->getSubSimResultCout();
	if (iResult < 1) //no simulation result
	{
		return;
	}
	
	{//save flight log
		CString sDescFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideFlightDescFileReport, _csProjPath );
		mFlightLogData.Save(sDescFileName.GetString());
	}

	{//save vehicle log
		CString sDescFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideVehicleDescFileReport, _csProjPath );
		mVehicleLogData.Save(sDescFileName.GetString());
	}

	{//save altobject log
		CString sAltObjectFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideALTObjectListFile, _csProjPath );
		mAltObjectLogData.Save(sAltObjectFileName.GetString());
	}
}

void AirsideReportLog::AddFlightLogItem( AirsideFlightLogItem& fltLog )
{
	mFlightLogData.SaveItem(fltLog);
}

void AirsideReportLog::AddVehicleLogItem( AirsideVehicleLogItem& vehLog )
{
	mVehicleLogData.SaveItem(vehLog);
}

AirsideReportLog::AirsideReportLog( void )
{
	m_pSimAndReportManager = NULL;
}

CSimAndReportManager* AirsideReportLog::GetSimReportManager()
{
	return m_pSimAndReportManager;
}

void AirsideReportLog::AddAltObjectLogItem( AirsideALTObjectLogItem& objLog )
{
	mAltObjectLogData.SaveObjectItem(objLog);
}
