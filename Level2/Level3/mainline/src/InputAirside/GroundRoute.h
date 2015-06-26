#pragma once

#include "ALTObject.h"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;
class GroundRouteItem;
typedef std::vector<GroundRouteItem*> GroundRouteItemVector;
typedef std::vector<GroundRouteItem*>::iterator GroundRouteItemIter;

class INPUTAIRSIDE_API GroundRouteItem
{
public:
	GroundRouteItem();
	~GroundRouteItem();
public:
	void ReadData(CADORecordset& adoRecordset);
	void SaveData();
	void DeleteData();
	void VerifyData();

	void SetGroundRouteID(int nGroundRouteID)     {    m_nGroundID = nGroundRouteID;    }
	void SetParentID(int nParentID)               {    m_nParentID = nParentID;         }
	void SetALTObjectID(int nALTObjID)            {    m_nALTObjID = nALTObjID;         }
	int  GetALTObjectID()                         {    return m_nALTObjID;              }
	void SetID(int nID)                           {    m_nID = nID;                     }
	void SetRunwayMarking(int nMarking)           {    m_nMarking = nMarking;           }
	int GetRunwayMarking()                        {    return m_nMarking;               }

	void AddChildItem(GroundRouteItem* pGroundRouteItem);
    GroundRouteItemVector GetChildItems();
	GroundRouteItemVector GetDelChildItems();
	void DeleteChildItem(GroundRouteItem* pGroundRouteItem);
	void DeleteAllChild();
	GroundRouteItemVector GetTailItems();

	int GetChildCount()                          {   return (int)m_ChildItem.size();      }
	GroundRouteItem* GetChildItem(int nIndex)    {   return m_ChildItem.at(nIndex);       }


protected:
	void UpdateData();

private:
	int m_nID;
	int m_nParentID;
	int m_nGroundID;	
	int m_nALTObjID;
	int m_nMarking;

	GroundRouteItemVector m_ChildItem;
	GroundRouteItemVector m_DelChildItem;

public:
	void ExportGroundRouteItem(CAirsideExportFile& exportFile);
	void ImportGroundRouteItem(CAirsideImportFile& importFile);

};


class INPUTAIRSIDE_API GroundRoute : public ALTObject
{
public:
	GroundRoute(int nAirportID);
	virtual ~GroundRoute(void);

	virtual void ReadObject(int nObjID);
	virtual void UpdateObject(int nObjID);
	virtual int SaveObject(int AirportID);
	virtual void DeleteObject(int nObjID);

	virtual ALTOBJECT_TYPE GetType()const {return ALT_GROUNDROUTE;} 
	virtual CString GetTypeName()const { return _T("GROUNDROUTE");} 

	GroundRouteItem* GetFirstItem();
	GroundRouteItem* GetItemWithGivenIntersectNodeID(int nIntersectNodeID);
	void SetFirstItem(GroundRouteItem* pGroundRouteItem);
	GroundRouteItemVector GetTailItems();
	void DeleteContent();
	GroundRouteItemVector GetExitItems();

	void SetName(LPCTSTR lpszName);
	void VerifyData();

private:
	void ReadData();
	void SaveData();
	void DeleteData();
	GroundRouteItem* GetItemWithGivenIntersectNodeID(GroundRouteItem* pGRItem, int nIntersectNodeID);

private:
	GroundRouteItem* m_pFirstRouteItem;
	GroundRouteItemVector m_DelFirstRouteItemCol;

public:
	static void ExportGroundRoutes(int nAirportID,CAirsideExportFile& exportFile);
	void ExportGroundRoute(CAirsideExportFile& exportFile);
	void ImportObject(CAirsideImportFile& importFile);
};
