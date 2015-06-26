#include "stdafx.h"
#include "WeatherScripts.h"
#include "../Database/ADODatabase.h"
void CPrecipitationTypeEventItem::ReadData(std::vector<CPrecipitationTypeEventItem*>* _Data )
{
	if(_Data == NULL)
		return  ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM AS_Weather_scripts")) ;
	CADORecordset _dataSet ;
	long count = -1 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,_dataSet) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CPrecipitationTypeEventItem* _item = NULL ;
	int id = -1 ;
	CString str ;
	long val = 0 ;
	while (!_dataSet.IsEOF())
	{
		_item = new CPrecipitationTypeEventItem ;
		_dataSet.GetFieldValue(_T("ID"),id) ;
		_item->SetID(id) ;

		_dataSet.GetFieldValue(_T("Precipitation_Type"),str) ;
		_item->SetType(str) ;

		_dataSet.GetFieldValue(_T("StartTime"),val) ;
		_item->SetStartTime(ElapsedTime(val)) ;

		_dataSet.GetFieldValue(_T("EndTime"),val) ;
		_item->SetEndTime(ElapsedTime(val)) ;

		_dataSet.GetFieldValue(_T("EVENNAME"),str) ;
		_item->SetName(str) ;

		_Data->push_back(_item) ;
		_dataSet.MoveNextData() ;
	}
}
void CPrecipitationTypeEventItem::WriteData(std::vector<CPrecipitationTypeEventItem*>* _Data )
{
	std::vector<CPrecipitationTypeEventItem*>::iterator iter = _Data->begin() ;
	for ( ; iter != _Data->end() ; iter++)
	{
		WriteData(*iter) ;
	}
}
void CPrecipitationTypeEventItem::DelData(std::vector<CPrecipitationTypeEventItem*>* _Data )
{
	std::vector<CPrecipitationTypeEventItem*>::iterator iter = _Data->begin() ;
	for ( ; iter != _Data->end() ;iter++)
	{
		DelData(*iter) ;
	}
}
void CPrecipitationTypeEventItem::WriteData(CPrecipitationTypeEventItem* _data)
{
	if(_data == NULL)
		return ;
	CString SQL ;
	try
	{
		if(_data->GetID() == -1)
		{
			SQL.Format(_T("INSERT INTO AS_Weather_scripts (Precipitation_Type,StartTime,EndTime,EVENNAME) VALUES('%s',%d,%d,'%s')"),
				_data->GetType(),
				_data->GetStartTime().asSeconds() ,
				_data->GetEndTime().asSeconds() ,
				_data->GetName()) ;
			int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			_data->SetID(id) ;
		}else
		{
			SQL.Format(_T("UPDATE AS_Weather_scripts SET Precipitation_Type = '%s' , StartTime = %d ,EndTime = %d,EVENNAME = '%s' WHERE ID = %d ") ,
				_data->GetType(),
				_data->GetStartTime().asSeconds(),
				_data->GetEndTime().asSeconds(),
				_data->GetName(),
				_data->GetID()) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
void CPrecipitationTypeEventItem::DelData(CPrecipitationTypeEventItem* _data)
{
	if(_data == NULL || _data->GetID() == -1)
		return ;
	CString SQL ; 
	SQL.Format(_T("DELETE * FROM AS_Weather_scripts WHERE ID = %d") , _data->GetID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;	
	}
}
CWeatherScripts::~CWeatherScripts()
{
	ClearData() ;
	ClearDelData() ;
}
void CWeatherScripts::ClearData()
{
	TY_DATA_ITER ITER = m_data.begin() ;
	for ( ; ITER != m_data.end() ; ITER++)
	{
		delete *ITER ;
	}
	m_data.clear() ;
}
void CWeatherScripts::ClearDelData()
{
	TY_DATA_ITER ITER = m_DelData.begin() ;
	for ( ; ITER != m_DelData.end() ; ITER++)
	{
		delete *ITER ;
	}
	m_DelData.clear() ;
}
CPrecipitationTypeEventItem* CWeatherScripts::AddPrecipitationTypeEventItem(CString& _name) 
{
	CPrecipitationTypeEventItem* item = new CPrecipitationTypeEventItem ;
	item->SetName(_name) ;
	m_data.push_back(item) ;
	return item ;
}
void CWeatherScripts::DelPrecipitationTypeEventItem(CPrecipitationTypeEventItem* _Item) 
{
	TY_DATA_ITER ITER = m_data.begin() ;
	for ( ; ITER != m_data.end() ; ITER++)
	{
		if(*ITER == _Item)
		{
			m_DelData.push_back(*ITER) ;
			m_data.erase(ITER) ;
			break ;
		}
	}
}
void CWeatherScripts::ReadData()
{
	CPrecipitationTypeEventItem::ReadData(&m_data) ;
}
void CWeatherScripts::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CPrecipitationTypeEventItem::WriteData(&m_data) ;
		CPrecipitationTypeEventItem::DelData(&m_DelData) ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
		return ;
	}
	
	ClearDelData() ;
}
void CWeatherScripts::ReSetData()
{

	TY_DATA_ITER ITER = m_data.begin() ;
	for ( ; ITER != m_data.end() ; ITER++)
	{
		m_DelData.push_back(*ITER) ;
	}
	m_data.clear() ;
}