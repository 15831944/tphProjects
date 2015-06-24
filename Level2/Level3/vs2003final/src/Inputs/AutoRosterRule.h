#pragma once
#include "../Common/elaptime.h"
#include "MultiMobConstraint.h"
#include "commondata/procid.h"
#include "../Common/dataset.h"
#include "Flight.h"
#include "Schedule.h"

class InputTerminal;
class CADORecordset;
class ProcessorID;
class ProcessorRoster;
class Flight;
class ProcessorRosterSchedule;
class ProcAssignEntry;
class CMultiMobConstraint;

class CPriorityRule
{
public:
	CPriorityRule(InputTerminal* pInputTerm);
	~CPriorityRule();

	void ReadData(CADORecordset& recordSet);
	void SaveData(int nParentID);
	void UpdateData();
	void DeleteData();

	void SetPriorityID(int nID);
	int GetPriorityID();

	void AddProcID(const CString& sProcID);
	void DelProcID(const CString& sProcID);
	const std::vector<CString>& GetProcList();

	void SetAdjacent(bool bAdjacent);
	bool IsAdjacent();

	void SetMinNum(int nNum);
	int GetMinNum();

private:
	void GetProcListName(CString& strName);
	void SetProcList(const CString& strName);


private:
	int m_nID;
	int m_nPriorityID;
	std::vector<CString> m_vProcList;
	bool m_bAdjacent;
	int m_nMinNum;

};

/////////////////////////////////////////////////////////////////////////////////
class CAutoRosterRule
{
public:
	CAutoRosterRule(InputTerminal* pInputTerm);
	virtual ~CAutoRosterRule(void);

	//bool AddProcRosterToProcAssignSchedules(std::vector<ProcAssignEntry*>& vProAssignEntries, const Flight* pFlight) ;

	virtual void ReadData(CADORecordset& recordSet);
	virtual void ReadRulesFromOldDB(CADORecordset& recordSet ) =0;
	virtual void SaveData();
	virtual void DeleteData();
	virtual void UpdateData();

	void SetProcessorID(const ProcessorID& procID);
	const ProcessorID& GetProcessorID();

	const CMultiMobConstraint& GetPaxType();
	void SetPaxType(const CMultiMobConstraint& paxType);

	void SetForEachFlight(bool bForEachFlight);
	bool IsForEachFlight();

	void SetDefNumProc(bool bDef);
	bool IsDefNumProc();

	void SetNumOfProc(int nNum);
	int GetNumOfProc();

	void SetBySchedule(bool bBySchedule);
	bool IsBySchedule();
	virtual bool IsDaily();

	const std::vector<CPriorityRule*>& GetPriorityRules();
	void AddPriorityRule(CPriorityRule* pRule);
	void RemovePriorityRule(CPriorityRule* pRule);

	void CopyData(CAutoRosterRule* pRule);


	bool AssignProcessorRosterAccordingToPrioirties(std::vector<ProcAssignEntry*>& vProAssignEntries, 
													const CMultiMobConstraint& mobType, 
													const ElapsedTime& tOpen, 
													const ElapsedTime& tClose, 
													bool bExclusive,
													std::vector<ProcessorRoster *>& vCreateRoster);

	bool CreateProcessorRoster(std::vector<ProcAssignEntry*>& vProAssignEntries, Flight* pFlight, char mode,std::vector<ProcessorRoster *>& vCreateRoster);

	virtual ElapsedTime GetRosterOpenTiem(Flight* pflight, char mode) =0;
	virtual ElapsedTime GetRosterCloseTime(Flight* pflight, char mode) =0;

	bool operator < (const CAutoRosterRule& rosterRule)const;
protected:
	int GetAvailableAdjacentProcessorSchedules( const std::vector<ProcAssignEntry*>& vProEntries, 
													std::vector<ProcessorRosterSchedule*>& vSchedules,
													const ElapsedTime& tOpen, 
													const ElapsedTime& tClose, 
													bool bExclusive,
													int nMinAdjacent,
													int nMaxSize);



protected:
	int m_nID;
	ProcessorID m_ProcID;
	std::vector<ProcessorID> m_vProcList;
	CMultiMobConstraint m_paxType;
	bool m_bBySchedule;
	bool m_bForEachFlight;
	bool m_bNumProc;
	int m_nNumProc;
	InputTerminal* m_pInputTerm;
	std::vector<CPriorityRule*> m_vPriorityRules;
	std::vector<CPriorityRule*> m_vDelPriorityRules;

};


/////////////////////////////////////////////////////////////////////////////////////////////
class CRosterRulesBySchedTime : public CAutoRosterRule
{
public:
	CRosterRulesBySchedTime(InputTerminal* pInputTerm) ;
	~CRosterRulesBySchedTime() ;

	enum TIMEINTERVALTYPE
	{
		Before =0,
		After
	};

	virtual void ReadData(CADORecordset& recordSet);
	virtual void ReadRulesFromOldDB(CADORecordset& recordSet );
	virtual void SaveData();
	virtual void DeleteData();
	void UpdateData();


	void SetOpenTime(const ElapsedTime& tTime);
	ElapsedTime GetOpenTime();

	void SetCloseTime(const ElapsedTime& tTime);
	ElapsedTime GetCloseTime();

	void SetOpenIntervalTimeType(TIMEINTERVALTYPE eType);
	TIMEINTERVALTYPE GetOpenIntervalTimeType();

	void SetCloseIntervalTimeType(TIMEINTERVALTYPE eType);
	TIMEINTERVALTYPE GetCloseIntervalTimeType();

	virtual ElapsedTime GetRosterOpenTiem(Flight* pflight, char mode) ;
	virtual ElapsedTime GetRosterCloseTime(Flight* pflight, char mode) ;

private:
	ElapsedTime m_tOpenTime;
	ElapsedTime m_tCloseTime;
	TIMEINTERVALTYPE m_eOpenType;
	TIMEINTERVALTYPE m_eCloseType;

};

////////////////////////////////////////////////////////////////////////////////////////////
class CRosterRulesByAbsoluteTime : public CAutoRosterRule
{
public:
	CRosterRulesByAbsoluteTime(InputTerminal* pInputTerm) ;
	~CRosterRulesByAbsoluteTime() ;

	virtual void ReadData(CADORecordset& recordSet);
	virtual void ReadRulesFromOldDB(CADORecordset& recordSet );
	virtual void SaveData();
	virtual void DeleteData();
	void UpdateData();


	void SetOpenTime(const ElapsedTime& tTime);
	ElapsedTime GetOpenTime();

	void SetCloseTime(const ElapsedTime& tTime);
	ElapsedTime GetCloseTime();

	void SetDaily(bool bDaily);
	virtual bool IsDaily();

	void ReadDataFromFile(ArctermFile& p_file) ;

	virtual ElapsedTime GetRosterOpenTiem(Flight* pflight, char mode) ;
	virtual ElapsedTime GetRosterCloseTime(Flight* pflight, char mode) ;

private:
	ElapsedTime m_tOpenTime;
	ElapsedTime m_tCloseTime;
	bool m_bDaily;

};

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//RosterUnassignFlight
class RosterAssignFlight
{
public:
	enum emOperation
	{
		emOp_Arrival = 0,
		emOp_Departure,
	};

	enum emAssignResult
	{
		// assign result
		emAssign_Null = 0,
		emAss_Sucess,
		emAss_Failed,
		emAss_NotInclude
	};

public:
	RosterAssignFlight(const CMultiMobConstraint* mobCons, Flight *pFlight, emOperation emOp, emAssignResult emAssign, CAutoRosterRule* pRule)
	{
		m_pPaxType			= mobCons;
		m_pFlight			= pFlight;
		m_emOperation		= emOp;
		m_emAssignResult	= emAssign;
		m_pRule				= pRule;
		if(m_pRule)
			m_nProcAssign	= pRule->GetNumOfProc();
		else
			m_nProcAssign	= 1;

	}
	~RosterAssignFlight()
	{
		Clear();
	}

	Flight *getFlight(){return m_pFlight;}

	bool isArrival() const {return m_emOperation == emOp_Arrival;}
	bool isFailedAssign() const {return m_emAssignResult == emAss_Failed;}
	bool isSuccessAssign() const {return m_emAssignResult == emAss_Sucess;}
	bool isExcludeAssign() const {return m_emAssignResult == emAss_NotInclude;}

	int  getProcAssignCount() const{return m_nProcAssign;}
	void setProcAssinCount(int nCount){m_nProcAssign = nCount;}
	
	const CMultiMobConstraint *getPaxType() {return m_pPaxType;}


	void AddRoster(ProcessorRoster *pRoster){m_vAssignedRoster.push_back(pRoster);}
	void AddRoster(std::vector<ProcessorRoster *>& vRoster)
	{
		std::vector<ProcessorRoster *>::iterator iter = vRoster.begin();
		for (; iter != vRoster.end(); ++ iter)
		{
			m_vAssignedRoster.push_back(*iter);
		}
	}
		
	int GetCount()const
	{
		return (int)m_vAssignedRoster.size();
	}

	ProcessorRoster* GetRoster(int idx)
	{
		if (idx < 0 || idx >= GetCount())
			return NULL;
		
		return m_vAssignedRoster[idx];
		m_vAssignedRoster.clear();
		if(m_pRule)
		{
			m_emAssignResult	= emAss_Failed;
			m_nProcAssign		= m_pRule->GetNumOfProc();	
		}
	}
	bool HaveRoster(ProcessorRoster* pRoster)const
	{
		if (std::find(m_vAssignedRoster.begin(),m_vAssignedRoster.end(),pRoster) != m_vAssignedRoster.end())
			return true;
		
		return false;
	}

	void DeleteRoster(ProcessorRoster* pRoster);

	void Clear();
public:
	Flight			*m_pFlight;
	emOperation		m_emOperation;
	emAssignResult	m_emAssignResult;

	//the count of processor need to be assigned for this flight
	int				m_nProcAssign;

	const CMultiMobConstraint *m_pPaxType;

	//the roster list which assigned to the flight
	std::vector<ProcessorRoster *> m_vAssignedRoster;

	CAutoRosterRule* m_pRule;
};
//////////////////////////////////////////////////////////////////////////
//RosterUnassignFlightList
class RosterAssignFlightList
{
public:
	RosterAssignFlightList()
	{

	}
	~RosterAssignFlightList()
	{
		for ( int nFlight = 0; nFlight < getCount(); ++ nFlight)
		{
			delete m_vFlight[nFlight];
		}
		m_vFlight.clear();
	}

public:
	int getCount() const
	{
		return static_cast<int>(m_vFlight.size());
	}

	void RemoveAll()
	{
		for ( int nFlight = 0; nFlight < getCount(); ++ nFlight)
		{
			delete m_vFlight[nFlight];
		}
		m_vFlight.clear();
	}

	RosterAssignFlight *getFlight(int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < getCount());
		if(nIndex >= 0 && nIndex < getCount())
			return m_vFlight[nIndex];

		return NULL;
	}

	RosterAssignFlight* GetRosterFlight(ProcessorRoster* pRoster)
	{
		for (int i = 0; i < getCount(); i++)
		{
			RosterAssignFlight* pFlight = getFlight(i);
			if (pFlight->HaveRoster(pRoster))
			{
				return pFlight;
			}
		}
		return NULL;
	}

	void removeFlight(RosterAssignFlight *pFlight)
	{
		std::vector<RosterAssignFlight *>::iterator iter =  m_vFlight.begin();
		for (; iter != m_vFlight.end(); ++ iter)
		{
			if(*iter  == pFlight)
			{
				m_vFlight.erase(iter);
				return;
			}
		}
	}

	RosterAssignFlight * AddFlight(const CMultiMobConstraint* mobCons, Flight *pFlight, RosterAssignFlight::emOperation emOp, RosterAssignFlight::emAssignResult emAssign, CAutoRosterRule* pRule)
	{
		//find the flight
		std::vector<RosterAssignFlight *>::iterator iter =  m_vFlight.begin();
		for (; iter != m_vFlight.end(); ++ iter)
		{
			RosterAssignFlight *pCurUnassignFlight = *iter;
			if( pCurUnassignFlight->m_pFlight  == pFlight  &&
				pCurUnassignFlight->m_emOperation == emOp )
			{
				if(pCurUnassignFlight->m_pPaxType == NULL)
				{
					if(emAssign == RosterAssignFlight::emAss_Sucess) //update the status
					{
						pCurUnassignFlight->m_pPaxType = mobCons;
						pCurUnassignFlight->m_emAssignResult = RosterAssignFlight::emAss_Sucess;
						if(pRule)
							pCurUnassignFlight->m_nProcAssign = pRule->GetNumOfProc();
						else
							pCurUnassignFlight->m_nProcAssign = 1;

						pCurUnassignFlight->m_pRule = pRule;
					}
					else if(emAssign ==  RosterAssignFlight::emAss_Failed)
					{
						pCurUnassignFlight->m_pPaxType = mobCons;
						pCurUnassignFlight->m_emAssignResult = RosterAssignFlight::emAss_Failed;
						if(pRule)
							pCurUnassignFlight->m_nProcAssign = pRule->GetNumOfProc();
						else
							pCurUnassignFlight->m_nProcAssign = 1;

						pCurUnassignFlight->m_pRule = pRule;

					}
					return pCurUnassignFlight;				
				}
				else if(pCurUnassignFlight->m_pPaxType == mobCons)
				{
					
					if(emAssign == RosterAssignFlight::emAss_Sucess) //update the status
					{
						pCurUnassignFlight->m_emAssignResult = RosterAssignFlight::emAss_Sucess;

					}
					else if(emAssign ==  RosterAssignFlight::emAss_Failed && pCurUnassignFlight->m_emAssignResult == RosterAssignFlight::emAss_NotInclude)
					{
						pCurUnassignFlight->m_emAssignResult = RosterAssignFlight::emAss_Failed;
					}
					return pCurUnassignFlight;
				}
				else if(pCurUnassignFlight->m_pPaxType != mobCons)
				{//check one constraint could fit the other one
					if(pCurUnassignFlight->m_emAssignResult == RosterAssignFlight::emAss_Failed && 
						emAssign == RosterAssignFlight::emAss_Sucess)
					{
						if(mobCons && mobCons->isMatched(*(pCurUnassignFlight->m_pPaxType->getConstraint(0))))
						{
							pCurUnassignFlight->m_pPaxType = mobCons;
							pCurUnassignFlight->m_emAssignResult = RosterAssignFlight::emAss_Sucess;
							if(pRule)
								pCurUnassignFlight->m_nProcAssign = pRule->GetNumOfProc();
							else
								pCurUnassignFlight->m_nProcAssign = 1;

							pCurUnassignFlight->m_pRule = pRule;

							return pCurUnassignFlight;
						}
					}
				}
			}
		}


		//new unassign flight
		RosterAssignFlight *pUnassignFlight = new RosterAssignFlight(mobCons, pFlight, emOp, emAssign, pRule);
		m_vFlight.push_back(pUnassignFlight);
		return pUnassignFlight;
	}
	void removeAssignedFlight()
	{

		int nCount = getCount();
		for (int nItem = nCount -1; nItem >= 0; -- nItem)
		{
			if(m_vFlight[nItem]->m_emAssignResult == RosterAssignFlight::emAss_Sucess)
			{
				delete m_vFlight[nItem];
				m_vFlight.erase(m_vFlight.begin() + nItem);
			}
		}
	}

	void ClearAssignRosters()
	{
		std::vector<RosterAssignFlight *>::iterator iter =  m_vFlight.begin();
		for (; iter != m_vFlight.end(); ++ iter)
		{
			RosterAssignFlight *pFlight  = (*iter);
			pFlight->Clear();
		}
	}

	void Initialize(FlightSchedule* pSchedule)
	{
		int nFlightCount = pSchedule->getCount();
		for( int i=0; i< nFlightCount;  i++ )
		{
			Flight* pFlight = pSchedule->getItem( i );
			if (pFlight->isArriving())
			{								
				AddFlight(NULL, pFlight, RosterAssignFlight::emOp_Arrival, RosterAssignFlight::emAssign_Null, NULL);
			}

			if(pFlight->isDeparting())
			{
				AddFlight(NULL, pFlight, RosterAssignFlight::emOp_Departure, RosterAssignFlight::emAssign_Null, NULL);
			}
		}
	}

protected:
	std::vector<RosterAssignFlight *> m_vFlight;
};

//////////////////////////////////////////////////////////////////////////
//CProcRosterRules
class CProcRosterRules : public DataSet
{
public:
	CProcRosterRules(InputTerminal* pInputTerm);
	~CProcRosterRules() ;

	virtual void readData(ArctermFile& p_file) ;
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file)const ;

	CAutoRosterRule* FindGroupRules(const ProcessorID& pro_id) ;

	virtual const char *getTitle (void) const
	{ return "Auto Assignment file"; }
	virtual const char *getHeaders (void) const
	{ return "Processor,IsDaily,Time,PassengerTy"; }

	void clear();

	void ReadDataFromOldDB(const ProcessorID& procID) ;
	void ReadDataFromDB(const ProcessorID& procID) ;
	void SaveDataToDB() ;

	void AddData(CAutoRosterRule* pData);
	void DelData(CAutoRosterRule* pData); 

	int GetDataCount();
	CAutoRosterRule* GetData(int idx);

	bool FindData(CAutoRosterRule* pData);

	void DoAutoRosterAssignment(std::vector<ProcAssignEntry*>& vProcAssignEntries, RosterAssignFlightList& unassignList = RosterAssignFlightList());

private:
	std::vector<CAutoRosterRule*> m_vData;
	std::vector<CAutoRosterRule*> m_vDelData;


};
























