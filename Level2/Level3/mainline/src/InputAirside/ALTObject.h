#pragma once
#include <string>
#include "..\Common\ALTObjectID.h"
#include "..\Common/path.h"
//#include "../Database/DBElementCollection_Impl.h"
//typedef ALTObjectID ALTObjectID;
#include "../Database/ADODatabase.h"
#include "ALT_BIN.h"
#include "../Common/path.h"
#include "../Common/Guid.h"
#include <common/ILayoutObject.h>
//class ALTObjectDisplayProp;
class CAirsideExportFile;
class CAirsideImportFile;
class ALTObjectDisplayProp;

#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API ALTObject : public ILayoutObject, public IFacilityObject
{
public:
	typedef ref_ptr<ALTObject> RefPtr;
public:
	ALTObject();	
	virtual ~ALTObject();
	
	virtual CString GetTypeName()const; 
	virtual bool IsAirportElement()const;
	virtual bool IsAirspaceElement()const;
	virtual bool IsTopographyElement()const;

	virtual ALTOBJECT_TYPE GetType()const;

protected:
	//ALTObjectDisplayProp * m_pDisplayProp;
	int m_nObjID;
	int m_nAptID;  // airport id for runway ... airspace id(prj id) for airroute ... structures...
				   // level id for stretch ... 	ALTObjectID m_str4Name;


	virtual  int  GetInsertScript() const;
	virtual  int  GetUpdateScript() const;
	virtual int  GetDeleteScript() const;
	virtual CString GetSelectScript(int nObjID) const;
	virtual CString GetSelectScriptForPath2008(int nObjID) const;

public:
	static void SetLockedByID(int nID, bool bLocked = true);
	static void SetLockedByType(ALTOBJECT_TYPE objType, bool bLocked = true);
	static bool GetLockedByeID(int nID, bool& bFailed);

	void setObjName(const ALTObjectID& objName);
	void getObjName(ALTObjectID& objName) const;

	ALTObjectID GetObjectName()const;

	CString GetObjNameString() const;
	//from level 0 to nLvel, 1,2,3
	CString GetObjNameString(int nLevel) const;
	void setID(int nObjID);
	int getID() const;

	void setAptID(int nAptID);
	int getAptID()const;

	void ReadObject(CADORecordset& adoRecordset);

	virtual void ReadObject(int nObjID);

	virtual int SaveObject(int AirportID);

	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	//new a display property
	virtual bool CopyData(const ALTObject* pObj);
	virtual ALTObjectDisplayProp * NewDisplayProp();
	virtual ALTObjectDisplayProp *getDisplayProp(bool reload = false);
	
	//altObj,input ,the object need to insert into database
	//objID, return, if successful, return the unique object id in the database
	//				 if field, throw a exception -- CADOException
	// the client proc must receive the exception
	int SaveObject(int AirportID,ALTOBJECT_TYPE objType);

	//altObj,input ,the object need to insert into database
	//objID, input, the unique object ID in the database
	virtual void UpdateObject(int nObjID);
	
	// nObjID , the object ID in the database, need to delete
	virtual void DeleteObject(int nObjID);

	static ALTObject* CreateObjectByType(const ALTOBJECT_TYPE& objType);
	static ALTObject* ReadObjectByID(int nObjID);
	static void GetObjectList(ALTOBJECT_TYPE objType,int nAirportID,std::vector<ALTObject>& vObject);

	static bool CheckObjNameIsValid(const ALTObjectID& objName,int nAirportID, int nObjID = -1);
	static bool DeleteObjectByID(int nObjID);

	static void GetObjectNameList(ALTOBJECT_TYPE objType,int nAirportID,ALTObjectIDList& lstObjectName);


	void GetNextObjectName( ALTObjectID& nameDest);
	static bool IsObjectExists(int nObjID);

	virtual void ExportObject(CAirsideExportFile& exportFile);
	static void ImportObjects(CAirsideImportFile& importFile);
	virtual void ImportObject(CAirsideImportFile& importFile);

	static CString GetTypeName(ALTOBJECT_TYPE objType);



protected:
	ALTObjectDisplayProp *m_pDispProp;
};


typedef std::vector< int> ALTObjectUIDList;
typedef std::vector< ALTObject::RefPtr > ALTObjectList;

