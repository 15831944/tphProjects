#include "StdAfx.h"
#include ".\airsidevehicleoperparameter.h"
#include <algorithm>
CAirsideVehicleOperParameter::CAirsideVehicleOperParameter(void):CParameters(),m_SubType(0)
{
}
CAirsideVehicleOperParameter::~CAirsideVehicleOperParameter(void)
{
}
void CAirsideVehicleOperParameter::AddVehicleType(CString& _vehicleType)
{
	std::vector<CString>::iterator iter = find(m_VehicleType.begin(),m_VehicleType.end(),_vehicleType) ;
	if(iter == m_VehicleType.end())
		m_VehicleType.push_back(_vehicleType) ;
}
void CAirsideVehicleOperParameter::DeleteVehickeType(CString& _vehicleType) 
{
	std::vector<CString>::iterator iter = find(m_VehicleType.begin(),m_VehicleType.end(),_vehicleType) ;
	if(iter != m_VehicleType.end())
		m_VehicleType.erase(iter) ;
}
void CAirsideVehicleOperParameter::AddPoolId(int _id)
{
	std::vector<int>::iterator iter = find(m_PoolID.begin(),m_PoolID.end(),_id) ;
	if(iter == m_PoolID.end())
		m_PoolID.push_back(_id) ;
}
void CAirsideVehicleOperParameter::DeletePoolID(int _id)
{
	std::vector<int>::iterator iter = find(m_PoolID.begin(),m_PoolID.end(),_id) ;
	if(iter != m_PoolID.end())
		m_PoolID.erase(iter) ;
}
BOOL CAirsideVehicleOperParameter::CheckVehicleType(CString& _vehicleType)
{
	if(m_VehicleType.empty())
		return TRUE ;
	for (int i = 0 ; i < (int) m_VehicleType.size() ;i++)
	{
		if(_vehicleType.CompareNoCase(m_VehicleType[i]) == 0)
			return TRUE ;
	}
		return FALSE ;
}
BOOL CAirsideVehicleOperParameter::CheckPoolId(int _id)
{
	if(m_PoolID.empty())
		return TRUE ;
	std::vector<int>::iterator iter = find(m_PoolID.begin(),m_PoolID.end(),_id) ;
	if(iter == m_PoolID.end())
		return FALSE ;
	else
		return TRUE ;
}
BOOL CAirsideVehicleOperParameter::CheckServerFlightID(AirsideFlightDescStruct& _constrint)
{
	if(m_vFlightConstraint.empty())
		return TRUE ;
	for (int i = 0 ;i < (int)m_vFlightConstraint.size();i++)
	{
		if(m_vFlightConstraint[i].fits(_constrint))
			return TRUE ;
	}
	return FALSE ;
}
BOOL CAirsideVehicleOperParameter::ExportFile(ArctermFile& _file)
{
	BOOL res = CParameters::ExportFile(_file) ;
	if(!res)
		return res ;
	int size = (int)m_VehicleExist.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	std::map<int,int>::iterator iter = m_VehicleExist.begin() ;
	for ( ; iter != m_VehicleExist.end() ;iter++)
	{
		_file.writeInt((*iter).first) ;
	}
	_file.writeLine() ;

	size = (int)m_VehicleType.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i= 0 ; i < size ;i++)
	{
		_file.writeField(m_VehicleType[i]) ;
	}
	_file.writeLine() ;

	size = (int)m_PoolID.size() ;
	_file.writeInt(size) ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.writeInt(m_PoolID[i]) ;
	}
	_file.writeLine() ;
	return TRUE ;
}
BOOL CAirsideVehicleOperParameter::ImportFile(ArctermFile& _file)
{

	if(!CParameters::ImportFile(_file))
		return FALSE ;
	m_VehicleExist.clear() ;
	m_VehicleType.clear();
	m_PoolID.clear() ;
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	int val = 0 ;
	_file.getLine() ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getInteger(val) ;
		AddVehicleTypeForShow(val) ;
	}
	_file.getLine() ;

	if(!_file.getInteger(size))
		return FALSE;
	
	TCHAR type[1024] = {0} ;
	_file.getLine() ;
	for (int i = 0 ; i <size ;i++)
	{
		_file.getField(type,1024) ;
		m_VehicleType.push_back(type) ;
	}

	_file.getLine() ;

	if(!_file.getInteger(size))
		return FALSE ;
	int poolID = -1;
	for (int i = 0 ; i <size ;i++)
	{
		_file.getInteger(poolID) ;
		m_PoolID.push_back(poolID) ;
	}
	return TRUE ;
}

CString CAirsideVehicleOperParameter::GetReportParamName()
{
	return _T("VehicleOperations\\VehicleOperations.prm");
}

void CAirsideVehicleOperParameter::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);
	m_VehicleExist.clear() ;
	m_VehicleType.clear();
	m_PoolID.clear() ;
	_file.getLine() ;
	int size = 0 ;
	if(!_file.getInteger(size))
		return;
	int val = 0 ;
	_file.getLine() ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getInteger(val) ;
		AddVehicleTypeForShow(val) ;
	}
	_file.getLine() ;

	if(!_file.getInteger(size))
		return;
	_file.getLine() ;
	TCHAR type[1024] = {0} ;

	for (int i = 0 ; i <size ;i++)
	{
		_file.getField(type,1024) ;
		m_VehicleType.push_back(type) ;
	}

	_file.getLine() ;

	if(!_file.getInteger(size))
		return  ;
	int poolID = -1;
	for (int i = 0 ; i <size ;i++)
	{
		_file.getInteger(poolID) ;
		m_PoolID.push_back(poolID) ;
	}
}

void CAirsideVehicleOperParameter::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file) ;
	
	int size = (int)m_VehicleExist.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	std::map<int,int>::iterator iter = m_VehicleExist.begin() ;
	for ( ; iter != m_VehicleExist.end() ;iter++)
	{
		_file.writeInt((*iter).first) ;
	}
	_file.writeLine() ;

	size = (int)m_VehicleType.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i= 0 ; i < size ;i++)
	{
		_file.writeField(m_VehicleType[i]) ;
	}
	_file.writeLine() ;

	size = (int)m_PoolID.size() ;
	_file.writeInt(size) ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.writeInt(m_PoolID[i]) ;
	}
	_file.writeLine() ;
}