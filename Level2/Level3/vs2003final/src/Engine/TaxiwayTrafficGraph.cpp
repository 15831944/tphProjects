#include "StdAfx.h"
#include ".\taxiwaytrafficgraph.h"

#include "proclist.h"
#include "terminal.h"
#include "../Inputs/AirsideInput.h"

TaxiwayTrafficGraph::TaxiwayTrafficGraph(void)
{
}

TaxiwayTrafficGraph::~TaxiwayTrafficGraph(void)
{
	ClearGraph();
}

//for redo
TaxiwayTrafficGraph& TaxiwayTrafficGraph::operator = (const TaxiwayTrafficGraph rhs_ )
{
	
	if(this==&rhs_)
		return *this;

	m_vVertexs = rhs_.m_vVertexs;
	m_vEdges = rhs_.m_vEdges;
	m_ProcList = rhs_.m_ProcList;
	m_pTerminal = rhs_.m_pTerminal;
	m_strPrjPath = rhs_.m_strPrjPath;

	return *this;
}

void TaxiwayTrafficGraph::Initialize(Terminal* _terminal, const CString& projPath)
{
	ASSERT(_terminal != NULL);
	m_pTerminal = _terminal;
	m_strPrjPath = projPath;


	ProcessorList* pProcList = _terminal->GetAirsideInput()->GetProcessorList();
	if (pProcList == NULL)
		return;
	int nCount = pProcList->getProcessorCount();
	for( int i=0; i<nCount; ++i)
	{
		if(pProcList->getProcessor(i)->getProcessorType() == TaxiwayProcessor)
		{
			TaxiwayProc* pProc = (TaxiwayProc*)pProcList->getProcessor(i);
			pProc->SetDirty(FALSE);
			m_ProcList.push_back(pProc);
			pProc->SetTrafficGraph(this);
		}
	}
}

void TaxiwayTrafficGraph::ClearGraph()
{
	m_vVertexs.clear();
	m_vEdges.clear();
	m_ProcList.clear();
}

bool TaxiwayTrafficGraph::AddTaxiwayProc(TaxiwayProc *pProc, bool bInit)
{
	ASSERT(pProc != NULL);

	m_ProcList.push_back(pProc);
	pProc->SetTrafficGraph(this);	
	pProc->SetDirty(TRUE);

	return true;
}

bool TaxiwayTrafficGraph::DeleteTaxiwayProc(TaxiwayProc *pProc)
{
	ASSERT(pProc != NULL);
	TaxiwayItr iter = std::find(m_ProcList.begin(), m_ProcList.end(), pProc);
	if (iter == m_ProcList.end())
		return false;
	
	m_ProcList.erase(iter);
	RebuildAll();
	return false;
}


bool TaxiwayTrafficGraph::UpdateTaxiwayProc(TaxiwayProc *pProc)
{
	ASSERT(pProc != NULL);

	if( m_ProcList.end() == std::find(m_ProcList.begin(), m_ProcList.end(), pProc))
		return false;

	pProc->SetDirty(TRUE);

	return true;
}

void TaxiwayTrafficGraph::RebuildAll()
{
	m_vVertexs.clear();
	m_vEdges.clear();

	//Update Current Procs
	for(TaxiwayItr iter = m_ProcList.begin(); iter != m_ProcList.end();iter++)
		addPath((*iter)->serviceLocationPath());

	// update segment list
	for (TaxiwayItr iter = m_ProcList.begin(); iter != m_ProcList.end(); iter++)
		RefreshTaxiwayProcInfo(*iter);

	//Save Changes
	ProcessorList* pProcList = m_pTerminal->GetAirsideInput()->GetProcessorList();
	pProcList->saveDataSet(m_strPrjPath, false);
}


//Note:all procs which intersects with source proc need redefine direction list
void TaxiwayTrafficGraph::RefreshTaxiwayProcInfo(TaxiwayProc* pProcSource, bool bInit)
{
	TaxiwayProc::SegmentList&segList = pProcSource->GetSegmentListDirect();

	Path* pPath = pProcSource->serviceLocationPath();
	ASSERT(pPath != NULL);
	std::vector<Point> intersectPoints;
	for (vexItr it = m_vVertexs.begin(); it != m_vVertexs.end(); it++)
	{	
		//include path point
		if(	IsPointOnPath(pPath, it->first) || pPath->within(it->first))
			intersectPoints.push_back(it->first);
	}

	TaxiwayTrafficGraph::SortProcIntersections(pProcSource,intersectPoints);

	
	//Save Temp Data
	std::vector<TaxiwayProc::DirectionType> vOldType;
	for (TaxiwayProc::SegmentItr iter = segList.begin(); iter != segList.end(); iter++)
		vOldType.push_back(iter->emType);
	segList.clear();

	for (int i=0; i< (int)intersectPoints.size()-1; i++)
	{
		TaxiwayProc::TaxiwaySegment segNew;
		segNew.StartPoint = intersectPoints[i];
		segNew.EndPoint = intersectPoints[i+1];
		if (i<(int)vOldType.size())
			segNew.emType = vOldType[i];
		segList.push_back(segNew);
	}
}

void TaxiwayTrafficGraph::SortProcIntersections(TaxiwayProc* pProc, std::vector<Point>&vPoints)
{
	Path* pPath = pProc->serviceLocationPath();
	int nCount = pPath->getCount();
	Path tempPath = *pPath;
	std::vector<Point> realIntersectPt;
	
	//find real intersect point
	for (PtItr iter = vPoints.begin(); iter != vPoints.end(); iter++)
		if (!IsServicelocationVertex(pProc, *iter))/*pPath->IsVertex(*iter)*/
			realIntersectPt.push_back(*iter);

	//Insert Points by order
	for(PtItr iter = realIntersectPt.begin(); iter != realIntersectPt.end(); iter++)
	{
		for (int i=1 ;i<tempPath.getCount(); i++)
		{
			Line ln(tempPath.getPoint(i-1), tempPath.getPoint(i));
			if (ln.contains(*iter))
			{
				tempPath.insertPointAfterAt(*iter, i-1);
				break;
			}
			else
			{
				double dist1 = (*iter).getDistanceTo(tempPath.getPoint(i-1));
				double dist2 = (*iter).getDistanceTo(tempPath.getPoint(i));
				if (dist1 - dist2 < minimize_distance)////pProc->GetWidth()
				{
					tempPath.insertPointAfterAt(*iter, i-1);
					break;
				}
			}

		}
	}

	vPoints.clear();
	for(int i=0; i<tempPath.getCount(); i++)
		vPoints.push_back(tempPath.getPoint(i));


}

bool TaxiwayTrafficGraph::IsPointOnPath(Path *path_, Point& point)
{
	for (int i=1; i<path_->getCount(); i++)
	{
		double dist1 =point.getDistanceTo(path_->getPoint(i-1))+point.getDistanceTo(path_->getPoint(i));
		double dist2 = (path_->getPoint(i-1) - path_->getPoint(i)).length();	

		if (dist1 - dist2 < minimize_distance )
			return true;
	}
	return false;
}


bool TaxiwayTrafficGraph::IsServicelocationVertex(TaxiwayProc* pProc, Point &point)
{
	ASSERT(pProc != NULL);

	Path * pPath = pProc->serviceLocationPath();
	double nWidth = pProc->GetWidth();

	for (int i=0; i<pPath->getCount(); i++)
	{
		Point pt = pPath->getPoint(i);
		if (point.distance(pt) < nWidth)//
			return true;
	}
	return false;
}


//------------------------------------------------------------------------
//add vertex make sure no same vertex
void TaxiwayTrafficGraph::addVertex( const vertex & newvex){

	if( m_vVertexs.end() == find( m_vVertexs.begin(), m_vVertexs.end(), newvex) )
		m_vVertexs.push_back( newvex );
}
//add edge make sure no same edge; filter out invalid edge
void TaxiwayTrafficGraph::addEdge( const edge & newedge){
	if(newedge.first == newedge.second )return;
	//find dupilcate edge
	for(edgeItr itr = m_vEdges.begin(); itr!= m_vEdges.end(); itr++)
	{
		if( (*itr).first==newedge.second && (*itr).second == newedge.first )
			return;
	}
	if( m_vEdges.end() == find ( m_vEdges.begin(), m_vEdges.end(), newedge ) )
		m_vEdges.push_back( newedge );

}

//find the same point id else return -1;
int TaxiwayTrafficGraph::findvertex( const Point& newPt ){
	vexItr itr;
	for( itr=m_vVertexs.begin(); itr!=m_vVertexs.end(); itr++){
		if( newPt.distance( (*itr).first ) < minimize_distance  ){
			return (*itr).second;
		}
	}
	return -1;
}

//get vertex iterator of id;
TaxiwayTrafficGraph::vexItr TaxiwayTrafficGraph::getVertex( int id ){
	vexItr itr= m_vVertexs.begin();
	for(;itr!=m_vVertexs.end();itr++){
		if( (*itr).second == id )return itr;
	}
	return itr;
}

//add a new vertex to the graph and return the id; if this new vertex is in the edge ,then it will split the edge.
int TaxiwayTrafficGraph::addVertex( const Point & newPt){
	int id=findvertex(newPt);
	if(id == -1){		
		vertex newvex( newPt, m_vVertexs.size()+1 );
		addVertex( newvex );
		id = newvex.second;
		edgeItr itr;
		for( itr=m_vEdges.begin(); itr!=m_vEdges.end(); itr++ ){
			double dist =(*getVertex( (*itr).first )).first.getDistanceTo( newPt) + (*getVertex( (*itr).second )).first.getDistanceTo(newPt) ;
			double dist2 = ((*getVertex( (*itr).first )).first ).getDistanceTo((*getVertex( (*itr).second )).first);			
			if( dist - dist2 < minimize_distance ){
				addEdge( edge((*itr).first, id) );
				addEdge( edge( id, (*itr).second ) );
				m_vEdges.erase(itr);
				break;
			}

		}
	}	
	return id;
}
// add an line to the graph ,if intersect other ,it will generate new vertexs and edges 
void TaxiwayTrafficGraph::addEdge( const Point & pFrom , const Point & pTo ){
	std::vector< int > PtIDList;
	PtIDList.push_back( addVertex( pFrom ) );
	std::vector<Point> & intersectPt = intersectPoints( pFrom, pTo );

	for(size_t i=0; i<intersectPt.size(); i++)
		PtIDList.push_back( addVertex( intersectPt[i] ) );
	PtIDList.push_back( addVertex(pTo) );
	int nPtCount = PtIDList.size();
	for(int i=0; i<nPtCount-1; i++){
		addEdge( edge(PtIDList[i],PtIDList[i+1]) );
	}

}

// in: a line Point from , Point to
// out: point list of intersection in order
std::vector<Point> TaxiwayTrafficGraph::intersectPoints( const Point & pFrom , const Point & pTo ){
	std::vector<Point> interPts;
	Line line1(pFrom,pTo);
	edgeItr itr;
	for( itr=m_vEdges.begin(); itr!=m_vEdges.end(); itr++ ){
		Line line2( (*getVertex((*itr).first)).first, (*getVertex((*itr).second)).first );
		//
		Point p1 = (*getVertex( (*itr).first )).first;
		Point p2 = (*getVertex( (*itr).second )).first;
		double dist =p1.getDistanceTo(pFrom)+p1.getDistanceTo(pTo);
		double dist2 = (pFrom - pTo).length();			
		if( dist - dist2 < minimize_distance ){
			interPts.push_back(p1);
			continue;
		}
		dist =p2.getDistanceTo(pFrom)+p2.getDistanceTo(pTo);
		dist2 = (pFrom - pTo).length();					
		if( dist - dist2 < minimize_distance ){
			interPts.push_back(p2);
			continue;
		}
		if( line1.intersects(line2) ){			  
			interPts.push_back( line1.intersection( line2 ) );
		}

	}
	//sort the point; nnn
	std::vector<Point>reslt;
	for(size_t i=0;i<interPts.size();i++){
		double dist = pFrom.distance( interPts[i] );
		std::vector<Point>::iterator ptitr;
		for(ptitr=reslt.begin();ptitr!=reslt.end();ptitr++){
			if( dist < pFrom.distance( *ptitr ) )break;
		}
		if(ptitr == reslt.end())reslt.push_back( interPts[i] );
		else reslt.insert( ptitr,interPts[i] );
	}

	return reslt;
}

int TaxiwayTrafficGraph::addPath(const Path *path_){

	ASSERT(path_);
	for(int i=0;i<path_->getCount()-1;i++){
		addEdge( path_->getPoint(i),path_->getPoint(i+1) );
	}

	return 0;
}