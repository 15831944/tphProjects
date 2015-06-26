/*****
*
*   Class       PaxLogEntry
*   Author      Lincoln Fellingham
*   Date        January 1994
*   Purpose     Binary passenger logs, store information on each pax in
*               in one file, and a complete listing of each flight's
*               activities in a second file that is indexed by the first
*
*               Note that the EventLog must be defined here and not in
*               the base class despite the identical functionality
*               because the class is static, and the file is not common
*               to all LogEntry classes
*
*   Revisions   Feb 15, 1996 - LF - subclass from LogItem class, used to
*               standardize interface logs and event lists
*
*****/

#ifndef PASSENGER_LOG_ENTRY_H
#define PASSENGER_LOG_ENTRY_H

#include <assert.h>
#include "common\fsstream.h"
#include "results\logitem.h"
#include "results\eventlog.h"
#include "common\term_bin.h"
#include "common\template.h"
#include "common\elaptime.h"

class TempMobileEventLog;

struct RotateAngle 
{
	double x;
	double y;
	double z;
	double w;
};
 

class MobLogEntry : public TerminalLogItem<MobDescStruct,MobEventStruct>
{
protected:

public:
	
    MobLogEntry();

    virtual ~MobLogEntry () {clearLog();};

    // accessors and modifiers
    virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;


    /*****
    *
    *   PaxEntry specific accessors & modifiers
    *
    *****/
	void SetMissFlightFlg( bool missflight) { item.bMissFlightDeath = missflight; }
	bool IsMissFlight() { return item.bMissFlightDeath; }

    long getID (void) const { return item.id; }
    int isTurnaround (void) const
        { return item.arrFlight != -1 && item.depFlight != -1; }
    int isGreeter (void) const { return item.intrinsic == 6; }
    int isArrival (void) const { return item.depFlight == -1; }
    int isArriving (void) const { return item.arrFlight != -1; }
    int isDeparture (void) const { return item.arrFlight == -1; }
    int isDeparting (void) const { return item.depFlight != -1; }
    int getIntrinsic (void) const { return item.intrinsic; }
    ElapsedTime getEntryTime (void)const;
	ElapsedTime getEnplanTime()const;
    ElapsedTime getExitTime (void)const;
	ElapsedTime getTerminalEntryTime(void)const;
	ElapsedTime getTerminalEndTime(void)const;
    int getGroupSize (void) const { return item.groupSize; }
    int getGroupMember (void) const { return item.groupMember; }
    int getBagCount (void) const { return item.bagCount; }
    int getCartCount (void) const { return item.cartCount; }
    int getArrivingFlight (void) const { return item.arrFlight; }

	void SetRealDepartureTime(long lTime) {item.flightRealDepartureTime = (long)lTime;}
	ElapsedTime GetRealDepartureTime()const;

	void SetMissFlightTime(long lTime) {item.missFlightTime = (long)lTime;}
	ElapsedTime GetMissFlightTime()const;
	//----------------------------------------------------------------------------------
	//Summary:
	//		passenger take carryon count
	//----------------------------------------------------------------------------------
	int GetCarryonCount(void)const {return item.carryonCount;}
	void SetCarryonCount(int nSize){item.carryonCount += nSize;}
	void ReleaseCarryon(){item.carryonCount = 0;}
	//----------------------------------------------------------------------------------

	// Get Dep id for transfer
    int getFlightIndex (void) const
        { return (item.depFlight == -1)? item.arrFlight: item.depFlight; }
	
	//get angle
 	const RotateAngle getAngle();

	long GetFileIndex()const {return item.fileID;}
    long getOwnerID (void) const { return item.ownerID; }
    void getPaxType (int *p_type) const;
    long getIndex (void) const { return item.indexNum; }
    int getDepFlight (void) const { return item.depFlight; }
    int getArrFlight (void) const { return item.arrFlight; }
	float getSpeed() const { return item.speed; }
//	int getNopaxAttached() const {return item.nopaxAttached;}
	int GetMobileType(){ return (int)item.mobileType; }
	bool getOwnStart(){ return item.ownStart==TRUE? true : false; }
	bool getEachPaxOwnNopax() { return item.eachPaxOwnNoPax==TRUE ? true : false; }
		
	//const MobDescStruct GetMobDescStruct() { return item; }
	const MobDescStruct& GetMobDesc() const { return item; }
	MobDescStruct& GetMobDesc(){ return item; }

    void setID (long p_id) { item.id = p_id; }
	void setFileIndex(long ldx) {item.fileID = ldx;}
    void setEntryTime (ElapsedTime p_time) { item.startTime = (long)p_time; }
	void setTerminalEntryTime(ElapsedTime p_time){item.terminalStartTime = (long)p_time;}
	void setTerminalEndTime(ElapsedTime p_time) {item.terminalEndTime = (long)p_time;}
	void setEnplanTime(ElapsedTime p_time);
    void setExitTime (ElapsedTime p_time) { item.endTime = (long)p_time; }
    void setArrFlight (int p_ndx) { item.arrFlight = (short)p_ndx; }
    void setDepFlight (int p_ndx) { item.depFlight = (short)p_ndx; }
    void setGroupSize (int p_num) { item.groupSize = (char)p_num; }
    void setGroupMember (int p_num) { item.groupMember = (char)p_num; }
    void setBagCount (int p_num) { item.bagCount = (char)p_num; }
    void setOwner (long p_id) { item.ownerID = p_id; }
    void setIntrinsic (int p_type) { item.intrinsic = (char) p_type; }
    void setPaxType (const int *p_type);
    void setCartCount (int p_num) { item.cartCount = (char)p_num; }
    void incPax (void) { item.id++; item.groupMember++; item.indexNum++; }
	void setIndex( long _lIndex ){ item.indexNum = _lIndex;	}
	void setSpeed( float _fSpeed ){ item.speed = _fSpeed; }
//	void setNopaxAttached(int p_num) { item.nopaxAttached = (char)p_num;}
	void setOwnStart( bool _bHasOwnStart ){ item.ownStart = _bHasOwnStart ? TRUE : FALSE; }
	void setEachPaxOwnNopax( bool _flag ) { item.eachPaxOwnNoPax = _flag ? TRUE : FALSE; }
    // returns TRUE if Mobile Element was alive between time t1 & t2
    int alive (ElapsedTime t1, ElapsedTime t2)
        { return (getEntryTime() < t2 && t1 < getExitTime()); }
	int alive (ElapsedTime p_time)
		{ return (getEntryTime() < p_time && p_time < getExitTime()); }

    int operator == (const MobLogEntry& p_entry) const
        { return item.startTime == p_entry.item.startTime; }
    int operator < (const MobLogEntry& p_entry) const
        { return item.startTime < p_entry.item.startTime; }

	void SetmobileType( int _nMobType ){ item.mobileType = (char)_nMobType; }

 	void SetAngle(const RotateAngle& angle);

//	virtual void setEventList (const MobEventStruct *p_item, int p_count);
//	virtual void setEventList( const std::vector<MobEventStruct>& p_item );
	virtual void saveEventsToLog();
	virtual void setToFirst (void);
	virtual MobEventStruct getNextEvent (void);
	virtual MobEventStruct getEvent (long p_ndx);
	virtual void updateEvent( MobEventStruct& _newEventData, long _lIdx );
	virtual void loadEvents (void);

	const std::vector<MobEventStruct*>& getMobEventList( void ) const;
	std::vector<MobEventStruct*>& getMobEventList( void );
	virtual void addEvent (const MobEventStruct &p_item);
	virtual int getLog (MobEventStruct* &p_log);
	virtual void clearLog (void);
	virtual void initLog (void);
    virtual long getCurrentCount (void) const;

	MobEventStruct* getPreEvent();
	void setTempLogFile(TempMobileEventLog * pTempLog){m_pMobTempLogFile = pTempLog;}
	
	void setBulkIndex(int id){ item.nBulkIndex = id; }
	int getBulkIndex()const{return item.nBulkIndex; }
	void setBulkFreqIndex(long idx){ item.nFreqIndex = idx; }
	long getBulkFreqIndex()const{ return item.nFreqIndex; }

	//BirthExceptionCode for detail
	void setBirthExceptionCode(short bCode){ item.bBulkProfileCapacity = bCode;}
	short getBirthExceptionCode()const {return item.bBulkProfileCapacity;}
private:
	std::vector<MobEventStruct*> m_vPaxEvenList;

protected:
	MobEventStruct m_preEvent;
	TempMobileEventLog *m_pMobTempLogFile;
};

#endif
