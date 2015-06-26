#pragma once
#include "../Common/elaptime.h"
#include "..\Common\FLT_CNST.H"

#include "InputAirsideAPI.h"

class CADORecordset;
class CAirsideExportFile;
class CAirsideImportFile;
class CRunwayTakeOffPosition;

enum Priority { FIFO =0, LongestQFirst, RoundRobin, DepartureSlot, PriorityFlightType, PriorityCount };

class INPUTAIRSIDE_API CPriorityData
{
public:
	CPriorityData(); ;
	~CPriorityData(); ;
public:
	Priority m_Type ;
	BOOL m_CrossPoint ;
	BOOL m_CanTakeOff ;
	BOOL m_CanEncroach ;
	FlightConstraint m_FltTy ;
	int m_ID ;

};
class INPUTAIRSIDE_API TakeOffSequenceInfo
{
public:
	TakeOffSequenceInfo(int nParentID,CAirportDatabase* _data);
	TakeOffSequenceInfo(CADORecordset &adoRecordset,CAirportDatabase* _data);
	~TakeOffSequenceInfo(void);
public:
	void SaveData(int nParentID);
	void DeleteData(void); 
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
protected:
	int m_nID;
	int m_nParentID;
	CAirportDatabase* m_AirportDB ;
public:
	ElapsedTime m_startTime;//start time 
	ElapsedTime m_endTime;//end time 
	CPriorityData m_priority[PriorityCount];//priority enum above
protected:
	void InitDefaultPriorityData(void);
	void ReadPriorityData(void);
	void SavePriorityData(void);
	void DeletePriorityData(void);
};

class INPUTAIRSIDE_API TakeOffPositionInfo
{
public:
	TakeOffPositionInfo(int nParentID);
	TakeOffPositionInfo(CADORecordset &adoRecordset);
	~TakeOffPositionInfo(void);
public:
	void SaveData(int nParentID);
	void DeleteData(void); 
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
protected:
	int m_nID;
	int m_nParentID;

public:
	int m_nTakeOffPositionID;//CRunwayTakeOffRTIntersectionData's id
	CString m_strTakeOffPositionName;//Maybe is too slow to find the name of TakeOff Position ,so need cache
	int m_nRwyID;
	int m_nRwyMark;
};

class INPUTAIRSIDE_API TakeoffSequencing
{
public:
	TakeoffSequencing(int nProjID,CAirportDatabase* _AirportDB ); 
	TakeoffSequencing(CADORecordset &adoRecordset,CAirportDatabase* _AirportDB );
	virtual ~TakeoffSequencing(void);

	int GetTakeOffSequenceInfoCount(void);
	TakeOffSequenceInfo * GetTakeOffSequenceInfo(int nIndex);
	bool AddTakeOffSequenceInfo(TakeOffSequenceInfo * pItemAdd);
	bool DeleteTakeOffSequenceInfoItem(TakeOffSequenceInfo * pTakeOffSequenceItem);

	int GetTakeOffPositionCount(void);
	TakeOffPositionInfo * GetTakeOffPositionInfo(int nIndex);
	bool AddTakeOffPositionInfo(TakeOffPositionInfo * pItemAdd);
	bool DeleteTakeOffPositionItem(TakeOffPositionInfo * pTakeOffPositionItem);
	void DeleteAllTakeOffPositionItem(void);

	void SaveData(void);
	void DeleteData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
protected:
	int	m_nID;
	int	m_nProjID;
	int m_nAirportID;//not use now
	CAirportDatabase* m_AirportDB ;
	std::vector<TakeOffSequenceInfo *> m_vrTakeOffSequenceInfo;
	std::vector<TakeOffSequenceInfo *> m_vrTakeOffSequenceInofDeleted;//deleted
	std::vector<TakeOffPositionInfo *> m_vrTakeOffPositionInfo;
	std::vector<TakeOffPositionInfo *> m_vrTakeOffPositionInfoDeleted;//deleted
protected:
	void Clear(void);
	void ReadTakeOffSequenceInfo(void);
	void SaveTakeOffSequenceInfo(void);
	void DeleteTakeOffSequenceInfo(void);
	void ReadTakeOffPositionInfo(void);
	void SaveTakeOffPositionInfo(void);
	void DeleteTakeOffPositionInfo(void);
	//CString FindTakeOffPositionName(int nTakeOffPositionID,CRunwayTakeOffPosition &rwTakeOffPosition,bool &bIDExist);
};


class INPUTAIRSIDE_API TakeoffSequencingList
{
public:
	TakeoffSequencingList(int nProjectID,CAirportDatabase* _AirportDB );
	virtual ~TakeoffSequencingList(void);

	int GetTakeoffSequencingCount() const;
	const TakeoffSequencing* GetItem(int nIndex) const;
	TakeoffSequencing* GetItem(int nIndex);
	bool AddItem(TakeoffSequencing* pNewItem);
	bool DeleteItem(TakeoffSequencing* pItem);	 

	static CString GetPriorityName(Priority priority);

	void ReadData(void);
	void SaveData(void);
	void DeleteData(void);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
protected:
	int m_nProjID;
	std::vector<TakeoffSequencing *> m_vrTakeOffSequenceItem;
	std::vector<TakeoffSequencing *> m_vrTakeOffSequenceItemDeleted;
	CAirportDatabase* m_AirportDB ;
protected:
	void Clear(void);

public:
	static void ImportTakeoffSequencing(CAirsideImportFile& importFile,CAirportDatabase* _AirportDB);
	static void ExportTakeoffSequencing(CAirsideExportFile& exportFile,CAirportDatabase* _AirportDB);

};