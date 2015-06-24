#pragma once




class KineticsAndSpaceDataList;
class CDocDisplayTimeDB;
class UnimpededStateTransTime;
class CTransitionTimeModifiers;
class CTargetLocationsList;
class UnexpectedBehaviorVariablesList;
class CSeatingPreferenceList;
class CWayFindingInterventionList;
class CPassengerType;
class StringDictionary;


enum KineticsAndSpaceDataType;
enum OnBoard_BodyState;



class COnBoardPaxInformation
{
public:
	COnBoardPaxInformation(void);
	~COnBoardPaxInformation(void);


public:
	//load all pax setting inforamtion
	void Initialize(int nProjID, StringDictionary * pStrDict);
public:
	double GetPaxKinetics(CPassengerType& paxTypePassed,KineticsAndSpaceDataType kineticsType) const;


	double GetDocumentDisplayTime(CPassengerType& paxTypePassed) const;


	double GetWayFindingIntervention(CPassengerType& paxTypePassed) const;

	
	double GetUnimpededStateTransTime(CPassengerType& paxTypePassed, OnBoard_BodyState stateOrignial, OnBoard_BodyState stateDest) const;


	int GetTransitionTimeModifier_Inbibitor(CPassengerType& paxTypePassed) const;

	int GetTransitionTimeModifier_MTrigger(CPassengerType& paxTypePassed) const;

	double GetTransitionTimeModifier_Factor(CPassengerType& paxTypePassed) const;

	double GetTargetLocation(CPassengerType& paxTypePassed) const;


	double GetUnexpectedBehaviorVariable(CPassengerType& paxTypePassed) const;


	double GetSeatingPerformance(CPassengerType& paxTypePassed) const;




protected:

protected:
	/* the follow information exists in KineticsAndSpaceData
	Speed =0,
		FreeInStep,
		CongestionInStep,
		QueueInStep,
		FreeSideStep,
		CongestionSideStep,
		QueueSideStep,
		InteractionDistance,
		AffinityFreeDistance,
		AffinityCongestionDistance,
		Squeezability
		*/
	//KineticsAndSpaceDataList* m_pKineticsAndSpaceData;
	std::map<int, KineticsAndSpaceDataList*> m_mapkineticsAndSapceData;
	
	//document display time
	CDocDisplayTimeDB* m_pDocDisplayTime;

	//way finding intervention
	CWayFindingInterventionList* m_pWayFindingIntervention;


	//unimpeded state transition time
	UnimpededStateTransTime* m_pUnimpededStateTransTime;

	//transition time modifiers
	CTransitionTimeModifiers* m_pTansitionTimeModifiers;

	//target location variables
	CTargetLocationsList* m_pTargetLocations;
	

	//unexpected behaviors
	UnexpectedBehaviorVariablesList*   m_pUnexpectedBehaviorVariables;

	//seating performance
	CSeatingPreferenceList* m_pSeatingPerformance;

	////carry on priorities


	

};
