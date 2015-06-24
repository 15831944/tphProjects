#pragma once
#include <algorithm>
#include "Database/ADODatabase.h"
#include "InputOnBoard/OnBoardAnalysisCondidates.h"
#include "Common/containr.h"
#include "Common/FLT_CNST.H"

class CAirportDatabase;
class CFlightTypeCandidate;
class COnboardFlight;
class FlightSchedule;
typedef CFlightTypeCandidate CandidateFlightTypeList;
typedef COnboardFlight CandidateFlight;

class OnBoardCandidateFlights : public UnsortedContainer < CandidateFlight > 
{
public:
	OnBoardCandidateFlights(int nPorjID, InputTerminal* pInTerminal);
	~OnBoardCandidateFlights(void);

	bool getCandidateFlightIndexInSchedule(FlightSchedule* _pfs, std::vector< int >& onBoardFlightIDs);
private:
	void Initialize();

	std::auto_ptr< COnBoardAnalysisCandidates > m_spAnalysisConditates;


};
