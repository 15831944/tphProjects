#include "stdafx.h"
#include "State.h"
#include "..\..\Common\CPPUtil.h"
#include "Common\FileOutPut.h"
//



void IStateMachine::ChangeState( IState* nextState,CARCportEngine* pEngine )
{
	//
	if(m_curState)
	{
		m_curState->Exit(pEngine);
	}
	cpputil::autoPtrReset(m_curState,nextState);
	OnStateChanged();
	if(m_curState)
	{
		m_curState->Entry(pEngine);
	}
}

void IStateMachine::ChangeState( IState* nextState,const ElapsedTime& t )
{
	m_changeEvent.StateChange(this,nextState,t);
}




IStateMachine::~IStateMachine()
{
	cpputil::autoPtrReset(m_curState);
}

void IStateMachine::ExecuteCurState( CARCportEngine* pEngine )
{
	if(m_curState && !m_changeEvent.isInList() )
	{ 
		m_curState->Execute(pEngine);
	}
}


void IState::TransitToState( IState* nextState,CARCportEngine* pEngine )
{
	if(IStateMachine* pMachine = getMachine() )
	{
		pMachine->ChangeState(nextState,pEngine);
	}
}

void IState::TransitToState( IState* nextState,const ElapsedTime& t )
{
	if(IStateMachine* pMachine = getMachine() )
	{
		pMachine->ChangeState(nextState,t);
	}
}


void ChangeStatevent::doProcess( CARCportEngine* pEngine )
{
	if(m_pMachine)
		m_pMachine->ChangeState(m_pnextState,pEngine);
}

void ChangeStatevent::StateChange( IStateMachine* pM, IState* pS,const ElapsedTime&t )
{
	m_pMachine = pM;
	m_pnextState = pS;
	setTime(t);
	addEvent();
}
