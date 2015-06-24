#include "stdafx.h"
#include "AreasPortals.h"
#include "paxflowconvertor.h"
#include "allpaxtypeflow.h"
#include "singlepaxtypeflow.h"
#include <CommonData/procid.h>
#include "..\common\dataset.h"
#include "..\common\exeption.h"
#include "..\common\point.h"
#include "..\common\line.h"
#include "..\engine\person.h"
#include "../Common/ARCTracker.h"
//#include "..\engine\terminal.h"
#include <boost/bind.hpp>

#include <iterator>
#define MAX_POINTS 1024

CAreas::CAreas() : DataSet( AreasFile )
{
}

CAreas::~CAreas()
{
	clear();
}


CPortals::CPortals() : DataSet( PortalsFile )
{
}

CPortals::~CPortals()
{
	clear();
}



void CPortals::clear()
{
	int nCount = m_vPortals.size();
	for( int i=0; i<nCount; i++ )
		delete m_vPortals[i];

	m_vPortals.clear();
}


void CPortals::readData(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
	    char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
	        throw new StringError ("Invalid Portal Name");

		long color;
		p_file.getInteger(color);

		int floor;
		p_file.getInteger( floor );

	    Point aPoint1;
		Point aPoint2;

	    if( !p_file.getPoint( aPoint1 ) )
	        throw new StringError ("Missing Portal Points");	

		p_file.getLine();
		if( p_file.isBlank() || p_file.isNewEntryLine() )
            throw new StringError( "Data Error" );

        // skip name, color and floor fields
        p_file.skipField (3);

        if( !p_file.getPoint (aPoint2))
	        throw new StringError ("Missing Portal Points");	

		CPortal* pPortal = new CPortal;
		pPortal->name = name;
		pPortal->color = (COLORREF) color;
		pPortal->floor = floor;
		pPortal->points.reserve(2);
		pPortal->points.push_back( ARCPoint2( aPoint1.getX(), aPoint1.getY() ) );
		pPortal->points.push_back( ARCPoint2( aPoint2.getX(), aPoint2.getY() ) );

		m_vPortals.push_back( pPortal );
		p_file.getLine();
	}
}


void CPortals::writeData(ArctermFile& p_file) const
{
	int nCount = m_vPortals.size();
    for( int i = 0; i < nCount; i++ )
    {
		CPortal* pPortal = m_vPortals[i];

		p_file.writeField( pPortal->name );

		p_file.writeInt((long) pPortal->color);

		p_file.writeInt( pPortal->floor );

		Point aPoint1( pPortal->points[0][VX], pPortal->points[0][VY], 0 );

		p_file.writePoint( aPoint1 );		

        p_file.writeLine();
		p_file.writeBlankFields(2);

		Point aPoint2( pPortal->points[1][VX], pPortal->points[1][VY], 0 );

		p_file.writePoint( aPoint2 );		

        p_file.writeLine();
    }
}

CPortal* CPortals::FindPortalByGuid( const CGuid& guid ) const
{
	CPortalList::const_iterator ite = std::find_if(m_vPortals.begin(), m_vPortals.end(), boost::BOOST_BIND(&CBaseObject::getGuid, _1) == guid);
	return m_vPortals.end() != ite ? *ite : NULL;
}
void CAreas::clear()
{
	int nCount = m_vAreas.size();
	for( int i=0; i<nCount; i++ )
		delete m_vAreas[i];

	m_vAreas.clear();
}



void CAreas::readData(ArctermFile& p_file)
{
	p_file.getLine();

    while (!p_file.isBlank ())
	{
	    char name[256];
		if( p_file.isBlankField() || !p_file.getShortField (name, 256) )
	        throw new StringError ("Invalid Area Name");

		long color;
		p_file.getInteger(color);

		int floor;
		p_file.getInteger( floor );

	    Point aPoint;

	    if( !p_file.getPoint( aPoint ) )
	        throw new StringError ("Missing Portal Points");	

		CArea* pArea = new CArea;
		pArea->name = name;
		pArea->color = (COLORREF) color;
		pArea->floor = floor;
		pArea->points.push_back( ARCPoint2( aPoint.getX(), aPoint.getY() ) );

		// make sure point limits are not exceeded
		for( int i = 1; p_file.getLine() && !p_file.isNewEntryLine(); i++ )
		{
			if (i + 1 >= MAX_POINTS)
				throw new StringError ("maximum point count exceeded by ");

			// skip name, color and floor fields
			p_file.skipField (3);

			if( p_file.getPoint (aPoint))
			{
				pArea->points.push_back( ARCPoint2( aPoint.getX(), aPoint.getY() ) );
			}
		}
		
		m_vAreas.push_back( pArea );
	}
}


void CAreas::writeData(ArctermFile& p_file) const
{
	int nCount = m_vAreas.size();
    for( int i = 0; i < nCount; i++ )
    {
		CArea* pArea = m_vAreas[i];

		p_file.writeField( pArea->name );
		p_file.writeInt((long) pArea->color);
		p_file.writeInt( pArea->floor );
		int nPtCount = pArea->points.size();
		for( int m=0; m<nPtCount; m++ )
		{
			if( m>0 )
				p_file.writeBlankFields(2);

			Point aPoint( pArea->points[m][VX], pArea->points[m][VY], 0 );

			p_file.writePoint( aPoint );		
	        p_file.writeLine();
		}
    }
}

CArea* CAreas::getAreaByName( const CString& _strArea )
{
	for( int i=0; i< static_cast<int>(m_vAreas.size()); i++ )
	{
		if( m_vAreas[i]->name == _strArea )
			return m_vAreas[i];
	}

	return NULL;
}


CArea* CAreas::getAreaByIndex( int _idx )
{
	assert( _idx>=0 && _idx < static_cast<int>(m_vAreas.size()) );
	return m_vAreas[_idx];
}

//////////////////////////////////////////////////////////////////////////
// function for engine( avoid density )
//////////////////////////////////////////////////////////////////////////
// init data before running engine
bool CAreas::InitDataBeforeEngine( CPaxFlowConvertor* _pConvertor, CAllPaxTypeFlow* _pAllFlow )
{
	PLACE_METHOD_TRACK_STRING();
	for( int i=0; i< static_cast<int>(m_vAreas.size()); i++ )
	{
		m_vAreas.at(i)->InitDataBeforeEngine();
	}
	
	// parse pax flow to get max count of each pax 
	_pAllFlow->FromOldToDigraphStructure( *_pConvertor );

	int iFlowCount = _pAllFlow->GetSinglePaxTypeFlowCount();
	for(int ii=0; ii<iFlowCount; ++ii )
	{
		CSinglePaxTypeFlow* _pSingleFlow = _pAllFlow->GetSinglePaxTypeFlowAt( ii );
		assert( _pSingleFlow );
		const CMobileElemConstraint* _pConstrint = _pSingleFlow->GetPassengerConstrain();

#ifdef DEBUG
		//char _szBuf[128];
		CString szBuf;
		_pConstrint->screenPrint(szBuf, 0, 128 );
#endif
		int _iDestPairCount = _pSingleFlow->GetFlowPairCount();
		for( int k=0; k< _iDestPairCount; k++ )
		{
			const CProcessorDestinationList* _flowPair =  _pSingleFlow->GetFlowPairAt( k );

#ifdef DEBUG 
		CString _strProcID = _flowPair->GetProcID().GetIDString();
#endif
			int _iDestCount = _flowPair->GetDestCount();
			for( int m = 0; m < _iDestCount; m++ )
			{
				CFlowDestination& _info = const_cast<CFlowDestination&>( _flowPair->GetDestProcAt(m) );

#ifdef DEBUG
		_strProcID = _info.GetProcID().GetIDString();
#endif
		
				for( int n=0; n<static_cast<int>(_info.GetDensityArea().size()); n++ )
				{
					CString _strAreaName = _info.GetDensityArea()[n];
					CArea* _pArea = getAreaByName( _strAreaName );
					if( _pArea )
					{
						_pArea->insertMaxcountItem( CAreaDensityKey( *_pConstrint, _info.GetProcID() ) , _info.GetDensityOfArea() );
					}
				}
			}
		}
	}
	
	return true;
}

// get the all intersect point
bool CAreas::getAllIntersectPoint2( const Point& _ptStart,const Point& _ptFrom, const Point& _ptTo, 
								    const ElapsedTime& _tEndTime, double _fSpeed,
						            INTERSECTPOINTLIST& _instersectPointList ) const
{
	double _dDistance = _ptStart.distance( _ptTo );
	ElapsedTime _tMoveTime = ElapsedTime( _dDistance / _fSpeed );
	ElapsedTime _tBegineTime = _tEndTime - _tMoveTime;

	return getAllIntersectPoint1( _ptStart, _ptFrom, _ptTo, _tBegineTime, _fSpeed, _instersectPointList );
}

bool CAreas::getAllIntersectPoint1( const Point& _ptStart,const Point& _ptFrom, const Point& _ptTo, 
 								    const ElapsedTime& _tStartTime, double _fSpeed,
								    INTERSECTPOINTLIST& _instersectPointList ) const
{
	_instersectPointList.clear();
	
	if( _ptFrom.getZ() != _ptTo.getZ() )
		return false;
	
	int i;
	INTERSECTPOINTLIST _tmpIntersectPointList;
	for( i=0; i< static_cast<int>(m_vAreas.size()); i++ )
	{
		m_vAreas[i]->getAllIntersectPoint( _ptFrom, _ptTo, _tmpIntersectPointList );
	}
	
	if( _tmpIntersectPointList.size()==0 )
		return false;

	//// sort IntersectList
	sortIntersectList( _ptFrom, _ptTo, _tmpIntersectPointList );

	// merge repeat intersect point
	int _iCurIndex = 0;
	AreaIntersectPoint _curInstersectPoint = _tmpIntersectPointList[ _iCurIndex ];
	_curInstersectPoint.intersectTime = _tStartTime + ElapsedTime(_ptStart.distance( _curInstersectPoint.point ) / _fSpeed );
	
	for( i=1; i<static_cast<int>(_tmpIntersectPointList.size()); i++ )
	{
		if( !_tmpIntersectPointList[i].point.preciseCompare( _tmpIntersectPointList[_iCurIndex].point ) )
		{
			_instersectPointList.push_back( _curInstersectPoint );
			
			_iCurIndex = i;
			_curInstersectPoint = _tmpIntersectPointList[ _iCurIndex ];
			_curInstersectPoint.intersectTime = _tStartTime + ElapsedTime( _ptStart.distance( _curInstersectPoint.point ) / _fSpeed );		
		}
		else
		{
			std::copy( _tmpIntersectPointList[i].intersectInfo.begin(), _tmpIntersectPointList[i].intersectInfo.end(),
						std::back_inserter( _curInstersectPoint.intersectInfo ) );
		}
	}	
	_instersectPointList.push_back( _curInstersectPoint );
	return true;
}

// person birth
void CAreas::handlePersonBirth( const Person* _person, const Point& _location, const ElapsedTime& _birthTime)
{
	for( int i=0; i< static_cast<int>(m_vAreas.size()); i++ )
	{
		CArea* _pArea = m_vAreas[i];
		if( _pArea->containsPoint( _location) )
			_pArea->personEntryArea( _person, _birthTime );
	}
}

// person death
void CAreas::handlePersonDeath( const Person* _person, const Point& _location, const ElapsedTime& _deathTime )
{
	for( int i=0; i< static_cast<int>(m_vAreas.size()); i++ )
	{
		CArea* _pArea = m_vAreas[i];
		if( _pArea->containsPoint( _location) )
			_pArea->personLeaveArea( _person, _deathTime );
	}
}

CArea* CAreas::FindAreaByGuid( const CGuid& guid ) const
{
	CAreaList::const_iterator ite = std::find_if(m_vAreas.begin(), m_vAreas.end(), boost::BOOST_BIND(&CBaseObject::getGuid, _1) == guid);
	return m_vAreas.end() != ite ? *ite : NULL;
}

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// class CArea
CArea::CArea():m_fZoneArea( -1.0 ), m_iCurPersonCount( 0 )
{

}

CArea::~CArea()
{
	
}

// init data before running engine
bool CArea::InitDataBeforeEngine( void )
{
	// first init the pollygon
	int _iPointCount = points.size();
	if( _iPointCount<=0 )
		return false;

	Point* _pTmpPoint = new Point[ _iPointCount ];
	for( int i=0; i< _iPointCount; i++ )
		_pTmpPoint[i] = Point( points[i][VX], points[i][VY], floor * SCALE_FACTOR );

	m_Pollygon.init( _iPointCount, _pTmpPoint );
	delete []_pTmpPoint;
	
	// compute zone area
	m_fZoneArea = m_Pollygon.calculateArea();

	// init current person count
	m_iCurPersonCount = 0;

	return true;
}

// get zone Area
double CArea::getZoneArea( void ) const
{
	return m_fZoneArea;
}

// get the all intersect point
bool CArea::getAllIntersectPoint( const Point& _ptFrom, const Point& _ptTo, 
								  INTERSECTPOINTLIST& _instersectPointList ) const
{
	int numPoints = m_Pollygon.getCount();
	if( numPoints < 2 )
		return false;
	
	if( m_Pollygon.getPoint(0).getZ() != _ptFrom.getZ() )
		return false;

	Point		       _tmpPoint;
	INTERSECTPOINTLIST __tmpIntersectList;
	AreaIntersectPoint _tmpIntersectPoint;
	_tmpIntersectPoint.intersectInfo.push_back( INTERSECT_INFO() );
	
    Line testLine (_ptFrom, _ptTo);
    Line polySegment (m_Pollygon.getPoint(0), m_Pollygon.getPoint(numPoints-1) );
/*
#ifdef DEBUG
	testLine.init( Point( -100.0, 0.0, 0.0), Point( 100.0, 0.0, 0.0) );
	polySegment.init( Point( 0.0, 0.0, 0.0), Point( 0.0, 100.0, 0.0 ) );
	int _iRes = testLine.intersects( polySegment, _tmpPoint );
#endif //DEBUG
*/		
	// first get all intersect point
    // test closing segment
    if ( numPoints > 2 && testLine.intersects (polySegment, _tmpPoint ))
	{
		if( !_tmpPoint.preciseCompare( _ptTo ) && !_tmpPoint.preciseCompare( _ptFrom ) )
		{
			_tmpIntersectPoint.point = _tmpPoint;
			_tmpIntersectPoint.intersectInfo[0] = INTERSECT_INFO(STATE_UNKNOW,const_cast<CArea*>(this),numPoints-1);
			__tmpIntersectList.push_back( _tmpIntersectPoint );
		}
	}
		
    // test each segment for intersection
    for (int  i = 0; i < numPoints - 1; i++)
    {
        polySegment.init (m_Pollygon.getPoint(i), m_Pollygon.getPoint(i+1) );
        if (testLine.intersects (polySegment, _tmpPoint))
		{
			if( !_tmpPoint.preciseCompare( _ptTo ) && !_tmpPoint.preciseCompare( _ptFrom ))
			{
				_tmpIntersectPoint.point = _tmpPoint;
				_tmpIntersectPoint.intersectInfo[0] = INTERSECT_INFO(STATE_UNKNOW,const_cast<CArea*>(this),i);
				__tmpIntersectList.push_back( _tmpIntersectPoint );
			}
		}
    }

	// start & end point if need
	addStartEndPtIfNeed( _ptFrom, _ptTo, __tmpIntersectList );

	if( __tmpIntersectList.size() == 0 )
		return false;
	
	sortIntersectList( _ptFrom, _ptTo, __tmpIntersectList );

	// unique the list
	INTERSECTPOINTLIST::iterator _theNewEnd = std::unique( __tmpIntersectList.begin(), __tmpIntersectList.end() );
	__tmpIntersectList.erase( _theNewEnd, __tmpIntersectList.end() );
		
	// set each point's state
	int _iContain = m_Pollygon.containsExcludeEdge( _ptFrom );
	for(int ii=0; ii<static_cast<int>(__tmpIntersectList.size()); ii++ )
	{
		if( ii%2 == _iContain)
			__tmpIntersectList[ii].intersectInfo[0].state = ENTRY_AREA;
		else
			__tmpIntersectList[ii].intersectInfo[0].state = LEAVE_AREA;
	}

	// copy temp list to
	std::copy( __tmpIntersectList.begin(), __tmpIntersectList.end(), 
			   std::back_inserter( _instersectPointList) );
	return true;	
}

// add start & end point if need
int CArea::addStartEndPtIfNeed( const Point& _ptA, const Point& _ptB, 
						 INTERSECTPOINTLIST& _instersectPointList ) const
{
	AreaIntersectPoint _tmpPt;
	_tmpPt.intersectInfo.push_back( INTERSECT_INFO( ) );

	int _iSegA,_iSegB;
	bool _bAInArea, _bBInArea;
	bool _bAIsEdgePt,_bBIsEdgePt;

	_bAIsEdgePt = m_Pollygon.isEdgePoint( _ptA, _iSegA );
	_bBIsEdgePt = m_Pollygon.isEdgePoint( _ptB, _iSegB );

	if( !_bAIsEdgePt )
		_bAInArea = m_Pollygon.containsExcludeEdge( _ptA ) ? true : false;
	else
		_bAInArea = false;

	if( !_bBIsEdgePt )
		_bBInArea = m_Pollygon.containsExcludeEdge( _ptB ) ? true : false;
	else
		_bBInArea = false;

	int _iSize = _instersectPointList.size();
	if( _bAIsEdgePt && _bBInArea && _iSize== 0 ||
		_bAIsEdgePt && !_bBInArea &&(_iSize>0 && _iSize%2 ==1 ) )
	{
		//entry
		_tmpPt.point = _ptA;
		_tmpPt.intersectInfo[0] = INTERSECT_INFO( STATE_UNKNOW, const_cast<CArea*>(this), _iSegA );
		_instersectPointList.push_back( _tmpPt );
	}
	
	if( _bBIsEdgePt && !_bAInArea && _iSize>0 ||
		_bBIsEdgePt && _bAInArea )
	{
		//leave
		_tmpPt.point = _ptB;
		_tmpPt.intersectInfo[0] = INTERSECT_INFO( STATE_UNKNOW, const_cast<CArea*>(this), _iSegB );
		_instersectPointList.push_back( _tmpPt );
	}

	return 1;
}

// test a point if in the area
int CArea::containsPoint( const Point& _ptTest ) const
{
	return m_Pollygon.containsExcludeEdge( _ptTest );
}


// check the area if is full
bool CArea::isFull(  const Person* _person, const ProcessorID& _destID ) const
{
	CMaxCountOfPaxType::const_iterator _iter;
	for( _iter = m_maxCoutOfPaxTYpe.begin();  _iter!= m_maxCoutOfPaxTYpe.end(); ++_iter )
	{
		const CAreaDensityKey& _key = _iter->first;
		if( _key.fits( _person->getType(), _destID ) )
			return m_iCurPersonCount >= _iter->second;
	}
	
	return false;	// can not find fits item, then can entry the area
}

// a person entry the area
int CArea::personEntryArea( const Person* _person, const ElapsedTime& _entryTime )
{
	m_iCurPersonCount += _person->GetActiveTotalSize();
	return m_iCurPersonCount;
}

// a person leave the area
int CArea::personLeaveArea( const Person* _person, const ElapsedTime& _leaveTime )
{
	m_iCurPersonCount -= _person->GetActiveTotalSize();
	//m_iCurPersonCount = max( 0, m_iCurPersonCount );
	return m_iCurPersonCount;
}

// get point by index
Point CArea::getPointByIndex( int _idx ) const
{
	assert( _idx>=0 && _idx<m_Pollygon.getCount() );
	return m_Pollygon.getPoint( _idx );
}

// insert max count item
void CArea::insertMaxcountItem( const CAreaDensityKey& _key, double _dDensity )
{
	CMaxCountOfPaxType::const_iterator _iter = m_maxCoutOfPaxTYpe.find( _key );
	if( _iter != m_maxCoutOfPaxTYpe.end() ) // already exist 
		return;
	
//	int _maxCount = int( m_fZoneArea * _dDensity );
	int _maxCount = int( m_fZoneArea / _dDensity );
	m_maxCoutOfPaxTYpe.insert( std::make_pair( _key, _maxCount) );
}

void sortIntersectList( const Point& _ptFrom, const Point& _ptTo, INTERSECTPOINTLIST& _intersectPointList )
{
	if( _ptFrom.getX() == _ptTo.getX() )
	{
		if( _ptFrom.getY() > _ptTo.getY() )
			std::sort( _intersectPointList.begin(), _intersectPointList.end(), sortByYValueMore );
		else
			std::sort( _intersectPointList.begin(), _intersectPointList.end(), sortByYValueLess );
	}
	else
	{
		if( _ptFrom.getX() > _ptTo.getX() )
			std::sort( _intersectPointList.begin(), _intersectPointList.end(), sortByXValueMore );
		else
			std::sort( _intersectPointList.begin(), _intersectPointList.end(), sortByXValueLess );
	}
}

CPortal::CPortal()
{

}

CPortal::~CPortal()
{

}
