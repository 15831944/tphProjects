#pragma once
#include "Common\elaptime.h"

class PaxOnboardBaseBehavior;
class PaxCellLocation;


class OnboardPaxFreeMovingConflict
{
public:
	class ConflictPax
	{
	public:
		ConflictPax()
		{
			m_dAngle = 0.0;
			m_dDistance = 0.0;
			m_pPax = NULL;
		}
		~ConflictPax()
		{

		}
	public:
		double m_dAngle;
		double m_dDistance;
		PaxOnboardBaseBehavior *m_pPax;

	private:
	};

public:
	OnboardPaxFreeMovingConflict(PaxOnboardBaseBehavior *pCurPax);
	~OnboardPaxFreeMovingConflict(void);

public:
	//handle the conflict
	//the method would give a solution anyway
	void HandleConflict(const PaxCellLocation& curLocation, const PaxCellLocation& nextLocation);

public:
	PaxOnboardBaseBehavior *m_pCurPax;

	std::vector<ConflictPax > m_vInsightPax;

	//current time
	ElapsedTime m_eCurrentTime;

protected:
	std::vector<PaxOnboardBaseBehavior *> GetPassengerConflict(const PaxCellLocation& nextLocation, PaxOnboardBaseBehavior *pBehavior) const;

	std::vector<PaxOnboardBaseBehavior *> GetPassengerRightAhead(const PaxCellLocation& curLocation, const PaxCellLocation& nextLocation, PaxOnboardBaseBehavior *pBehavior) const;
};
