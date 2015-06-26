#pragma once
#include "altobject.h"
#include "..\Common\path2008.h"

class INPUTAIRSIDE_API AirSector :
	public ALTObject
{

friend class CAirSector3D;
public:

	static const GUID& getTypeGUID();


	AirSector(void);
	AirSector(const AirSector& othersector);
	//AirSector& operator = (const AirSector& othersector);

	virtual ~AirSector(void);
	virtual ALTOBJECT_TYPE GetType()const { return ALT_SECTOR; } ;
	virtual CString GetTypeName()const  { return "Sector"; };

	virtual bool CopyData(const ALTObject* pObj);
private:
	CPath2008 m_path;
	double m_dLowAlt;
	double m_dHighAlt;
	double m_dBandInterval;

	//database operation
public:
	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

	const CPath2008& GetPath()const;
	void SetPath(const CPath2008& path){ m_path.init(path);}

	void SetLowAltitude(double dAlt);
	double GetLowAltitude();

	void SetHighAltitude(double dAlt);
	double GetHighAltitude();

	void SetBandInterval(double dInter){m_dBandInterval = dInter;}
	double GetBandInterval(){ return m_dBandInterval;}

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);

	void UpdatePath();
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

protected:
	int m_nPathID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

public:
	static void ExportSectors(CAirsideExportFile& exportFile);
	void ExportSector(CAirsideExportFile& exportFile);

	void ImportObject(CAirsideImportFile& importFile);


};
