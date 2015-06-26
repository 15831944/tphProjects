#pragma  once 
#include "runway.h"
#include <utility>
#include <map>
#include <vector>
#include "../Common/FLT_CNST.H"
#include "../InputAirside/Runway.h"
#include "../InputAirside/DeicePad.h"
#include "../InputAirside/stand.h"
#include "AirsideImportExportManager.h"
#include "common\ALTObjectID.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CTakeoffQueueProcessor
{
public:
	CTakeoffQueueProcessor(void);
	CTakeoffQueueProcessor(int nProjID);
	~CTakeoffQueueProcessor(void);

	BOOL ReadData(int nProjID);
	BOOL SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	std::pair<int,int> GetMapAt(int iIndex);
	int GetMapCount(void);
	int GetRunwayMarkingIndex(int iIndex);

	void InsertMapKey(int nRunwayID,int nRunwayMarkingIndex);
	void SetMapValue(int iIndex,int iQueueLen);
	void DeleteMap(int iIndex);
protected:
	CString GetSelectScript(int nProjID) const;
	CString GetInsertScript(std::map<int,int>::const_iterator citrMapFirst,std::map<int,int>::const_iterator citrMapSecond) const;
	CString GetDeleteScript() const;
protected:
	int m_nProjID;
	std::map<int ,int > m_mapRunwayID2MarkingIndex;//first is runway id , and it map to this runway marking.
	std::map <int , int > m_takeoffQueue;//	map runway id to max queue length.
};

class INPUTAIRSIDE_API CDeicingQueueProcessor
{
public:
	CDeicingQueueProcessor(void);
	CDeicingQueueProcessor(int nProjID);
	~CDeicingQueueProcessor(void);

	BOOL ReadData(int nProjID);
	BOOL SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	std::pair<CString,int> GetMapAt(int iIndex);
	int GetMapCount(void);

	void InsertMapKey(CString strDeicePad);
	void SetMapValue(int iIndex,int iQueueLen);
	void DeleteMap(int iIndex);
protected:
	CString GetSelectScript(int nProjID) const;
	CString GetInsertScript(CString strFirst,int iSecond) const;
	CString GetDeleteScript() const;
protected:
	int m_nProjID;
	std::map <CString , int > m_deicingQueue;	// map deice pad  to max queue length,and the DeicePad has a family concept
};

class INPUTAIRSIDE_API CSlotTimeProcessor
{
public:
	CSlotTimeProcessor(void);
	CSlotTimeProcessor(int nProjID);
	~CSlotTimeProcessor(void);

	BOOL ReadData(int nProjID);
	BOOL SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	std::pair<CString,int> GetMapAt(int iIndex);
	int GetMapCount(void);

	void InsertMapKey(CString strFltType);
	void SetMapValue(int iIndex,int iMaxTime);
	void DeleteMap(int iIndex);
protected:
	CString GetSelectScript(int nProjID) const;
	CString GetInsertScript(CString strFirst,int iSecond) const;
	CString GetDeleteScript() const;
protected:
	int m_nProjID;
	std::map <CString , int > m_slotTime;	//this map flight type to max slot time ,but i can't ensure that string be unique.
};

class INPUTAIRSIDE_API CInboundFltNumLimiter
{
public:
	CInboundFltNumLimiter(void);
	CInboundFltNumLimiter(int nProjID);
	~CInboundFltNumLimiter(void);

	BOOL ReadData(int nProjID);
	BOOL SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

	std::pair<CString,int> GetMapAt(int iIndex);
	int GetMapCount(void);

	void InsertMapKey(CString strStandName);
	void SetMapValue(int iIndex,int iFltNumber);
	void DeleteMap(int iIndex);
protected:
	CString GetSelectScript(int nProjID) const;
	CString GetInsertScript(CString strFirst,int iSecond) const;
	CString GetDeleteScript() const;
protected:
	int m_nProjID;
	std::map <CString , int > m_inboundFltNumLimit;	//map stand family to max number of aircraft inbound
};


class INPUTAIRSIDE_API CPushBackClearanceProcessor
{
public:
	CPushBackClearanceProcessor(CAirportDatabase *  _pAirportDatabase);
	CPushBackClearanceProcessor(int nProjID,CAirportDatabase *  _pAirportDatabase);
	~CPushBackClearanceProcessor(void);

	BOOL ReadData(int nProjID);
	BOOL SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ExportPushBackClearance(CAirsideExportFile& exportFile,CAirportDatabase *  _pAirportDatabase);
	static void ImportPushBackClearance(CAirsideImportFile& importFile);

	//take off queue
	int GetTakeoffQueueCount(void);
	int GetTakeoffQueueRunwayID(int nIndex);//runway id
	int GetTakeoffQueueRunwayMarkingIndex(int nIndex);//runway marking index
	int GetTakeoffQueueLengthAtRunway(int nIndex);
	void AddTakeoffQueueRunway(int nRunwayID,int nRunwayMarkingIndex);
	void SetTakeoffQueueRunwayOfQueueLength(int iIndex,int iQueueLen);
	void DeleteTakeoffQueueRunway(int iIndex);

	//deicing pad queue
	int GetDeicingQueueCount(void);
	ALTObjectID GetDeicingQueueDeicePad(int nIndex);
	int GetDeicingQueueLengthAtDeicePad(int nIndex);
	CString GetDeicingQueueDeicePadName(int iIndex); 
	void AddDeicingQueueDeicePadName(CString strDeicePadName);
	void SetDeicingQueueDeicePadOfQueueLength(int iIndex,int iQueueLen);
	void DeleteDeicingQueueDeicePad(int iIndex);

	//slot time of flight type
	int GetSlotTimeCount(void);
	FlightConstraint GetSlotTimeFltType(int nIndex);
	int GetFltTypeMaxSlotTime(int nIndex);
	CString GetSlotTimeFltTypeName(int iIndex);//return value == FlightConstraint::printConstraint();
	void AddSlotTimeFltTypeName(CString strFltType);
	void SetMaxSlotTimeOfFltType(int iIndex,int iMaxTime);
	void DeleteSlotTimeFltType(int iIndex);

	//stand family for inbound flight type
	int GetCountOfStandForInboundFlt(void);
	ALTObjectID GetStandForInboundFlt(int nIndex);
	int GetInboundFltNumberAtStand(int nIndex);
	CString GetNameOfStandForFltInbound(int iIndex);
	void AddStandForFltInbound(CString strStandName);
	void SetFltInboundNumberAtStand(int iIndex,int iFltNumber);
	void DeleteStandForFltInbound(int iIndex); 

	double GetDistancNoPushBackWithin(void)const;
	void SetDistancNoPushBackWithin(double dDistancNoPushBackWithin);

	long GetTimeNoLeadWithin(void)const;
	void SetTimeNoLeadWithin(long lTimeNoLeadWithin);

	long GetMaxOutboundFltToAssignedRunway(void)const;
	void SetMaxOutboundFltToAssignedRunway(long lMaxOutboundFltToAssignedRunway);

	long GetMaxOutboundFltToAllRunway(void)const;
	void SetMaxOutboundFltToAllRunway(long lMaxOutboundFltToAllRunway);
protected:
	CString GetSelectScript(int nProjID) const;
	CString GetUpdateScript() const;
	CString GetInsertScript() const;
	CString GetDeleteScript() const;
protected:
	int m_nProjID;

	double m_dDistancNoPushBackWithin;
	long m_lTimeNoLeadWithin;

	CTakeoffQueueProcessor m_takeoffQueue;
	CDeicingQueueProcessor m_deicingQueue;
	CSlotTimeProcessor     m_slotTime;
	CInboundFltNumLimiter  m_inboundFltNumLimiter;

	CAirportDatabase *  m_pAirportDatabase ;

	long m_lMaxOutboundFltToAssignedRunway;
	long m_lMaxOutboundFltToAllRunway;
};
