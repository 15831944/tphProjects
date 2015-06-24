#pragma once
#include "SSignal.h"

class CARCportEngine;
enum Goal_Status{ gs_active, gs_inactive, gs_completed, gs_failed };


class SGoalBase/*:  public SignalHandler*/
{
public:
	SGoalBase():m_iStatus(gs_inactive){ }
	//logic to run each update-step
	virtual void  Process(CARCportEngine* pEngine);
	//logic to run when the goal is activated.
	virtual void  Activate(CARCportEngine* pEngine){ m_iStatus = gs_active; }
	//logic to run when the goal is satisfied. (typically used to switch
	//off any active steering behaviors)
	virtual void  Terminate() {}


	virtual void doProcess(CARCportEngine* pEngine){ setComplete(); }


	//status
	bool         isComplete()const{return m_iStatus == gs_completed;} 
	bool         isActive()const{return m_iStatus == gs_active;}
	bool         isInactive()const{return m_iStatus == gs_inactive;}
	bool         isFailed()const{return m_iStatus == gs_failed;}
	Goal_Status	 getStatus()const{ return m_iStatus; }


	void		setComplete(){ m_iStatus = gs_completed; }
	void		setFailed(){ m_iStatus = gs_failed; }
	void		setActive(){ m_iStatus = gs_active; }
	void		setInactive(){ m_iStatus = gs_inactive; }

	//if m_iStatus is failed this method sets it to inactive so that the goal
	//will be reactivated (replanned) on the next update-step.
	void  ReactivateIfFailed();
	//a Goal is atomic and cannot aggregate subgoals yet we must implement
	//this method to provide the uniform interface required for the goal
	//hierarchy.
	//virtual void AddSubgoalFront(SGoalBase* g){throw std::runtime_error("Cannot add goals to atomic goals");}	
	void  ActivateIfInactive(CARCportEngine* pEngine);


	void setDesc(CString s){ m_sDesc = s; } 
	CString getDesc()const{ return m_sDesc; }
protected:
	Goal_Status       m_iStatus;
	//an enumerated value indicating the goal's status (active, inactive,
	//completed, failed)
	CString			  m_sDesc;
};

template <class entity_type>
class SGoal :  public SGoalBase
{	
public:
	SGoal(entity_type*  pE):m_pOwner(pE){}
public:
	//a pointer to the entity that owns this goal
	entity_type*    m_pOwner;	
};

//////////////////////////////////////////////////////////////////////////
class SGoal_Composite : public SGoalBase
{
public:
	virtual void  Process(CARCportEngine* pEngine);
	//when this object is destroyed make sure any subgoals are terminated
	//and destroyed.
	virtual ~SGoal_Composite(){RemoveAllSubgoals();}

	//adds a subgoal to the front of the subgoal list
	void  AddSubgoalFront(SGoalBase* g);
	void AddSubgoalEnd(SGoalBase* g);

	//this method iterates through the subgoals and calls each one's Terminate
	//method before deleting the subgoal and removing it from the subgoal list
	void  RemoveAllSubgoals();

	virtual void PostProcessSubgoals(CARCportEngine* pEngine){}
	//virtual bool DefaultHandleSignal(SSignal* msg);

private:
	typedef std::vector<SGoalBase* > SubgoalList;
	//bool ForwardMessageToFrontMostSubgoal(SSignal* msg);	

protected:
	//composite goals may have any number of subgoals
	SubgoalList   m_SubGoals;
	//processes any subgoals that may be present
	virtual void  ProcessSubgoals(CARCportEngine* pEngine);

};



template <class entity_type>
class SGoal_Squence : public SGoal_Composite
{
public:
	SGoal_Squence(entity_type*  pE):m_pOwner(pE){}
public:
	//a pointer to the entity that owns this goal
	entity_type*    m_pOwner;	
protected:	
};

//excepetion
class SGoalException
{
public:
	SGoalException(const CString& s = _T("")){ sDesc = s; }
	virtual CString getDesc()const{ return sDesc; } 
	virtual ~SGoalException(){}
protected:
	CString sDesc;
};