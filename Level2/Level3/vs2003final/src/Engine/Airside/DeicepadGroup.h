#pragma once


class DeicepadGroup;
class IntersectionNodeInSim;
class AirsideFlightInSim;
class DeicePadInSim;
class StandLeadOutLineInSim;

class DeicepadGroupEntry
{
public:
	DeicepadGroupEntry(DeicepadGroup* pGroup, IntersectionNodeInSim* pNode);
	~DeicepadGroupEntry();

	//************************************
	// Method:    WakeFlight
	// FullName:  DeicepadGroupEntry::WakeFlight
	// Access:    public 
	// Returns:   AirsideFlightInSim*
	// Qualifier: Wake a flight in the queue for entering DeicePadInSim,
	//            if succeed, return the flight,
	//            else return NULL
	// Parameter: DeicePadInSim * pDeicepad
	//************************************
	AirsideFlightInSim* WakeFlight(DeicePadInSim* pDeicepad, const ElapsedTime& exitT);

	DeicepadGroup* GetGroup() const { return m_pGroup; }
	IntersectionNodeInSim* GetIntersectionNode() const { return m_pNode; }

	AirsideFlightInSim* GetFlightWaiting() const { return m_pFlightWaiting; }
	void SetFlightWaiting(AirsideFlightInSim* val) { m_pFlightWaiting = val; }

protected:

private:
	DeicepadGroup* m_pGroup;
	IntersectionNodeInSim* m_pNode;
// 	typedef std::vector<AirsideFlightInSim*> AirsideFlightList;
// 	AirsideFlightList m_vFlightQ;
	AirsideFlightInSim* m_pFlightWaiting;
};


class DeicepadGroup
{
public:
	DeicepadGroup(ALTObjectID groupID);
	~DeicepadGroup();


	//************************************
	// Method:    AssignEntryPoint
	// FullName:  DeicepadGroup::AssignEntryPoint
	// Access:    public 
	// Returns:   void
	// Qualifier: Initialize entry points of DeicepadGroup
	//************************************
	void InitEntryPoint();


	//************************************
	// Method:    WakeFlight
	// FullName:  DeicepadGroup::WakeFlight
	// Access:    public 
	// Returns:   AirsideFlightInSim*
	// Qualifier: call DeicepadGroupEntry::WakeFlight, use round-robin algorithm
	// Parameter: DeicePadInSim * pDeicepad
	//************************************
	AirsideFlightInSim* WakeFlight(DeicePadInSim* pDeicepad, const ElapsedTime& exitT);

	//************************************
	// Method:    AssignDeicepad
	// FullName:  DeicepadGroup::AssignDeicepad
	// Access:    public 
	// Returns:   DeicePadInSim*
	// Qualifier: try to assign DeicePadInSim for the flight, return NULL if failed
	// Parameter: AirsideFlightInSim * pFlight
	//************************************
	DeicePadInSim* AssignDeicepad(AirsideFlightInSim* pFlight);

	void AddDeicepad(DeicePadInSim* pDeicepad);

	const ALTObjectID& GetGroupID() const { return m_groupID; }

	void AddApproachingFlight(AirsideFlightInSim* pFlight);
	void RemoveApproachingFlight(AirsideFlightInSim* pFlight);
	int  GetApproachingFlightNum() const;

	class IDEqual
	{
	public:
		IDEqual(ALTObjectID altID)
			: m_altID(altID)
		{

		}
		bool operator()(const DeicepadGroup* pGroup) const;

	private:
		ALTObjectID m_altID;
	};

	typedef std::vector<DeicepadGroupEntry*> EntryList;
	EntryList GetEntryList() const { return m_vEntries; }
private:

	typedef std::vector<DeicePadInSim*> DeicepadList;
	DeicepadList m_vDeicepads;

	ALTObjectID m_groupID;

	EntryList m_vEntries;
	int m_nLastEntry; // last entry index where waked a flight

	// flight that approaching this group
	typedef std::vector<AirsideFlightInSim*> AirsideFlightList;
	AirsideFlightList m_vFlightApproaching;

	class  DeicePadIDSorter
	{
	public:
		bool operator()(DeicePadInSim* lhs, DeicePadInSim* rhs);
	};

};

