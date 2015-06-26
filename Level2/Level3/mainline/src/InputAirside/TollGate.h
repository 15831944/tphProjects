#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "..\Database\ADODatabase.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API TollGate:public ALTObject
{
	friend class CTollGate3D;
public:
	static const GUID& getTypeGUID();
	TollGate(void);
	~TollGate(void);
protected:
	CPath2008 m_path;
	int m_nPathID;
	double m_dRotation;
public:
	const CPath2008& GetPath(void)const;
	void SetPath(const CPath2008& _path);

	double GetRotation(void)const;
	void SetRotation(double dRotation);
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_TOLLGATE;} 
	virtual CString GetTypeName()const { return _T("TOLLGATE");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportTollGate(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();
};
