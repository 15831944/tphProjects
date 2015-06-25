#pragma once
#include "../../Common/Referenced.h"
#include "../../InputAirside/TaxiwayConstraint.h"
#include "../../InputAirside/DirectionalityConstraints.h"
#include "../../Common/FLT_CNST.H"
#include "AirsideEngineCommon.h"
#include <vector>
#include <map>

class AirsideFlightInSim;

class WeightWingspanConstraintsInSim
{
public:
	WeightWingspanConstraintsInSim(){};
	virtual ~WeightWingspanConstraintsInSim(){};

public:
	bool canServe(const AirsideFlightInSim *pFlight) const;
	void addItem(TaxiwayConstraintType type, int nValue);

protected:

	bool isExist(TaxiwayConstraintType type) const;
	std::map< TaxiwayConstraintType, int > m_mapWeightWingspanCons;
};

class FlightTypeRestrictionsInSim
{
public:
	FlightTypeRestrictionsInSim(){};
	virtual ~FlightTypeRestrictionsInSim(){};

	bool canServe(const AirsideFlightInSim* pFlight) const;

	void addItem(FlightConstraint& cons);

protected:
	bool isExist(FlightConstraint& cons);
	std::vector<FlightConstraint> m_vectFltTypeCons;

};

class DirectionalityConstraintsInSim
{
public:
	DirectionalityConstraintsInSim():m_dBearingDegree(0.0){};
	virtual ~DirectionalityConstraintsInSim(){};

	bool canServe(DirectionalityConstraintType emType, double dBearingDegree = 0.0);

	void SetDirectionConstraintType(DirectionalityConstraintType emType);
	void SetBearingDegree(double dValue);
protected:
	DirectionalityConstraintType m_emDirection;
	double						 m_dBearingDegree;
};

class CTaxiwayClosureConstraint
{
public:
	class ClosureTimeRage
	{
	public:
		ClosureTimeRage(ElapsedTime epStart, ElapsedTime epEnd)
		{
			startTime = epStart;
			endTime = epEnd;
		}
		~ClosureTimeRage()
		{

		}
	public:
		bool TimeInClosureRange(ElapsedTime eptime) const
		{
			return ((eptime > startTime)&& (eptime < endTime));
		}

	protected:
		ElapsedTime startTime;
		ElapsedTime endTime;
		

	};

public:
	CTaxiwayClosureConstraint();
	~CTaxiwayClosureConstraint();

public:
	void addItem(ElapsedTime epStartTime,ElapsedTime epEndTime);

	bool canServe(const AirsideFlightInSim *pFlight) const;

protected:
	std::vector<ClosureTimeRage > m_vTimeConstraint;

};


class TaxiSpeedConstraintsInSim
{
public:
	// speed unit: 	CMpS
	typedef std::pair<FlightConstraint/*Flight Type*/, double/*Speed Constraint*/> value_type;

	TaxiSpeedConstraintsInSim(){};
	/*virtual */~TaxiSpeedConstraintsInSim(){};

	// method to retrieve constrained speed of flight
	// return false if no constrain presents,
	// else true, and the constrain speed is stored to dSpeed
	bool GetConstrainedSpeed(const AirsideFlightInSim* pFlight, double& dSpeed) const;

	// method to add new constraint item,
	// only access this method in TaxiwaySegConstraintsProperties::InitProperties
	void addItem(const FlightConstraint& cons, double dSpeed);
	void addItem(const value_type& item);

private:
	typedef std::vector<value_type> dataset_type;
	dataset_type m_vTaxiSpeedConstraints;
};


class TaxiwaySegConstraintsProperties
{
public:
	TaxiwaySegConstraintsProperties(void);
	virtual ~TaxiwaySegConstraintsProperties(void);

public:
	bool canServe(const AirsideFlightInSim *pFlight) const;
	bool GetTaxiwayConstrainedSpeed(const AirsideFlightInSim* pFlight, double& dSpeed) const;

	void AddWeightWingspanConstraint(TaxiwayConstraintType type, int nValue);
	void AddFlightTypeRestrictions(FlightConstraint& fltCons);
	void SetDirectionConstraintType(DirectionalityConstraintType emDir);
	void SetBearingDegree(double dValue);
	void AddClosureTimeConstraint(ElapsedTime epStartTime,ElapsedTime epEndTime);
	void AddTaxiSpeedConstraint(const FlightConstraint& cons, double dSpeed);

//	void InitProperties(int nProjID);

protected:
	WeightWingspanConstraintsInSim		m_weightWingspanCons;
	FlightTypeRestrictionsInSim			m_fltTypeCons;
	DirectionalityConstraintsInSim		m_directionCons;
	CTaxiwayClosureConstraint			m_closureConstraint;
	TaxiSpeedConstraintsInSim           m_taxiSpeedConstraints;
};
