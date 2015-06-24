#if !defined(AREAPORTALS_H)
#define AREAPORTALS_H

// include
#include <CommonData/AreaPortalBase.h>
#include "common\dataset.h"
#include "common\ARCVector.h"
#include "common\pollygon.h"
#include "areaintersectpoint.h"
#include "AreaDensityOfPaxtype.h"

// declare
class Person;
class Terminal;
class CPaxFlowConvertor;
class CAllPaxTypeFlow;
class ProcessorID;

//////////////////////////////////////////////////////////////////////////
// class CPortal
//typedef CNamedPointList CPortal;
class CPortal : public CPortalBase
{
public:
	CPortal();
	~CPortal();

};

typedef std::vector<CPortal*> CPortalList;
class CPortals : public DataSet {
public:
	CPortals();
	virtual ~CPortals();
	CPortalList m_vPortals;

	CPortal* FindPortalByGuid(const CGuid& guid) const;

    virtual void clear ();

	virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;

    virtual const char *getTitle(void) const { return "Portals Data"; }
    virtual const char *getHeaders (void) const{  return "Name,Color,Floor,Points,,,"; }
};

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// class CArea
//typedef CNamedPointList CArea;
class CArea: public CAreaBase
{
private:
	Pollygon m_Pollygon;		// change this area to a pollygon 
	double	 m_fZoneArea;		// zone area
	int		 m_iCurPersonCount;	// cur person count;
	CMaxCountOfPaxType m_maxCoutOfPaxTYpe; 
	
public:
	CArea();
	~CArea();

	void GetAreaCoveredRegion(Pollygon& region) { region = m_Pollygon; };

	// init data before running engine
	bool InitDataBeforeEngine( void );

	// get zone Area
	double getZoneArea( void ) const;

	// get the all intersect point
	bool getAllIntersectPoint( const Point& _ptFrom, const Point& _ptTo, 
						       INTERSECTPOINTLIST& _instersectPointList ) const;

	// test a point if in the area
	int containsPoint( const Point& _ptTest ) const;

	// check the area if is full
	bool isFull( const Person* _person, const ProcessorID& _destID ) const;

	// a person entry the area
	int personEntryArea( const Person* _person, const ElapsedTime& _entryTime );

	// a person leave the area
	int personLeaveArea( const Person* _person, const ElapsedTime& _leaveTime );

	// get point by index
	Point getPointByIndex( int _idx ) const;

	// get point count
	int getPointCount( void ) const { return m_Pollygon.getCount();	}

	// insert max count item
	void insertMaxcountItem( const CAreaDensityKey& _key, double _dDensity );

private:
	// add start & end point if need
	int addStartEndPtIfNeed( const Point& _ptFrom, const Point& _ptTo, 
							INTERSECTPOINTLIST& _instersectPointList ) const;
};

typedef std::vector<CArea*> CAreaList;
class CAreas : public DataSet 
{
public:
	CAreas();
	virtual ~CAreas();
	CAreaList m_vAreas;

	CArea* FindAreaByGuid(const CGuid& guid) const;

    virtual void clear();

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Areas Data"; }
    virtual const char *getHeaders (void) const{  return "Name,Color,Floor,Points,,,"; }

	CArea* getAreaByName( const CString& _strArea );
	CArea* getAreaByIndex( int _idx );
	//////////////////////////////////////////////////////////////////////////
	// function for engine( avoid density )
	//////////////////////////////////////////////////////////////////////////
	// init data before running engine
	bool InitDataBeforeEngine( CPaxFlowConvertor* _pConvertor, CAllPaxTypeFlow* _pAllFlow );

	// get the all intersect point
	// get start time
	bool getAllIntersectPoint1( const Point& _ptStart, const Point& _ptFrom, const Point& _ptTo, 
							    const ElapsedTime& _tStartTime, double _fSpeed,
							    INTERSECTPOINTLIST& _instersectPointList ) const;
	
	// get end time
	bool getAllIntersectPoint2( const Point& _ptStart, const Point& _ptFrom, const Point& _ptTo, 
								const ElapsedTime& _tEndTime, double _fSpeed,
								INTERSECTPOINTLIST& _instersectPointList ) const;

	// person birth
	void handlePersonBirth( const Person* _person, const Point& _location, const ElapsedTime& _birthTime );

	// person death
	void handlePersonDeath( const Person* _person, const Point& _location, const ElapsedTime& _deathTime );
};

static void sortIntersectList( const Point& _ptFrom, const Point& _ptTo, INTERSECTPOINTLIST& _intersectPointList );
#endif // !defined(AREAPORTALS_H);