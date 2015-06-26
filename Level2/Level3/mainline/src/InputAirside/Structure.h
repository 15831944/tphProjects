#pragma once
#include "altobject.h"
#include "..\Common\Path2008.h"
#include "InputAirsideAPI.h"

struct INPUTAIRSIDE_API StructFace 
{
	StructFace();
	BOOL bShow;
	CString strFaceName;
	CString strPicPath;
};

struct UserInputData 
{
	UserInputData():dElevation(0.0)
		,dObstruction(0.0)
		,dStructHeight(1000)
	{

	}
	double dElevation; 
	double dStructHeight; 
	double dObstruction;
};
class INPUTAIRSIDE_API Structure :
	public ALTObject
{
public:
	Structure(void);
	Structure(const Structure& otherStrurct);
	Structure& operator = (const Structure& otherStruct);

	virtual ~Structure(void);

	static const GUID& getTypeGUID();
	virtual ALTOBJECT_TYPE GetType()const { return ALT_STRUCTURE; } ;
	virtual CString GetTypeName()const  { return "Structure"; };
	void clear();

	void UpdatePath();
private:
	CPath2008 m_path;
	int m_nPathID;
	std::vector<StructFace*>m_vStructFace;
	std::vector<UserInputData*>m_vUserData;

	void DeepCopy(const Structure& otherStruct);

	//database operation
public:
	const CPath2008& GetPath()const {return m_path;}
	void SetPath(const CPath2008& path) { m_path = path;}

	CPath2008& GetPath(){ return m_path; }	 
	
	const int GetPathID() { return m_nPathID;}
	void SetPathID(int nPathID) { m_nPathID = nPathID;}

	void AddUserItem(UserInputData* pUserData){m_vUserData.push_back(pUserData);}
	void AddStructItem(StructFace* pStructData){m_vStructFace.push_back(pStructData);}

	StructFace* getStructFace(int nIndex){ return m_vStructFace.at(nIndex);}
	UserInputData* getUserData(int nIndex) { return m_vUserData.at(nIndex);}

	int getUserDataCount()const{ return (int)m_vUserData.size(); }
	int getStructFaceCount()const{ return (int)m_vStructFace.size(); }

	CString FormatUserData(std::vector<UserInputData*>&vUserData);
	CString FormatStructFace(std::vector<StructFace*>&vStructFace);

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);
	

	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();	
	virtual ALTObjectDisplayProp * NewDisplayProp(); 
	virtual bool CopyData(const ALTObject* pobj);

protected:
	void ConveyToUserData(CString& strUserData);
	void ConveyToStructFace(CString& strStructFace);

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID);
	virtual CString GetUpdateScript(int nObjID);
	virtual CString GetDeleteScript(int nObjID) const;

};
