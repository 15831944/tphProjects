#pragma once

#include "Agentbind.h"
//#include <common/elaptime.h>
class SimSystem;
class Message;
class ElapsedTime;

//Simulation thread ,
class SimAgentEvent;
class SimAgent : public SimObserver
{
	DynamicClassStatic(SimAgent)
public:
	SimAgent(){ nBlockCnt=0; mpDependAgents =0; }

	//agents deepened check
	bool bCanDependOnAgents(AgentsBindNode* pNode);

	bool DependOnAgents(AgentsBindNode* pNode);
	bool DependOnAgents(SimAgent* pAgent);

	void RemoveDepend();
	void SetDepend(AgentsBindNode* pNode);

	//
	virtual void OnNotify( SimAgent* pAgent, SimMessage& msg )=0;	//notify by other agents
	virtual bool IsActive()const=0;
	virtual void OnEvent(const ElapsedTime& tTime){  }
	//virtual ElapsedTime getCurTime()const=0;

	/// for a agent can observer by many agents bind node
	typedef std::vector<AgentsBindNode*> SimObserverList;
	void ObserverBy(AgentsBindNode* pNode);
	void RemoveObserver(AgentsBindNode* pNode);	

	void NotifyOtherAgents(SimMessage& msg);
	///
	bool _IsActiveNode( const DependCheckSession& );
	bool _bBlocked()const{ return nBlockCnt>0; }
	void _Block(bool b){	b?++nBlockCnt:--nBlockCnt; }


	friend class AgentsBindNode;
protected:	
	virtual SimSystem* _getSimSystem()const;	
	void _BeNotify( SimAgent* pAgent,SimMessage& msg);	
	//block from being graph loop visit
	int nBlockCnt; 
	SimObserverList mObservers;
	AgentsBindNode* mpDependAgents; //agents that are going to notify this
};




