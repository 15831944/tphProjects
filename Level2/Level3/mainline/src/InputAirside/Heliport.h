#pragma once
#include "altobject.h"
#include "../Common/path2008.h"

class INPUTAIRSIDE_API Heliport :
	public ALTObject
{
	friend class CHeliport3D;
public:
	Heliport(void);
	~Heliport(void);
	static const GUID& getTypeGUID();
	virtual ALTOBJECT_TYPE GetType()const {return ALT_HELIPORT;}
	virtual CString GetTypeName()const {return "Heliport";}

	const CPath2008& GetPath()const;
	void SetPath(const CPath2008& path);

	void SetRadius(double radius);
	double GetRadius()const;

	CPath2008& getPath();

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	void UpdatePath();
	virtual void DeleteObject(int nObjID);

	virtual bool CopyData(const ALTObject* pObj);

	static void ExportHeliport(int nAirportID,CAirsideExportFile& exportFile);
	virtual void ExportObject(CAirsideExportFile& exportFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

protected:
	CPath2008 m_path;
	double m_Radius;

protected:
	int m_nPathID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;
};
