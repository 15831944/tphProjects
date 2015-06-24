// PipeDataSet.cpp: implementation of the CPipeDataSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeDataSet.h"
#include "pipegraph.h"
#include "../Common/ARCTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPipeDataSet::CPipeDataSet()
 : DataSet(PipeDataSetFile,2.3f)
{
	m_pPipeMgr = new CPipeGraphMgr;
	m_pInputLandside = NULL;
}

CPipeDataSet::~CPipeDataSet()
{
	m_vPipes.clear();
	if( m_pPipeMgr )
	{
		m_pPipeMgr->clearGraphMgr();
		delete m_pPipeMgr;
	}
}

void CPipeDataSet::readObsoleteData( ArctermFile& p_file )
{
	ASSERT( this->m_pInTerm );
	int iCount = -1;
	p_file.getLine();
	p_file.getInteger( iCount );
	for( int i=0; i<iCount ; ++i )
	{
		CPipe temp( m_pInTerm );
		temp.readObsoleteData( p_file );
		m_vPipes.push_back( temp );
	}
}

void CPipeDataSet::readData (ArctermFile& p_file)
{
	ASSERT( this->m_pInTerm );
	int iCount = -1;
	p_file.getLine();
	p_file.getInteger( iCount );
	for( int i=0; i<iCount ; ++i )
	{
		CPipe temp( m_pInTerm );
		temp.ReadData( p_file,m_pInputLandside );
		m_vPipes.push_back( temp );
	}
}

void CPipeDataSet::writeData (ArctermFile& p_file) const
{
	p_file.writeInt( (int)m_vPipes.size() );
	p_file.writeLine();
	PIPEVECTOR::const_iterator iter = m_vPipes.begin();
	PIPEVECTOR::const_iterator iterEnd = m_vPipes.end();
	for(; iter != iterEnd; ++iter )
	{
		iter->WriteData( p_file );
	}
}
void CPipeDataSet::clear (void)
{
	m_vPipes.clear();	
	m_pPipeMgr->clearGraphMgr();
}
void CPipeDataSet::AddPipe( CPipe& _newPipe )
{
	//ASSERT( _newPipe.m_pTerm );
	m_vPipes.push_back( _newPipe );
}
CPipe* CPipeDataSet::GetPipeAt( int _iIndex )
{
	ASSERT( _iIndex >=0 && _iIndex < static_cast<int>(m_vPipes.size()) );
	return &m_vPipes[ _iIndex ];
}
const CPipe& CPipeDataSet::operator [] ( int _iIndex ) const
{
	ASSERT( _iIndex >=0 && _iIndex < static_cast<int>(m_vPipes.size()) );
	return m_vPipes.at( _iIndex );
}

void CPipeDataSet::DeletePipeAt(  int _iIndex  )
{
	ASSERT( _iIndex >=0 && _iIndex < static_cast<int>(m_vPipes.size()) );
	m_vPipes.erase( m_vPipes.begin() + _iIndex );
}


void CPipeDataSet::CalculateAllIntermediatData()
{
	PLACE_METHOD_TRACK_STRING();
	int nPipeCount = m_vPipes.size();
	for( int i=0; i<nPipeCount; i++ )
	{
		m_vPipes[i].CalculateIntermediatData(i);
	}
}
void CPipeDataSet::CalculateAllTimePointOnSideWalk()
{
	PLACE_METHOD_TRACK_STRING();
	int nPipeCount = m_vPipes.size();
	for( int i=0; i<nPipeCount; i++ )
	{
		m_vPipes[i].CalculatePointTimeOnSildWalk(i);
	}
}

// Give a start point, end point, find the pipe list should flow.
std::vector<int> CPipeDataSet::AutomaicGetPipe( Point _ptStart, Point _ptEnd )
{
	std::vector<int> resList;

	// list all the possible pipe for _ptStart, _ptEnd
	int nPipeCount = m_vPipes.size();
	if( nPipeCount == 0 )
		return resList;

	SortedContainer<PointToPipeInfo> sortedStartList;
	sortedStartList.OwnsElements( TRUE );
	SortedContainer<PointToPipeInfo> sortedEndList;
	sortedEndList.OwnsElements( TRUE );

	for( int i=0; i<nPipeCount; i++ )
	{
		CPipe* pPipe = GetPipeAt( i );

		// pipe in different floor should not be involved
		if( pPipe->GetPipePointAt( 0 ).m_point.getZ() != _ptStart.getZ() ||
			pPipe->GetPipePointAt( 0 ).m_point.getZ() != _ptEnd.getZ() )
		{
			continue;
		}

		CPointToPipeXPoint xStart = pPipe->GetIntersectionPoint( _ptStart );
		DistanceUnit dStart = _ptStart.distance( xStart );
		PointToPipeInfo* pStartInfo = new PointToPipeInfo;
		pStartInfo->m_nIndex = i;
		pStartInfo->m_dDisToPipe = dStart;
		pStartInfo->m_pPipe = pPipe;
		sortedStartList.addItem( pStartInfo );

		CPointToPipeXPoint xEnd = pPipe->GetIntersectionPoint( _ptEnd );
		DistanceUnit dEnd = _ptEnd.distance( xEnd );
		PointToPipeInfo* pEndInfo = new PointToPipeInfo;
		pEndInfo->m_nIndex = i;
		pEndInfo->m_dDisToPipe = dEnd;
		pEndInfo->m_pPipe = pPipe;
		sortedEndList.addItem( pEndInfo );
	}


	nPipeCount = sortedStartList.getCount();
	

	// need to process nPipeCount == 0

///	// TRACE( "\nSTART: " );
	for( i=0; i<nPipeCount; i++ )
	{
		PointToPipeInfo* p = sortedStartList.getItem( i );
//		// TRACE( "%.2f,%s; ", p->m_dDisToPipe, p->m_pPipe->GetPipeName() );
	}
//	// TRACE( "\n" );

//	// TRACE( "\nEND: " );
	for( i=0; i<nPipeCount; i++ )
	{
		PointToPipeInfo* p = sortedEndList.getItem( i );
//		// TRACE( "%.2f,%s; ", p->m_dDisToPipe, p->m_pPipe->GetPipeName() );
	}
//	// TRACE( "\n" );

	// 1 - 1
	PointToPipeInfo* pStartInfo1 = sortedStartList.getItem( 0 );
	PointToPipeInfo* pEndInfo1 = sortedEndList.getItem( 0 );

	DistanceUnit dTotal;
	if( GetPipeList( pStartInfo1, pEndInfo1, dTotal, resList ) )
		return resList;

	if( nPipeCount < 2 )
	{
		resList.clear();
		return resList;
	}

	PointToPipeInfo* pStartInfo2 = sortedStartList.getItem( 1 );
	PointToPipeInfo* pEndInfo2 = sortedEndList.getItem( 1 );

	// 1 - 2, 2 - 1
	std::vector<int> resList12;
	std::vector<int> resList21;
	DistanceUnit dTotal12;
	DistanceUnit dTotal21;
	bool res1 = GetPipeList( pStartInfo1, pEndInfo2, dTotal12, resList12 );
	bool res2 = GetPipeList( pStartInfo2, pEndInfo1, dTotal21, resList21 );
	if( res1 && res2 )
	{
		if( dTotal12 <= dTotal21 )
			return resList12;
		else
			return resList21;
	}
	if( res1 )
		return resList12;

	if( res2 )
		return resList21;

	// 2 - 2 
	if( GetPipeList( pStartInfo2, pEndInfo2, dTotal, resList ) )
		return resList;

	// failed.
	resList.clear();	
	return resList;
}


// input: 
// PointToPipeInfo* _pStartInfo, PointToPipeInfo* _pEndInfo

// output
// DistanceUnit& _dTotal, 	std::vector<int>& _resList

// 

bool CPipeDataSet::GetPipeList( PointToPipeInfo* _pStartInfo, PointToPipeInfo* _pEndInfo, DistanceUnit& _dTotal, 	std::vector<int>& _resList )
{
	_resList.clear();
	_dTotal = _pStartInfo->m_dDisToPipe + _pEndInfo->m_dDisToPipe;
	if( _pStartInfo->m_nIndex == _pEndInfo->m_nIndex )
	{
		_resList.push_back( _pStartInfo->m_nIndex );
		return true;
	}

	_resList.push_back( _pStartInfo->m_nIndex );
	int nCount = _pStartInfo->m_pPipe->FindShortestPathToPipe( _pEndInfo->m_nIndex, _resList );
	if( nCount > 0 )
		return true;

	return false;
}



void CPipeDataSet::DetachMovingSideWalk( const ProcessorID& _movingSideProcID )
{
	PIPEVECTOR::iterator iter = m_vPipes.begin();
	PIPEVECTOR::iterator iterEnd = m_vPipes.end();
	for(; iter != iterEnd; ++iter )
	{
		iter->DetachSideWalk( _movingSideProcID );
	}
}
void CPipeDataSet::ReplaceMovingSideWalk( const ProcessorID& _oldProcID, const ProcessorID& _newProcID )
{
	PIPEVECTOR::iterator iter = m_vPipes.begin();
	PIPEVECTOR::iterator iterEnd = m_vPipes.end();
	for(; iter != iterEnd; ++iter )
	{
		//iter->DetachSideWalk( _movingSideProcID );
	}
}

void CPipeDataSet::AutomaicGetPipeFlowCoveredRegion( Point& _ptStart, Point& _ptEnd ,POLLYGONVECTOR& _regions, Point& _ptFlowStart, Point& _ptFlowEnd )
{
	//_regions.clear();
	CalculateAllIntermediatData();
	std::vector<int> vPipeIdx = AutomaicGetPipe( _ptStart, _ptEnd );
	
	if( vPipeIdx.size() <= 0 )
		return;

	GetPipeListCoveredRegion( _ptStart, _ptEnd, vPipeIdx, _regions,_ptFlowStart, _ptFlowEnd );

	
}

void CPipeDataSet::GetPipeListFlowCoveredRegion( Point& _ptStart, Point& _ptEnd ,std::vector<int>& _vPipeIdx,POLLYGONVECTOR& _regions, Point& _ptFlowStart, Point& _ptFlowEnd )
{
	CalculateAllIntermediatData();
	//_regions.clear();
	GetPipeListCoveredRegion( _ptStart, _ptEnd, _vPipeIdx, _regions,_ptFlowStart, _ptFlowEnd );
}

void CPipeDataSet::GetPipeListCoveredRegion( Point& _ptStart, Point& _ptEnd ,std::vector<int>& _vPipeIdx,POLLYGONVECTOR& _regions,Point& _ptFlowStart, Point& _ptFlowEnd )
{
	int iPipeCount = _vPipeIdx.size();


	std::vector<int> vPipeList2;
	for( int i=0; i<iPipeCount; i++ )
	{
		CPipe* pPipe = GetPipeAt( _vPipeIdx[i] );
		if( pPipe->GetZ() == _ptStart.getZ() )
			vPipeList2.push_back( _vPipeIdx[i] );

	}
	iPipeCount = vPipeList2.size();
	if( iPipeCount == 0 )
		return;

	CPointToPipeXPoint entryPoint;
	CPointToPipeXPoint exitPoint;
	std::vector<CMobPipeToPipeXPoint> vMidPoint;	// num count should be nPipeCount - 1
	

	CPipe* pPipe1 = NULL;
	CPipe* pPipe2 = NULL;
	for(  i=0; i<iPipeCount; i++ )
	{
		if( i == 0 )
		{
			pPipe1 = GetPipeAt( vPipeList2[0] );
			entryPoint = pPipe1->GetIntersectionPoint( _ptStart );
			if( iPipeCount == 1 )
			{
				exitPoint = pPipe1->GetIntersectionPoint( _ptEnd );
			}
			else
			{
				pPipe2 = GetPipeAt( vPipeList2[1] );
				CMobPipeToPipeXPoint midPt;
				if( pPipe1->GetIntersectionPoint( pPipe2, entryPoint, midPt ) )
					vMidPoint.push_back( midPt );
				
			}
		}
		else if( i == iPipeCount - 1 )
		{
			exitPoint = pPipe1->GetIntersectionPoint( _ptEnd);
			vMidPoint[vMidPoint.size()-1].SetOutInc( exitPoint );
		}
		else
		{
			pPipe2 = GetPipeAt( vPipeList2[i+1] );
			CMobPipeToPipeXPoint midPt;
			if( pPipe1->GetIntersectionPoint( pPipe2, vMidPoint[vMidPoint.size()-1], midPt ) )
			{
				vMidPoint[vMidPoint.size()-1].SetOutInc( midPt );
				vMidPoint.push_back( midPt );
			}
		}

		pPipe1 = pPipe2;
	}

	CPipe tempPipe( m_pInTerm );
	PIPEPOINT tempPoint;
	tempPoint.m_width = 1200;
	
	CPipe* pPipe=GetPipeAt( vPipeList2[0] );
	PTONSIDEWALK pointList;	
	int nMidPoint = vMidPoint.size();
	if( nMidPoint == 0 )
	{
		/*
		CTimePointOnSideWalk temp;
		temp.SetOnSideWalkFlag( false );
		
		temp.SetPoint( entryPoint );
		pointList.push_back( temp );
		*/
		pPipe->GetPointListForLog( vPipeList2[0],entryPoint, exitPoint, 50,pointList );
		
		//temp.SetPoint( exitPoint );
		//pointList.push_back( temp );

		int iPointCount = pointList.size();
		for( int k=0; k<iPointCount; ++k )
		{
			tempPoint.m_point = pointList[ k ];	
			tempPipe.AddPipePoint( tempPoint );
		}
		
		pointList.clear();
	}
	else
	{
		/*
		CTimePointOnSideWalk temp;
		temp.SetOnSideWalkFlag( false );
		temp.SetPoint( entryPoint );
		pointList.push_back( temp );
*/
		
		pPipe->GetPointListForLog( vPipeList2[0],entryPoint, vMidPoint[0], 50,pointList );
	//	temp.SetPoint( pPipe->GetDestPoint( vMidPoint[0], 50 ) );
	//	pointList.push_back( temp );

		// process mid point
		for( int i=1; i<nMidPoint; i++ )
		{
			pPipe = GetPipeAt( _vPipeIdx[i] );
			//pointList = pPipe->GetPointListForLog( vMidPoint[i-1], vMidPoint[i], nPercent );

			pPipe->GetPointListForLog( vPipeList2[0],vMidPoint[i-1], vMidPoint[i], 50 ,pointList );
			
		//	temp.SetPoint( pPipe->GetDestPoint( vMidPoint[i] , 50 ) );
		//	pointList.push_back( temp );
		}

		// process exit point
		pPipe = GetPipeAt( vPipeList2[iPipeCount-1] );

		pPipe->GetPointListForLog( vPipeList2[0],vMidPoint[nMidPoint-1], exitPoint, 50,pointList );

	//	temp.SetPoint( exitPoint );
	//	pointList.push_back( temp );

		int iPointCount = pointList.size();
		for( int k=0; k<iPointCount; ++k )
		{
			tempPoint.m_point = pointList[ k ];	
			tempPipe.AddPipePoint( tempPoint );
		}
		pointList.clear();
	}

	
	tempPipe.CalculateTheBisectLine();
	tempPipe.GetPipeCoveredRegion( _regions );
	_ptFlowStart = entryPoint;
	_ptFlowEnd = exitPoint;
}

void CPipeDataSet::clearAllEntryPointList()
{
	for(unsigned i=0; i<m_vPipes.size(); i++ )
	{
		m_vPipes[i].clearEntryPointList();
	}
}

//////////////////////////////////////////////////////////////////////////
int CPipeDataSet::GetPointListForLog(CGraphVertexList& _shortestPath, int _nPercent, PTONSIDEWALK &_pointList)
{
	int iPointCount = _shortestPath.getCount();
	if (iPointCount < 3)
		return 0;

	std::vector< CMobPipeToPipeXPoint > mobPointList;
	preDealThePath( _shortestPath, mobPointList );

	int					iMobPoint = 0;
	Point				point;
	CPointToPipeXPoint	point2Pipe;
	PIPEPOINT			pointOnPipe;
	CPipeToPipeXPoint	pipe2Pipe;
	CGraphVertex		vertexPoint,vertexNextPoint;
	CPipe*				pipe = NULL;
	int					iPipeIndex1, iPipeIndex2;
	int					iSegment1 = -1, iSegment2= -1;
	DistanceUnit		dPara1, dPara2;
	bool				bHavePipe2PipePoint = false;

	CTimePointOnSideWalk temp;
	temp.SetOnSideWalkFlag( false );

	vertexPoint = _shortestPath.getItem(1);
	pipe		= GetPipeAt(vertexPoint.getPipeIndex());
	iPipeIndex1	= vertexPoint.getPipeIndex();

	switch( vertexPoint.getVertexType() )
	{
	case POINT2PIPE:
		point2Pipe = vertexPoint.getPt2Pipe();
		point = pipe->GetDestPoint( point2Pipe, _nPercent );
		iSegment1 = point2Pipe.GetSegmentIndex();
		break;

	case POINTONPIPE:
		pointOnPipe = vertexPoint.getPtOnPipe();
		point = pipe->GetDestPoint( pointOnPipe.iSegment, _nPercent );
		iSegment1 = pointOnPipe.iSegment;
		break;

	case PIPE2PIPE:
		pipe2Pipe = vertexPoint.getPipe2Pipe();
		point = pipe->GetDestPoint( pipe2Pipe, _nPercent );
		iSegment1 = pipe2Pipe.GetSegmentIndex();

		bHavePipe2PipePoint = true;
		iMobPoint++;
		break;

	default:
		assert(0);
	}

	temp.SetPoint(point);
	temp.SetPipeIdx(iPipeIndex1);
	_pointList.push_back(temp);

	if (iPointCount > 3)// have more than one point on pipe
	{
		for(int i=2; i<= iPointCount-2; i++ )
		{
//			// TRACE("\r\n %d point:\r\n", i );
			vertexNextPoint = _shortestPath.getItem( i );
			iPipeIndex2		= vertexNextPoint.getPipeIndex();
//			// TRACE(" pipe index: %d\r\n", iPipeIndex2 );
			switch( vertexNextPoint.getVertexType() )
			{
			case POINT2PIPE:
				// from pipe to outside point
				//assert(0);
//				// TRACE(">>>>> point type: point 2 pipe\r\n>>>>> pass through the point\r\n");
				point2Pipe = vertexNextPoint.getPt2Pipe();
				if( iPipeIndex1 == iPipeIndex2 && vertexPoint.getVertexType()==POINTONPIPE)
				{
					iSegment2 = point2Pipe.GetSegmentIndex();
					point = pipe->GetDestPoint( point2Pipe, _nPercent );
					dPara1 = pipe->GetRealDistanceFromStartOnPipe( iSegment1);
					dPara2 = pipe->GetRealDistanceFromStartOnPipe( point );
					pipe->InsertPointOnMovingSideWalk( iSegment1, iSegment2, _pointList, dPara1, dPara2 );
				}
				continue;
				break;
				
			case POINTONPIPE:

				//moving on the same pipe.
				pointOnPipe = vertexNextPoint.getPtOnPipe();
				iSegment2 = pointOnPipe.iSegment;
				
				pipe = GetPipeAt( iPipeIndex2 );
//				// TRACE(">>>>> point type: point on pipe\r\n>>>>> segment : %d\r\n", iSegment2 );
				//////////////////////////////////////////////////////////////////////////
				// process the moving side walk
				if( iPipeIndex1 == iPipeIndex2 )
				{
//					// TRACE(">>>>>>>>>> process the moving side walk\r\n");
					if(vertexPoint.getVertexType()==PIPE2PIPE)
					{
						dPara1 = pipe->GetRealDistanceFromStartOnPipe( vertexPoint.getPipe2Pipe() );
					}
					else if(vertexPoint.getVertexType()==POINT2PIPE)
					{
						dPara1 = pipe->GetRealDistanceFromStartOnPipe( vertexPoint.getPt2Pipe() );
					}
					else
						dPara1 = pipe->GetRealDistanceFromStartOnPipe( iSegment1 );

					dPara2 = pipe->GetRealDistanceFromStartOnPipe( iSegment2 );
					pipe->InsertPointOnMovingSideWalk( iSegment1, iSegment2, _pointList, dPara1, dPara2 );
				}
				point = pipe->GetDestPoint( iSegment2, _nPercent );
				temp.SetPoint( point );
				temp.SetPipeIdx(iPipeIndex2);
				_pointList.push_back( temp );
				break;
				
			case PIPE2PIPE:
				{
					// from pipe to pipe.
					pipe2Pipe = vertexNextPoint.getPipe2Pipe();
					iSegment2 = pipe2Pipe.GetSegmentIndex();
	//				// TRACE(">>>>> point type: pipe 2 pipe\r\n>>>>> segment : %d\r\n", iSegment2 );

					int sourcepipe = -1;
					if( vertexPoint.getVertexType() != PIPE2PIPE)
					{
						sourcepipe = iPipeIndex1;
					}
					else
					{
						if( vertexPoint.getPipe2Pipe().GetPipeIndex1() == pipe2Pipe.GetPipeIndex1() ||  
							vertexPoint.getPipe2Pipe().GetPipeIndex2() == pipe2Pipe.GetPipeIndex1())
							iPipeIndex2 = pipe2Pipe.GetPipeIndex1();
						
						else if( vertexPoint.getPipe2Pipe().GetPipeIndex1() == pipe2Pipe.GetPipeIndex2() ||  
								vertexPoint.getPipe2Pipe().GetPipeIndex2() == pipe2Pipe.GetPipeIndex2() ) 
							iPipeIndex2 = pipe2Pipe.GetPipeIndex2();

						else
							assert(0);
						
						iSegment2 = pipe2Pipe.GetOtherSegIndex();
						sourcepipe = iPipeIndex2;
	//					// TRACE(">>>>>>>>>> pipe index: %d\r\n", iPipeIndex2 );
					}
					pipe = GetPipeAt(sourcepipe);

					bool bInInc = pipe->SameDirection(vertexPoint.getVertexPoint(), vertexNextPoint.getVertexPoint());
					CPipe* nextPipe = NULL;
					if (pipe2Pipe.GetPipeIndex1() == sourcepipe)
					{
						nextPipe = GetPipeAt(pipe2Pipe.GetPipeIndex2());
					}
					else
					{
						nextPipe = GetPipeAt(pipe2Pipe.GetPipeIndex1());
					}

					bool bOutInc = nextPipe->SameDirection(vertexNextPoint.getVertexPoint(), _shortestPath.getItem(i + 1).getVertexPoint());
					mobPointList[iMobPoint].SetInInc(bInInc);
					mobPointList[iMobPoint].SetOutInc(bOutInc);

					bool orderChanged = mobPointList[iMobPoint].OrderChanged();
					int percent = _nPercent;
					if (orderChanged && sourcepipe == pipe2Pipe.GetPipeIndex1())
						percent = 100 - _nPercent;

					point = pipe->GetDestPoint(mobPointList[iMobPoint], percent);
					if( orderChanged)
						_nPercent = 100 - _nPercent;

					if( vertexPoint.getVertexType() == POINTONPIPE )
					{
						dPara1 = pipe->GetRealDistanceFromStartOnPipe( iSegment1 );
						dPara2 = pipe->GetRealDistanceFromStartOnPipe( point );
						pipe->InsertPointOnMovingSideWalk( iSegment1, iSegment2, _pointList, dPara1, dPara2 );
					}

					temp.SetPoint( point );
					temp.SetPipeIdx(sourcepipe);
					_pointList.push_back( temp );
					bHavePipe2PipePoint = true;
					iMobPoint++;
				}
				break;
				
			default:
				assert(0);
			}
			
			vertexPoint = vertexNextPoint;
			iPipeIndex1 = iPipeIndex2;
			iSegment1   = iSegment2;
		}

		//////////////////////////////////////////////////////////////////////////
		// to point
//		// TRACE( "to point:\r\n" );
		vertexPoint = _shortestPath.getItem( iPointCount-2 );
		pipe		= GetPipeAt( vertexPoint.getPipeIndex() );
		iPipeIndex1		= vertexPoint.getPipeIndex();
//		// TRACE(">>>>> pipe index: %d\r\n", iPipeIndex1 );
		// percent
		switch( vertexPoint.getVertexType() )
		{
		case POINT2PIPE:
			point2Pipe = vertexPoint.getPt2Pipe();
			point = pipe->GetDestPoint( point2Pipe, _nPercent );

//			// TRACE(">>>>> point type: point to pipe\r\n>>>>> segment :don't know\r\n" );
			break;
			
		case POINTONPIPE:
			pointOnPipe = vertexPoint.getPtOnPipe();
			point = pipe->GetDestPoint( pointOnPipe.iSegment, _nPercent );
//			// TRACE(">>>>> point type: point on pipe\r\n>>>>> segment :don't know\r\n" );
			break;
			
		case PIPE2PIPE:
			pipe2Pipe = vertexPoint.getPipe2Pipe();
			point = pipe->GetDestPoint( pipe2Pipe, _nPercent );
			if( mobPointList[iMobPoint].OrderChanged() )
				_nPercent = 100 - _nPercent; 
			iMobPoint++;
//			// TRACE(">>>>> point type: pipe to pipe\r\n>>>>> segment :don't know\r\n" );
			break;
			  
		default:
			assert(0);
		}
		temp.SetPoint( point );
		temp.SetPipeIdx(vertexPoint.getPipeIndex());
		_pointList.push_back( temp );
		//////////////////////////////////////////////////////////////////////////
		
	}

	return _pointList.size();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPipeDataSet::preDealThePath( CGraphVertexList& _shortestPath, std::vector<CMobPipeToPipeXPoint>& mobPointList )
{
	CGraphVertex vertex;
	CMobPipeToPipeXPoint mobPoint;

	mobPointList.clear();
	for( int i =0; i<_shortestPath.getCount(); i++ )
	{
		vertex = _shortestPath.getItem( i );
		if( vertex.getVertexType() == PIPE2PIPE )
		{
			mobPoint.SetPipeToPipeXPoint( vertex.getPipe2Pipe() );
			if( mobPointList.size() >1 )
			{
				mobPointList[ mobPointList.size() -1 ].SetOutInc( mobPoint );	
			}
			mobPointList.push_back( mobPoint );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	/*if( mobPointList.size() >1 )
	{
		mobPointList[ mobPointList.size() -1 ].SetOutInc( mobPoint );	
	}*/

}

CPipe* CPipeDataSet::FindPipeByGuid( const CGuid& guid, const GUID& cls_guid )
{
	if (cls_guid == CPipe::getTypeGUID())
	{
		for (PIPEVECTOR::iterator ite=m_vPipes.begin();ite!=m_vPipes.end();ite++)
		{
			if (ite->getGuid() == guid)
				return &(*ite);
		}
	}
	ASSERT(FALSE);
	return NULL;
}

void CPipeDataSet::SetLandsideInput( InputLandside* pLandsideInput )
{
	m_pInputLandside = pLandsideInput;
}
