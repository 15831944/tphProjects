// FlightScheduleAddUtil.h: interface for the CFlightScheduleAddUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLIGHTSCHEDULEADDUTIL_H__D14CF522_A5BF_461E_84A0_11186ACB8D86__INCLUDED_)
#define AFX_FLIGHTSCHEDULEADDUTIL_H__D14CF522_A5BF_461E_84A0_11186ACB8D86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class ProbabilityDistribution;
class InputTerminal;
#include <vector>
#include "../Common/FLT_CNST.H"
#include "common\elaptime.h"
class CFlightScheduleAddUtil  
{
	FlightConstraint m_flightConstraint;
	ProbabilityDistribution* m_pChangeDistribution;
	ProbabilityDistribution* m_pTimeOffsetDistribution;
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
	InputTerminal* m_pTerm;
public:
	CFlightScheduleAddUtil( InputTerminal* _pTerm );
	virtual ~CFlightScheduleAddUtil();
public:

	void SetFlightConstraint(const FlightConstraint& _flightConstraint ){ m_flightConstraint = _flightConstraint;	}
	void SetChageDistribution( ProbabilityDistribution* _pChangeDist) { m_pChangeDistribution = _pChangeDist;	}
	void SetTimeOffsetDistribution( ProbabilityDistribution* _pTimeOffsetDis ){ m_pTimeOffsetDistribution = _pTimeOffsetDis;	}
	void SetStartTime ( const ElapsedTime& _startTime ){ m_startTime = _startTime;	}
	void SetEndTime( const ElapsedTime& _endTime ){ m_endTime = _endTime;	}

	const FlightConstraint& GetFlightConstraint()const { return m_flightConstraint ;	}
	const ProbabilityDistribution* GetChageDistribution()const { return m_pChangeDistribution;	}
	const ProbabilityDistribution* GetTimeOffsetDistribution()const { return m_pTimeOffsetDistribution ;	}
	const ElapsedTime& GetStartTime ()const{ return m_startTime ;	}
	const ElapsedTime& GetEndTime()const{ return m_endTime ;	}

	//create new flight according distribution
	void BuildNewFlight( std::vector<Flight*>& _vNewFlights );

	void WriteToFile( ArctermFile& p_file)const;
	void ReadFromFile( ArctermFile& p_file);

	// replace old changedistribution with the new one, and delete it
	void ReplaceChangeDistribution( ProbabilityDistribution* _pChangeDist );

	// replace old timeoffsetdistribution with the new one, and delete it
	void ReplaceTimeoffsetDistribution( ProbabilityDistribution* _pTimeOffsetDis );


private:
	// find all flight whose flight constraint is fit with m_flightConstraint
	void FindAllExistFlight( std::vector<Flight*>& _vExistFlights  );

	//modify airline name ( that is , if prototype flight is A23, then new flight is A24 etc.
	void BuildFlightAirlineName( Flight* _pPrototype, Flight* _pNewFlight);

	bool IfFlightExistInSchedule( CString sAirLine, CString sFlightID );
};

#endif // !defined(AFX_FLIGHTSCHEDULEADDUTIL_H__D14CF522_A5BF_461E_84A0_11186ACB8D86__INCLUDED_)





















