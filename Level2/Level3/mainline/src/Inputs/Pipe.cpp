// Pipe.cpp: implementation of the CPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pipe.h"
#include "PipeDataSet.h"
#include<algorithm>
#include "common\pollygon.h"
#include "common\line.h"
#include "PipeGraph.h"
#include "common\ARCVector.h"
#include "common\ARCMathCommon.h"
#include "in_term.h"
#include "Engine/LandsideSimulation.h"
#include "Engine/LandsideResourceManager.h"
#include "Engine/CrossWalkInSim.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPipe::CPipe(InputTerminal* _pTerm)
{
	m_pTerm = _pTerm;
	m_bDisplayFlag = true;
}

CPipe::~CPipe()
{
	m_vPipeShape.clear();
	m_vSideWalkInPipe.clear();
}
void CPipe::AttachSideWalk( const ProcessorID& _procID )
{
	ASSERT( m_pTerm );
	ASSERT( m_pTerm->procList->getProcessor( _procID.GetIDString() ) );
	ASSERT( m_pTerm->procList->getProcessor( _procID.GetIDString() )->getProcessorType() == MovingSideWalkProc );
	//ASSERT( m_pDocProcList->getProcessor( _iIdxInProcList ) );
	//ASSERT( m_pDocProcList->getProcessor( _iIdxInProcList )->getProcessorType() == MovingSideWalkProc );
	if( CanAttached( _procID ) )
		m_vSideWalkInPipe.push_back( _procID );
}
bool CPipe::IsSideWalkHasAttach( const ProcessorID& _procID ) const
{	
	MOVINGSIDEWALKID::const_iterator iter = std::find( m_vSideWalkInPipe.begin(), m_vSideWalkInPipe.end() , _procID );
	return iter != m_vSideWalkInPipe.end();
}

//test if the processo can attach with this pipe
bool CPipe::CanAttached( const ProcessorID& _procID  )const
{
	ASSERT( m_pTerm );	
	Processor* pProc = m_pTerm->procList->getProcessor( _procID.GetIDString() );
	if( pProc && pProc->getProcessorType() == MovingSideWalkProc )
	{
		POLLYGONVECTOR regionVector ;
		GetPipeCoveredRegion( regionVector );
		return ((MovingSideWalk*)pProc)->IsContainedBy( regionVector )  ;			
	}
	return false;
}

void CPipe::DetachSideWalk( const ProcessorID& _procID )
{
	
	MOVINGSIDEWALKID::iterator iter = std::find( m_vSideWalkInPipe.begin(), m_vSideWalkInPipe.end() , _procID );
	if( iter != m_vSideWalkInPipe.end() )
		m_vSideWalkInPipe.erase( iter );
}

void CPipe::ReplaceMovingSideWalk( const ProcessorID& _oldProcID, const ProcessorID& _newProcID )
{
	MOVINGSIDEWALKID::iterator iter = std::find( m_vSideWalkInPipe.begin(), m_vSideWalkInPipe.end() , _oldProcID );
	if( iter != m_vSideWalkInPipe.end() )
	{
		iter->copyID( _newProcID );
	}
}
MovingSideWalk* CPipe::GetSideWalkAt(  int _iIndex  )
{
 	ASSERT( m_pTerm );
	ASSERT( _iIndex >=0 && _iIndex < static_cast<int>(m_vSideWalkInPipe.size()) );

	Processor* pProc = m_pTerm->procList->getProcessor( m_vSideWalkInPipe[ _iIndex ].GetIDString() );
	if(  pProc && pProc->getProcessorType() == MovingSideWalkProc )
		return  (MovingSideWalk*)pProc ;
	else
		return NULL;
	
}


void CPipe::WriteData( ArctermFile& p_file )const
{
	p_file.writeField( m_sName );
	p_file.writeInt( (int)m_vPipeShape.size() );
	p_file.writeLine();
	PIPESHAPE::const_iterator iter = m_vPipeShape.begin();
	PIPESHAPE::const_iterator iterEnd = m_vPipeShape.end();
	for(; iter != iterEnd ; ++iter )
	{
		p_file.writePoint( iter->m_point );
		p_file.writeDouble( iter->m_width /SCALE_FACTOR);
		p_file.writeLine();
	}

	p_file.writeInt( (int)m_vSideWalkInPipe.size() );
	p_file.writeLine();
	MOVINGSIDEWALKID::const_iterator iterSideWalk = m_vSideWalkInPipe.begin();
	MOVINGSIDEWALKID::const_iterator iterSideWalkEnd = m_vSideWalkInPipe.end();
	for(; iterSideWalk != iterSideWalkEnd; ++iterSideWalk )
	{
		iterSideWalk->writeProcessorID( p_file );
		p_file.writeLine();			
	}
	
	WritAttachCrosswalkeData(p_file);
}

void CPipe::readObsoleteData( ArctermFile& p_file )
{
	ASSERT( m_pTerm );
	int iCount = -1;
	char sName[256];
	p_file.getLine();
	p_file.getField( sName ,256 );
	m_sName = sName;
	p_file.getInteger( iCount );
	for( int i=0; i< iCount ;++i )
	{
		PIPEPOINT temp;
		p_file.getLine();
		p_file.getPoint( temp.m_point );
		p_file.getFloat( temp.m_width );
		temp.m_width *= SCALE_FACTOR;
		m_vPipeShape.push_back( temp );
	}

	p_file.getLine();
	iCount = -1;
	p_file.getInteger( iCount );
	for( int ii=0; ii<iCount; ++ii )
	{
		ProcessorID tempID;
		//tempID.setID()
		tempID.SetStrDict( m_pTerm->inStrDict );
		p_file.getLine();
		tempID.readProcessorID( p_file );
		m_vSideWalkInPipe.push_back( tempID );
	}

	CalculateTheBisectLine();
}

void CPipe::ReadData(  ArctermFile& p_file,InputLandside* pInputLandside )
{
	ASSERT( m_pTerm );
	int iCount = -1;
	char sName[256];
	p_file.getLine();
	p_file.getField( sName ,256 );
	m_sName = sName;
	p_file.getInteger( iCount );
	for( int i=0; i< iCount ;++i )
	{
		PIPEPOINT temp;
		p_file.getLine();
		p_file.getPoint( temp.m_point );
		p_file.getFloat( temp.m_width );
		temp.m_width *= SCALE_FACTOR;
		m_vPipeShape.push_back( temp );
	}

	p_file.getLine();
	iCount = -1;
	p_file.getInteger( iCount );
	for( i=0; i<iCount; ++i )
	{
		ProcessorID tempID;
		//tempID.setID()
		tempID.SetStrDict( m_pTerm->inStrDict );
		p_file.getLine();
		tempID.readProcessorID( p_file );
		m_vSideWalkInPipe.push_back( tempID );
	}

	CalculateTheBisectLine();

	ReadAttachCrosswalkData(p_file,pInputLandside);
}








// calculate the intermediate data
// bisect line, intersection point of the pipe.
void CPipe::CalculateIntermediatData( int _pipeindex )
{
	CalculateTheBisectLine();
	CalculateTheIntersectionPointWithPipe( _pipeindex );

	//////////////////////////////////////////////////////////////////////////
	for( unsigned i = 0; i< m_vPipeShape.size(); i++ )
	{
		m_vPipeShape[i].iSegment = i;
	}
	//////////////////////////////////////////////////////////////////////////
}

void CPipe::CalculatePointTimeOnSildWalk(int idx)
{
	ASSERT( m_pTerm );
	int iCount = m_vSideWalkInPipe.size();
	std::vector<CPointToPipeXPoint> pList;
	//CPointToPipeXPoint* pList = new CPointToPipeXPoint[ iCount ];
	
	for( int i=0; i<iCount; ++i )
	{
		Processor* pSideWalk = m_pTerm->procList->getProcessor( m_vSideWalkInPipe[ i ].GetIDString() );
		ASSERT( pSideWalk && pSideWalk->getProcessorType() == MovingSideWalkProc );
		
		CPointToPipeXPoint temp = GetIntersectionPoint( pSideWalk->getServicePoint( 0 ) );
		temp.SetOtherInfo( i );
		pList.push_back( temp );
	}
	if( iCount >=2 )
	{
		std::sort( pList.begin(), pList.end() );
	}
	
	m_vPointTimeOnSideWalk.clear();
	for( int ii=0; ii<iCount; ++ii )
	{
		//Processor* pSideWalk = m_pTerm->procList->getProcessor( m_vSideWalkInPipe[ i ].GetIDString() );
		Processor* pSideWalk = m_pTerm->procList->getProcessor( m_vSideWalkInPipe[ pList[ii].GetOtherInfo() ].GetIDString() );
		ASSERT( pSideWalk && pSideWalk->getProcessorType() == MovingSideWalkProc );
		int iStartIdx = ( GetIntersectionPoint( pSideWalk->getServicePoint( 0 ) ) ).GetSegmentIndex();
		int iEndIdx = (GetIntersectionPoint( pSideWalk->getServicePoint( pSideWalk->serviceLocationLength() - 1  ) )).GetSegmentIndex();

///		// TRACE("\n%s\n",pSideWalk->getID()->GetIDString() );
		DistanceUnit dStartDistance = ( GetIntersectionPoint( pSideWalk->getServicePoint(0) ) ).GetDistanceFromStartPt();
		DistanceUnit dEndDistance = ( GetIntersectionPoint( pSideWalk->getServicePoint( pSideWalk->serviceLocationLength() - 1  ) ) ).GetDistanceFromStartPt();
		dStartDistance += GetRealDistanceFromStartOnPipe( iStartIdx );
		dEndDistance += GetRealDistanceFromStartOnPipe( iEndIdx );
		(( MovingSideWalk*)pSideWalk)->CaculatePointTime( m_vPointTimeOnSideWalk ,idx, iStartIdx, iEndIdx ,dStartDistance,dEndDistance);		
	}

	int iSize = m_vPointTimeOnSideWalk.size();
	for(int ii=0; ii<iSize ; ++ii )
	{
		m_vPointTimeOnSideWalk[ii].SetRealDistanceFromStart( GetRealDistanceFromStartOnPipe( m_vPointTimeOnSideWalk[ii] ) );
		
	}
	std::sort( m_vPointTimeOnSideWalk.begin(), m_vPointTimeOnSideWalk.end() );

//	delete [] pList;
}

// Get Intersection point on the pipe from the point.
// input: 
//	point
// output:
//  CPipeInterSectionPoint:  point index in the pipe,  the distance from start point of that segment..
CPointToPipeXPoint CPipe::GetIntersectionPoint( const Point& _targetPoint )
{
	CPointToPipeXPoint resPoint;
	int nPointCount = GetPipePointCount();
	Point startPoint = GetPipePointAt( 0 ).m_point;

	DistanceUnit dShortestDistance;
	DistanceUnit dResDistance = 0.0;
	Point ptIntersection;
	int nResIndex = -1;
	for( int i=1; i<nPointCount; i++ )
	{
		Point destPoint = GetPipePointAt( i ).m_point;
		Line line( startPoint, destPoint );
		Point intersectionPt;
		DistanceUnit dDistFromStartPt;
		if( line.GetIntersectPoint( _targetPoint, intersectionPt, dDistFromStartPt ) )
		{
			DistanceUnit dDist = _targetPoint.distance( intersectionPt );
			if( nResIndex < 0 || dShortestDistance > dDist )
			{
				dShortestDistance = dDist;
				dResDistance = dDistFromStartPt;
				ptIntersection = intersectionPt;
				nResIndex = i - 1;
			}
		}
		startPoint = destPoint;
	}
		// the point does not perp with the lines of pipe.
	for( int ii=0; ii<nPointCount; ii++ )
	{
		Point destPoint = GetPipePointAt( ii ).m_point;
		DistanceUnit dDist = _targetPoint.distance( destPoint );
		if( nResIndex < 0 || dShortestDistance > dDist )
		{
			dResDistance = 0.0;
			dShortestDistance = dDist;
			ptIntersection = destPoint;
			//////////////////////////////////////////////////////////////////////////
			// modify by bird
		//	nResIndex = i;
			nResIndex = ii==0 ? ii : ii-1;

			resPoint.SetPointOnPipe(true);
			resPoint.SetPointIndex(ii);

		}		
	}		

	resPoint.SetPoint( (const Point&)ptIntersection );
	resPoint.SetSegmentIndex( nResIndex );
	resPoint.SetDistanceFromStartPt( dResDistance );
	return resPoint;
}


// Get the intersection point with other pipe base on the intersection point, in other words, find the shortest distance.
// input: 
//	other pipe, CPipeInterSectionPoint to make the shortest path
// output:
//  result x point
// return true / false tell if suceed.
bool CPipe::GetIntersectionPoint( CPipe* _pOtherPipe, const CPointToPipeXPoint& _pIntersectionPt, CMobPipeToPipeXPoint& _pResXPoint  )
{
	int nPipeCount = m_pTerm->m_pPipeDataSet->GetPipeCount();
	
	int i=0;
	for(;  i<nPipeCount; i++ )
	{
		if( m_pTerm->m_pPipeDataSet->GetPipeAt( i ) == _pOtherPipe )
			break;
	}
	assert( i < nPipeCount );
 
	PIPEINTERSECTIONPOINTLIST ptList = m_vPipeIntersectionPoint[i];
	int nPtCount = ptList.size();
	if( nPtCount <= 0 )
		return false;

	int nDeltaIdx = -1;
	DistanceUnit dDeltaDist = -1.0;
	int nSelIndex = -1;
	for( int m=0; m<nPtCount; m++ )
	{
		CPipeToPipeXPoint interSectionPt2 = ptList[m];
		int nThisDeltaIdx = interSectionPt2.GetSegmentIndex() - _pIntersectionPt.GetSegmentIndex();
		if( nThisDeltaIdx < 0 )
			nThisDeltaIdx = -nThisDeltaIdx;
		DistanceUnit dThisDeltaDist = interSectionPt2.GetDistanceFromStartPt() - _pIntersectionPt.GetDistanceFromStartPt();
		if( dThisDeltaDist < 0 )
			dThisDeltaDist = -dThisDeltaDist;

		if( nSelIndex < 0 || nDeltaIdx > nThisDeltaIdx 
				|| ( nDeltaIdx == nThisDeltaIdx && dDeltaDist > dThisDeltaDist ) )
		{
			nSelIndex = m;
			nDeltaIdx = nThisDeltaIdx;
			dDeltaDist = dThisDeltaDist;
		}
	}


	_pResXPoint.SetPipeToPipeXPoint( 	ptList[nSelIndex] );
	return true;
}

void CPipe::InsertPointOnMovingSideWalkWithOrdeer( bool _bOrder, int _iStartIdx , int _iEndIdx, PTONSIDEWALK& _vPointResutl ,DistanceUnit _dFromDistance ,DistanceUnit _dEndDistance )
{
	int iMovingSideIdx = -1;
	int iCornerIdx = -1;
	int iCount = m_vPointTimeOnSideWalk.size();
	
	if( _bOrder )
	{
		//DistanceUnit dPreStartDistance = -1.0;
		DistanceUnit dPreEndDistance = -1.0;
		int iPreWalkIdx= -1;
		for( int i=0; i<iCount; ++i )
		{	
			if( iPreWalkIdx == -1 )
			{
				iPreWalkIdx =	m_vPointTimeOnSideWalk[0].GetSideWalkIdx();
			}
			int iStartIdx = m_vPointTimeOnSideWalk[i].GetStartPointIdxInPipe();
			int iEndIdx = m_vPointTimeOnSideWalk[i].GetEndPointIdxInPipe();
			DistanceUnit dStartDistance = m_vPointTimeOnSideWalk[i].GetStartDistance();
			DistanceUnit dEndDistance = m_vPointTimeOnSideWalk[i].GetEndDistance();
			int iSideWalkIdx = m_vPointTimeOnSideWalk[i].GetSideWalkIdx();
			
			if( _iStartIdx < _iEndIdx )
			{			
				if( iStartIdx <= iEndIdx )
				{
					if( iStartIdx >= _iStartIdx && iEndIdx <= _iEndIdx && dStartDistance < dEndDistance )
					{
						if( _dFromDistance < dStartDistance && _dEndDistance > dEndDistance )
						{
							if( dStartDistance+0.5 >= dPreEndDistance || iPreWalkIdx == iSideWalkIdx )
							{
								_vPointResutl.push_back( m_vPointTimeOnSideWalk[i] );
								dPreEndDistance = dEndDistance;
								iPreWalkIdx = iSideWalkIdx;
							}
						}
					}
				}
			}
			else
			{
				if( iStartIdx == _iStartIdx  )
				{				
					if( _dFromDistance < _dEndDistance // person from little index ---> big index
						&& dStartDistance < dEndDistance // moving side walk from little index ---> big index
						&& dEndDistance < _dEndDistance 
						&& dStartDistance > _dFromDistance )// 
					{
						if( dStartDistance+0.5 >= dPreEndDistance || iPreWalkIdx == iSideWalkIdx )
							{
								_vPointResutl.push_back( m_vPointTimeOnSideWalk[i] );
								dPreEndDistance = dEndDistance;
								iPreWalkIdx = iSideWalkIdx;
							}
						
					}
				}
			}
		}

	}
	else 
	{
		DistanceUnit dPreEndDistance = 99999999999.0;
		int iPreWalkIdx= -1;
		for( int i=iCount-1; i>=0; --i )
		{	

			if( iPreWalkIdx == -1 )
			{
				iPreWalkIdx =	m_vPointTimeOnSideWalk[iCount-1].GetSideWalkIdx();
			}
			int iStartIdx = m_vPointTimeOnSideWalk[i].GetStartPointIdxInPipe();
			int iEndIdx = m_vPointTimeOnSideWalk[i].GetEndPointIdxInPipe();
			DistanceUnit dStartDistance = m_vPointTimeOnSideWalk[i].GetStartDistance();
			DistanceUnit dEndDistance = m_vPointTimeOnSideWalk[i].GetEndDistance();
			int iSideWalkIdx = m_vPointTimeOnSideWalk[i].GetSideWalkIdx();
			
			if ( _iStartIdx > _iEndIdx )
			{
				if( iStartIdx >= iEndIdx )
				{
					if( iStartIdx <= _iStartIdx && iEndIdx >= _iEndIdx && dStartDistance > dEndDistance)
					{
						if( _dFromDistance > dStartDistance && _dEndDistance < dEndDistance )
						{
							if( dStartDistance-0.5 < dPreEndDistance || iPreWalkIdx == iSideWalkIdx )
							{
								_vPointResutl.push_back( m_vPointTimeOnSideWalk[i] );
								dPreEndDistance = dEndDistance;
								iPreWalkIdx = iSideWalkIdx;
							}
						}
					}
				}
			}
			else
			{
				if( iStartIdx == _iStartIdx  )
				{				
					 if( _dFromDistance > _dEndDistance // person from big index ---> little index
						&& dStartDistance > dEndDistance // moving side walk from big index ---> little index
						//&& dEndDistance > _dEndDistance )// 
						&& _dFromDistance > dStartDistance 
						&& _dEndDistance < dEndDistance )// 
					{
						if( dStartDistance-0.5 < dPreEndDistance || iPreWalkIdx == iSideWalkIdx )
						{
							_vPointResutl.push_back( m_vPointTimeOnSideWalk[i] );
							dPreEndDistance = dEndDistance;
							iPreWalkIdx = iSideWalkIdx;
						}
					}
				}
			}
		}
	}
}
// _bOrder = true; little--->big 
void CPipe::InsertPointOnMovingSideWalk(  int _iStartIdx , int _iEndIdx, PTONSIDEWALK& _vPointResutl ,DistanceUnit _dFromDistance ,DistanceUnit _dEndDistance )
{
	//_dFromDistance += GetRealDistanceFromStartOnPipe( _iStartIdx );
	//_dEndDistance += GetRealDistanceFromStartOnPipe( _iEndIdx );

	int iMovingSideIdx = -1;
	int iCornerIdx = -1;
	int iCount = m_vPointTimeOnSideWalk.size();
	if( _iStartIdx < _iEndIdx )
	{
		InsertPointOnMovingSideWalkWithOrdeer( true, _iStartIdx, _iEndIdx, _vPointResutl, _dFromDistance, _dEndDistance );
	}
	else if( _iStartIdx > _iEndIdx )
	{
		InsertPointOnMovingSideWalkWithOrdeer( false, _iStartIdx, _iEndIdx, _vPointResutl, _dFromDistance, _dEndDistance );
	}
	else
	{
		if( _dFromDistance < _dEndDistance )
		{
			InsertPointOnMovingSideWalkWithOrdeer( true, _iStartIdx, _iEndIdx, _vPointResutl, _dFromDistance, _dEndDistance );
		}
		else
		{
			InsertPointOnMovingSideWalkWithOrdeer( false, _iStartIdx, _iEndIdx, _vPointResutl, _dFromDistance, _dEndDistance );
		}
	}
	
}

// give the two intersection points, get all the points for logs 
//std::vector<Point> CPipe::GetPointListForLog( const CPointToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent )
void CPipe::GetPointListForLog( int nPipeIndex,const CPointToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent, PTONSIDEWALK& _vPointResutl )
{
	unsigned nIndex1 = _ptFrom.GetSegmentIndex();
	unsigned nIndex2 = _ptTo.GetSegmentIndex();
	//_vPointResutl.clear();
	//std::vector<Point> vPointList;

	// get from point
	
	Point pt = GetDestPoint( _ptFrom, _nPercent );
	CTimePointOnSideWalk temp;
	temp.SetOnSideWalkFlag( false );
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );
	
	
	
	
	//	InsertPointOnMovingSideWalk( nIndex1, nIndex2, _vPointResutl, GetRealDistanceFromStartOnPipe( _ptFrom ), GetRealDistanceFromStartOnPipe( _ptTo ) );
	DistanceUnit dFrom = GetRealDistanceFromStartOnPipe( _ptFrom );

	DistanceUnit dEnd = GetRealDistanceFromStartOnPipe( _ptTo );
	if( nIndex1 < nIndex2 )
	{
		// get middle point
		for( unsigned i=nIndex1; i<=nIndex2 && i<m_vPipeShape.size() -1; i++)
		{	
			DistanceUnit dPara1 = dFrom > GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd < GetRealDistanceFromStartOnPipe( i+1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i+1 );
			InsertPointOnMovingSideWalk( i, i+1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd >= GetRealDistanceFromStartOnPipe( i+1 ) && i != nIndex2 )
			{
				Point pt = GetDestPoint( i+1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		}
	}
	else if( nIndex1 > nIndex2 )
	{
		for( unsigned i=nIndex1+1; i>=nIndex2 && i>0 ; i--)
		{
			if( i>= m_vPipeShape.size() )
			{
				continue;
			}
			DistanceUnit dPara1 = dFrom < GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd > GetRealDistanceFromStartOnPipe( i-1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i-1 );
			InsertPointOnMovingSideWalk( i, i-1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd < GetRealDistanceFromStartOnPipe( i-1 ) && i-1 != nIndex2 )
			{
				Point 	pt = GetDestPoint( i-1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
			
		}	
	}
	else
	{
		InsertPointOnMovingSideWalk( nIndex1, nIndex1, _vPointResutl, dFrom , dEnd  );

	}
	
	
	// get last point
	
	pt = GetDestPoint( _ptTo, _nPercent );
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );
	
	//vPointList.push_back( pt );

//	return vPointList;
}


// give one intersection point.
// one intersection point between pipe.
// get a list of point for the logs.
// last point on the pipe to pipe. at this stage, we have to know the order information.
//std::vector<Point> CPipe::GetPointListForLog( const CPointToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo, int _nPercent )
void CPipe::GetPointListForLog( int nPipeIndex,const CPointToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo, int _nPercent, PTONSIDEWALK& _vPointResutl )
{
	unsigned nIndex1 = _ptFrom.GetSegmentIndex();
	unsigned nIndex2 = _ptTo.GetSegmentIndex();

	//std::vector<Point> vPointList;
	//_vPointResutl.clear();
	CTimePointOnSideWalk temp;
	temp.SetOnSideWalkFlag( false );
	// get from point

	Point pt = GetDestPoint( _ptFrom, _nPercent );
	temp.SetPoint( pt );
	//vPointList.push_back( pt );
	_vPointResutl.push_back( temp );

	//InsertPointOnMovingSideWalk( nIndex1, nIndex2, _vPointResutl, GetRealDistanceFromStartOnPipe( _ptFrom ), GetRealDistanceFromStartOnPipe( _ptTo ) );
	DistanceUnit dFrom = GetRealDistanceFromStartOnPipe( _ptFrom );

	DistanceUnit dEnd = GetRealDistanceFromStartOnPipe( _ptTo );
	if( nIndex1 < nIndex2 )
	{
		// get middle point
		for( unsigned i=nIndex1; i<=nIndex2 && i<m_vPipeShape.size() -1; i++)
		{	
			DistanceUnit dPara1 = dFrom > GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd < GetRealDistanceFromStartOnPipe( i+1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i+1 );
			InsertPointOnMovingSideWalk( i, i+1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd > GetRealDistanceFromStartOnPipe( i+1 ) && i != nIndex2 )
			{
				Point 	pt = GetDestPoint( i+1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		}
	}
	else if( nIndex1 > nIndex2 )
	{
		for( unsigned i=nIndex1+1; i>=nIndex2 && i>0 ; i--)
		{
			if( i>= m_vPipeShape.size() )
			{
				continue;
			}
			DistanceUnit dPara1 = dFrom < GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd > GetRealDistanceFromStartOnPipe( i-1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i-1 );
			InsertPointOnMovingSideWalk( i, i-1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd < GetRealDistanceFromStartOnPipe( i-1 ) && i-1 != nIndex2 )
			{
				Point pt = GetDestPoint( i-1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		
			
		}	
	}
	else
	{
		InsertPointOnMovingSideWalk( nIndex1, nIndex1, _vPointResutl, dFrom , dEnd  );

	}
	
	// get last point
	
	pt = GetDestPoint( _ptTo, _nPercent );
	//vPointList.push_back( pt );
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );
	

	//return vPointList;
}

// give two intersection point between pipe.
// get a list of point for the logs.
// last point on the pipe to pipe. at this stage, we have to know the order information.
//std::vector<Point> CPipe::GetPointListForLog( const CMobPipeToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo, int _nPercent )
void CPipe::GetPointListForLog( int nPipeIndex,const CMobPipeToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo,  int _nPercent,PTONSIDEWALK& _vPointResutl )
{
	unsigned nIndex1 = _ptFrom.GetOtherSegIndex();
	unsigned nIndex2 = _ptTo.GetSegmentIndex();

	//_vPointResutl.clear();
	CTimePointOnSideWalk temp;
	temp.SetOnSideWalkFlag( false );
	Point pt;
	//CTimePointOnSideWalk temp;
/*
	
	// get from point
	Point pt = GetDestPoint( _ptFrom, _nPercent );
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );
*/
	//InsertPointOnMovingSideWalk( nIndex1, nIndex2, _vPointResutl, GetRealDistanceFromStartOnPipe( _ptFrom ), GetRealDistanceFromStartOnPipe( _ptTo ) );

	DistanceUnit dFrom = GetRealDistanceFromStartOnPipe( _ptFrom );

	DistanceUnit dEnd = GetRealDistanceFromStartOnPipe( _ptTo );
	if( nIndex1 < nIndex2 )
	{
		// get middle point
		for( unsigned i=nIndex1; i<=nIndex2 && i<m_vPipeShape.size() -1; i++)
		{	
			DistanceUnit dPara1 = dFrom > GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd < GetRealDistanceFromStartOnPipe( i+1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i+1 );
			InsertPointOnMovingSideWalk( i, i+1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd > GetRealDistanceFromStartOnPipe( i+1 ) && i != nIndex2 )
			{
				Point pt = GetDestPoint( i+1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		}
	}
	else if( nIndex1 > nIndex2 )
	{
		for( unsigned i=nIndex1+1; i>=nIndex2 && i>0 ; i--)
		{
			if( i>= m_vPipeShape.size() )
			{
				continue;
			}
			DistanceUnit dPara1 = dFrom < GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd > GetRealDistanceFromStartOnPipe( i-1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i-1 );
			InsertPointOnMovingSideWalk( i, i-1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd < GetRealDistanceFromStartOnPipe( i-1 ) && i-1 != nIndex2 )
			{
				Point pt = GetDestPoint( i-1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		
			
		
		}	
	}
	else
	{
		InsertPointOnMovingSideWalk( nIndex1, nIndex1, _vPointResutl, dFrom , dEnd  );

	}	

	// get last point
	pt = GetDestPoint( _ptTo, _nPercent );
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );

	//return vPointList;
}

// give one intersection point between pipe.
// give one intersection point.
// get a list of point for the logs.
// last point on the pipe to pipe. at this stage, we have to know the order information.
//std::vector<Point> CPipe::GetPointListForLog( const CMobPipeToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent )
void CPipe::GetPointListForLog( int nPipeIndex,const CMobPipeToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent , PTONSIDEWALK& _vPointResutl )
{
	unsigned nIndex1 = _ptFrom.GetOtherSegIndex();
	unsigned nIndex2 = _ptTo.GetSegmentIndex();

	//_vPointResutl.clear();
	CTimePointOnSideWalk temp;
	temp.SetOnSideWalkFlag( false );
	
	Point pt ;
	/*
	// get from point
	Point pt = GetDestPoint( _ptFrom, _nPercent );
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );
*/

	DistanceUnit dFrom = GetRealDistanceFromStartOnPipe( _ptFrom );

	DistanceUnit dEnd = GetRealDistanceFromStartOnPipe( _ptTo );
	if( nIndex1 < nIndex2 )
	{
		// get middle point
		for( unsigned i=nIndex1; i<=nIndex2 && i<m_vPipeShape.size() -1; i++)
		{	
			if( i>= m_vPipeShape.size() )
			{
				continue;
			}
			DistanceUnit dPara1 = dFrom > GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd < GetRealDistanceFromStartOnPipe( i+1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i+1 );
			InsertPointOnMovingSideWalk( i, i+1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd > GetRealDistanceFromStartOnPipe( i+1 ) && i != nIndex2 )
			{
				Point pt = GetDestPoint( i+1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		}
	}
	else if( nIndex1 > nIndex2 )
	{
		for( unsigned i=nIndex1+1; i>=nIndex2 && i>0 ; i--)
		{
			DistanceUnit dPara1 = dFrom < GetRealDistanceFromStartOnPipe( i ) ? dFrom :  GetRealDistanceFromStartOnPipe( i );
			DistanceUnit dPara2 = dEnd > GetRealDistanceFromStartOnPipe( i-1 ) ? dEnd :  GetRealDistanceFromStartOnPipe( i-1 );
			InsertPointOnMovingSideWalk( i, i-1, _vPointResutl, dPara1 , dPara2  );
			if( dEnd < GetRealDistanceFromStartOnPipe( i-1 ) && i-1 != nIndex2 )
			{
				Point pt = GetDestPoint( i-1, _nPercent );
				temp.SetPoint( pt );
				_vPointResutl.push_back( temp );
			}
		
			
		
		}	
	}
	else
	{
		InsertPointOnMovingSideWalk( nIndex1, nIndex1, _vPointResutl, dFrom , dEnd  );

	}
	
	// get last point
	pt = GetDestPoint( _ptTo, _nPercent );
	
	temp.SetPoint( pt );
	_vPointResutl.push_back( temp );

	
}



/*



// function:
//	Get List of points for logs
// inuput: 
//  Sourc Intersection Point, Dest Intersection Point, Percentage
// output;
//  List of points.
std::vector<Point> CPipe::GetPointListForLog( const CPipeInterSectionPoint& _ptFrom, const CPipeInterSectionPointWithPipe& _ptTo, int _nPercent )
{
	int nIndex1 = _ptFrom.GetSegmentIndex();
	int nIndex2 = _ptTo.GetSegmentIndex();

	std::vector<Point> vPointList;
	if( nIndex1 <= nIndex2 )
	{
		// get from point
		Point pt = GetDestPoint( nIndex1, _nPercent, dDistFromStart1 );
		vPointList.push_back( pt );

		// get middle point
		for( int i=nIndex1+1; i<=nIndex2; i++)
		{
			pt = GetDestPoint( i, _nPercent, 0 );
			vPointList.push_back( pt );
		}

		// get last point
		pt = GetDestPoint( _ptTo, _nPercent );
		vPointList.push_back( pt );
	}
	else
	{
		// get from point
		Point pt = GetDestPoint( nIndex1, _nPercent, dDistFromStart1 );
		vPointList.push_back( pt );

		// get middle point
		for( int i=nIndex1; i>nIndex2; i--)
		{
			pt = GetDestPoint( i, _nPercent, 0 );
			vPointList.push_back( pt );
		}

		// get last point
		pt = GetDestPoint( nIndex2, _nPercent, dDistFromStart2 );
		vPointList.push_back( pt );
	}	
	return vPointList;

}


*/


// get point for the given index of pipe point and percent
Point CPipe::GetDestPoint( int _nIndex, int _nPercent )
{
	Point ptStart = m_vPipeShape[_nIndex].m_bisectPoint1;
	Point ptEnd = m_vPipeShape[_nIndex].m_bisectPoint2;
	Point targetVector( ptStart, ptEnd );

	double d = _nPercent;
	d /= 100.0;
	targetVector *= d;
	return ptStart + targetVector;
}

// get point for the given intersected center point and percent
Point CPipe::GetDestPoint( const CPointToPipeXPoint& _pt, int _nPercent )
{ 
	int nIndex = _pt.GetSegmentIndex();
	DistanceUnit dFromStart = _pt.GetDistanceFromStartPt();


	//Length
	Point ptStart = m_vPipeShape[nIndex].m_bisectPoint1;
	Point ptEnd = m_vPipeShape[nIndex].m_bisectPoint2;
   	DistanceUnit dStart = ptStart.distance(GetPipePointAt(nIndex).m_point);
	DistanceUnit dEnd = ptEnd.distance(GetPipePointAt(nIndex+1).m_point);
    
	Point targetVector( ptStart, ptEnd );

	// calculate the width
	DistanceUnit dWidth;
	DistanceUnit dWidth1 = m_vPipeShape[nIndex].m_width;
	int nPointCount = m_vPipeShape.size();
	if( dFromStart >= 0 && nIndex < nPointCount-1 )
	{
		// the point will not be on the bisect line
		// and the width will be different.
		DistanceUnit dWidth2 = m_vPipeShape[nIndex+1].m_width;
		Point point1 = m_vPipeShape[nIndex].m_point;
		Point point2 = m_vPipeShape[nIndex+1].m_point;
		dWidth = GetWidth( point1, point2, dWidth1, dWidth2, dFromStart );

		// calc 
		Point centerLineVector( point1, point2 );
		Point perpVector = centerLineVector.perpendicularY();
		perpVector.length( dWidth / 2 );
		ptStart = _pt + perpVector;

		dWidth *= _nPercent;
		dWidth /= 100.0;
		perpVector.length( dWidth );


		//Point nextPoint = GetPipePointAt( nIndex+1).m_point;//add matt
		//Point ptNextStart = m_vPipeShape[nIndex+1].m_bisectPoint1;
        //if(dFromStart==0) return ptNextStart - perpVector; 

		return ptStart - perpVector;
	}

	dWidth = dWidth1;
	dWidth *= _nPercent;
	dWidth /= 100.0;
	targetVector.length( dWidth );
	
//select closed point
/*
	if(nIndex +1 <= nPointCount)
	{
		Point nextPoint = GetPipePointAt( nIndex+1).m_point;
		Point ptNextStart = m_vPipeShape[nIndex+1].m_bisectPoint1;
		Point ptNextEnd = m_vPipeShape[nIndex+1].m_bisectPoint2;
		
		DistanceUnit dDist_cur = _pt.distance( ptStart );
		DistanceUnit dNextDist = _pt.distance(nextPoint);
		if(dDist_cur > dNextDist)
			return ptNextStart + targetVector;
	//	else 
		//	return ptNextStart - targetVector;
	}*/


	return ptStart + targetVector;
}



// get point for the given pipe intersected point and percent
Point CPipe::GetDestPoint( const CMobPipeToPipeXPoint& _pt, int _nPercent )
{
	Point pt1 = _pt.GetPoint1();
	Point pt2 = _pt.GetPoint2();
	Point ptV( pt1, pt2 );
	ptV *= ( (double)_nPercent / 100.0 );
	return pt1 + ptV;
}


// calculate the intersection point with other pipes.
void CPipe::CalculateTheIntersectionPointWithPipe( int _pipeindex )
{
	m_vPipeIntersectionPoint.clear();
	m_vEntryPointList.clear();
	int nPipeCount = m_pTerm->m_pPipeDataSet->GetPipeCount();
	for( int i=0; i<nPipeCount; i++ )
	{
		CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( i );
		assert( pPipe );

		PIPEINTERSECTIONPOINTLIST pipeIntersectionList;
		if( pPipe != this )
		{
			if( m_vPipeShape[0].m_point.getZ() == pPipe->m_vPipeShape[0].m_point.getZ() )
			{
				pipeIntersectionList = CalculateThisIntersectionPointWithOtherPipe( pPipe, i, _pipeindex);		
			}
			
		}
		m_vPipeIntersectionPoint.push_back( pipeIntersectionList );
	}
}



// calculate the intersection point with other pipe
PIPEINTERSECTIONPOINTLIST CPipe::CalculateThisIntersectionPointWithOtherPipe( CPipe* _pOtherPipe,int _pipeindex1, int _pipeindex2 )
{
	PIPEINTERSECTIONPOINTLIST pipeIntersectionPointList;
	int nThisSegCount = GetPipePointCount() - 1;
	int nThatSegCount = _pOtherPipe->GetPipePointCount() - 1;
	for( int i=0; i<nThisSegCount; i++ )
	{
		for( int m=0; m<nThatSegCount; m++ )
		{
			Point ptThisStart = GetPipePointAt( i ).m_point;
			Point ptThisEnd = GetPipePointAt( i+1 ).m_point;
			Line thisLine( ptThisStart, ptThisEnd );
			Point ptThatStart = _pOtherPipe->GetPipePointAt( m ).m_point;
			Point ptThatEnd = _pOtherPipe->GetPipePointAt( m+1 ).m_point;
			Line thatLine( ptThatStart, ptThatEnd );
			
			Point ptIntersects;
			if( thisLine.intersects( thatLine, ptIntersects ) )
			{
				CPipeToPipeXPoint point;
				point.SetPoint( ptIntersects );

				Point ptSameOrder1;
				Point ptSameOrder2;
				Point ptDiffOrder1;
				Point ptDiffOrder2;

				Point thisVector;
				Point thatVector;

				DistanceUnit dThisWidthStart = GetPipePointAt( i ).m_width;
				DistanceUnit dThisWidthEnd = GetPipePointAt( i + 1 ).m_width;
				DistanceUnit dThatWidthStart = _pOtherPipe->GetPipePointAt( m ).m_width;
				DistanceUnit dThatWidthEnd = _pOtherPipe->GetPipePointAt( m + 1 ).m_width;

				DistanceUnit dFromStart1 = point.distance( ptThisStart );
				DistanceUnit dFromStart2 = point.distance( ptThatStart );
				DistanceUnit dThisWidth = GetWidth( ptThisStart, ptThisEnd, dThisWidthStart, dThisWidthEnd, dFromStart1 );
				DistanceUnit dThatWidth = GetWidth( ptThatStart, ptThatEnd, dThatWidthStart, dThatWidthEnd, dFromStart2 );

				// ptSameOrder1				
				// ptSameOrder2
				thisVector = Point( ptThisEnd, ptThisStart );
				thisVector.length( dThatWidth / 2.0 );
				thatVector = Point( ptThatStart, ptThatEnd );
				thatVector.length( dThisWidth / 2.0 );
				Point sameOrderVector = thisVector + thatVector;
				ptSameOrder1 = ptIntersects - sameOrderVector;
				ptSameOrder2 = ptIntersects + sameOrderVector;
				if( IsCrossOver( GetPipePointAt( i ).m_bisectPoint1, GetPipePointAt( i ).m_bisectPoint2, ptSameOrder1, ptSameOrder2 ) )
				{
					Point ptTemp = ptSameOrder1;
					ptSameOrder1 = ptSameOrder2;
					ptSameOrder2 = ptTemp;
				}

				// ptDiffOrder2
				// ptDiffOrder1
				thisVector = Point( ptThisStart, ptThisEnd );
				thisVector.length( dThatWidth / 2.0 );
				thatVector = Point( ptThatStart, ptThatEnd );
				thatVector.length( dThisWidth / 2.0 );
				Point diffOrderVector = thisVector + thatVector;
				ptDiffOrder1 = ptIntersects - diffOrderVector;
				ptDiffOrder2 = ptIntersects + diffOrderVector;
				if( IsCrossOver( GetPipePointAt( i ).m_bisectPoint1, GetPipePointAt( i ).m_bisectPoint2, ptDiffOrder1, ptDiffOrder2 ) )
				{
					Point ptTemp = ptDiffOrder1;
					ptDiffOrder1 = ptDiffOrder2;
					ptDiffOrder2 = ptTemp;
				}

				point.SetSegmentIndex( i );
				point.SetDistanceFromStartPt( dFromStart1 );
				point.SetSameOrderPoints( ptSameOrder1, ptSameOrder2 );
				point.SetDiffOrderPoints( ptDiffOrder1, ptDiffOrder2 );
				point.SetOtherSegIndex( m );
				point.SetOtherDistFromStart( dFromStart2 );
				//////////////////////////////////////////////////////////////////////////
				point.SetPipeIndex1( _pipeindex1 );
				point.SetPipeIndex2( _pipeindex2 );
				//////////////////////////////////////////////////////////////////////////
				
				pipeIntersectionPointList.push_back( point );
			}
		}
	}
	return pipeIntersectionPointList;
}
DistanceUnit CPipe::GetRealDistanceFromStartOnPipe( unsigned _iEndIdx )const
{
	ASSERT( _iEndIdx >= 0 && _iEndIdx < m_vPipeShape.size() );
	if( _iEndIdx == 0 )
		return 0.0;
	DistanceUnit dResult=0.0;
	for( unsigned i=0; i<_iEndIdx && i< m_vPipeShape.size() -1 ; ++i )
	{
		dResult += m_vPipeShape[i].m_point.distance( m_vPipeShape[i+1].m_point );
	}
	return dResult;
}

DistanceUnit CPipe::GetRealDistanceFromStartOnPipe( const Point& _ptPoint  )
{
	CPointToPipeXPoint temp = GetIntersectionPoint( _ptPoint );
	return GetRealDistanceFromStartOnPipe( temp.GetSegmentIndex() ) + temp.GetDistanceFromStartPt();
}

// recursive function, find the shortest path.
// return 0 if circle or cannot find one.
// return count if find the 
int CPipe::FindShortestPathToPipe( int _nDestIdx, std::vector<int>& _resList )
{
	int nPiToPiCount = m_vPipeIntersectionPoint.size();
	if( nPiToPiCount == 0 )
		return 0;


	std::vector<int> resList = _resList;
	int nResCount = 0;
	for( int i=0; i<nPiToPiCount; i++ )
	{
		PIPEINTERSECTIONPOINTLIST ptList = m_vPipeIntersectionPoint[i];
		if( ptList.size() == 0 )
			continue;				// no intersection with pipe i;

		// have x with i;
		if( i == _nDestIdx )
		{
			// find the result
			_resList.push_back( i );
			return _resList.size();
		}

		// check circle
		int nInCount = _resList.size();

		int m=0;
		for( ; m<nInCount; m++ )
		{
			if( _resList[m] == i )
				break;
		}

		if( m < nInCount )
			continue;

		// no circle
		CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( i );
		std::vector<int> tmpList = _resList;
		tmpList.push_back( i );
		int nCount = pPipe->FindShortestPathToPipe( _nDestIdx, tmpList );
		if( nCount > 0 && ( nResCount == 0 || nCount < nResCount ) )
		{
			nResCount = nCount;
			resList = tmpList;
		}
	}
	_resList = resList;
	return nResCount;
}


// get all intersect point on segment _iSegment
int CPipe::getIntersectPointOnSegement( int _iSegment, PIPEINTERSECTIONPOINTLIST& _vertex)
{
	assert( _iSegment>=0 && _iSegment < static_cast<int>(m_vPipeShape.size()) - 1 );
	_vertex.clear();
	CPipeToPipeXPoint point;
	for(unsigned i=0; i<m_vPipeIntersectionPoint.size(); i++)
	{
		PIPEINTERSECTIONPOINTLIST pipeIntersectList = m_vPipeIntersectionPoint[i];
		for( unsigned j=0; j<pipeIntersectList.size(); j++ )
		{
			point = pipeIntersectList[j];
			if( point.GetSegmentIndex() == _iSegment )
			{
				_vertex.push_back( point);
			}
		}
	}
	
	for( int ii=0; ii < static_cast<int>(m_vEntryPointList.size()); ii++ )
	{
		point = m_vEntryPointList[ii];
		if( point.GetSegmentIndex() == _iSegment )
		{
			_vertex.push_back( point);
		}
	}
	
	return _vertex.size();
}

void CPipe::addEntryPointToList(const CPipeToPipeXPoint& point)
{
	m_vEntryPointList.push_back( point );
	return;
}

// clear
void CPipe::clearEntryPointList()
{
	m_vEntryPointList.clear();
	return;
}

void CPipe::GetNearestVertex(const CPointToPipeXPoint& point, CGraphVertex& vertexLeft, CGraphVertex& vertexRight)
{
	int nSegment = point.GetSegmentIndex();
	PIPEINTERSECTIONPOINTLIST vertexList;
	getIntersectPointOnSegement(nSegment, vertexList);

	const PIPEPOINT& ptLeft = GetPipePointAt(nSegment);
	const PIPEPOINT& ptRight = GetPipePointAt(nSegment + 1);

	int leftVertexIndex = -1;
	double leftDistance = point.distance(ptLeft.m_point);
	double leftVertexDistance = 0.0;
	
	int rightVertexIndex = -1;
	double rightVertexDistance = 0.0;
	double rightDistance = point.distance(ptRight.m_point);

	int nIntersectionCount = (int)vertexList.size();
	for (int i = 0; i < nIntersectionCount; i++)
	{
		double thisLeftDistance = vertexList[i].distance(ptLeft.m_point);
		double thisRightDistance = vertexList[i].distance(ptRight.m_point);

		if (thisLeftDistance > leftVertexDistance && thisLeftDistance < leftDistance)
		{
			leftVertexIndex = i;
			leftVertexDistance = thisLeftDistance;
		}
		else if (thisRightDistance > rightVertexDistance && thisRightDistance < rightDistance)
		{
			rightVertexIndex = i;
			rightVertexDistance = thisRightDistance;
		}
	}

	if (leftVertexIndex == -1)
	{
		vertexLeft.setVertexpoint(ptLeft.m_point);
		//vertexLeft.setVertexType(POINTONPIPE);
		//vertexLeft.setPtOnPipe(ptLeft);
	}
	else
	{
		vertexLeft.setVertexpoint(vertexList[leftVertexIndex]);
		//vertexLeft.setVertexType(POINT2PIPE);
		//vertexLeft.setPt2Pipe(vertexList[leftVertexIndex]);
	}

	if (rightVertexIndex == -1)
	{
		vertexRight.setVertexpoint(ptRight.m_point);
		//vertexLeft.setVertexType(POINTONPIPE);
		//vertexLeft.setPtOnPipe(ptLeft);
	}
	else
	{
		vertexRight.setVertexpoint(vertexList[rightVertexIndex]);
		//vertexRight.setVertexType(POINT2PIPE);
		//vertexRight.setPt2Pipe(vertexList[leftVertexIndex]);
	}

}

void CPipe::ReadAttachCrosswalkData( ArctermFile& p_file,InputLandside* pInputLandside )
{
	p_file.getLine();
	
	//retrieve attach crosswalk count
	int nCount = 0;
	p_file.getInteger(nCount);

	p_file.getLine();
	for (int i = 0; i < nCount; i++)
	{
		CrossWalkAttachPipe attachPipe(pInputLandside);
		attachPipe.ReadData(p_file);
		m_vCrossWalkAttach.push_back(attachPipe);
		p_file.getLine();
	}
}

void CPipe::WritAttachCrosswalkeData( ArctermFile& p_file ) const
{
	//write attach crosswalk count
	int nCount = (int)m_vCrossWalkAttach.size();
	p_file.writeInt(nCount);

	p_file.writeLine();
	for (int i = 0; i < nCount; i++)
	{
		CrossWalkAttachPipe attachPipe = m_vCrossWalkAttach.at(i);
		attachPipe.WriteData(p_file);
		p_file.writeLine();
	}
	p_file.writeLine();
}

int CPipe::GetAttachCrosswalkCount() const
{
	return (int)m_vCrossWalkAttach.size();
}

void CPipe::AddAttachCrosswalk( const CrossWalkAttachPipe& attachPipe )
{
	ASSERT(attachPipe.GetAttachWalk());

	std::vector<CrossWalkAttachPipe>::iterator iter = std::find(m_vCrossWalkAttach.begin(),m_vCrossWalkAttach.end(),attachPipe);
	if (iter == m_vCrossWalkAttach.end())
	{
		m_vCrossWalkAttach.push_back(attachPipe);
	}
}

void CPipe::RemoveAttachCrosswalk(int nIdx)
{
	if (nIdx >= 0 && nIdx < GetAttachCrosswalkCount())
	{
		m_vCrossWalkAttach.erase(m_vCrossWalkAttach.begin() + nIdx);
	}
}

void CPipe::RemoveAttachCrosswalk( LandsideCrosswalk* pCorsswalk )
{
	CrossWalkAttachPipe attachPipe(NULL);
	attachPipe.AttachCrossWalk(pCorsswalk);
	
	std::vector<CrossWalkAttachPipe>::iterator iter = std::find(m_vCrossWalkAttach.begin(),m_vCrossWalkAttach.end(),attachPipe);
	if (iter != m_vCrossWalkAttach.end())
	{
		m_vCrossWalkAttach.erase(iter);
	}
}

CrossWalkAttachPipe* CPipe::GetAttachCrosswalk( LandsideCrosswalk* pCorsswalk )
{
	ASSERT(pCorsswalk);
	CrossWalkAttachPipe attachPipe(NULL);
	attachPipe.AttachCrossWalk(pCorsswalk);

	std::vector<CrossWalkAttachPipe>::iterator iter = std::find(m_vCrossWalkAttach.begin(),m_vCrossWalkAttach.end(),attachPipe);
	if (iter != m_vCrossWalkAttach.end())
	{
		return &*iter;
	}

	return NULL;
}

CrossWalkAttachPipe& CPipe::GetAtachCrosswalk( int nIdx )
{
	ASSERT(nIdx >= 0);
	ASSERT(nIdx < GetAttachCrosswalkCount());

	return m_vCrossWalkAttach[nIdx];
}

void CPipe::GetPipeCrosswalkRegion( POLLYGONVECTOR& _regions ,POLLYGONVECTOR& _texcoord ) const
{
	_regions.clear();
	for (int i = 0; i < GetAttachCrosswalkCount(); i++)
	{
		const CrossWalkAttachPipe& crossPipe = m_vCrossWalkAttach.at(i);
		GetCrosswalkRegion(crossPipe,_regions,_texcoord);
	}
}

void CPipe::GetCrosswalkRegion(const CrossWalkAttachPipe& crossPipe, POLLYGONVECTOR& _regions, POLLYGONVECTOR& _texcoord ) const
{
	const GroupIndex& groupIndex = crossPipe.GetGroupIndex();
	if (groupIndex.start == -1 || groupIndex.end == -1)
		return;
	
	int nMax = MAX(groupIndex.start,groupIndex.end);
	int nMin = MIN(groupIndex.start,groupIndex.end);
	size_t iPipePointCount = nMax - nMin;
	if( iPipePointCount <1 )
		return;

	double u=0;
	double v=0;
	for(int i=0;i<(int)m_vPipeShape.size()-1;i++){
		const PIPEPOINT& pthis = m_vPipeShape[i];
		const PIPEPOINT& pnext = m_vPipeShape[i+1];
		double uplus = pthis.m_bisectPoint1.distance(pnext.m_bisectPoint1);
		double vplus = pthis.m_bisectPoint2.distance(pnext.m_bisectPoint2);

		if( i>= nMin && i<nMax ){
			Pollygon temp;
			Point vertex[4];
			vertex[0] = pthis.m_bisectPoint1;
			vertex[1] = pthis.m_bisectPoint2;
			vertex[2] = pnext.m_bisectPoint2;
			vertex[3] = pnext.m_bisectPoint1;
			temp.init( 4, vertex );
			_regions.push_back( temp );

			temp[0] = Point(0,u,0);
			temp[1] = Point(1,v,0);
			temp[2] = Point(1,v+vplus,0);
			temp[3] = Point(0,u+uplus,0);
			_texcoord.push_back( temp);
		}
		if(i>=nMax)
			break;
		
		u+= uplus;
		v+= vplus;
		std::swap(u,v);
	}

	//PIPESHAPE::const_iterator iter = m_vPipeShape.begin() + nMin;
	//PIPESHAPE::const_iterator iterNext = iter;
	//++iterNext;
	//PIPESHAPE::const_iterator iterEnd = m_vPipeShape.begin() + nMax + 1;
	//for(int i=0; iterNext != iterEnd ; ++iter,++iterNext,++i )
	//{
	//	Pollygon temp;
	//	Point vertex[4];
	//	vertex[0] = iter->m_bisectPoint1;
	//	vertex[1] = iter->m_bisectPoint2;
	//	vertex[2] = iterNext->m_bisectPoint2;
	//	vertex[3] = iterNext->m_bisectPoint1;
	//	temp.init( 4, vertex );

	//
	//	//GetSiglePollygon( *iter, *iterNext, temp );
	//	_regions.push_back( temp );
	//}
}

void CPipe::WritePipePax(LandsideSimulation* pLandsideSim,const Point& pt,const Point& ptDest, int iPaxID,
						 const ElapsedTime& eTime,const ElapsedTime& tMoveTime,bool bCreate /*= true*/ )
{
	for (int i = 0; i < GetAttachCrosswalkCount(); i++)
	{
		CrossWalkAttachPipe& crossPipe = GetAtachCrosswalk(i);
		double dResIdx = GetPointIndex(pt);
		double dDestIdx = GetPointIndex(ptDest);
		bool bDec = dResIdx < dDestIdx ? true : false;
		if (crossPipe.OverlapCrosswalk(dResIdx,dDestIdx))
		{
			LandsideCrosswalk* pCrossWalk = crossPipe.GetAttachWalk();
			if (pLandsideSim->GetResourceMannager() == NULL)
				continue;

			CCrossWalkInSim* pCrosswalkInSim = pLandsideSim->GetResourceMannager()->GetCrosswalk(pCrossWalk);
			if (pCrosswalkInSim == NULL)
				continue;
			
			ElapsedTime enterTime;
			ElapsedTime exitTime;
			if (crossPipe.InCrossWalk(dResIdx))
			{
				enterTime = eTime;
			}
			else
			{
				double dDeta = fabs(dDestIdx - dResIdx);
				double dData  = 0.0;
				GroupIndex groupIndex = crossPipe.GetGroupIndex();
				if (bDec)
				{
					dData = MIN(groupIndex.start,groupIndex.end);
				}
				else
				{
					dData = MAX(groupIndex.start,groupIndex.end);
				}

				double dDist = fabs(dData - dResIdx);
				double dRate = 0;
				if(dDeta > 0.1)
					dRate = dDist / dDeta;

				enterTime = eTime + tMoveTime * dRate;
			}

			if (crossPipe.InCrossWalk(dDestIdx))
			{
				exitTime = eTime + tMoveTime;
			}
			else
			{
				double dDeta = fabs(dDestIdx - dResIdx);
				double dData  = 0.0;
				GroupIndex groupIndex = crossPipe.GetGroupIndex();
				if (bDec)
				{
					dData = MAX(groupIndex.start,groupIndex.end);
				}
				else
				{
					dData = MIN(groupIndex.start,groupIndex.end);
				}

				double dDist = fabs(dData - dResIdx);
				double dRate = 0;
				if(dDeta > 0.1)
					dRate = dDist / dDeta;

				exitTime = eTime + tMoveTime * dRate;
			}

			pCrosswalkInSim->WritePassengerOnCrosswalk(iPaxID,enterTime,exitTime,bCreate);
		}
	}
}
