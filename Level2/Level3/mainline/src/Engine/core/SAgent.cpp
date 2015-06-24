#include "StdAfx.h"
#include ".\sagent.h"
#include "../event.h"
#include "Common/CPPUtil.h"

class AgentTerminateEvent : public Event
{
public:
	AgentTerminateEvent(SAgent* pAgent, const ElapsedTime& t)
	:m_pAgent(pAgent)
	{ 
		m_strDesc.Format("%s Terminate", pAgent->getTypeDesc().GetString() );
		setTime(t); 
	}

	virtual int getEventType (void) const 
	{
		return AgentTerminatEvent;
	}
	virtual const char *getTypeName (void) const
	{
		return m_strDesc.GetString();
	}
	virtual int kill(){ return 1; }

	int process(CARCportEngine* pEngine )
	{
		m_pAgent->OnTerminate(pEngine);
		//m_pAgent->ClearSignal();
		m_pAgent->DeActive();
		return 1;
	}
protected:
	SAgent* m_pAgent;
	CString m_strDesc;
};
//active loop event



ElapsedTime SAgent::curTime()
{
	return SEvent::curTime();
}


//S_BEGIN_SIGNALMAP(SAgent,SignalHandler)
//S_END_SIGNALMAP

SAgent::~SAgent()
{
	DeActive();
	//ClearSignal();	
}



void SAgent::Activate( const ElapsedTime& nextT )
{
	m_activator.ActiveAgent(this, nextT);
}

void SAgent::DeActive()
{
	m_activator.removeFromEventList();
}

void SAgent::Terminate(const ElapsedTime& t)
{
	ASSERT(curTime()<=t);
	AgentTerminateEvent* newTerminateEvt = new AgentTerminateEvent(this,t);
	newTerminateEvt->addEvent();
}

//void SAgent::Terminate()
//{
//	DeActive();
//	ClearSignal();
//	Terminate(curTime());
//}


CString SAgent::getTypeDesc() const
{
	return _T("Agent");
}

SAgent::SAgent()
{
}

void SAgent::OnNotify( IObserveSubject* subjuct, int nMsg )
{
	Activate(curTime());
}

void SEvent::removeFromEventList()
{
	if(m_pEventList)
	{
		for(int i=0;i<m_pEventList->getCount();i++)
		{
			if( this == m_pEventList->getItem(i) )
			{
				m_pEventList->removeItem(i);
				break;
			}
		}			
	}
}

void SAgentActivator::doProcess( CARCportEngine* pEngine )
{	
	if(m_pAgent)
		m_pAgent->OnActive(pEngine);
}


void SAgentActivator::ActiveAgent( SAgent* pAgent, const ElapsedTime& t )
{
	m_pAgent = pAgent;
	setTime(t);
	addEvent();
}


SAgentActivator::SAgentActivator()
{	
	m_pAgent = NULL;
}
