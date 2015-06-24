#include "StdAfx.h"
#include "Simagent.h"

#include "agentbind.h"
#include <cassert>
//#include "system.h"


class SingleAgentBind : public AgentsBindNode
{
public:
	SingleAgentBind(SimAgent* pAgent){ 
		mpAgent = pAgent; 
		pAgent->ObserverBy(this);
	}	

	bool _IsActiveNode(const DependCheckSession& session )
	{
		return mpAgent->_IsActiveNode(session);
	}

	void _Block(bool b)
	{
		return mpAgent->_Block(b);
	}

	virtual ~SingleAgentBind()
	{
		mpAgent->RemoveObserver(this);
	}

protected:
	SimAgent* mpAgent;
};
//
class TwoBind : public AgentsBindNode
{
public:
	TwoBind(AgentsBindNode* p1, AgentsBindNode*p2)
	{
		mp1 = p1; 
		p1->SetObserver(this);
		mp2 = p2;
		p2->SetObserver(this);
		bActive =false; 
	}		
	virtual ~TwoBind()
	{
		delete mp1;
		delete mp2;
	}

	void _Block(bool b)
	{
		mp1->_Block(b);
		mp2->_Block(b);
	}

	bool _IsActiveNode(const DependCheckSession& session )
	{		
		if(mlastSession != session )
		{		
			bActive = _checkDependActive(session);			
			mlastSession = session;
		}
		return bActive;
	}	

	virtual bool _checkDependActive(const DependCheckSession& session )=0;

	AgentsBindNode* mp1;
	AgentsBindNode* mp2;

	//optimize code for fast check
	DependCheckSession mlastSession;
	bool bActive;

};

//
class AndBind : public TwoBind
{ 
public:
	AndBind(AgentsBindNode* p1, AgentsBindNode*p2):TwoBind(p1,p2)
	{

	}	

	bool _checkDependActive(const DependCheckSession& session)
	{
		mp2->_Block(true);
		bool bp1 = mp1->_IsActiveNode(session);		
		mp2->_Block(false);

		if(!bp1)
			return false;		

		mp1->_Block(true);
		bool bp2 = mp2->_IsActiveNode(session);	
		mp1->_Block(false);

		return bp1 && bp2;
	}	
};

//
class OrBind : public TwoBind
{
public:
	OrBind(AgentsBindNode* p1, AgentsBindNode*p2):TwoBind(p1,p2){}	

	bool _checkDependActive(const DependCheckSession& session)
	{	
		mp2->_Block(true);
		bool bp1 = mp1->_IsActiveNode(session);
		mp2->_Block(false);

		if(bp1)return true;	

		mp1->_Block(true);
		bool bp2 = mp2->_IsActiveNode(session);
		mp1->_Block(false);

		return bp1 || bp2;
	}
};


AgentsBindNode* AgentsBindNode::OR( AgentsBindNode* pb )
{
	return new OrBind(this,pb);
}

AgentsBindNode* AgentsBindNode::AND( AgentsBindNode* pb )
{
	return new AndBind(this,pb);
}

AgentsBindNode* AgentsBindNode::Bind( SimAgent* pA )
{
	return new SingleAgentBind(pA);
}

void AgentsBindNode::_BeNotify( SimAgent* pAgent,SimMessage& msg )
{
	assert(mhObserver);
	if(mhObserver)
		mhObserver->_BeNotify(pAgent,msg);
}
