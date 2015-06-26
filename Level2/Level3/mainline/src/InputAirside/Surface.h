#pragma once
#include "altobject.h"
#include "..\Common\path2008.h"

class INPUTAIRSIDE_API Surface :
	public ALTObject
{
friend class CSurface3D;
public:
	Surface(void);
	virtual ~Surface(void);
	virtual ALTOBJECT_TYPE GetType()const { return ALT_SURFACE; } ;
	virtual CString GetTypeName()const  { return "Surface"; };
	typedef ref_ptr<Surface> RefPtr;

	static const GUID& getTypeGUID();
private:
	CPath2008 m_path;
	std::string m_strTexture;

	//database operation
public:

	const CPath2008& GetPath()const{ return m_path;}
	void SetPath(const CPath2008& path){ m_path.init(path);}

	CString  GetTexture(){ return m_strTexture.c_str();}
	void SetTexture(const CString& strTexture){ m_strTexture = (const char *)strTexture;}

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	void UpdatePath();
	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

protected:
	int m_nPathID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;


public:
	static void ExportSurfaces(CAirsideExportFile& exportFile);
	
	void ExportSurface(CAirsideExportFile& exportFile);

	virtual void ImportObject(CAirsideImportFile& importFile);
};
