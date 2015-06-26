#pragma  once 
class CADORecordset;
class DeicePadQueueItem;
class DeicePadQueueRoute;
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API DeicePadQueueList
{
public:
	DeicePadQueueList();
	~DeicePadQueueList();
public:
	void AddItem(DeicePadQueueItem* pPadQueueItem);
	bool DeleteItem(DeicePadQueueItem* pPadQueueItem);
	bool DeleteItemData(int nDeicePadID);
	bool DeleteItem(int nIndex);
	DeicePadQueueItem* GetItem(int nIndex);
	int GetCount()const {return (int)m_vDeiceQueueItem.size();}

	void ReadData(int nProjID);
	void SaveData(int nProjID);
private:
	int m_nProjID;
	std::vector<DeicePadQueueItem*> m_vDeiceQueueItem;
	std::vector<DeicePadQueueItem*> m_vDelDeiceQueueItem;
};

class INPUTAIRSIDE_API DeicePadQueueItem
{
public:
	DeicePadQueueItem();
	~DeicePadQueueItem();

public:
	void AddItem(DeicePadQueueRoute* pRoute);
	bool DeleteItem(DeicePadQueueRoute* pRoute);
	bool DeleteItemData(int nFitID);
	bool DeleteItem(int nIndex);
	DeicePadQueueRoute* GetItem(int nIndex);
	int GetCount()const {return (int)m_vDeiceQueueRoute.size();}

	void setDeicePadID(int nDeicePadID) { m_nDeicePadID = nDeicePadID;}
	int getDeicePadID() { return m_nDeicePadID;}

	void ReadData(CADORecordset& adoRecordset);
	void ReadData();
	void SaveData(int nProjID);
	void DeleteData();
	void UpdateData();

private:
	int m_nDeicePadID;
	int m_nID;
	std::vector<DeicePadQueueRoute*> m_vDeiceQueueRoute;
	std::vector<DeicePadQueueRoute*> m_vDelDeiceQueueroute;
};

class INPUTAIRSIDE_API DeicePadQueueRoute
{
public:
	DeicePadQueueRoute();
	~DeicePadQueueRoute();

public:
	void Additem(int nDeiceRouteID);
	bool DeleteItem(int nIndex);
	void UpdataItem(int nIndex,int RouteID);
	int getRouteID(int nIndex);
	int GetCount(){ return (int)m_vDeiceRouteIDList.size();}

	int getDeiceRouteIDListCount();
	void setVDeiceRouteID(CString& strRouteList);
	void setDeiceRouteListString(std::vector<int>vRouteIDList);
	CString& getDeiceRouteListString();

	void setFitPadID(int nFitPadID) { m_nFitPadID = nFitPadID;}
	int getFitPadID() { return m_nFitPadID;}

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nID);
	void DeleteData();
	void UpdateData();

private:
	int m_nID;
	int m_nFitPadID;
	CString m_strDeiceRouteList;
	std::vector<int> m_vDeiceRouteIDList;
};