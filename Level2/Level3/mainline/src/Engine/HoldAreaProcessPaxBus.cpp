#include "StdAfx.h"
#include ".\holdareaprocesspaxbus.h"
#include "hold.h"
#include "person.h"
#include "..\Common\elaptime.h"
HoldAreaPax::HoldAreaPax()
:m_pHoldArea(NULL)
{

}

HoldAreaPax::~HoldAreaPax()
{

}

void HoldAreaPax::SetHold( HoldingArea* pHold )
{
	m_pHoldArea = pHold;
}

HoldingArea* HoldAreaPax::GetHold() const
{
	return m_pHoldArea;
}

void HoldAreaPax::AddPax( Person* pPerson )
{
	if (m_mobileList.Find(pPerson) == INT_MAX)
	{
		m_mobileList.addItem(pPerson);
	}
}

void HoldAreaPax::Process( const ElapsedTime& time )
{
	if(m_pHoldArea == NULL)
		return;

	for (int i = 0; i < m_mobileList.getCount(); i++)
	{
		Person* pPerson = (Person*)m_mobileList.getItem(i);
		m_pHoldArea->releasePax(pPerson,time);
	}
	m_mobileList.clear();
}

HoldAreaProcessPaxBus::HoldAreaProcessPaxBus(void)
{
}

HoldAreaProcessPaxBus::~HoldAreaProcessPaxBus(void)
{
	Clear();
}

void HoldAreaProcessPaxBus::AddHoldPax( HoldingArea* pHoldarea, Person* pPerson )
{
	for (unsigned i = 0; i < m_holdPaxList.size(); i++)
	{
		HoldAreaPax* pHoldPax = m_holdPaxList.at(i);
		if (pHoldPax->GetHold() == pHoldarea)
		{
			pHoldPax->AddPax(pPerson);
			return;
		}
	}
	
	HoldAreaPax* pNewHoldPax = new HoldAreaPax;
	pNewHoldPax->SetHold(pHoldarea);
	pNewHoldPax->AddPax(pPerson);
	m_holdPaxList.push_back(pNewHoldPax);
}

void HoldAreaProcessPaxBus::Process(const ElapsedTime& time)
{
	for (unsigned i = 0; i < m_holdPaxList.size(); i++)
	{
		HoldAreaPax* pHoldPax = m_holdPaxList.at(i);
		pHoldPax->Process(time);
	}
	Clear();
}

void HoldAreaProcessPaxBus::Clear()
{
	for (unsigned i = 0; i < m_holdPaxList.size(); i++)
	{
		delete m_holdPaxList[i];
	}
	m_holdPaxList.clear();
}