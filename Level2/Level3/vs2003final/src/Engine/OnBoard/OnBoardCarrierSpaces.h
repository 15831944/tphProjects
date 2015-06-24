#pragma once
#include "Common/containr.h"
#include "carrierSpace/carrierSpace.h"

class carrierSpace;
class OnBoardCandidateFlights;

class OnBoardCarrierSpaces : public UnsortedContainer < carrierSpace > 
{
public:
	OnBoardCarrierSpaces(const OnBoardCandidateFlights* pCandidateFlts);
	~OnBoardCarrierSpaces(void);

	carrierSpace* getCarrierSpace(CString strACType);

private:
	void Initialize(const OnBoardCandidateFlights* pCandidateFlts);
};
