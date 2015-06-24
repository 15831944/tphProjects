#pragma once

#include "TerminalMobElementBehavior.h"
#include "visitor.h"

class VisitorTerminalBehavior:public TerminalMobElementBehavior
{
	friend class PaxVisitor;
public:
	VisitorTerminalBehavior(PaxVisitor* pVisitor);
	virtual ~VisitorTerminalBehavior(){};
protected:
	int m_nFreeMoving;

	int m_nFreeMovingIndex;

	// only used when check bag leave baggageproc
	// because now we generate a invalid event when pax have picked up its check bag
	bool m_bEventInvalid;

	bool m_bIsGreeting;
	bool m_bInTray; // If in tray host
	bool m_bWithOwner;//walk with owner

public:
	void SetEventInvalidFlag( bool _bFlag ){ m_bEventInvalid = _bFlag;	};
	bool GetEvnetInvalidFlag()const { return m_bEventInvalid;	};
	//attaches visitor to a passenger.
	void initPassenger( Passenger* _pPax );

	// if in tray host
	bool IfInTrayHost()const {return m_bInTray;}
	void SetInTrayHost(bool bInTrayHost) {m_bInTray = bInTrayHost;}

	//walk with owner
	void SetWithOwner(bool bWalkWithOwner){m_bWithOwner = bWalkWithOwner;}
	bool IfWalkWithOwner()const {return m_bWithOwner;}

	void SetGreetingTag(bool _tag){m_bIsGreeting = _tag;}
	bool GetGreetingTag(){return m_bIsGreeting;}

	void SetFlowList(ProcessorDistribution *_pFlowList){m_pFlowList=_pFlowList;}

	//It is overloaded to process greeting events.
	virtual void processServerArrival (ElapsedTime _time);

	//method called by Passenger to update visitor's location, state.
	int update( ElapsedTime _time, Point _OwnerPreLocation, bool _bBackup );

	bool IfHaveOwnFlow(ElapsedTime _time);

	virtual bool StickNonPaxDestProcsOverload(const ElapsedTime& _curTime);
	virtual bool ProcessPassengerSync(const ElapsedTime& _curTime);
protected:
	virtual bool HandleGreetingLogic( Processor** _pProc ,bool _bStationOnly,ElapsedTime _curTime);
	PaxVisitor* GetVisitor();
	Passenger* GetOwner();
	int  GetOwnerId(void);

};