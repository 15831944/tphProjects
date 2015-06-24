#pragma once

#include <common/Types.hpp>
#include <common/elaptime.h>
class SimAgent;

class SimMessage
{
	DynamicClassStatic(SimMessage)
public:		
	ElapsedTime getTime()const{ return mTime; }
	SimMessage& setTime(const ElapsedTime& t){ mTime=t; return *this ;}
	ElapsedTime mTime;
};

struct DependCheckSession
{
	int mnSysState;
	SimAgent* mpCheckAgent;
	DependCheckSession(){ mpCheckAgent = 0; mnSysState = -1; }
	DependCheckSession(int nSystem, SimAgent* pCheckAgent){ mpCheckAgent = pCheckAgent; mnSysState = nSystem; }
	bool operator!=(const DependCheckSession& oS){ return !(mnSysState==oS.mnSysState && mpCheckAgent==oS.mpCheckAgent); }
}; //identify the node be checked

//CLASS_HANDLE(SimObserver)
class SimObserver //: BASE_CLASS
{
public:
	virtual void _BeNotify( SimAgent* pAgent,SimMessage& msg)=0;
	virtual bool _IsActiveNode(const DependCheckSession& session )=0;
};


//////////////////////////////////////////////////////////////////////////
class AgentsBindNode : public SimObserver
{
public:
	AgentsBindNode(){ mhObserver = 0; }
	virtual ~AgentsBindNode(){}
	///
	virtual void _Block(bool b)=0;
	void _BeNotify( SimAgent* pAgent,SimMessage& msg);

	AgentsBindNode* OR(AgentsBindNode* pb);
	AgentsBindNode* AND(AgentsBindNode* pb);
	inline AgentsBindNode* OR(SimAgent* pA){ return OR(Bind(pA));}
	inline AgentsBindNode* AND(SimAgent* pA){ return AND(Bind(pA)); }
	static AgentsBindNode* Bind(SimAgent* pA);

	void SetObserver(SimObserver* pObserver){	mhObserver = pObserver; }
protected:	
	SimObserver* mhObserver;
};
