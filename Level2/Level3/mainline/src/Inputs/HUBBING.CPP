#include "stdafx.h"
#include "inputs\hubbing.h"
#include "inputs\pax_cnst.h"
#include "inputs\pax_db.h"
#include "inputs\flight.h"
#include "inputs\schedule.h"
#include "inputs\probab.h"
#include "inputs\in_term.h"
#include "inputs\fltdist.h"
#include "common\termfile.h"
#include "common\exeption.h"



HubbingSpecs::HubbingSpecs() : DataSet (HubbingDataFile,(float)2.4)
{
    transiting.setUnits ("PERCENT");
    transferring.setUnits ("PERCENT");
    minTransferTime.setUnits ("MINUTES");
	transferWindow.setUnits ("MINUTES");
	transferDatabase.setUnits ("");
}

void HubbingSpecs::initDefaultValues (void)
{
    ProbabilityDistribution *defaultDist;
    ConstraintEntry *anEntry;

    // init default transiting distribution
    defaultDist = new ConstantDistribution (10);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CPassengerConstraint(m_pInTerm), defaultDist);
    transiting.addEntry (anEntry);

    // init default transferring distribution
    defaultDist = new ConstantDistribution (20);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CPassengerConstraint(m_pInTerm), defaultDist);
	 transferring.addEntry (anEntry);

    // init default min transfer time distribution
    defaultDist = new ConstantDistribution (30);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CPassengerConstraint(m_pInTerm), defaultDist);
    minTransferTime.addEntry (anEntry);

    // init default transfer time window distribution
    defaultDist = new ConstantDistribution (240);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CPassengerConstraint(m_pInTerm), defaultDist);
    transferWindow.addEntry (anEntry);
}

void HubbingSpecs::clear (void)
{
    transiting.clear();
    transferring.clear();
    minTransferTime.clear();
	transferWindow.clear();
	transferDatabase.clear();
}

void HubbingSpecs::readData (ArctermFile &p_file)
{
	assert( m_pInTerm );
    transiting.readDatabase (p_file, "TRANSITING", m_pInTerm);
    transferring.readDatabase (p_file, "TRANSFERRING", m_pInTerm);
    minTransferTime.readDatabase (p_file, "MIN_TRANSFER_TIME", m_pInTerm);
    transferWindow.readDatabase (p_file, "TRANSFER_WINDOW", m_pInTerm);
    transferDatabase.readDatabase (p_file, "TRANSFER_DESTINATIONS", m_pInTerm);

    wipeIntrinsic (transiting);
    wipeIntrinsic (transferring);
    wipeIntrinsic (minTransferTime);
    wipeIntrinsic (transferWindow);
    wipeIntrinsic (transferDatabase);
}

void HubbingSpecs::readObsoleteData( ArctermFile& p_file )
{
	assert( m_pInTerm );
	float fVersion = p_file.getVersion();

	if( fVersion < (float)2.0 )
	{
		ReadVersion10( p_file );
	}
	else if( fVersion == (float)2.3 )	// this is a error version
	{
		transiting.readErrorDatabase (p_file, "TRANSITING", m_pInTerm);
		transferring.readErrorDatabase (p_file, "TRANSFERRING", m_pInTerm);
		minTransferTime.readErrorDatabase (p_file, "MIN_TRANSFER_TIME", m_pInTerm);
		transferWindow.readErrorDatabase (p_file, "TRANSFER_WINDOW", m_pInTerm);
		transferDatabase.readErrorDatabase (p_file, "TRANSFER_DESTINATIONS", m_pInTerm);
		
		wipeIntrinsic (transiting);
		wipeIntrinsic (transferring);
		wipeIntrinsic (minTransferTime);
		wipeIntrinsic (transferWindow);
		wipeIntrinsic (transferDatabase);
	}
	else
	{
		readData( p_file );
	}
}

// removes intrinsic type from all constraints
void HubbingSpecs::wipeIntrinsic (CMobileElemConstraintDatabase &p_db)
{
    int count = p_db.getCount();
    ConstraintEntry *anEntry;
    CPassengerConstraint *aConst;

    for (int i = 0; i < count; i++)
    {
		  anEntry = p_db.getItem (i);
		  aConst = (CPassengerConstraint *)anEntry->getConstraint();
        aConst->setIntrinsicType (0);
    }
}

void HubbingSpecs::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );
    transiting.writeDatabase (p_file, "TRANSITING", m_pInTerm );
    transferring.writeDatabase (p_file, "TRANSFERRING", m_pInTerm);
    minTransferTime.writeDatabase (p_file, "MIN_TRANSFER_TIME", m_pInTerm);
    transferWindow.writeDatabase (p_file, "TRANSFER_WINDOW", m_pInTerm);
    transferDatabase.writeDatabase (p_file, "TRANSFER_DESTINATIONS", m_pInTerm);
}

float HubbingSpecs::getTransitPercent (const CMobileElemConstraint& p_type) const
{
    const ProbabilityDistribution *prob = transiting.FindFit (p_type);
    return (prob)? (float)prob->getRandomValue(): 0;
}

float HubbingSpecs::getTransferPercent (const CMobileElemConstraint& p_type) const
{
    const ProbabilityDistribution *prob = transferring.FindFit(p_type);
    return (prob)? (float)prob->getRandomValue(): 0;
}

ElapsedTime HubbingSpecs::getMinTransferTime (const CMobileElemConstraint& p_type) const
{
    const ProbabilityDistribution *prob = minTransferTime.FindFit(p_type);
    ElapsedTime minXferTime = 60.0 * prob->getRandomValue();
    return minXferTime;
}

ElapsedTime HubbingSpecs::getTransferWindow (const CMobileElemConstraint& p_type) const
{
    const ProbabilityDistribution *prob = transferWindow.FindFit (p_type);
    ElapsedTime xferWindow = 60.0 * prob->getRandomValue();
    return xferWindow;
}
/*
Flight *HubbingSpecs::getDepartingFlight (const CMobileElemConstraint& p_type, int p_ndx) const
{
    FlightTypeDistribution *prob;
    prob = (FlightTypeDistribution *)transferDatabase.FindFit (p_type);

    if (prob == NULL)
        return getRandomDeparture (p_type, p_ndx);


	assert( m_pInTerm );
    Flight *aFlight = m_pInTerm->flightSchedule->getItem (p_ndx);
    ElapsedTime startTime = aFlight->getArrTime() + getMinTransferTime (p_type);
    ElapsedTime endTime = startTime + getTransferWindow (p_type);

    const FlightConstraint *destType;
    Flight *destFlight;

    // try a random series
    for (int i = 0; i < prob->getCount(); i++)
    {
        destType = prob->getDestinationGroup (RANDOM);
		  destFlight = m_pInTerm->flightSchedule->getDeparture
				(startTime, endTime, *destType);
        if (destFlight) //Jan98 don't know why negative :  (!destFlight)
            return destFlight;

    }
    // try a sequential series
    for (i = 0; i < prob->getCount(); i++)
    {
        destType = prob->getDestinationGroup (SEQUENTIAL);
        destFlight = m_pInTerm->flightSchedule->getDeparture
            (startTime, endTime, *destType);
        if (destFlight)
            return destFlight;
    }
    return NULL;
}
*/
Flight *HubbingSpecs::getRandomDeparture (const CMobileElemConstraint& p_type, int p_ndx, FlightSchedule* _pFlightSchedule ) const
{
	assert( m_pInTerm );
	Flight *aFlight = _pFlightSchedule->getItem (p_ndx);
	ElapsedTime startTime = aFlight->getArrTime() + getMinTransferTime (p_type);
	ElapsedTime endTime = startTime + getTransferWindow (p_type);
	return _pFlightSchedule->getRandomDeparture (startTime, endTime, p_ndx);
}

// delete all references to the passed index at p_level
void HubbingSpecs::deletePaxType (int p_level, int p_index)
{
	 transiting.deletePaxType (p_level, p_index);
	 transferring.deletePaxType (p_level, p_index);
	 minTransferTime.deletePaxType (p_level, p_index);
	 transferWindow.deletePaxType (p_level, p_index);
	 transferDatabase.deletePaxType (p_level, p_index);
}


//Jan98 : to protect the sum of percentage values of all flight departing
// from not equal to 100, it needs to retrieve "transferDatabase" to check
// total percent
CMobileElemConstraintDatabase* HubbingSpecs::getPassengerConstraintDatabase(int i_nx)
{
	//if (i_nx == 4)
		return getTransferDatabase ();
}



void HubbingSpecs::ReadVersion10( ArctermFile& p_file )
{
	assert( m_pInTerm );
	transiting.readDatabase (p_file, "TRANSITING", m_pInTerm);
    transferring.readDatabase (p_file, "TRANSFERRING", m_pInTerm);
    minTransferTime.readDatabase (p_file, "MIN_TRANSFER_TIME", m_pInTerm);
    transferWindow.readDatabase (p_file, "TRANSFER_WINDOW", m_pInTerm);
    p_file.seekToRecord ("Arrival", 0);

    CPassengerConstraint *aConst;
    ConstraintEntry *anEntry;
    FlightTypeDistribution *destinations;

    while (p_file.getLine())
    {
        aConst = new CPassengerConstraint(m_pInTerm);
        aConst->readConstraint (p_file);

        destinations = new FlightTypeDistribution;
        destinations->readVersion1 (p_file, m_pInTerm );

        anEntry = new ConstraintEntry;
		  anEntry->initialize (aConst, destinations);
		  transferDatabase.addEntry (anEntry);
    }
    wipeIntrinsic (transiting);
    wipeIntrinsic (transferring);
    wipeIntrinsic (minTransferTime);
    wipeIntrinsic (transferWindow);
    wipeIntrinsic (transferDatabase);
}

