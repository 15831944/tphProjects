#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "../Database/DBElement.h"
#include "InputAirsideAPI.h"
#include "Landside/LandsideParkingLot.h"

class INPUTAIRSIDE_API VehiclePoolParking : public ALTObject
{
	friend class CVehiclePoolParking3D;
public:
	typedef ref_ptr<VehiclePoolParking> RefPtr;
	static const GUID& getTypeGUID();
	VehiclePoolParking(void);
	~VehiclePoolParking(void);
protected:
	CPath2008 m_path;
	int m_nPathID;
public:
	ParkingLotParkingSpaceList m_parkspaces;
	ParkingDrivePipeList m_dirvepipes;
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_VEHICLEPOOLPARKING;} 
	virtual CString GetTypeName()const { return _T("VEHICLEPOOLPARKING");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportVehiclePoolParking(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();
	virtual bool CopyData(const ALTObject* pObj);
};