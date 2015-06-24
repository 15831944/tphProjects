//---------------------------------------------------------------------------------------
//			Class:			SeatAssignmentInSim
//			Author:			sky.wen
//			Date:			Jun 17,2011
//			Purpose:		retrieve seat from rules of seat assignment
//---------------------------------------------------------------------------------------
#pragma once

class COnboardPaxCabinAssign;
class OnboardFlightInSim;
class CSeatingPreferenceList;
class Person;
class OnboardSeatInSim;
class COnboardSeatTypeDefine;
class ALTObjectID;
class CSeatingPreferenceItem;
enum SeatType;
class CFlightSeatTypeDefine;
class StringDictionary;
class CFlightPaxCabinAssign;
enum SeatAssignmentType;
//---------------------------------------------------------------------------------------
//Summary:
//		base class of free seating and preference seating
//----------------------------------------------------------------------------------------
class SeatAssignBehavior
{
public:
	SeatAssignBehavior(OnboardFlightInSim* pOnboardFlightInSim,CFlightPaxCabinAssign* pOnboardPaxCabin);
	virtual ~SeatAssignBehavior();

	//-------------------------------------------------------------------------
	//Summary:
	//		abstract method to get seat
	//-------------------------------------------------------------------------
	virtual OnboardSeatInSim* GetSeat(Person* pPerson,CString& strMessage)const = 0;

protected:
	//--------------------------------------------------------------------------
	//Summary:
	//		retrieve unassign seat list
	//-------------------------------------------------------------------------
	bool GetUnassignSeatList(std::vector<OnboardSeatInSim*>& vSeatList)const;
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
	bool GetFlightCabinData(Person* pPerson,const std::vector<OnboardSeatInSim*>& unAssignSeatList, std::vector<OnboardSeatInSim*>& vSeatList)const;

protected:
	OnboardFlightInSim* m_pOnboardFlightInSim;//relative to onboard flight
	CFlightPaxCabinAssign* m_pFlightCabin;//check passenger constraint fit
};

//----------------------------------------------------------------------------------------
//Summary:
//		Preference seating rules
//----------------------------------------------------------------------------------------
class PreferenceSeatingBehavior: public SeatAssignBehavior
{
public:
	PreferenceSeatingBehavior(OnboardFlightInSim* pOnboardFlightInSim,CFlightPaxCabinAssign* pOnboardPaxCabin,\
		CSeatingPreferenceList* pSeatingPreferenceList);
	virtual ~PreferenceSeatingBehavior();

	//----------------------------------------------------------------------------
	//Summary:
	//		retrieve seat by preference rules
	//Parameters:
	//		pPerson: passenger who want to get seat
	//return:
	//		return seat by preference rules
	//----------------------------------------------------------------------------
	virtual OnboardSeatInSim* GetSeat(Person* pPerson,CString& strMessage)const;

private:
	//--------------------------------------------------------------------------------
	//Summary:
	//		fit all the seat preference priority
	//Parameters:
	//		pItem: current item to fit
	//		pPerson: person who to retrieve seat
	//Return
	//		NULL: does get
	//		NOT NULL: seat person who want to get
	//----------------------------------------------------------------------------------
	OnboardSeatInSim* BuildPrioritySeat(std::vector<OnboardSeatInSim*>& vSeatList,CSeatingPreferenceItem* pItem,Person* pPerson)const;
	//----------------------------------------------------------------------------------
	//Summary:
	//		fit priority item to get seat
	//Parameters:
	//		emType: priority type
	//		pFlightSeat: onboard flight
	//		pPerson: person who wants get seat
	//Return:
	//		seat that person who wants to get
	//------------------------------------------------------------------------------------
	OnboardSeatInSim* GetPrioritySeat(std::vector<OnboardSeatInSim*>& vSeatList,SeatType emType,Person* pPerson)const;
private:
	CSeatingPreferenceList*	m_pSeatPreferenceList;	//user define data for preference
};

//----------------------------------------------------------------------------------------
//Summary:
//		free seating rules
//----------------------------------------------------------------------------------------
class FreeSeatingBehavior : public SeatAssignBehavior
{
public:
	FreeSeatingBehavior(OnboardFlightInSim* pOnboardFlightInSim,CFlightPaxCabinAssign* pOnboardPaxCabin);
	virtual ~FreeSeatingBehavior();

	virtual OnboardSeatInSim* GetSeat(Person* pPerson,CString& strMessage)const;
};

//---------------------------------------------------------------------------------------
//Summary:
//		composite of seat assignment rules and retrieve seat
//---------------------------------------------------------------------------------------
class SeatAssignmentInSim
{
public:
	SeatAssignmentInSim(OnboardFlightInSim* pOnboardFlightInSim);
	~SeatAssignmentInSim(void);

public:
	//---------------------------------------------------------------------------------
	//Summary:
	//		retrieve seat
	//Parameters:
	//		pPerson: who wants to get seat
	//return:
	//		return seat by seat assignment rules
	//--------------------------------------------------------------------------------
	OnboardSeatInSim* GetSeat(Person* pPerson,CString& strMessage);
	//-----------------------------------------------------------------------------
	//Summary:
	//		create instance by seat assignment rules
	//-----------------------------------------------------------------------------
	void Initialize(CSeatingPreferenceList* pSeatingPreferenceList, CFlightPaxCabinAssign* pOnboardPaxCabin,const SeatAssignmentType& emType);
private:
	OnboardFlightInSim* m_pOnboardFlightInSim;//onboard flight relative
	SeatAssignBehavior* m_pSeatAssignBehavior;//strategy to get seat
};
