#include "StdAfx.h"
#include ".\depclimbout.h"
//#include "../Inputs/IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"

using namespace ADODB;

DepClimbOuts::DepClimbOuts(CAirportDatabase* pAirportDB)
 : m_pInputTerminal(NULL)
 , m_pAirportDB(pAirportDB)
{
}

DepClimbOuts::~DepClimbOuts()
{
}
void DepClimbOuts::ReadData(int nProjID)
{
	//InsertDataIfNeed();
	m_nProjID = nProjID;
	CString strSQL;
	strSQL.Format("SELECT * FROM FltPerformDepClimbOut WHERE ProjID = %d", m_nProjID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	m_vDepClimbOuts.clear();

	while (!pRS->GetadoEOF())
	{
		DepClimbOut depClimbOut;
		depClimbOut.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

		CString strFltType = (char*)(_bstr_t)pRS->Fields->GetItem(_bstr_t("FltType"))->GetValue();
		depClimbOut.m_fltType.SetAirportDB(m_pAirportDB);
		depClimbOut.m_fltType.setConstraintWithVersion(strFltType);
		//depClimbOut.m_fltType = strFltType;

		depClimbOut.m_nMinHorAccel = (double)pRS->Fields->GetItem(_bstr_t("MinHorAccel"))->GetValue();
		depClimbOut.m_nMaxHorAccel = (double)pRS->Fields->GetItem(_bstr_t("MaxHorAccel"))->GetValue();
		depClimbOut.m_nMinVerticalSpeedToEntoute = (double)pRS->Fields->GetItem(_bstr_t("MinVerticalSpeed"))->GetValue();
		depClimbOut.m_nMaxVerticalSpeedToEntoute = (double)pRS->Fields->GetItem(_bstr_t("MaxVerticalSpeed"))->GetValue();
		depClimbOut.m_fuelBurn = (double)pRS->Fields->GetItem(_bstr_t("FuelBurn"))->GetValue();
		depClimbOut.m_cost = (double)pRS->Fields->GetItem(_bstr_t("Cost"))->GetValue();

		m_vDepClimbOuts.push_back(depClimbOut);

		pRS->MoveNext();
	}
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

	if(m_vDepClimbOuts.size() < 1)
	{
		DepClimbOut depClimbOut;
		InsertDepClimbOut(depClimbOut);
		GetDepClimbOuts().push_back(depClimbOut);
	}

}

void DepClimbOuts::InsertDepClimbOut(DepClimbOut& depClimbOut)
{
	CString strSQL;
	char szFltType[1024] = {0};
	depClimbOut.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("INSERT INTO FltPerformDepClimbOut \
				  (ProjID, FltType, MinHorAccel, MaxHorAccel,MinVerticalSpeed, MaxVerticalSpeed, FuelBurn, Cost)\
				  VALUES(%d, '%s', %f, %f, %f, %f, %f, %f)",
				  m_nProjID, szFltType,
				  depClimbOut.m_nMinHorAccel,
				  depClimbOut.m_nMaxHorAccel,
				  depClimbOut.m_nMinVerticalSpeedToEntoute,
				  depClimbOut.m_nMaxVerticalSpeedToEntoute,
				  depClimbOut.m_fuelBurn,
				  depClimbOut.m_cost
				  );

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	strSQL.Format("SELECT TOP 1 ID FROM FltPerformDepClimbOut ORDER BY ID DESC");
	pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (!pRS->GetadoEOF())
		depClimbOut.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
	
}

void DepClimbOuts::UpdateDepClimbOut(DepClimbOut& depClimbOut)
{
	CString strSQL;
	char szFltType[1024] = {0};
	depClimbOut.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("UPDATE FltPerformDepClimbOut \
				  SET FltType='%s', MinHorAccel=%f ,MaxHorAccel=%f ,MinVerticalSpeed=%f,\
				  MaxVerticalSpeed=%f, FuelBurn=%f, Cost=%f, ProjID=%d WHERE id = %d",
				  szFltType, depClimbOut.m_nMinHorAccel,depClimbOut.m_nMaxHorAccel,
				  depClimbOut.m_nMinVerticalSpeedToEntoute,depClimbOut.m_nMaxVerticalSpeedToEntoute,
				  depClimbOut.m_fuelBurn, depClimbOut.m_cost, 
				  m_nProjID, depClimbOut.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}

void DepClimbOuts::DeleteDepClimbOut(DepClimbOut& depClimbOut)
{
	CString strSQL;
	strSQL.Format("Delete From FltPerformDepClimbOut WHERE ID = %d", depClimbOut.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

}
void DepClimbOuts::ExportDepClimbOuts(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	DepClimbOuts depClimbOuts(pAirportDB);
	depClimbOuts.ReadData(exportFile.GetProjectID());
	depClimbOuts.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void DepClimbOuts::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("DepClimbOuts");
	exportFile.getFile().writeLine();

	std::vector<DepClimbOut>::iterator iter = m_vDepClimbOuts.begin();
	for (;iter != m_vDepClimbOuts.end(); ++iter)
	{
		exportFile.getFile().writeInt(iter->m_nID);
		char szFltType[1024] = {0};
		(iter->m_fltType).WriteSyntaxStringWithVersion(szFltType);
		exportFile.getFile().writeField(szFltType);
		exportFile.getFile().writeDouble(iter->m_nMinHorAccel);
		exportFile.getFile().writeDouble(iter->m_nMaxHorAccel);
		exportFile.getFile().writeDouble(iter->m_nMinVerticalSpeedToEntoute);
		exportFile.getFile().writeDouble(iter->m_nMaxVerticalSpeedToEntoute);
		exportFile.getFile().writeDouble(iter->m_fuelBurn);
		exportFile.getFile().writeDouble(iter->m_cost);
		exportFile.getFile().writeLine();
	}
}
void DepClimbOuts::ImportDepClimbOuts(CAirsideImportFile& importFile)
{
	DepClimbOuts depClimbOuts(NULL);
	depClimbOuts.SetProjectID(importFile.getNewProjectID());
	while (!importFile.getFile().isBlank())
	{
		DepClimbOut depClimbOut;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		CString strFltType;
		importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

		importFile.getFile().getFloat(depClimbOut.m_nMinHorAccel);
		importFile.getFile().getFloat(depClimbOut.m_nMaxHorAccel);
		importFile.getFile().getFloat(depClimbOut.m_nMinVerticalSpeedToEntoute);
		importFile.getFile().getFloat(depClimbOut.m_nMaxVerticalSpeedToEntoute);
		importFile.getFile().getFloat(depClimbOut.m_fuelBurn);
		importFile.getFile().getFloat(depClimbOut.m_cost);


		depClimbOuts.ImportInsertDepClimbOut(depClimbOut,strFltType);
		importFile.getFile().getLine();

	}
}

void DepClimbOuts::ImportInsertDepClimbOut(DepClimbOut& depClimbOut,CString strFltType)
{
	CString strSQL;
	strSQL.Format("INSERT INTO FltPerformDepClimbOut \
				  (ProjID, FltType, MinHorAccel, MaxHorAccel,MinVerticalSpeed, MaxVerticalSpeed, FuelBurn, Cost)\
				  VALUES(%d, '%s', %f, %f, %f, %f, %f, %f)",
				  m_nProjID, strFltType,
				  depClimbOut.m_nMinHorAccel,
				  depClimbOut.m_nMaxHorAccel,
				  depClimbOut.m_nMinVerticalSpeedToEntoute,
				  depClimbOut.m_nMaxVerticalSpeedToEntoute,
				  depClimbOut.m_fuelBurn,
				  depClimbOut.m_cost
				  );

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
}
bool FlightTypeCompare(DepClimbOut& fItem,DepClimbOut& sItem)
{
	if (fItem.m_fltType.fits(sItem.m_fltType))
		return false;
	else if(sItem.m_fltType.fits(fItem.m_fltType))
		return true;

	return false;
}
void DepClimbOuts::SortFlightType()
{
	std::sort(m_vDepClimbOuts.begin(),m_vDepClimbOuts.end(),FlightTypeCompare);
}

double DepClimbOuts::GetFuelBurnByFlightType( AirsideFlightDescStruct& _flttype )
{
	DepClimbOut defaultClimbOut ;
	BOOL HasDefault = FALSE ;
	for (int i = 0 ; i < (int)m_vDepClimbOuts.size() ;i++)
	{
		DepClimbOut ClimbOut ;
		ClimbOut = m_vDepClimbOuts[i] ;
		
		if(ClimbOut.m_fltType.isDefault())
		{
			defaultClimbOut = ClimbOut ;
			HasDefault = TRUE ;
			continue ;
		}
		if(ClimbOut.m_fltType.fits(_flttype))
			return ClimbOut.m_fuelBurn ;

	}
	if(HasDefault)
		return defaultClimbOut.m_fuelBurn ;
	else
		return 0 ;
}