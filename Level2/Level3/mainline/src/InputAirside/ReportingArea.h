#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "../Database/DBElement.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CReportingArea:public ALTObject
{
	friend class CReportingArea3D;
public:
	typedef ref_ptr<CReportingArea> RefPtr;

	CReportingArea(void);
	~CReportingArea(void);

protected:
	CPath2008 m_path;
	int m_nPathID;
public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_REPORTINGAREA;} 
	virtual CString GetTypeName()const { return _T("REPORTINGAREA");} 

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
};
