/*****
*
*   Module:     CTransferFlightConstraint
*   Author:     porter ma
*   Date:       ten,2006
*   Purpose:    CTransferFlightConstraint defines informations of the transfer 
                FlightConstraint,including min transfer time and transfer windows
*               
*
*
*****/
#pragma once
#include "probab.h"

class CTransferFlightConstraint
{
protected:
	FlightConstraint m_DestFlight;//transfer flight
	ProbabilityDistribution* m_MinTransferTime;//transfer min time
	ProbabilityDistribution* m_TransferWindow;//transfer window
	

public:
	CTransferFlightConstraint() 
	{
		m_MinTransferTime = new ConstantDistribution(30);
		m_TransferWindow = new ConstantDistribution(240);
	}

	virtual ~CTransferFlightConstraint() 
	{
		delete m_MinTransferTime;
		delete m_TransferWindow;
	}

	const CTransferFlightConstraint &operator = (const CTransferFlightConstraint &p_const)
	{
		m_DestFlight = p_const.m_DestFlight;
		m_MinTransferTime = p_const.m_MinTransferTime;
		m_TransferWindow = p_const.m_TransferWindow;
	}

	void setFlightConstriant( FlightConstraint _fConstriant)
	{ 
		m_DestFlight = _fConstriant;
	}

    FlightConstraint* getFlightConstraint()
	{
		return &m_DestFlight;
	}

	void setMinTransferTime( ProbabilityDistribution* _minTime)
	{
		m_MinTransferTime = _minTime;
	}

	ProbabilityDistribution* getMinTransferTime()
	{
		return m_MinTransferTime;
	}

	void setTransferWindow( ProbabilityDistribution* _transferWindow)
	{
		m_TransferWindow = _transferWindow;
	}

	ProbabilityDistribution* getTransferWindow()
	{
		return m_TransferWindow;
	}

};