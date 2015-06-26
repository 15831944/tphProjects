#pragma once
#include "InputAirsideAPI.h"


// CPreferRouteList
enum RouteType
{
	ShortType,
	PreferType
};
class CADORecordset;
class CDeicePadItem;
class CPreferRouteItem;
class CRoutePath;
class INPUTAIRSIDE_API CPreferRouteList
{

public:
	CPreferRouteList();
	~CPreferRouteList();

public:
	void setProjID(int nProjID) { m_nProjID = nProjID;}
	int getProjID(){ return m_nProjID;}

	void setPreferRoute(RouteType emRouteType) { m_emRouteType = emRouteType;}
	RouteType getPreferRoute() { return m_emRouteType;}

	void AddItem(CPreferRouteItem* pPreferItem);
	bool DeleteItem(CPreferRouteItem* pPreferItem);
	bool DeleteItemData(int nStandID);
	bool DeleteItem(int nIndex);
	CPreferRouteItem* GetItem(int nIndex);
	CPreferRouteItem* GetItemData(int nStandID);
	int getPreferRouteCount();

	void ReadData(int nProjID);
	void SaveData(int nProjID,RouteType emRouteType);
protected:
	std::vector<CPreferRouteItem*> m_vPerferRouteList;
	std::vector<CPreferRouteItem*>m_vDelPreferRouteList;
	RouteType m_emRouteType;//PerferType prefer route,ShortType shortest path to deice pad 
	int m_nProjID;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CPreferRouteItem
{
public:
	CPreferRouteItem();
    ~CPreferRouteItem();

public:
	void setStandID(int nStandID) { m_nStandID = nStandID;}
	int getStandID() { return m_nStandID;}

	void AddItem(CDeicePadItem* pDeicePadItem);
	bool DeleteItem(CDeicePadItem* pDeicePadItem);
	bool DeleteItemData(int nDeicePadID);
	bool DeleteItem(int nIndex);
	CDeicePadItem* GetItem(int nIndex);
	CDeicePadItem* GetItemData(int nDeicePadID);
	int getDeicePadCount();

	void ReadData(CADORecordset& adoRecordset);
	void ReadData();
	void SaveData(int nProjID);
	void DeleteData();
	void UpdateData();
private:
	std::vector<CDeicePadItem*> m_vDeicePadList;
	std::vector<CDeicePadItem*>m_vDelDeicePadList;
	int m_nStandID;
	int m_nID;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CDeicePadItem
{
public:
	CDeicePadItem();
	~CDeicePadItem();

public:
	void setDeicePadID(int nDeicePadID) { m_nDeicePadID = nDeicePadID;}
	int getDeicePadID() { return m_nDeicePadID;}

	void AddItem(CRoutePath* pRoute);
	bool DeleteItem(CRoutePath* pRoute);
	bool DeleteItemData(int nFitID);
	bool DeleteItem(int nIndex);
	CRoutePath* GetItem(int nIndex);
	CRoutePath* GetItemData(int nFitPadID);
	int GetCount();

	void ReadData(CADORecordset& adoRecordset);
	void ReadData();
	void SaveData(int nID);
	void DeleteData();
	void UpdateData();
private:
	std::vector<CRoutePath*> m_vRoutePath;
	std::vector<CRoutePath*> m_vDelRoutePath;
	int m_nDeicePadID;
	int m_nID;
	int m_nFitStandID;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CRoutePath
{
public:
	CRoutePath();
	~CRoutePath();
public:
	void Additem(int RouteID);
	bool DeleteItem(int nIndex);
	void UpdataItem(int nIndex,int RouteID);

	int getRouteID(int nIndex) { return m_vRouteIDList[nIndex];}

	void setFitPadID(int nFitID) { m_nFitPadID = nFitID;}
	int getFitPadID() { return m_nFitPadID;}

	int getRouteIDListCount();
	void setVRouteID(CString& strRouteList);
	void setRouteListString(std::vector<int>vRouteIDList);
	CString& getRouteListString();

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nID);
	void DeleteData();
	void UpdateData();
private:
	CString m_strRouteList;
	std::vector<int> m_vRouteIDList;//save all routeID
	int m_nID;
	int m_nFitPadID;
};