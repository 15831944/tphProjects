#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include <vector>
#include "..\Database\DBElement.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API TrafficLight:public ALTObject
{
	friend class CTrafficLight3D;
public:
	TrafficLight(void);
	~TrafficLight(void);
	static const GUID& getTypeGUID();

protected:
	CPath2008 m_path;
	int m_nPathID;
	double m_dRotation;
    int m_nIntersect;
public:
	const CPath2008& GetPath(void) const;
	void SetPath(const CPath2008& _path);

	const int GetIntersecID() const;
	void SetIntersectID(const int intersecID);
	
	double GetRotation(void)const;
	void SetRotation(double dRotation);
public:
	virtual ALTOBJECT_TYPE GetType()const {return ALT_TRAFFICLIGHT;} 
	virtual CString GetTypeName()const { return _T("TRAFFICLIGHT");} 

	virtual void ReadObject(int nObjID);
	virtual int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
	virtual CString GetSelectScript(int nObjID) const;

	static void ExportTrafficLight(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual ALTObject * NewCopy();
	virtual bool CopyData(const ALTObject* pObj);
	virtual ALTObjectDisplayProp* NewDisplayProp();
	
};
