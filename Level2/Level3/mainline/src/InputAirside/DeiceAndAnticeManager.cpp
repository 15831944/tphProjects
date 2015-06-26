#include "StdAfx.h"
#include "DeiceAndAnticeManager.h"

//////////////////////////////////////////////////////////////////////////////////
///CDeiceAndAnticeInitionalSurfaceCondition
CDeiceAndAnticeInitionalSurfaceCondition::CDeiceAndAnticeInitionalSurfaceCondition(AircraftSurfacecondition emType)
:m_emType(emType)
,m_nID(-1)
{

}

CDeiceAndAnticeInitionalSurfaceCondition::~CDeiceAndAnticeInitionalSurfaceCondition()
{

}


PriorityType CDeiceAndAnticeInitionalSurfaceCondition::getPriotity(int nPriority)
{
	if (m_deicePadPriority.m_nPriority == nPriority)
	{
		return DeicePad_Type;
	}
	else if (m_depStandPriority.m_nPriority == nPriority)
	{
		return DepStand_Type;
	}
	else if (m_leadOutPriority.m_nPriority == nPriority)
	{
		return LeadOut_Type;
	}
	else if (m_remoteStandPriority.m_nPriority == nPriority)
	{
		return RemoteStand_Type;
	}

	return NONTYPE;
}

void CDeiceAndAnticeInitionalSurfaceCondition::SaveData()
{
	CString strSQL = _T("");
	if (m_nID != -1)
	{
		return UpdateData();
	}

	strSQL.Format(_T("INSERT INTO AIRDEING_CONDITION(DEICENUMBER,DEICEFLOWRATE,ANTNUMBER,\
					 ANTFLOWRATE,DEICEPADPRIORITY,DEICEPADID,DEICEPADQUEUE,DEICEPADWAITTIME,DEICEPADTYPE,\
					 DEICEPADENGINESTATE,DEPSTANDPRIORITY,DEPSTANDID,DEPSTANDWITHINTIME,DEPSTANDWAITFLUIDTIME,\
					 DEPSTANDWAITINSPECTIONTIME,DEPSTANDENGINESTATE,LEADOUTPRIORITY,LEADOUTDISTANCE,LEADOUTWITHINTIME,LEADOUTWAITFLUIDTIME,\
					 LEADOUTWAITINSPECTIONTIME,LEADOUTENGINESTATE,REMOTESTANDPRIORITY,REMOTESTANDID,REMOTESTANDWITHINTIME,\
					 REMOTESTANDWAITFLUIDTIME,REMOTESTANDWAITINSPECTIONTIME,REMOTESTANDTYPE,REMOTESTANDENGINESTATE\
					 ) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"),m_deIceVehicles.m_nNumber,m_deIceVehicles.m_nFlowRate,m_antiIceVehicles.m_nNumber,\
					 m_antiIceVehicles.m_nFlowRate,m_deicePadPriority.m_nPriority,m_deicePadPriority.m_deicePad,\
					 m_deicePadPriority.m_nDeicePadQueue,m_deicePadPriority.m_nWaittime,m_deicePadPriority.m_emType,m_deicePadPriority.m_emEngineState,\
					 m_depStandPriority.m_nPriority,m_depStandPriority.m_Stand,m_depStandPriority.m_nStandWithinTime,m_depStandPriority.m_nWaitFluidTime,\
					 m_depStandPriority.m_nWaitInspectionTime,m_depStandPriority.m_emEngineState,m_leadOutPriority.m_nPriority,m_leadOutPriority.m_nDistance,\
					 m_leadOutPriority.m_nStandWithinTime,m_leadOutPriority.m_nWaitFluidTime,m_leadOutPriority.m_nWaitInspectionTime,m_leadOutPriority.m_emEngineState,m_remoteStandPriority.m_nPriority,\
					 m_remoteStandPriority.m_Stand,m_remoteStandPriority.m_nStandWithinTime,m_remoteStandPriority.m_nWaitFluidTime,m_remoteStandPriority.m_nWaitInspectionTime,\
					 m_remoteStandPriority.m_emType,m_remoteStandPriority.m_emEngineState);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CDeiceAndAnticeInitionalSurfaceCondition::ReadData(int nDeiceAndAnticePrecipitionTypeID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AIRDEING_CONDITION WHERE ID = %d"),nDeiceAndAnticePrecipitionTypeID);
	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if(!rs.IsEOF())
	{
		m_nID = nDeiceAndAnticePrecipitionTypeID;
		rs.GetFieldValue(_T("DEICENUMBER"),m_deIceVehicles.m_nNumber);
		rs.GetFieldValue(_T("DEICEFLOWRATE"),m_deIceVehicles.m_nFlowRate);
	
		rs.GetFieldValue(_T("ANTNUMBER"),m_antiIceVehicles.m_nNumber);
		rs.GetFieldValue(_T("ANTFLOWRATE"),m_antiIceVehicles.m_nFlowRate);

		rs.GetFieldValue(_T("DEICEPADPRIORITY"),m_deicePadPriority.m_nPriority);
		rs.GetFieldValue(_T("DEICEPADID"),m_deicePadPriority.m_deicePad);
		rs.GetFieldValue(_T("DEICEPADQUEUE"),m_deicePadPriority.m_nDeicePadQueue);
		rs.GetFieldValue(_T("DEICEPADWAITTIME"),m_deicePadPriority.m_nWaittime);
		int nPostionMethod = -1;
		rs.GetFieldValue(_T("DEICEPADTYPE"),nPostionMethod);
		m_deicePadPriority.m_emType = (PostionMethod)nPostionMethod;
		int nEngineState = -1;
		rs.GetFieldValue(_T("DEICEPADENGINESTATE"),nEngineState);
		m_deicePadPriority.m_emEngineState = (EngineState)nEngineState;

		rs.GetFieldValue(_T("DEPSTANDPRIORITY"),m_depStandPriority.m_nPriority);
		rs.GetFieldValue(_T("DEPSTANDID"),m_depStandPriority.m_Stand);
		rs.GetFieldValue(_T("DEPSTANDWITHINTIME"),m_depStandPriority.m_nStandWithinTime);
		rs.GetFieldValue(_T("DEPSTANDWAITFLUIDTIME"),m_depStandPriority.m_nWaitFluidTime);
		rs.GetFieldValue(_T("DEPSTANDWAITINSPECTIONTIME"),m_depStandPriority.m_nWaitInspectionTime);
		nEngineState = -1;
		rs.GetFieldValue(_T("DEPSTANDENGINESTATE"),nEngineState);
		m_depStandPriority.m_emEngineState = (EngineState)nEngineState;

		rs.GetFieldValue(_T("LEADOUTPRIORITY"),m_leadOutPriority.m_nPriority);
		rs.GetFieldValue(_T("LEADOUTDISTANCE"),m_leadOutPriority.m_nDistance);
		rs.GetFieldValue(_T("LEADOUTWITHINTIME"),m_leadOutPriority.m_nStandWithinTime);
		rs.GetFieldValue(_T("LEADOUTWAITFLUIDTIME"),m_leadOutPriority.m_nWaitFluidTime);
		rs.GetFieldValue(_T("LEADOUTWAITINSPECTIONTIME"),m_leadOutPriority.m_nWaitInspectionTime);
		nEngineState = -1;
		rs.GetFieldValue(_T("LEADOUTENGINESTATE"),nEngineState);
		m_leadOutPriority.m_emEngineState = (EngineState)nEngineState;

		rs.GetFieldValue(_T("REMOTESTANDPRIORITY"),m_remoteStandPriority.m_nPriority);
		rs.GetFieldValue(_T("REMOTESTANDID"),m_remoteStandPriority.m_Stand);
		rs.GetFieldValue(_T("REMOTESTANDWITHINTIME"),m_remoteStandPriority.m_nStandWithinTime);
		rs.GetFieldValue(_T("REMOTESTANDWAITFLUIDTIME"),m_remoteStandPriority.m_nWaitFluidTime);
		rs.GetFieldValue(_T("REMOTESTANDWAITINSPECTIONTIME"),m_remoteStandPriority.m_nWaitInspectionTime);
		nPostionMethod = -1;
		rs.GetFieldValue(_T("REMOTESTANDTYPE"),nPostionMethod);
		m_remoteStandPriority.m_emType = (PostionMethod)nPostionMethod;

		nEngineState = -1;
		rs.GetFieldValue(_T("REMOTESTANDENGINESTATE"),nEngineState);
		m_remoteStandPriority.m_emEngineState = (EngineState)nEngineState;
	}
}

void CDeiceAndAnticeInitionalSurfaceCondition::RemoveData()
{
	if(m_nID >= 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE FROM AIRDEING_CONDITION WHERE ID = %d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CDeiceAndAnticeInitionalSurfaceCondition::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AIRDEING_CONDITION SET DEICENUMBER = %d,DEICEFLOWRATE = %d,ANTNUMBER = %d,\
					 ANTFLOWRATE = %d,DEICEPADPRIORITY = %d, DEICEPADID=%d,DEICEPADQUEUE=%d,DEICEPADWAITTIME=%d,\
					 DEICEPADTYPE=%d,DEICEPADENGINESTATE=%d,DEPSTANDPRIORITY=%d,DEPSTANDID=%d,DEPSTANDWITHINTIME=%d,\
					 DEPSTANDWAITFLUIDTIME=%d,DEPSTANDWAITINSPECTIONTIME=%d,DEPSTANDENGINESTATE=%d,LEADOUTPRIORITY=%d,\
					 LEADOUTDISTANCE=%d,LEADOUTWITHINTIME=%d,LEADOUTWAITFLUIDTIME=%d,LEADOUTWAITINSPECTIONTIME=%d,LEADOUTENGINESTATE=%d,\
					 REMOTESTANDPRIORITY=%d,REMOTESTANDID=%d,REMOTESTANDWITHINTIME=%d,REMOTESTANDWAITFLUIDTIME=%d,\
					 REMOTESTANDWAITINSPECTIONTIME=%d,REMOTESTANDTYPE=%d,REMOTESTANDENGINESTATE=%d WHERE ID = %d"),\
					 m_deIceVehicles.m_nNumber,m_deIceVehicles.m_nFlowRate,m_antiIceVehicles.m_nNumber,m_antiIceVehicles.m_nFlowRate,\
					 m_deicePadPriority.m_nPriority,m_deicePadPriority.m_deicePad,m_deicePadPriority.m_nDeicePadQueue,\
					 m_deicePadPriority.m_nWaittime,m_deicePadPriority.m_emType,m_deicePadPriority.m_emEngineState,m_depStandPriority.m_nPriority,\
					 m_depStandPriority.m_Stand,m_depStandPriority.m_nStandWithinTime,m_depStandPriority.m_nWaitFluidTime,\
					 m_depStandPriority.m_nWaitInspectionTime,m_depStandPriority.m_emEngineState,m_leadOutPriority.m_nPriority,m_leadOutPriority.m_nDistance,\
					 m_leadOutPriority.m_nStandWithinTime,m_leadOutPriority.m_nWaitFluidTime,m_leadOutPriority.m_nWaitInspectionTime,m_leadOutPriority.m_emEngineState,m_remoteStandPriority.m_nPriority,\
					 m_remoteStandPriority.m_Stand,m_remoteStandPriority.m_nStandWithinTime,m_remoteStandPriority.m_nWaitFluidTime,m_remoteStandPriority.m_nWaitInspectionTime,\
					 m_remoteStandPriority.m_emType,m_remoteStandPriority.m_emEngineState,m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

///////////////////////////////////////////////////////////////////////////////////
///CDeiceAndAnticePrecipitionType
CDeiceAndAnticePrecipitionType::CDeiceAndAnticePrecipitionType(Precipitationtype emType)
:m_emType(emType)
,m_nID(-1)
,m_clear(TY_Clear)
,m_lightFrost(TY_Light_Frost)
,m_icelayer(TY_Ice_layer)
,m_dryPowderSnow(TY_Dry_powder_snow)
,m_wetSnow(TY_Wet_Snow)
{
	
}

CDeiceAndAnticePrecipitionType::~CDeiceAndAnticePrecipitionType()
{

}

void CDeiceAndAnticePrecipitionType::SaveData()
{
	CString strSQL = _T("");
	if (m_nID != -1)
	{
		return UpdateData();
	}

	m_clear.SaveData();
	m_lightFrost.SaveData();
	m_icelayer.SaveData();
	m_dryPowderSnow.SaveData();
	m_wetSnow.SaveData();

	strSQL.Format(_T("INSERT INTO AIRDEING_PRECITPITION(CLEARID,LIGHTFROSTID,ICELAYERID,\
					 DRYPOWERSNOWID,WETSNOWID) VALUES (%d,%d,%d,%d,%d)"),m_clear.getID(),m_lightFrost.getID(),\
					 m_icelayer.getID(),m_dryPowderSnow.getID(),m_wetSnow.getID());
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CDeiceAndAnticePrecipitionType::ReadData(int nDeiceAndAnticePrecipitionTypeID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AIRDEING_PRECITPITION WHERE ID = %d"),nDeiceAndAnticePrecipitionTypeID);
	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if(!rs.IsEOF())
	{
		m_nID = nDeiceAndAnticePrecipitionTypeID;
		int nDeiceAndAnticeInitionalSurfaceConditionID = -1;
		rs.GetFieldValue(_T("CLEARID"),nDeiceAndAnticeInitionalSurfaceConditionID);
		m_clear.ReadData(nDeiceAndAnticeInitionalSurfaceConditionID);

		nDeiceAndAnticeInitionalSurfaceConditionID = -1;
		rs.GetFieldValue(_T("LIGHTFROSTID"),nDeiceAndAnticeInitionalSurfaceConditionID);
		m_lightFrost.ReadData(nDeiceAndAnticeInitionalSurfaceConditionID);
		
		nDeiceAndAnticeInitionalSurfaceConditionID = -1;
		rs.GetFieldValue(_T("ICELAYERID"),nDeiceAndAnticeInitionalSurfaceConditionID);
		m_icelayer.ReadData(nDeiceAndAnticeInitionalSurfaceConditionID);
		
		nDeiceAndAnticeInitionalSurfaceConditionID = -1;
		rs.GetFieldValue(_T("DRYPOWERSNOWID"),nDeiceAndAnticeInitionalSurfaceConditionID);
		m_dryPowderSnow.ReadData(nDeiceAndAnticeInitionalSurfaceConditionID);

		nDeiceAndAnticeInitionalSurfaceConditionID = -1;
		rs.GetFieldValue(_T("WETSNOWID"),nDeiceAndAnticeInitionalSurfaceConditionID);
		m_wetSnow.ReadData(nDeiceAndAnticeInitionalSurfaceConditionID);
	}
}

void CDeiceAndAnticePrecipitionType::RemoveData()
{
	if(m_nID >= 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE FROM AIRDEING_PRECITPITION WHERE ID = %d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);

		m_clear.RemoveData();
		m_lightFrost.RemoveData();
		m_icelayer.RemoveData();
		m_dryPowderSnow.RemoveData();
		m_wetSnow.RemoveData();
	}
}

void CDeiceAndAnticePrecipitionType::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AIRDEING_PRECITPITION SET CLEARID = %d,LIGHTFROSTID = %d,ICELAYERID = %d,\
					 DRYPOWERSNOWID = %d,WETSNOWID = %d WHERE ID = %d"),m_clear.getID(),m_lightFrost.getID(),\
					 m_icelayer.getID(),m_dryPowderSnow.getID(),m_wetSnow.getID(),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	m_clear.UpdateData();
	m_lightFrost.UpdateData();
	m_icelayer.UpdateData();
	m_dryPowderSnow.UpdateData();
	m_wetSnow.UpdateData();
}

CDeiceAndAnticeInitionalSurfaceCondition* CDeiceAndAnticePrecipitionType::GetData( AircraftSurfacecondition surf )
{
	switch (surf)
	{
	case TY_Clear:
		return getClear();
	case TY_Light_Frost:
		return getLightFrost();
	case TY_Ice_layer:
		return getIceLayer();
	case TY_Dry_powder_snow:
		return getDryPowerSnow();
	case TY_Wet_Snow:
		return getWetSnow();
	default:
		return NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////
///CDeiceAndAnticeStand
CDeiceAndAnticeStand::CDeiceAndAnticeStand()
:m_nID(-1)
,m_nonFreezing(TY_Nonfreezing)
,m_freezingDrizzle(TY_Freezingdrizzle)
,m_lightSnow(TY_LightSnow)
,m_mediumSnow(TY_MediumSnow)
,m_heavySnow(TY_Heavysnow)
{

}

CDeiceAndAnticeStand::~CDeiceAndAnticeStand()
{

}

void CDeiceAndAnticeStand::InitFromDBRecordset(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_nID);
	
		recordset.GetFieldValue(_T("STANDID"),m_standID);
		
		int nDeiceAndAnticePrecipitionTypeID = -1;
		recordset.GetFieldValue(_T("NONFREEZINGID"),nDeiceAndAnticePrecipitionTypeID);
		m_nonFreezing.ReadData(nDeiceAndAnticePrecipitionTypeID);
	
		nDeiceAndAnticePrecipitionTypeID = -1;
		recordset.GetFieldValue(_T("FREEZINGDRIZZLEID"),nDeiceAndAnticePrecipitionTypeID);
		m_freezingDrizzle.ReadData(nDeiceAndAnticePrecipitionTypeID);

		nDeiceAndAnticePrecipitionTypeID = -1;
		recordset.GetFieldValue(_T("LIGHTSNOWID"),nDeiceAndAnticePrecipitionTypeID);
		m_lightSnow.ReadData(nDeiceAndAnticePrecipitionTypeID);

		nDeiceAndAnticePrecipitionTypeID = -1;
		recordset.GetFieldValue(_T("MEDIUMSNOWID"),nDeiceAndAnticePrecipitionTypeID);
		m_mediumSnow.ReadData(nDeiceAndAnticePrecipitionTypeID);

		nDeiceAndAnticePrecipitionTypeID = -1;
		recordset.GetFieldValue(_T("HEAVYSNOWID"),nDeiceAndAnticePrecipitionTypeID);
		m_heavySnow.ReadData(nDeiceAndAnticePrecipitionTypeID);
	}
}

void CDeiceAndAnticeStand::SaveData(int nParentID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}


	m_nonFreezing.SaveData();
	m_freezingDrizzle.SaveData();
	m_lightSnow.SaveData();
	m_mediumSnow.SaveData();
	m_heavySnow.SaveData();

	CString strSQL =_T("");
	strSQL.Format(_T("INSERT INTO AIRDEING_STAND(PARENTID,STANDID,NONFREEZINGID,\
					 FREEZINGDRIZZLEID,LIGHTSNOWID,MEDIUMSNOWID,HEAVYSNOWID) VALUES\
					 (%d,%d,%d,%d,%d,%d,%d)"),nParentID,m_standID,m_nonFreezing.getID(),m_freezingDrizzle.getID(),\
					 m_lightSnow.getID(),m_mediumSnow.getID(),m_heavySnow.getID());
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CDeiceAndAnticeStand::UpdateData()
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE AIRDEING_STAND SET STANDID = %d\
					 WHERE ID = %d"),m_standID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
	m_nonFreezing.UpdateData();
	m_freezingDrizzle.UpdateData();
	m_lightSnow.UpdateData();
	m_mediumSnow.UpdateData();
	m_heavySnow.UpdateData();
}

void CDeiceAndAnticeStand::RemoveData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AIRDEING_STAND WHERE (ID = %d);"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	m_nonFreezing.RemoveData();
	m_freezingDrizzle.RemoveData();
	m_lightSnow.RemoveData();
	m_mediumSnow.RemoveData();
	m_heavySnow.RemoveData();
}

CDeiceAndAnticeInitionalSurfaceCondition* CDeiceAndAnticeStand::GetData( Precipitationtype preT,AircraftSurfacecondition surf )
{
	switch (preT)
	{
	case TY_Nonfreezing:
		return m_nonFreezing.GetData(surf);
	case TY_Freezingdrizzle:
		return m_freezingDrizzle.GetData(surf);
	case TY_LightSnow:
		return m_lightSnow.GetData(surf);
	case TY_MediumSnow:
		return m_mediumSnow.GetData(surf);
	case TY_Heavysnow:
		return m_heavySnow.GetData(surf);
	default:
		return NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////
///CDeiceAndAnticeFlightType
CDeiceAndAnticeFlightType::CDeiceAndAnticeFlightType()
:m_nID(-1)
{

}

CDeiceAndAnticeFlightType::~CDeiceAndAnticeFlightType()
{
	std::vector<CDeiceAndAnticeStand*>::iterator iter = m_vStand.begin();
	for (; iter != m_vStand.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vDelStand.begin();
	for (; iter != m_vDelStand.end(); ++iter)
	{
		delete *iter;
	}
	m_vStand.clear();
	m_vDelStand.clear();
}

int CDeiceAndAnticeFlightType::getCount()
{
	return (int)m_vStand.size();
}

void CDeiceAndAnticeFlightType::addItem(CDeiceAndAnticeStand* pItem)
{
	ASSERT(pItem);
	m_vStand.push_back(pItem);
}

CDeiceAndAnticeStand* CDeiceAndAnticeFlightType::getItem(int ndx)
{
	ASSERT(ndx >= 0 && ndx < getCount());
	return m_vStand[ndx];
}

void CDeiceAndAnticeFlightType::removeItem(CDeiceAndAnticeStand* pItem)
{
	std::vector<CDeiceAndAnticeStand*>::iterator iter;
	iter = std::find(m_vStand.begin(),m_vStand.end(),pItem);
	if (iter != m_vStand.end())
	{
		m_vDelStand.push_back(*iter);
		m_vStand.erase(iter);
	}
}

void CDeiceAndAnticeFlightType::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AIRDEING_STAND WHERE PARENTID = %d"),m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while(!adoRecordset.IsEOF())
	{
		CDeiceAndAnticeStand* pItem = new CDeiceAndAnticeStand();
		pItem->InitFromDBRecordset(adoRecordset);
		m_vStand.push_back(pItem);
		adoRecordset.MoveNextData();
	}
}

void CDeiceAndAnticeFlightType::InitFromDBRecordset(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_nID);
		CString strFltType;
		recordset.GetFieldValue(_T("FLIGHTTYPE"),strFltType);

		if (m_pAirportDB)
		{
			m_fltType.SetAirportDB(m_pAirportDB);
			m_fltType.setConstraintWithVersion(strFltType.GetBuffer(strFltType.GetLength()+1));
			strFltType.ReleaseBuffer(strFltType.GetLength()+1);
		}
	}
	ReadData();
}

void CDeiceAndAnticeFlightType::SaveData(int nParentID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	else
	{
		CString strSQL = _T("");
		char szFltType[1024] = {0};
		CString strFltType = _T("");
		m_fltType.WriteSyntaxStringWithVersion(szFltType);
		strFltType = szFltType;
		strSQL.Format(_T("INSERT INTO AIRDEING_FLIGHTTYPE (PARENTID,FLIGHTTYPE) VALUES (%d,'%s')"),nParentID,strFltType);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<CDeiceAndAnticeStand*>::iterator iter = m_vStand.begin();
	for (; iter != m_vStand.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vDelStand.begin();
	for (; iter != m_vDelStand.end(); ++iter)
	{
		(*iter)->RemoveData();
	}
}

void CDeiceAndAnticeFlightType::UpdateData()
{
	char szFltType[1024] = {0};
	CString strFltType = _T("");
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strFltType = szFltType;
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AIRDEING_FLIGHTTYPE\
					 SET FLIGHTTYPE ='%s' WHERE (ID = %d)"),strFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CDeiceAndAnticeStand *>::iterator iter = m_vStand.begin();
	for (;iter != m_vStand.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vDelStand.begin();
	for (;iter != m_vDelStand.end(); ++iter)
	{
		(*iter)->RemoveData();
	}
}

void CDeiceAndAnticeFlightType::RemoveData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AIRDEING_FLIGHTTYPE WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CDeiceAndAnticeStand *>::iterator iter = m_vStand.begin();
	for (;iter != m_vStand.end(); ++iter)
	{
		(*iter)->RemoveData();
	}

	iter = m_vDelStand.begin();
	for (;iter != m_vDelStand.end(); ++iter)
	{
		(*iter)->RemoveData();
	}
}

#include <inputairside/ALTObjectGroup.h>
CDeiceAndAnticeInitionalSurfaceCondition* CDeiceAndAnticeFlightType::GetData(const ALTObjectID& stand, Precipitationtype preT,AircraftSurfacecondition surf )
{
	for(int i=0;i<(int)m_vStand.size();i++)
	{
		CDeiceAndAnticeStand* pStandSet = m_vStand.at(i);
		ALTObjectGroup altgrp;
		altgrp.ReadData( pStandSet->getStand() );
		if( stand.idFits(altgrp.getName()) )
		{
			return pStandSet->GetData(preT,surf);
		}
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////
///CDeiceAndAnticeManager
CDeiceAndAnticeManager::CDeiceAndAnticeManager(CAirportDatabase* pAirportDB)
:m_pAirportDB(pAirportDB)
{

}

CDeiceAndAnticeManager::~CDeiceAndAnticeManager()
{
	std::vector<CDeiceAndAnticeFlightType*>::iterator iter = m_vFlightType.begin();
	for (; iter != m_vFlightType.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vDelFlightType.begin();
	for (; iter != m_vDelFlightType.end(); ++iter)
	{
		delete *iter;
	}
	m_vFlightType.clear();
	m_vDelFlightType.clear();
}

int CDeiceAndAnticeManager::getCount()
{
	return (int)m_vFlightType.size();
}


void CDeiceAndAnticeManager::addItem(CDeiceAndAnticeFlightType* pItem)
{
	ASSERT(pItem);
	m_vFlightType.push_back(pItem);
}

CDeiceAndAnticeFlightType* CDeiceAndAnticeManager::getItem(int ndx)
{
	ASSERT(ndx >= 0 && ndx < getCount());
	return m_vFlightType[ndx];
}

void CDeiceAndAnticeManager::removeItem(CDeiceAndAnticeFlightType* pItem)
{
	std::vector<CDeiceAndAnticeFlightType*>::iterator iter;
	iter = std::find(m_vFlightType.begin(),m_vFlightType.end(),pItem);
	if (iter != m_vFlightType.end())
	{
		m_vDelFlightType.push_back(*iter);
		m_vFlightType.erase(iter);
	}
}

void CDeiceAndAnticeManager::ReadData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AIRDEING_FLIGHTTYPE WHERE PARENTID = %d "),nProjID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while(!adoRecordset.IsEOF())
	{
		CDeiceAndAnticeFlightType* pItem = new CDeiceAndAnticeFlightType();
		pItem->SetAirportDB(m_pAirportDB);
		pItem->InitFromDBRecordset(adoRecordset);
		m_vFlightType.push_back(pItem);
		adoRecordset.MoveNextData();
	}
}

void CDeiceAndAnticeManager::SaveData(int nProjID)
{
	
		std::vector<CDeiceAndAnticeFlightType*>::iterator iter = m_vFlightType.begin();
		for (; iter != m_vFlightType.end(); ++iter)
		{
			(*iter)->SaveData(nProjID);
		}

		iter = m_vDelFlightType.begin();
		for (; iter != m_vDelFlightType.end(); ++iter)
		{
			(*iter)->RemoveData();
		}
	
}


class FlightTypeOrderLess
{
public:
	bool operator()(CDeiceAndAnticeFlightType* pT1, CDeiceAndAnticeFlightType* pT2)
	{
		if(pT1 && pT2 )
		{
			return pT1->GetFltType().fits(pT2->GetFltType())!=0;
		}
		return false;
	}
};
void CDeiceAndAnticeManager::SortByFltType()
{
	std::sort(m_vFlightType.begin(),m_vFlightType.end(), FlightTypeOrderLess() );
}