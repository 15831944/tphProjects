#include "StdAfx.h"
#include ".\Shape3DMeshManager.h"

#include <string>

CShape3DMeshManager::CShape3DMeshManager(void)
{
}

CShape3DMeshManager::~CShape3DMeshManager(void)
{
}

CShape3DMeshManager& CShape3DMeshManager::GetInstance()
{
	static CShape3DMeshManager m;
	return m;
}

bool CShape3DMeshManager::Reload( CString strAppDir )
{
	bool bRet = m_pluginFile.ReadFile(strAppDir + "\\Databases\\media\\Shapes.Plugin\\Shapes.ini");
	bRet = bRet && m_animationShapeFile.ReadFile(strAppDir + "\\Databases\\media\\Pax\\Shapes.ini");
	bRet = bRet && m_flightFile.ReadFile(strAppDir + "\\Databases\\media\\Shapes.Flight\\Shapes.ini");
	bRet = bRet && m_vehicleFile.ReadFile(strAppDir + "\\Databases\\media\\Shapes.Vehicle\\Shapes.ini");
	return bRet;
}

bool CShape3DMeshManager::GetPluginShapeName( CString& strMeshFileName, int nGroup, int nIndex )
{
	std::string strKey;
	std::string strValue;
	if (m_pluginFile.GetKeyValue(nGroup, nIndex, strKey, strValue))
	{
		strMeshFileName = strValue.c_str();
// 		strMeshFileName += _T(".mesh");
		return true;
	}
	return false;
}

bool CShape3DMeshManager::GetAnimationShapeName( CString& strMeshFileName, int nGroup, int nIndex )
{
	std::string strKey;
	std::string strValue;
	if (m_animationShapeFile.GetKeyValue(nGroup, nIndex, strKey, strValue))
	{
		strMeshFileName = strValue.c_str();
// 		strMeshFileName += _T(".mesh");
		return true;
	}
	return false;
}

bool CShape3DMeshManager::GetFlightShapeName( CString& strMeshFileName, CString strAcType )
{
	std::string strValue;
	if (m_flightFile.GetValue("Flight", (LPCTSTR)strAcType, strValue))
	{
		strMeshFileName = strValue.c_str();
		return true;
	}
	return false;
}

bool CShape3DMeshManager::GetAirlineImageFileName( CString& strImageFileName, CString strAirlineCode )
{
	std::string strValue;
	if (m_flightFile.GetValue("AirlineImages", (LPCTSTR)strAirlineCode, strValue))
	{
		strImageFileName = strValue.c_str();
		return true;
	}
	return false;
}

bool CShape3DMeshManager::GetVehicleShapeName( CString& strMeshFileName, int nIndex )
{
	std::string strKey;
	std::string strValue;
	if (m_vehicleFile.GetKeyValue("Vehicle", nIndex, strKey, strValue))
	{
		strMeshFileName = strValue.c_str();
		return true;
	}
	return false;
}

bool CShape3DMeshManager::GetVehicleShapeName( CString& strMeshFileName, CString strShapeName )
{
	std::string strValue;
	if (m_vehicleFile.GetValue("Vehicle", (LPCTSTR)strShapeName, strValue))
	{
		strMeshFileName = strValue.c_str();
		return true;
	}
	return false;
}
