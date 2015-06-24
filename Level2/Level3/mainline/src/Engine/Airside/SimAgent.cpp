#include "StdAfx.h"
#include "SimAgent.h"
#include <cassert>
#include "SimSystem.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
bool SimAgent::bCanDependOnAgents( AgentsBindNode* pbindNode )
{
	if(pbindNode)
		//check is there a watch loop in the observer graph	
		return pbindNode->_IsActiveNode( DependCheckSession(_getSimSystem()->getState(), this) );	

	return true;
}



void SimAgent::_BeNotify( SimAgent* pAgent,SimMessage& msg )
{
	if(pAgent == this)
		return;

	if(_bBlocked())
		return;

	_Block(true);
	OnNotify(pAgent,msg);
	_Block(false);
}

void SimAgent::ObserverBy( AgentsBindNode* pNode )
{
	SimObserverList::iterator itr = std::find(mObservers.begin(),mObservers.end(),pNode);
	if(itr==mObservers.end())
		mObservers.push_back(pNode);
}

void SimAgent::RemoveObserver( AgentsBindNode* pNode )
{
	SimObserverList::iterator itr = std::find(mObservers.begin(),mObservers.end(),pNode);
	if(itr!=mObservers.end())
		mObservers.erase(itr);
}



void SimAgent::NotifyOtherAgents( SimMessage& msg )
{
	SimObserverList observerList = mObservers;
	for( SimObserverList::iterator itr = observerList.begin();itr!=observerList.end(); ++itr )
	{
		SimObserver* pBind = (*itr);
		if(pBind)
			pBind->_BeNotify(this,msg);
	}		
}

bool SimAgent::DependOnAgents( AgentsBindNode* pNode )
{
	if( bCanDependOnAgents(pNode) )
	{		
		SetDepend(pNode);
		return true;
	}	
	delete pNode;	
	return false;
}

bool SimAgent::DependOnAgents( SimAgent* pAgent )
{
	return DependOnAgents( AgentsBindNode::Bind(pAgent) );
}


void SimAgent::RemoveDepend()
{
	delete mpDependAgents;
	mpDependAgents = 0;
	_getSimSystem()->stateUp();
}

void SimAgent::SetDepend( AgentsBindNode* pNode )
{
	RemoveDepend();
	mpDependAgents = pNode;
	pNode->SetObserver(this);
	_getSimSystem()->stateUp();
}

bool SimAgent::_IsActiveNode( const DependCheckSession& session)
{
	if(session.mpCheckAgent == this)
		return false;		

	if(!IsActive())
		return false;

	if( mpDependAgents )
	{	
		if(_bBlocked())
			return false;

		_Block(true);
		bool b = mpDependAgents->_IsActiveNode(session);			
		_Block(false);
		return b;
	}
	return true;	
}

SimSystem* SimAgent::_getSimSystem() const
{
	return &SimSystem::Instance();
}
