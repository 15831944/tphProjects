#pragma once

#include "../common/template.h"
#include "Process.h"
class Person;
class Conveyor;

class DependentProcessorList
{
public:
	DependentProcessorList(Processor* pProc);
	~DependentProcessorList(void);

	void enableDependentProc(bool bEnable){ m_bEnableProcDependent = bEnable; }

	bool isDependentProcUsed(){ return m_bEnableProcDependent; }

	bool allProcessorsOccupied(Person* _person) const;

	void addDependentProcessor(Processor* _processor);

	void AddWaitingPairInDependentProc(const WaitingPair& _wait);

protected:
	//check loader conveyor is vacant 
	bool CheckLoaderVacant(Conveyor* pDependentProc,Person* _person)const;

	int GetCheckBaggageCount(Person* _person)const;
private:
	bool m_bEnableProcDependent;
	std::vector< Processor* > m_vectDependentProc;

	Processor* m_pProc;//owner processor

};
