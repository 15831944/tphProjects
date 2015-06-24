#pragma once

#include "TerminalMobElementBehavior.h"

class TrayHost;
class PaxTerminalBehavior: public TerminalMobElementBehavior
{
protected:
	ElapsedTime m_tMaxTime;
	int m_lWantToAttatchedVisID;
	BOOL hasBusServer ; 
	//Sync person and it's family
	CSimFlowSync* m_pSimFlowSync;
public:
	PaxTerminalBehavior(Passenger* pPerson);
	virtual ~PaxTerminalBehavior();

	bool CopyVisitListToTrayHost(ElapsedTime _time);
	bool CheckHostHaveOwnFlow(TrayHost* pHost,ElapsedTime _time);

	//Processes "greeting" between an arrival and their greeter.
	void greet( PaxVisitor* _pVisitor, ElapsedTime _time);

	virtual void processServerArrival (ElapsedTime p_time);

	virtual bool StickVisitorListForDestProcesOverload(const ElapsedTime& _curTime);
	virtual bool ProcessPassengerSync(const ElapsedTime& _curTime);
	bool ProcessVisitorSync(Person* pVisitor,const ProcessorID& procID,const ElapsedTime& _curTime);
	void MakePassengerSyncAvailable(const ElapsedTime& curTime);
	void MakeVisitorSyncAvailable(Person* pVisitor, const ElapsedTime& curTime);
	void LoadSyncData(CInputFlowSync* pInputFlowSync);

	// attach my check bags
	void AttachBags( std::vector<Person*>&  _vBags, const ElapsedTime& _time );

	void SetMaxTime(ElapsedTime eventTime) 
	{
		if (eventTime > m_tMaxTime)
		{
			m_tMaxTime = eventTime;
		}
	}
	const ElapsedTime& GetMaxTime() { return m_tMaxTime;}

	void CreateSyncFlow();
	CSimFlowSync* GetSyncFlow()const;

protected:
	virtual bool HandleGreetingLogic( Processor** _pProc ,bool _bStationOnly, ElapsedTime _curTime );
	Passenger* GetPax();
};