#pragma once

#include <Common/IniFile.h>

class RENDERENGINE_API CShape3DMeshManager
{
protected:
	CShape3DMeshManager(void);
	~CShape3DMeshManager(void);

public:
	static CShape3DMeshManager& GetInstance();

	bool Reload(CString strAppDir);

	bool GetPluginShapeName(CString& strMeshFileName, int nGroup, int nIndex);
	bool GetAnimationShapeName(CString& strMeshFileName, int nGroup, int nIndex);
	bool GetFlightShapeName(CString& strMeshFileName, CString strAcType);
	bool GetAirlineImageFileName(CString& strImageFileName, CString strAirlineCode);
	bool GetVehicleShapeName(CString& strMeshFileName, int nIndex);
	bool GetVehicleShapeName(CString& strMeshFileName, CString strShapeName);

private:
	CIniFile m_pluginFile;
	CIniFile m_animationShapeFile;
	CIniFile m_flightFile;
	CIniFile m_vehicleFile;
};
