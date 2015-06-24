// HubbingDatabase.h: interface for the CHubbingDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUBBINGDATABASE_H__61F9CC0C_19F7_489D_A10B_089206EB3D59__INCLUDED_)
#define AFX_HUBBINGDATABASE_H__61F9CC0C_19F7_489D_A10B_089206EB3D59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HubbingData.h"
#include "common\containr.h"
#include "common\dataset.h"

class CHubbingDatabase : public SortedContainer<CHubbingData>, public DataSet  
{

public:
	CHubbingDatabase();
	virtual ~CHubbingDatabase();

	// add a default CHubbingData with the passed passenger type
	CHubbingData* AddDefaultEntry( const CPassengerConstraint& _paxConstr );

	// used by DataSet for set up the default value if there is no file
    virtual void initDefaultValues();

	// 
    virtual void clear (void);

    virtual void readData (ArctermFile& p_file);
    virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Data describing operations of transiting and transferring passengers"; }
    virtual const char *getHeaders (void) const
        { return "Variable,Pax Type,Units,Distribution,Parameters"; }


	float getTransitPercent (const CPassengerConstraint& p_type) const;
	float getTransferPercent (const CPassengerConstraint& p_type) const;
	ElapsedTime getMinTransferTime (const CPassengerConstraint& p_type ) const;
	ElapsedTime getTransferWindow (const CPassengerConstraint& p_type) const;
    Flight* getDepartingFlight (const CPassengerConstraint& p_type, int p_ndx, FlightSchedule* _pFlightSchedule) const;
    Flight *getRandomDeparture (const CPassengerConstraint& p_type, int p_ndx, FlightSchedule* _pFlightSchedule) const;


    // delete all references to the passed index at p_level
    void deletePaxType( int p_level, int p_index );
	//by CPassengerConstraint and FlightConstraint ,get the time
	ElapsedTime getMinTransferTime (const CPassengerConstraint& p_type ,const FlightConstraint& flight) const;
	ElapsedTime getTransferWindow (const CPassengerConstraint& p_type, const FlightConstraint& flight) const;

protected:

	// fill the empty item with default value
	void FillEmptyWithDefault();


	// find the item with same paxtype
	int FindEqual( const CPassengerConstraint _aPaxConst );

	// looking for the closest match, return -1 for not found.
	int FindBestMatch( const CPassengerConstraint& _aPaxConst ) const;

	// removes intrinsic type from all constraints
	void WipeIntrinsic();

	//read the HubData from the file of _file
	bool ReadTransiting( ArctermFile& _file, CHubbingData* _pHubbingData );
	bool ReadTransfering( ArctermFile& _file, CHubbingData* _pHubbingData );
	bool ReadTransferDest( ArctermFile& _file, CHubbingData* _pHubbingData );
	bool ReadMinTransferTime( ArctermFile& _file, CHubbingData* _pHubbingData, int nIndex );
	bool ReadTransferWindow( ArctermFile& _file, CHubbingData* _pHubbingData, int nIndex );

};

#endif // !defined(AFX_HUBBINGDATABASE_H__61F9CC0C_19F7_489D_A10B_089206EB3D59__INCLUDED_)
