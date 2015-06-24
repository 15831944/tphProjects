#include "StdAfx.h"
#include "SGoal.h"

void SGoalBase::ReactivateIfFailed()
{
	if (isFailed())
	{
		m_iStatus = gs_inactive;
	}
}

void SGoalBase::ActivateIfInactive( CARCportEngine* pEngine )
{
	if (isInactive())
	{
		Activate(pEngine);   
	}
}

void SGoalBase::Process( CARCportEngine* pEngine )
{
	if(isComplete()|| isFailed() )
		return ;

	ActivateIfInactive(pEngine);

	if(isActive())
		doProcess(pEngine);
	return;
}

void SGoal_Composite::RemoveAllSubgoals()
{
	for (SubgoalList::iterator it = m_SubGoals.begin();
		it != m_SubGoals.end();
		++it)
	{  
		//(*it)->Terminate();

		delete *it;
	}

	m_SubGoals.clear();
}

void SGoal_Composite::AddSubgoalFront( SGoalBase* g )
{
	//add the new goal to the front of the list
	m_SubGoals.insert(m_SubGoals.begin(),g);
}

void SGoal_Composite::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);

	if(isActive())
	{
		ProcessSubgoals(pEngine);

		//update statues
		while (!m_SubGoals.empty())
		{  
			if(m_SubGoals.front()->isComplete())
			{
				m_SubGoals.front()->Terminate();
				delete m_SubGoals.front(); 
				m_SubGoals.erase(m_SubGoals.begin());
				continue;
			}
			else if(m_SubGoals.front()->isFailed()){
				m_iStatus = gs_failed;
				return;
			}
			else{
				m_iStatus = gs_active;
				return;
			}
		}
		//all goal completed
		PostProcessSubgoals(pEngine);
		m_iStatus = gs_completed;
	}	
	
	
}

//bool SGoal_Composite::ForwardMessageToFrontMostSubgoal( SSignal* msg )
//{
//	if (!m_SubGoals.empty())
//	{
//		return m_SubGoals.front()->SignalProc(msg);
//	}
//	//return false if the message has not been handled
//	return false;
//}

void SGoal_Composite::ProcessSubgoals( CARCportEngine* pEngine )
{
	//remove all completed and failed goals from the front of the subgoal list
	//no more subgoals to process - return 'completed'		
	while (!m_SubGoals.empty())
	{  
		if(m_SubGoals.front()->isComplete())
		{
			m_SubGoals.front()->Terminate();
			delete m_SubGoals.front(); 
			m_SubGoals.erase(m_SubGoals.begin());
			continue;
		}
		else if(m_SubGoals.front()->isFailed()){
			return;
		}
		return m_SubGoals.front()->Process(pEngine);
	}
	
	
}

void SGoal_Composite::AddSubgoalEnd( SGoalBase* g )
{
	m_SubGoals.push_back(g);
}

//bool SGoal_Composite::DefaultHandleSignal( SSignal* msg )
//{
//	return ForwardMessageToFrontMostSubgoal(msg);
//}
