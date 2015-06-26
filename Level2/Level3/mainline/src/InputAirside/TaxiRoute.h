#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "RouteItem.h"
#include "InputAirsideAPI.h"

class ALTObjectID;

enum ALTOBJECT_TYPE;

enum BOUNDROUTETYPE
{
	INBOUND = 0,
	OUTBOUND,
	DEICING,
	POSTDEICING,
	BOUND
};

struct RecaptureAndConditionData 
{
	int m_nID;
	int m_nRouteID;
	int m_nAcOnRouteCount; // Number of AC on route > N, not include AC on route use the same route
	int m_nAcOnRouteStopTime; // AC on route stopped > T Secs, not include AC on route use the same route
	int m_nPrecdingAcSlowerPercent; // Preceding on route slower than P%, not include AC on route use the same route
	int m_nPriorityIndex;
	bool m_bRecapture;
	CString m_sName;

	RecaptureAndConditionData()
		:m_nID(-1)
		,m_nAcOnRouteCount(0)
		,m_nAcOnRouteStopTime(0)
		,m_nPrecdingAcSlowerPercent(0)
		,m_nRouteID(-1)
		,m_nPriorityIndex(-1)
		,m_bRecapture(false)
		,m_sName(_T(""))
	{

	}

	void SaveDataToDB();
	void DeleteDataFromDB();
	void InitFromDBRecordset(CADORecordset& recordset);
};

class INPUTAIRSIDE_API CRoutePriority :public DBElementCollection<CRouteItem>
 {
 public:
 	CRoutePriority();
 	~CRoutePriority();
 
 	void SaveDataToDB();
 	void DeleteDataFromDB();
 	void InitFromDBRecordset(CADORecordset& recordset);
 
 	void DeleteData();
	void DeleteItem(const CRouteItem* dbElement);


	bool IfHasChangeCondition()const;//check the priority whether be last condition
	bool IfHasRepcaputreCondition()const;//check the priority in priority list whether be head

	int m_nRoutID;
	bool m_bHasChangeCondition;
 	RecaptureAndConditionData m_RepConData;
 };

enum BoundRouteChoice // enumeration definition, to be same as BOUNDROUTEASSIGNMENTSTATUS
{					// but with some element no used, e.g. RouteChoice_TimeShortestPath
	RouteChoice_ShortestPath = 0,
	RouteChoice_TimeShortestPath, // not used, just to be compatible with BOUNDROUTEASSIGNMENT_TIMESHORTPATH
	RouteChoice_Preferred,
};

class INPUTAIRSIDE_API CIn_OutBoundRoute : public DBElementCollection<CRoutePriority>
{
public:
	CIn_OutBoundRoute(void);
	virtual ~CIn_OutBoundRoute(void);

	typedef std::vector<int> ObjIDList;

public:
	virtual BOUNDROUTETYPE GetRouteType()const;
	void SetRouteAssignmentID(int nOutboundRouteAssignmentID);
	int GetRouteAssignmentID();

	void SetAllRunway(BOOL bAllRunway);
	BOOL IsAllRunway()const;

	void SetAllStand(BOOL bAllStand);
	BOOL IsAllStand()const;

	void SetAllDeice(BOOL bAllDeice);
	BOOL IsAllDeice()const;

	void SetAllStartPosition(BOOL bAllStartPosition);
	BOOL IsAllStartPosition()const;

	void SetAllTaxiInterruptLine(BOOL bAllTaxiInterruptLine);
	BOOL IsAllTaxiInterruptLine()const;

	void SetStandGroupIDList(const ObjIDList& vStandGroupIDList);
	void GetStandGroupIDList(ObjIDList& vStandGroupIDList)const;

	void SetRunwayExitIDList(const ObjIDList& vRunwayExitIDList);
	void GetRunwayExitIDList(ObjIDList& vRunwayExitIDList)const;

	void SetDeiceGroupIDList(const ObjIDList& vDeiceGroupIDList);
	void GetDeiceGroupIDList(ObjIDList& vDeiceGroupIDList)const;

	void SetStartPositionGroupIDList(const ObjIDList& vStartPositionGroupList);
	void GetStartPositionGroupIDList(ObjIDList& vStartPositionGroupList)const;

	void SetTaxiInterruptLineIDList(const ObjIDList& vTaxiInterruptLineList);
	void GetTaxiInterruptLineIDList(ObjIDList& vTaxiInterruptLineList)const;

	void DeleteItem(const CRoutePriority* dbElement);
	//DBElementCollection
	virtual void SaveData(int nID);
	virtual void SaveOriginAndDestination();
	virtual void DeleteData();
	virtual void DeleteOriginAndDestination();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void ReadOriginAndDestination();

	virtual void DeleteDataFromDB();

	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	bool IsFitRoute(const ALTObjectID& standID, int nRwyExitID) const;
	bool operator == (const CIn_OutBoundRoute& route);

	CString GetDestinationString() const; // for GUI display

public:
	BoundRouteChoice GetBoundRouteChoice() const;
	void SetBoundRouteChoice(BoundRouteChoice eRouteChoice);

	void SaveStandGroupList();
	void SaveRunwayExitList();
	void SaveDeiceGroupList()const; //to database
	void SaveStartPositionGroupList();
	void SaveTaxiInterruptLineList();

	//read list data, make sure m_nID is valid
	void ReadDeiceGroupList();
	void ReadRunwayExitList();
	void ReadStandGroupList();
	void ReadStartPositionList();
	void ReadTaxiInterrupLineList();

	void DeleteStandGroupList();
	void DeleteRunwayExitList();
	void DeleteDeiceGroupList()const; //delete data in database
	void DeleteStartPositionGroupList();
	void DeleteTaxiInterruptLineList();

	void ExchangePriorityItem(CRoutePriority* pPriorityPre,CRoutePriority* pPriorityNext);
	CRoutePriority* GetPriorityItem(int nPriorityIndex);
	void ResetPriorityIndex(CRoutePriority* pPriority);
	void SortAssignPriorityIndex();
	void AssignPriorityChangeCondition();

	int GetMinPriorityIndex();//get min priority index
	int GetMaxPriorityIndex();//get max Priority index;
protected:
	int m_nRouteAssignmentID;

	BoundRouteChoice m_eRouteChoice;

	ObjIDList m_vRunwayExitIDList;
	ObjIDList m_vStandGroupList;
	ObjIDList m_vDeiceGroupList;
	ObjIDList m_vStartPositionGroupList;
	ObjIDList m_vTaxiInterruptLineList;
	BOOL m_bAllRunway;
	BOOL m_bAllStand;
	BOOL m_bAllDeice;
	BOOL m_bAllStartPosition;
	BOOL m_bAllTaxiInterruptLine;

	static bool IsALTGroupInList(const ALTObjectID& altID, const ObjIDList& vIDList);


private:
	class PrioritySorter
	{
	public:
		bool operator()(CRoutePriority* lhs, CRoutePriority* rhs)
		{
			return lhs->m_RepConData.m_nPriorityIndex < rhs->m_RepConData.m_nPriorityIndex;
		}
	};

	class PriorityIndexAssigner
	{
	public:
		PriorityIndexAssigner(int nIndex = 0)
			: m_nIndex(nIndex)
		{
		}
		void operator()(CRoutePriority* pData)
		{
			pData->m_RepConData.m_nPriorityIndex = m_nIndex++;
		}
	private:
		int m_nIndex;
	};
};

////////////////////////////////////////CIn_BoundRoute///////////////////////////////////
class INPUTAIRSIDE_API CIn_BoundRoute : public CIn_OutBoundRoute
{
public:
	CIn_BoundRoute(void);
	virtual ~CIn_BoundRoute(void);

public:
	BOUNDROUTETYPE GetRouteType()const{ return INBOUND; }

	//DBElement
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;


};

////////////////////////////////////////COut_BoundRoute///////////////////////////////////
class INPUTAIRSIDE_API COut_BoundRoute : public CIn_OutBoundRoute
{
public:
	COut_BoundRoute(void);
	virtual ~COut_BoundRoute(void);

public:
	BOUNDROUTETYPE GetRouteType()const{ return OUTBOUND; }

	bool IsFitRouteStand(const ALTObjectID& standID, int nRwyExitID) const;
	bool IsFitRouteDeicepad(const ALTObjectID& deicepadID, int nRwyExitID) const;
	bool IsFitRouteStartPosition(const ALTObjectID& startPosID, int nRwyExitID) const;
	bool IsFitRouteTaxiInterruptLine(int nTaxiInterruptLineID, int nRwyExitID) const;

	//DBElement
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;

    CString GetOriginString() const; // for GUI display

protected:
	static BOOL RecALTObject( CString& strName, BOOL bHasPrev, BOOL bAll, const ObjIDList& recALTObj, ALTOBJECT_TYPE objType, LPCTSTR orgTypeTag );
};
///////////////////////////////////Deice_Route///////////////////////////////////////
class INPUTAIRSIDE_API CDeice_BoundRoute : public CIn_BoundRoute
{
public:
	bool IsFitRoute(const ALTObjectID& standID,const ALTObjectID& deiceID) const;
public:
	BOUNDROUTETYPE GetRouteType()const{ return DEICING; }
	//DBElement
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
};
