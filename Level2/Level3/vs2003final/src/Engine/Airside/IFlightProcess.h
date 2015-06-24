#pragma once
#include "Common/CPPUtil.h"
class ClearanceItem;
class Clearance;

enum Process_Status{ ps_active, ps_inactive, ps_completed, ps_failed };
class IFlightProcess
{
public:
	IFlightProcess(){ m_state = ps_inactive; }
	virtual bool Process(ClearanceItem& lastItem, Clearance& newClearance)=0; //return next process
	virtual ~IFlightProcess(){}
	Process_Status GetStatus()const{ return m_state; }
	bool isActive()const{ return m_state == ps_active; }
	bool isCompleted()const{ return m_state == ps_completed; }
	bool isFailed()const{ return m_state == ps_failed; }
	bool isInActive()const{ return m_state == ps_inactive; }

	void setActive(){ m_state = ps_active; }
	void setComplete(){ m_state = ps_completed ;}
	void setFailed(){ m_state = ps_failed; }
	void setInActive(){ m_state = ps_inactive; }
protected:
	Process_Status m_state;
	
};
