// IntegratedStation.h: interface for the IntegratedStation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTEGRATEDSTATION_H__69A6D73F_A501_458F_AD14_9A14BF0D8BB0__INCLUDED_)
#define AFX_INTEGRATEDSTATION_H__69A6D73F_A501_458F_AD14_9A14BF0D8BB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Car.h"
#include "PROCESS.H"

#include <CommonData\ExtraProcProp.h>
#include "..\inputs\StationLayout.h"

class MovingTrain;
class IntegratedStation : public Processor, public IntegratedStationProp
{ 
private:
	CAR m_cars;
	HoldingArea* m_pPreBoardingArea;
	MovingTrain* m_pTrain;
	//all other station which share preboarding with me
	std::vector<IntegratedStation*>m_vPreBoardingShareWithStation;

	CStationLayout m_layout;

protected:
	//write IntegratedStation's special info
	virtual int writeSpecialProcessor(ArctermFile& procFile) const;
	//read special info from files
	virtual int readSpecialProcessor(ArctermFile& procFile);

		virtual int readSpecialField(ArctermFile& procFile) ;
	virtual int writeSpecialField(ArctermFile& procFile) const ;

	
public:

	IntegratedStation();
	IntegratedStation ( const IntegratedStation& _anotherStation);
	virtual ~IntegratedStation();

	IntegratedStation& operator =  ( const IntegratedStation& _anotherStation);

	void InitLocalData();

	void UpdateTempInfo( CStationLayout* _pTempLayout);
	

	int GetCarCount()const;
	virtual Path* GetPreBoardAreaInConstrain();
	virtual Path* GetPreBoardAreaServPath();
	virtual Path* GetCarAreaServPath(int icar);
	virtual int GetCarEntryDoorCount(int icar);
	virtual int GetCarExitDoorCount(int icar);
	virtual Path* GetCarEntryDoorServPath(int icar,int idoor);
	virtual Path* GetCarExitDoorServPath(int icar,int idoor);


	CCar* GetCar( int _nIdx );;
	void DeleteCar( int _nIdx );
	
	
	HoldingArea* GetPreBoardingArea() { return m_pPreBoardingArea;}

	//Returns processor type.
    virtual int getProcessorType (void) const { return IntegratedStationProc; };

    //Returns processor name.
    virtual const char *getProcessorName (void) const { return "INTEGRATED_STATION"; };
	//check if the station definition is valid
	BOOL IsAValidStation();	

	void Rotate (DistanceUnit degrees);
	void DoOffset( DistanceUnit _x,DistanceUnit _y);
	void Mirror( const Path* _p );

	// Clean all sim engine related data, for the new sim process
	virtual void CleanData();

    // Kills all passengers stranded in queue at end of simulation.
    // Returns a count of the stranded passengers.
    virtual int clearQueue (ElapsedTime curTime);

    // Removes Person from the processor's occupants list.
	virtual void removePerson (const Person *aPerson);

	std::vector<IntegratedStation*>& GetSharePreboardingAreaStation() { return m_vPreBoardingShareWithStation;	}
	virtual void initSpecificMisc (const MiscData *miscData) ;

	void SetTrain(MovingTrain* pTrain);
	MovingTrain* GetTrain() const;
	void ClearStationTrain(MovingTrain* pTrain);

	CStationLayout *GetLayout();
public:
	virtual void UpdateMinMax();
	virtual ExtraProcProp* GetExtraProp() { return this; }
	virtual CStationLayout* GetStationLayout();
};

#endif // !defined(AFX_INTEGRATEDSTATION_H__69A6D73F_A501_458F_AD14_9A14BF0D8BB0__INCLUDED_)
