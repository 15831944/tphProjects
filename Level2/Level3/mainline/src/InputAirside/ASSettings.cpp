#include "StdAfx.h"
#include "assettings.h"
#include "../Database/ARCportDatabaseConnection.h"
using namespace ns_AirsideInput;
using namespace ADODB;
/************************************************************************/
/*   for weather time settings                                          */
/************************************************************************/
weatherTime::weatherTime():m_nID(-1),weatherType(0),from_tm(0),to_tm(0){}
vWeatherTimes::vWeatherTimes():m_nSettingsID(-1){}
CString vWeatherTimes::m_strTableName = "WeatherTime";


//remember to set m_nSettingID to Settings' id;
//read data form database to mem
void vWeatherTimes::ReadData()
{
	if(m_nSettingsID<0)return;
	clear();
	CString _selectStr;
	_selectStr.Format("Select * From %s Where SettingsID = %d",m_strTableName,m_nSettingsID);
	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_bstr_t(_selectStr), NULL, adOptionUnspecified);
		while (!pRS->GetadoEOF())
		{
			weatherTime wetherdata;
			wetherdata.m_nID  = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
			wetherdata.weatherType = (int)pRS->Fields->GetItem(_bstr_t("WetherType"))->GetValue();
			wetherdata.from_tm = (double) pRS->Fields->GetItem(_bstr_t("FromTime"))->GetValue();
			wetherdata.to_tm = (double) pRS->Fields->GetItem(_bstr_t("ToTime"))->GetValue();

			push_back( wetherdata );
			pRS->MoveNext();
		}
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
	
}
//save all data to database
void vWeatherTimes::SaveData(vWeatherTimes& newWeatherTm)
{
	
	for(iterator itr = newWeatherTm.begin();itr!=newWeatherTm.end();itr++)
	{
		if( (*itr).m_nID < 0 )
			InsertData(*itr);
		else
			UpdateData(*itr);
	}
	//delete unexist data
	for(iterator itr = begin();itr!=end();itr++)
	{
		int m_nid = (*itr).m_nID;
		const_iterator newitr;
		for(newitr = newWeatherTm.begin(); newitr != newWeatherTm.end();newitr++)
		{
			if( (*newitr).m_nID == m_nid )break;
		}
		// *itr not exsit in the new v;
		if(newitr == newWeatherTm.end())
		{
			DeleteData( *itr);
		}
	}	
	
	
}
//insert a new weatherdata data to database
void vWeatherTimes::InsertData(weatherTime& weathTm)
{
	if( !(weathTm.m_nID < 0) )return;
	CString _insertStr;
	_insertStr.Format("Insert into %s(SettingsID,FromTime,ToTime,WetherType) Values(%d,%f,%f,%d)",			  
				  m_strTableName,m_nSettingsID, weathTm.from_tm,weathTm.to_tm,weathTm.weatherType);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_insertStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
	
}
//update a exist item 
void vWeatherTimes::UpdateData(weatherTime& weathTm)
{
	if( weathTm.m_nID <0) return;
	CString _updateStr;
	_updateStr.Format("UPDATE %s SET SettingsID=%d,FromTime=%f,ToTime=%f,WetherType=%d WHERE ID = %d",  
		m_strTableName, m_nSettingsID, weathTm.from_tm, weathTm.to_tm,weathTm.weatherType, weathTm.m_nID);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)return;
	try{	
		_RecordsetPtr pRS = conPtr->Execute(_updateStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
//delete a exist item
void vWeatherTimes::DeleteData(weatherTime& weathTm)
{
	if(weathTm.m_nID<0)return;
	CString _deleteStr;
	_deleteStr.Format("DELETE FROM %s WHERE ID = %d",  
		m_strTableName, weathTm.m_nID);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_deleteStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}


/****************************************************
for dynamic runway assignment
*****************************************************/
dynamicRWasgn::dynamicRWasgn():m_nID(-1){
	assignType = 0;
	assigncase = 0;
	effectiveTmFrom = 0;
	effectiveTmTo = 0;
}
vRwAsigns::vRwAsigns():m_nSettingsID(-1){}
CString vRwAsigns::m_strTableName  = "runwayAssign";

//remember to set m_nSettingID to Settings' id;
void vRwAsigns::ReadData()
{
	if(m_nSettingsID<0)return;
	clear();
	CString _selectStr;
	_selectStr.Format("Select * From %s Where SettingsID=%d",m_strTableName,m_nSettingsID);
	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_selectStr.AllocSysString(), NULL, adOptionUnspecified);
		while (!pRS->GetadoEOF())
		{
			dynamicRWasgn _rwassgin;
			_rwassgin.m_nID = (int) pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
			_rwassgin.assignType = (int)pRS->Fields->GetItem(_bstr_t("assignType"))->GetValue();
			_rwassgin.assigncase = (int)pRS->Fields->GetItem(_bstr_t("assigncase"))->GetValue();
			_rwassgin.effectiveTmFrom = (double)pRS->Fields->GetItem(_bstr_t("EffectiveTmFrom"))->GetValue();
			_rwassgin.effectiveTmTo = (double)pRS->Fields->GetItem(_bstr_t("EffectiveTmTo"))->GetValue();
	//		_rwassgin.m_nSettingsID = (int) m_nSettingsID;
			push_back(_rwassgin);

			pRS->MoveNext();
		}
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}

}
//save all data to database
void vRwAsigns::SaveData(vRwAsigns& newRwAsgn)
{
	for(iterator itr = newRwAsgn.begin();itr!=newRwAsgn.end();itr++)
	{
		if( (*itr).m_nID < 0 )
			InsertData(*itr);
		else
			UpdateData(*itr);
	}
	//delete unexist data
	for(iterator itr = begin();itr!=end();itr++)
	{
		int m_nid = (*itr).m_nID;
		const_iterator newitr;
		for(newitr = newRwAsgn.begin(); newitr != newRwAsgn.end();newitr++)
		{
			if( (*newitr).m_nID == m_nid )break;
		}
		// *itr not exsit in the new v;
		if(newitr == newRwAsgn.end())
		{
			DeleteData( *itr);
		}
	}	
}
//insert a new runway assign data to database
void vRwAsigns::InsertData(dynamicRWasgn& _dynamicAsgn)
{
	if( !(_dynamicAsgn.m_nID < 0) )return;
	CString _insertStr;
	_insertStr.Format("Insert into  %s (SettingsID, assignType, assigncase, EffectiveTmFrom, EffectiveTmTo) Values(%d, %d, %d, %f, %f)",			  
		m_strTableName, m_nSettingsID, _dynamicAsgn.assignType, _dynamicAsgn.assigncase, _dynamicAsgn.effectiveTmFrom, _dynamicAsgn.effectiveTmTo);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_insertStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
//update a exist item 
void vRwAsigns::UpdateData(dynamicRWasgn& _dynamicAsgn)
{
	if( _dynamicAsgn.m_nID <0) return;
	CString _updateStr;
	_updateStr.Format("UPDATE %s SET SettingsID = %d, assignType = %d, assigncase= %d,EffectiveTmFrom = %f,EffectiveTmTo=%f WHERE ID = %d",  
		m_strTableName, m_nSettingsID, _dynamicAsgn.assignType, _dynamicAsgn.assigncase, _dynamicAsgn.effectiveTmFrom,_dynamicAsgn.effectiveTmTo, _dynamicAsgn.m_nID);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_updateStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
//delete a exist item
void vRwAsigns::DeleteData(dynamicRWasgn& _dynamicAsgn)
{
	if(_dynamicAsgn.m_nID<0)return;
	CString _deleteStr;
	_deleteStr.Format("DELETE FROM %s WHERE ID = %d",  
		m_strTableName, _dynamicAsgn.m_nID);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_deleteStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
/************************************************************************/
/*  for rw Mode assignment                                              */
/************************************************************************/
rwMode::rwMode():m_nID(-1),Mode(0),mark("")//Index(0)
{
}
vRwModes::vRwModes():m_nSettingsID(-1){}

CString vRwModes::m_strTableName = "RwMode";
void vRwModes::ReadData()
{
	if(m_nSettingsID<0)return;
	clear();
	CString _selectStr;
	_selectStr.Format("Select * From %s Where SettingsID = %d",	m_strTableName, m_nSettingsID);
	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_selectStr.AllocSysString(), NULL, adOptionUnspecified);
		while (!pRS->GetadoEOF())
		{
			rwMode _rwMode;
			_rwMode.m_nID = (int) pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
//			_rwMode.Index = (int)pRS->Fields->GetItem(_bstr_t("RwIndex"))->GetValue();
			_rwMode.mark = (CString)pRS->Fields->GetItem(_bstr_t("RwIndex"))->GetValue();
			_rwMode.Mode = (int)pRS->Fields->GetItem(_bstr_t("RwMode"))->GetValue();		
			//_rwMode.m_nSettingsID = (int) m_nSettingsID;
			push_back(_rwMode);

			pRS->MoveNext();
		}

		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}

}
//save all data to database
void vRwModes::SaveData(vRwModes& _newRwMds)
{
	for(iterator itr = _newRwMds.begin();itr!=_newRwMds.end();itr++)
	{
		if( (*itr).m_nID < 0 )
			InsertData(*itr);
		else
			UpdateData(*itr);
	}
	//delete unexist data
	for(iterator itr = begin();itr!=end();itr++)
	{
		int m_nid = (*itr).m_nID;
		const_iterator newitr;
		for(newitr = _newRwMds.begin(); newitr != _newRwMds.end();newitr++)
		{
			if( (*newitr).m_nID == m_nid )break;
		}
		// *itr not exsit in the new v;
		if(newitr == _newRwMds.end())
		{
			DeleteData( *itr);
		}
	}	
}
//insert a new runway assign data to database
void vRwModes::InsertData(rwMode& _rwmode)
{
	if( !(_rwmode.m_nID < 0) )return;
	CString _insertStr;
	_insertStr.Format("Insert into  %s (SettingsID, RwIndex, RwMode) Values(%d,'%s', %d)",			  
		m_strTableName, m_nSettingsID, _rwmode.mark, _rwmode.Mode);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_insertStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
//update a exist item 
void vRwModes::UpdateData(rwMode& _rwmode)
{
	if( _rwmode.m_nID <0) return;
	CString _updateStr;
	_updateStr.Format("UPDATE %s SET SettingsID = %d, RwIndex = '%s', RwMode= %d WHERE ID = %d",  
		m_strTableName, m_nSettingsID, _rwmode.mark,_rwmode.Mode, _rwmode.m_nID);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_updateStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
//delete a exist item
void vRwModes::DeleteData(rwMode& _rwmode)
{
	if(_rwmode.m_nID<0)return;
	CString _deleteStr;
	_deleteStr.Format("DELETE FROM %s WHERE ID = %d",  
		m_strTableName, _rwmode.m_nID);				

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_deleteStr.AllocSysString(), NULL, adOptionUnspecified);
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
/************************************************************************/
/*   for settings                                                  */
/************************************************************************/
CString CSettings::m_strTableName = "AirsideSettings";
CSettings::CSettings()
{
	m_nID = -1;
	m_nProjectID = -1;
	m_strt_t = 36495.0;
	m_stop_t = 36496.0;
	m_minr = 0.5;
	m_headwind = 6;	
	m_virtual_ = false;
	m_hist_int = 12;
	m_usr_sw = false;	
	
	
}


void CSettings::ReadData()
{	
	if(m_nProjectID<0)return;
	CString _selectStr = GetSelectScript();
	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	try{
		_RecordsetPtr pRS = conPtr->Execute(_selectStr.AllocSysString(), NULL, adOptionUnspecified);
		while (pRS->adoEOF == VARIANT_FALSE)
		{
			m_nID = (int) pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
			m_strt_t = (double) pRS->Fields->GetItem(_bstr_t("Start_tm"))->GetValue();
			m_stop_t = (double) pRS->Fields->GetItem(_bstr_t("Stop_tm"))->GetValue();
			m_minr = (double) pRS->Fields->GetItem(_bstr_t("Minr"))->GetValue();	
			m_headwind = (double) pRS->Fields->GetItem(_bstr_t("HeadWind"))->GetValue();
			m_virtual_ = (int)pRS->Fields->GetItem(_bstr_t("Virtual"))->GetValue() == 0?false:true; 
			m_hist_int = (int)pRS->Fields->GetItem(_bstr_t("Hist_int"))->GetValue();
			m_usr_sw = (int)pRS->Fields->GetItem(_bstr_t("Usr_sw"))->GetValue() == 0?false:true;

			pRS->MoveNext();
		}	
		if (pRS)
			if (pRS->State == adStateOpen)
				pRS->Close();
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}

	
	//
	m_vWeatherTm.m_nSettingsID = m_nID;	
	m_vWeatherTm.ReadData();
	m_vRwMd.m_nSettingsID = m_nID;
	m_vRwMd.ReadData();
	m_vRwasgn.m_nSettingsID = m_nID;
	m_vRwasgn.ReadData();
}
void CSettings::SaveData(CSettings& _newSettings){
	if(!IsDataValid(_newSettings))return;
	m_strt_t = _newSettings.m_strt_t;
	m_stop_t = _newSettings.m_stop_t;
	m_minr = _newSettings.m_minr;
	m_headwind = _newSettings.m_headwind;	
	m_virtual_ = _newSettings.m_virtual_;
	m_hist_int = _newSettings.m_hist_int;
	m_usr_sw = _newSettings.m_usr_sw;


	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;	
	try{
		if( m_nID < 0)
		{ // insert a new item
			CString _insertStr;
			_insertStr = GetInsertScript();

			_RecordsetPtr pRS = conPtr->Execute((_bstr_t)_insertStr, NULL, adOptionUnspecified);		
			if (pRS)
				if (pRS->State == adStateOpen)
					pRS->Close();
		}
		else  //update exit data
		{
			CString _updateStr = GetUpdateScript();
			_RecordsetPtr pRS = conPtr->Execute((_bstr_t)_updateStr, NULL, adOptionUnspecified);
			if (pRS)
				if (pRS->State == adStateOpen)
					pRS->Close();
		}
	}catch (_com_error &e) 
	{
		CString strError = e.ErrorMessage();
		return;
	}
	//ReadData();

	m_vRwasgn.SaveData(_newSettings.m_vRwasgn);
	m_vRwasgn.ReadData();
	m_vRwMd.SaveData(_newSettings.m_vRwMd);
	m_vRwMd.ReadData();
	m_vWeatherTm.SaveData(_newSettings.m_vWeatherTm);
	m_vWeatherTm.ReadData();
	//delete unexsit data
	//...

}
CString CSettings::GetSelectScript() const { 	
	CString strSQL = "";
    strSQL.Format("Select * From %s Where ProjID = %d",
		m_strTableName, m_nProjectID);
	return strSQL;
}
//get sql for update
CString CSettings::GetUpdateScript() const {
	int nVirtual = m_virtual_ ? 1 : 0;
	int nUsrSw = m_usr_sw ? 1 : 0;	
	CString strSQL ;
	strSQL.Format("Update %s Set Start_tm=%f, Stop_tm=%f,MinR=%f, HeadWind=%f, Virtual=%d, Hist_Int=%d, Usr_sw=%d Where ID = %d",
				m_strTableName, m_strt_t, m_stop_t,m_minr,m_headwind, nVirtual, m_hist_int,nUsrSw, m_nID);

	return strSQL;

}
//get sql for deletion
CString CSettings::GetDeleteScript() const { 
	 CString strSQL = "";

	strSQL.Format("Delete From %s Where ID = %d", 
	 	m_strTableName, m_nID);

	return strSQL;
 
 }
//get sql for insertion
CString CSettings::GetInsertScript() const {
	int nVirtual = m_virtual_ ? 1 : 0;
	int nUsrSw = m_usr_sw ? 1 : 0;

	CString strSQL = "";
	strSQL.Format("Insert into %s(ProjID,Start_tm,Stop_tm,MinR,HeadWind,Virtual,Hist_Int,Usr_sw) Values (%d, %f, %f, %f,%f, %d, %d, %d)",
		m_strTableName, m_nProjectID, m_strt_t, m_stop_t,m_minr,m_headwind, nVirtual, m_hist_int,nUsrSw);

	return strSQL;
}
//check the data is valid
bool CSettings::IsDataValid(CSettings &_newSettings)
{
	return true;
}


