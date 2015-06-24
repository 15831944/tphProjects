#include "StdAfx.h"
#include ".\seatassignmentinsim.h"
#include "..\InputOnboard\OnboardPaxCabinAssign.h"
#include "..\InputOnboard\SeatingPreference.h"
#include "OnboardFlightInSim.h"
#include "person.h"
#include "OnboardSeatInSim.h"
#include "..\InputOnboard\SeatTypeSpecification.h"
#include "Common\ALTObjectID.h"
#include "Common\strdict.h"
#include "..\InputOnboard\Seat.h"
#include "..\InputOnboard\FlightPaxCabinAssign.h"


SeatAssignBehavior::SeatAssignBehavior(OnboardFlightInSim* pOnboardFlightInSim,CFlightPaxCabinAssign* pOnboardPaxCabin)
:m_pOnboardFlightInSim(pOnboardFlightInSim)
,m_pFlightCabin(pOnboardPaxCabin)
{
	
}

SeatAssignBehavior::~SeatAssignBehavior()
{
	
}
//--------------------------------------------------------------------------
//Summary:
//		retrieve unassign seat list
//-------------------------------------------------------------------------
bool SeatAssignBehavior::GetUnassignSeatList( std::vector<OnboardSeatInSim*>& vSeatList )const
{
 	ASSERT(m_pOnboardFlightInSim);
 	if (m_pOnboardFlightInSim == NULL)
 		return vSeatList.empty();
 
 	std::vector<OnboardSeatInSim *>* pSeatList = m_pOnboardFlightInSim->GetOnboardSeatList();
 	std::vector<OnboardSeatInSim *>::const_iterator iter = pSeatList->begin();
 	for (; iter != pSeatList->end(); ++iter)
 	{
 		if (*iter == NULL)
 			continue;
 
 		if ((*iter)->IsAssigned())
 			continue;
 
 		vSeatList.push_back(*iter);
 	}

	return vSeatList.empty();
}
//------------------------------------------------------------------------
//Summary:
//		retrieve fit define cabin constraint
//Parameters:
//		pPerson: person who wants to find correct seat
//		vSeatList[out]: all fit seat list
//		unAssignSeatList: unassign seat list
//Return:
//		true: find seat
//		false: does not find
//-----------------------------------------------------------------------
bool SeatAssignBehavior::GetFlightCabinData( Person* pPerson,const std::vector<OnboardSeatInSim*>& unAssignSeatList,std::vector<OnboardSeatInSim*>& vSeatList) const
{
	ASSERT(m_pOnboardFlightInSim);
	if (m_pOnboardFlightInSim == NULL)
		return vSeatList.empty();

	std::vector<OnboardSeatInSim *>::const_iterator iter = unAssignSeatList.begin();
	for (; iter != unAssignSeatList.end(); ++iter)
	{
		if (*iter == NULL)
			continue;

		if ((*iter)->IsAssigned())
			continue;

		CSeat* pSeat = (*iter)->m_pSeat;


		if (m_pFlightCabin && !m_pFlightCabin->valid(pSeat,pPerson->getLogEntry().GetMobDesc()))
		{
			continue;
		}

		vSeatList.push_back(*iter);
	}

	return vSeatList.empty();
}



PreferenceSeatingBehavior::PreferenceSeatingBehavior(OnboardFlightInSim* pOnboardFlightInSim,CFlightPaxCabinAssign* pOnboardPaxCabin,\
													 CSeatingPreferenceList* pSeatingPreferenceList)
:SeatAssignBehavior(pOnboardFlightInSim,pOnboardPaxCabin)
,m_pSeatPreferenceList(pSeatingPreferenceList)
{
	
}

PreferenceSeatingBehavior::~PreferenceSeatingBehavior()
{
	
}

//----------------------------------------------------------------------------
//Summary:
//		retrieve seat by preference rules
//Parameters:
//		pPerson: passenger who want to get seat
//return:
//		return seat by preference rules
//----------------------------------------------------------------------------
OnboardSeatInSim* PreferenceSeatingBehavior::GetSeat( Person* pPerson,CString& strMessage) const
{
	//retrieve unassign seat list
	std::vector<OnboardSeatInSim*> vSeatList;
	if (GetUnassignSeatList(vSeatList))
	{
		strMessage.Format(_T("There are not enough seats to assign"));
		return NULL;
	}
	//retrieve cabin type data
	std::vector<OnboardSeatInSim*> vCabinSeatList;
	if(GetFlightCabinData(pPerson,vSeatList,vCabinSeatList))
	{	
		strMessage.Format(_T("Please check cabin definition!"));
		return NULL;
	}
	
	//check preference
	std::vector<CSeatingPreferenceItem*> vSeatPerferenceList;
	m_pSeatPreferenceList->GetSeatPreference(vSeatPerferenceList,pPerson->getLogEntry().GetMobDesc());
	
	for (int i = 0; i < (int)vSeatPerferenceList.size(); i++)
	{
		CSeatingPreferenceItem* pItem = vSeatPerferenceList[i];
		OnboardSeatInSim* pSeatInSim = BuildPrioritySeat(vCabinSeatList,pItem,pPerson);
		if (pSeatInSim)
		{
			return pSeatInSim;
		}
	}

	return vCabinSeatList.at(random(vCabinSeatList.size()));
}

//--------------------------------------------------------------------------------
//Summary:
//		fit all the seat preference priority
//Parameters:
//		pItem: current item to fit
//		pPerson: person who to retrieve seat
//Return
//		NULL: does get
//		NOT NULL: seat person who want to get
//--------------------------------------------------------------------------------
OnboardSeatInSim* PreferenceSeatingBehavior::BuildPrioritySeat( std::vector<OnboardSeatInSim*>& vSeatList,CSeatingPreferenceItem* pItem, Person* pPerson ) const
{
	ASSERT(m_pOnboardFlightInSim);
	if (m_pOnboardFlightInSim == NULL)
		return NULL;
	
	for (int i = 0 ; i < NO_TYPE; i++)
	{
		SeatType emType = pItem->GetPriority(i);
		if (emType == NO_TYPE)
			break;
		
		OnboardSeatInSim* pSeat = GetPrioritySeat(vSeatList,emType,pPerson);
		if (pSeat)
		{
			return pSeat;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------------
//Summary:
//		fit priority item to get seat
//Parameters:
//		emType: priority type
//		pFlightSeat: onboard flight
//		pPerson: person who wants get seat
//Return:
//		seat that person who wants to get
//----------------------------------------------------------------------------------
OnboardSeatInSim* PreferenceSeatingBehavior::GetPrioritySeat(std::vector<OnboardSeatInSim*>& vSeatList,SeatType emType,Person* pPerson)const
{
	for (size_t i = 0; i < vSeatList.size(); i++)
	{
		OnboardSeatInSim* pSeatInSim = vSeatList.at(i);
		if (pSeatInSim->fitSeatType(OnboardSeatType(emType)))
		{
			return pSeatInSim;
		}
	}
	return NULL;
}


FreeSeatingBehavior::FreeSeatingBehavior(OnboardFlightInSim* pOnboardFlightInSim,CFlightPaxCabinAssign* pOnboardPaxCabin)
:SeatAssignBehavior(pOnboardFlightInSim,pOnboardPaxCabin)
{

}

FreeSeatingBehavior::~FreeSeatingBehavior()
{

}

//----------------------------------------------------------------------------
//Summary:
//		retrieve seat by free rules
//Parameters:
//		pPerson: passenger who wants to get seat
//return:
//		return seat by free rules
//----------------------------------------------------------------------------
OnboardSeatInSim* FreeSeatingBehavior::GetSeat( Person* pPerson,CString& strMessage) const
{
	//retrieve unassign seat list
	std::vector<OnboardSeatInSim*> vSeatList;
	if (GetUnassignSeatList(vSeatList))
	{
		strMessage.Format(_T("There are not enough seats to assign"));
		return NULL;
	}
	//retrieve cabin type data
	std::vector<OnboardSeatInSim*> vCabinSeatList;
	if(GetFlightCabinData(pPerson,vSeatList,vCabinSeatList))
	{	
		strMessage.Format(_T("Please check cabin definition!"));
		return NULL;
	}
	return vCabinSeatList.at(random(vCabinSeatList.size()));
}

SeatAssignmentInSim::SeatAssignmentInSim( OnboardFlightInSim* pOnboardFlightInSim )
:m_pOnboardFlightInSim(pOnboardFlightInSim)
,m_pSeatAssignBehavior(NULL)
{
	
}

SeatAssignmentInSim::~SeatAssignmentInSim(void)
{
	if (m_pSeatAssignBehavior)
	{
		delete m_pSeatAssignBehavior;
		m_pSeatAssignBehavior = NULL;
	}
}

//---------------------------------------------------------------------------------
//Summary:
//		retrieve seat
//Parameters:
//		pPerson: who wants to get seat
//return:
//		return seat by seat assignment rules
//--------------------------------------------------------------------------------
OnboardSeatInSim* SeatAssignmentInSim::GetSeat( Person* pPerson,CString& strMessage )
{
	ASSERT(m_pSeatAssignBehavior);
	if (m_pSeatAssignBehavior == NULL)
		return NULL;
	
	return m_pSeatAssignBehavior->GetSeat(pPerson,strMessage);
}

//-----------------------------------------------------------------------------
//Summary:
//		create instance by seat assignment rules
//-----------------------------------------------------------------------------
void SeatAssignmentInSim::Initialize( CSeatingPreferenceList* pSeatingPreferenceList, CFlightPaxCabinAssign* pOnboardPaxCabin,const SeatAssignmentType& emType)
{
	ASSERT(m_pOnboardFlightInSim);
	if (m_pOnboardFlightInSim == NULL)
		return;

	switch(emType)
	{
	case FREE_STEATING:
		{
			m_pSeatAssignBehavior = new FreeSeatingBehavior(m_pOnboardFlightInSim,pOnboardPaxCabin);
		}	
		break;
	case PREFERENCE_BASES:
		{	
			m_pSeatAssignBehavior = new PreferenceSeatingBehavior(m_pOnboardFlightInSim,pOnboardPaxCabin,pSeatingPreferenceList);
		}	
		break;
	default:
		break;
	}
}