// conveyor.h: interface for the Conveyor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVEYOR_H__074379D4_C2BA_40CB_B751_ED87D38170BB__INCLUDED_)
#define AFX_CONVEYOR_H__074379D4_C2BA_40CB_B751_ED87D38170BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCESS.H"
#include "inputs\Pipe.h"
#include <CommonData/ExtraProcProp.h>

class MiscConveyorData;
#define PUSHER_RELEASED_BY_FULL			1
#define PUSHER_RELEASED_BY_NEXT_AVAIL	2
#define PUSHER_RELEASED_BY_MAXTIME		4
#define PUSHER_RELEASED_BY_SCHD_PICK	8

class Conveyor : public Processor, public ConveyorProp
{
public:

private:
	Conveyor* m_pPerformer;//!!!!!!!!!!!!!!,bad code
	bool m_bNeedSyn;
	Path m_realPath;
	
protected:
	MobileElementList m_approaching;	// approaching element 	

public:
	Conveyor();
	virtual ~Conveyor();
public:
	// Returns ProcessorClassType corresponding to proc type.
    virtual int getProcessorType (void) const { return ConveyorProc; };

	virtual ExtraProcProp* GetExtraProp() { return this; }

    //Returns processor name. It will be override by subclasses.
    //Without special specific, define a processor name value as "Point". 
    virtual const char *getProcessorName (void) const { return "Conveyor"; };

	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;

	virtual void initServiceLocation (int pathCount, const Point *pointList);

	void CalculateTheBisectLine();

	//get all region the conveyor covered, it consisted of a list of pollygon.
	void GetCoveredRegion( POLLYGONVECTOR& _regions ) const;

	void SetWidth( DistanceUnit _dWidth ) ;
	DistanceUnit GetWidth()const { return m_dWidth;	};

	void ConstructPerformer( const MiscConveyorData * _pMiscData );

	SUBCONVEYORTYPE GetSubConveyorType()const { return m_enConveyorSubType;	};	
			
	virtual bool CopyOtherData(Processor* _pDestProc);

	virtual HoldingArea* GetRandomHoldingArea(void);
public:
	virtual void InitData( const MiscConveyorData * _pMiscData );
	virtual void beginService (Person *aPerson, ElapsedTime curTime);
	
	// Adds Person to the processor's occupants list.
    virtual void addPerson (Person *aPerson);

	// Removes Person from the processor's occupants list.
	virtual void removePerson (const Person *aPerson);

	// check if the conveyor is vacant
	virtual bool isVacant( Person* pPerson  = NULL ) const;
	virtual bool isExceedProcCapacity( void ) const;
	virtual int GetEmptySlotCount( void ) const;
	virtual void LogDebugInfo();


	// notify conveyor that one of its dest processor is available now
	virtual bool destProcAvailable ( const ElapsedTime& time );

	//It sets Person's next destination based on state & processor data
    virtual void getNextLocation (Person *aPerson);

	// if need to wake up the processor
	bool ifNeedToNotice( void ) const;

	virtual void RemoveHeadPerson(const ElapsedTime& time);
	
	// get the release person' time
	ElapsedTime getReleasePersonTime( void ) const;

	virtual long GetStopReason()const;

	virtual void writeLog (void);
	virtual void sortLog  ( void );
    virtual void flushLog (void);

	virtual void flushPersonLog(const ElapsedTime& _time);

	// Sorter functions. [5/18/2004]
	void InitSorter();
	Processor* GetSorterExitProcs(int _nIndex);
	int GetSorterEntryProcs(const ProcessorID& _procsID);
	bool IsSorterEntryVacant(int _nEntryProcsIndex);
	void SetLinkedSorterStep(const ElapsedTime& _time);
	bool CanAcceptPersonToSorter(const ProcessorID& _entryProcsID, const ElapsedTime& _time) const;

	virtual Processor* CreateNewProc();
	Conveyor* GetPerformer() { return m_pPerformer;}
	MobileElementList& GetApproaching(){return m_approaching;}

	const Path* serviceLocationExceedTwoFloorPath(void);

	//conveyor processor path state is changed
	virtual void synFlag();

	int clearPerson( const ElapsedTime& _time );
protected:
	virtual long CaculateConveyorCapacity();
	virtual long getLoad()const;
	
private:
	bool IsCrossOver( Point _pt11, Point _pt12, Point _pt21, Point _pt22 );

private:
	virtual void addPersonIntoApproachList( Person *_pPerson );
	virtual void addPersonIntoOccupiedList( Person *_pPerson );
	virtual void removePersonFromOccupiedList(const Person *_pPerson );
	virtual void removePersonFromApproachList(const Person* _pPerson);
	virtual void getDestinationLocation( Person* aPerson );
	virtual bool isExceedConveyorCapacity( void ) const;
	virtual bool _ifNeedToNotice( void ) const;
	virtual ElapsedTime _getReleasePersonTime( void ) const;		

	// check to see if there is at least one dest processor is available now
	//virtual bool TryToSetNextAvailableProcessor( Person * _pPerson )const;
};

#endif // !defined(AFX_CONVEYOR_H__074379D4_C2BA_40CB_B751_ED87D38170BB__INCLUDED_)
