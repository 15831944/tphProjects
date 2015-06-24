// TaxiwayProc.cpp: implementation of the TaxiwayProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TaxiwayProc.h"
#include "../Common/line.h"
#include "RunwayProc.h"
#include "./StandProc.h"

#include "TaxiwayTrafficGraph.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TaxiwayProc::TaxiwayProc()
{	
	m_bEditable=false;
	m_dWidth=1000.0;
	//mark
	m_imarkPosAfterPtID=0;
	m_dmarkPosRelatePos=0.0;	
	m_sMarking="";

	m_pTrafficGraph = NULL;
	m_bDirty = TRUE;
}

TaxiwayProc::~TaxiwayProc()
{

}
void TaxiwayProc::SnapTo(Processor * proc)
{
	if(proc->getProcessorType() == RunwayProcessor  )
	{
		//snap this taxiway to the runway
		RunwayProc *pRunway=(RunwayProc *)proc;
		double dwidth=pRunway->GetWidth();
		Point end1=m_location.getPoint(0);
		Point snapPoint=GetSnapPoint(pRunway->serviceLocationPath(),dwidth,end1);
		m_location[0]=snapPoint;
		Point end2=m_location.getPoint(m_location.getCount()-1);
		snapPoint=GetSnapPoint(pRunway->serviceLocationPath(),dwidth,end2);
		m_location[m_location.getCount()-1]=snapPoint;
		//Need Reset TaxiwayGraph,reset intersection and derections
		SetDirty(TRUE);
		//if (m_pTrafficGraph)
		//	m_pTrafficGraph->UpdateTaxiwayProc(this);
		
	}
	else if(proc->getProcessorType() == TaxiwayProcessor)
	{
		TaxiwayProc *pTaxiway=(TaxiwayProc *)proc;
		double dwidth=pTaxiway->GetWidth();
		Point end1=m_location.getPoint(0);
		Point snapPoint=GetSnapPoint(pTaxiway->serviceLocationPath(),dwidth,end1);
		m_location[0]=snapPoint;
		Point end2=m_location.getPoint(m_location.getCount()-1);
		snapPoint=GetSnapPoint(pTaxiway->serviceLocationPath(),dwidth,end2);
		m_location[m_location.getCount()-1]=snapPoint;
		
		//Need Reset TaxiwayGraph,reset intersection and derections
		SetDirty(TRUE);
		//if (m_pTrafficGraph)
		//	m_pTrafficGraph->UpdateTaxiwayProc(this);
	}
	else
	{NULL;}
	CalculateSegment();
}
void TaxiwayProc::autoSnap(ProcessorList *procList)
{
	//snap end points to runways//snap end points to taxiways
	int nProcCount =procList->getProcessorCount();
	for(int i=0;i<nProcCount;i++)
	{
		Processor * pProc=procList->getProcessor(i);
		if(pProc->getProcessorType()== RunwayProcessor 
			||pProc->getProcessorType() == TaxiwayProcessor )
		{
			SnapTo(pProc);
		}
	}
	//make the stand proc snap to self
	for(int i=0;i<nProcCount;i++){
		Processor * pProc=procList->getProcessor(i);
		if(pProc->getProcessorType() ==TaxiwayProcessor)
		{
			((TaxiwayProc * )pProc)->SnapTo(this);
		}
		else if(pProc->getProcessorType() == StandProcessor)
		{
			((StandProc *)pProc)->SnapTo(this);
		}
	}
}
void TaxiwayProc::SetMarkingPosition(UINT ptID, double relatePos){
	UINT nCount=m_location.getCount();
	if( ptID <nCount-1 ){
		m_imarkPosAfterPtID=ptID;
	}else m_imarkPosAfterPtID=nCount-2;

	m_dmarkPosRelatePos=relatePos;
	
}
void TaxiwayProc ::getMarkingPosition(Point & startPos, double  & rotateAngle ){
	if(m_imarkPosAfterPtID >m_location.getCount()-2) m_imarkPosAfterPtID=m_location.getCount()-2;

	Line _line( m_location.getPoint(m_imarkPosAfterPtID),m_location.getPoint(m_imarkPosAfterPtID+1));
	startPos= _line.getInlinePoint(m_dmarkPosRelatePos);
	ARCVector2 k;
	k.reset(m_location.getPoint(m_imarkPosAfterPtID),m_location.getPoint(m_imarkPosAfterPtID+1));
	rotateAngle = k.AngleFromCoorndinateX();
}

void TaxiwayProc::removePoint(UINT ptID){
	ASSERT(ptID<(UINT)m_location.getCount());
	ASSERT(ptID<m_vSegments.size());
	std::vector<TAXIWAYSEGMENT> ::iterator itr=m_vSegments.begin();
	for(size_t i=0;i<ptID;i++){
		itr++;
	}
	m_vSegments.erase(itr);
	m_location.RemovePointAt(ptID);
	initSegments();
}
int TaxiwayProc::readSpecialField(ArctermFile& procFile)
{
	//width
	procFile.getFloat( m_dWidth );
	m_dWidth *=SCALE_FACTOR;
	//marking
	char sBuf[256] ="";
	procFile.getField( sBuf, 256 );
	m_sMarking = sBuf;
	procFile.getInteger(m_imarkPosAfterPtID);
	procFile.getFloat(m_dmarkPosRelatePos);
	//segments
	int nCount;
	procFile.getInteger(nCount);
	m_vSegments.clear();
	for(int i=0;i<nCount;i++){
		TAXIWAYSEGMENT segment;
		procFile.getFloat(segment.ptdir[VX]) ;procFile.getFloat(segment.ptdir[VY]) ;
		procFile.getFloat(segment.length_left);procFile.getFloat(segment.length_right);
		m_vSegments.push_back(segment);
	}

	//Direction List
	int nDirectionCount;
	procFile.getInteger(nDirectionCount);
	m_segmentList.clear();
	for (int i=0; i<nDirectionCount; i++)
	{
		TaxiwaySegment seg;
		procFile.getPoint(seg.StartPoint);
		procFile.getPoint(seg.EndPoint);
		int nType;
		procFile.getInteger(nType);

		//Start&End Point will updated by TaxiwayTrafficGraph
		seg.emType = (DirectionType)nType;
		m_segmentList.push_back(seg);
	}

	return TRUE;
}
int TaxiwayProc::writeSpecialField(ArctermFile& procFile) const
{
	//width
	procFile.writeFloat( (float)(m_dWidth/SCALE_FACTOR) );
	//marking
	procFile.writeField( m_sMarking.c_str() );
	procFile.writeInt(m_imarkPosAfterPtID);
	procFile.writeFloat((float)m_dmarkPosRelatePos);
	//
	int nCount=m_vSegments.size();

	procFile.writeInt(nCount);
	for(int i=0;i<nCount;i++){
		const TAXIWAYSEGMENT &segment=m_vSegments[i];
		procFile.writeFloat((float)segment.ptdir[VX]) ;procFile.writeFloat((float)segment.ptdir[VY]) ;
		procFile.writeFloat((float)segment.length_left);procFile.writeFloat((float)segment.length_right);
	}

	// write  deraction list(intersection point list info can generated
	// with TaxiwayTrafficGraph according to taxiways position )
	int nDirectionCount = m_segmentList.size();
	procFile.writeInt(nDirectionCount);
	for(int i=0; i<nDirectionCount; i++)
	{
		procFile.writePoint(m_segmentList[i].StartPoint);
		procFile.writePoint(m_segmentList[i].EndPoint);
		procFile.writeInt((int)m_segmentList[i].emType);
	}

	return TRUE;
}
void TaxiwayProc::SetWidth(double width){
	m_dWidth=width;
	initSegments();
}

void TaxiwayProc::initServiceLocation (int pathCount, const Point *pointList)
{
    if (pathCount < 2)
        throw new StringError ("Processor must have not less than 2 service m_location");//must >=2

    m_location.init (pathCount, pointList);

	//RecalcGeometry();
}

void TaxiwayProc::initSegments(){
	DistanceUnit defaultlength=m_dWidth/2.0;
	UINT nptCount=m_location.getCount();
	if(nptCount<2)return;
	m_vSegments.clear();
	Path & path=m_location;
	Point  p_pre=path.getPoint(0);
	Point  p_this=path.getPoint(0);
	Point  p_next=path.getPoint(1);
	//get first segment;	
	ARCVector2 k;
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();	

	TAXIWAYSEGMENT taxiseg;
	taxiseg.ptdir=k;
	taxiseg.length_left=taxiseg.length_right=defaultlength;
	m_vSegments.push_back(taxiseg);

	//get mid segment;
	ARCVector2 k1;
	ARCVector2 k2;
	for(UINT i=1;i<nptCount-1;i++)
	{		
		p_this=path.getPoint(i);
		p_next=path.getPoint(i+1);
		k1.reset(p_pre,p_this);
		k2.reset(p_this,p_next);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(90);
		k.Normalize();		

		taxiseg.ptdir=k;
		
		double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );		
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);
		taxiseg.length_left=taxiseg.length_right=defaultlength*dAdjustedWidth;
			
		m_vSegments.push_back(taxiseg);

		p_pre=p_this;
		p_this=p_next;

	}
	//get the last segment ;
	p_this=path.getPoint(nptCount-2);
	p_next=path.getPoint(nptCount-1);
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();	
	taxiseg.ptdir=k;
	taxiseg.length_left=taxiseg.length_right=defaultlength;	
	m_vSegments.push_back(taxiseg);
}
void TaxiwayProc::CalculateSegment(){
	
	UINT nptCount=m_location.getCount();
	if(nptCount<2)return;	
	
	Path & path=m_location;
	Point  p_pre=path.getPoint(0);
	Point  p_this=path.getPoint(0);
	Point  p_next=path.getPoint(1);
	//get first segment;	
	ARCVector2 k;
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();	
	if((int)m_vSegments.size()!=nptCount)initSegments();
	std::vector<TAXIWAYSEGMENT>::iterator segitr=m_vSegments.begin();
	
	(*segitr).ptdir=k;
	segitr++;
	//get mid segment;
	ARCVector2 k1;
	ARCVector2 k2;
	for(UINT i=1;i<nptCount-1;i++)
	{		
		
		p_this=path.getPoint(i);
		p_next=path.getPoint(i+1);
		k1.reset(p_pre,p_this);
		k2.reset(p_this,p_next);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(90);
		k.Normalize();		
		/*double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);		*/
		(* segitr).ptdir=k;
		
		/*(* segitr).length_left= m_dWidth*dAdjustedWidth;
		(* segitr).length_right= m_dWidth * dAdjustedWidth;*/

		segitr++;
		p_pre=p_this;
		p_this=p_next;

	}
	//get the last segment ;
	p_this=path.getPoint(nptCount-2);
	p_next=path.getPoint(nptCount-1);
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();	
	(*segitr).ptdir=k;
	
}
void TaxiwayProc::AddNewPoint(const Point& newPtPos ){
	//get the nearest line segment
	int recordpos=-1;
	int nCount=m_location.getCount();
	Point newaddpt;
	double minDist=10000.0;
	for(int i=0;i<nCount-1;i++){
		Point & p1=m_location.getPoint(i);
		Point & p2=m_location.getPoint(i+1);
		//get distance to the line (p1,p2);
		DistanceUnit _dist=abs((p2.getY()-p1.getY())*(newPtPos.getX()-p1.getX())-(newPtPos.getY()-p1.getY())*(p2.getX()-p1.getX()))/p1.getDistanceTo(p2);
		if(_dist<minDist){			
			Line prjline(p1,p2);
			Point prjPt=prjline.getProjectPoint(newPtPos);
			if(	prjline.comprisePt(prjPt)){
				newaddpt=prjPt;
				recordpos=i;
				minDist=_dist;				
			}
		}
	
	}
	
	//add segments 
	if(recordpos<0)return;
	if(recordpos<m_imarkPosAfterPtID)m_imarkPosAfterPtID++;
	std::vector<TAXIWAYSEGMENT>::iterator itr;
	itr=m_vSegments.begin();
	for(int i=0;i<recordpos+1;i++){
		itr++;
	}

	TAXIWAYSEGMENT newsegment;	
	double ratio=(newaddpt-m_location.getPoint(recordpos)).length()/(m_location.getPoint(recordpos+1)-newaddpt).length();
	
	newsegment.ptdir=m_vSegments[recordpos].ptdir + m_vSegments[recordpos+1].ptdir * ratio;
	newsegment.ptdir.Normalize();
	ARCVector2 vline;
	vline.reset(m_location[recordpos],m_location[recordpos+1]);
	vline.Rotate(90);
	double dcos=newsegment.ptdir.GetCosOfTwoVector(vline);
	double dcospre=m_vSegments[recordpos].ptdir.GetCosOfTwoVector(vline);
	newsegment.length_left=m_vSegments[recordpos].length_left *dcospre /dcos;
	newsegment.length_right=m_vSegments[recordpos].length_right *dcospre /dcos;

	m_vSegments.insert(itr,newsegment);
	//add point to the path;
	m_location.insertPointAfterAt(newaddpt,recordpos);
	//CalculateSegment();

	SetDirty(TRUE);
//	if (m_pTrafficGraph)
//		m_pTrafficGraph->UpdateTaxiwayProc(this);

}
//Move sidePoint change the 
void TaxiwayProc::MoveSidePoint(DistanceUnit dx,DistanceUnit dy,UINT ceterPointID,TaxiwayEnum whichside){
	UINT nCount=m_vSegments.size();
	ASSERT( ceterPointID < nCount );
	
	TAXIWAYSEGMENT & segment=m_vSegments[ceterPointID];
	Point &centerPt=m_location.getPoint(ceterPointID);
	Point  sidePt;
	DistanceUnit length= (whichside==LEFTSIDE)?segment.length_left:-segment.length_right;
	sidePt.setX(centerPt.getX()+length*segment.ptdir[VX]+dx);
	sidePt.setY(centerPt.getY()+length*segment.ptdir[VY]+dy);
	ARCVector2 dir;
	if(whichside==LEFTSIDE)
	{
		dir.reset(centerPt,sidePt);
		segment.length_left=dir.Length();
	
	}
	else{
		dir.reset(sidePt,centerPt);
		segment.length_right=dir.Length();
	}
	dir.Normalize();
	segment.ptdir=dir;	
}
void TaxiwayProc::MoveCenterPoint(DistanceUnit dx,DistanceUnit dy,UINT centerPointID)
{
	ASSERT(centerPointID< (UINT)m_location.getCount());
	Point & pt=m_location[centerPointID];
	pt.setX(pt.getX()+dx);pt.setY(pt.getY()+dy);
	initSegments();
}
void TaxiwayProc ::MoveMarkPosition(const Point & newPtPos){
	int recordpos=m_imarkPosAfterPtID;
	double relatePos=m_dmarkPosRelatePos;
	int nCount=m_location.getCount();	
	double minDist=m_dWidth*2.0;
	for(int i=0;i<nCount-1;i++){
		Point & p1=m_location.getPoint(i);
		Point & p2=m_location.getPoint(i+1);
		//get distance to the line (p1,p2);
		DistanceUnit _dist=abs((p2.getY()-p1.getY())*(newPtPos.getX()-p1.getX())-(newPtPos.getY()-p1.getY())*(p2.getX()-p1.getX()))/p1.getDistanceTo(p2);
		if(_dist<minDist){				
			double r;
			ARCVector3 _line(p1,p2);
			r=( ARCVector3(p1,newPtPos).dot(_line) )/_line.Magnitude2();
			if(r<1 && r>0){
				relatePos=r;
				minDist=_dist;
				recordpos=i;
			}			
		}

	}
	if(recordpos>=0){
		NULL;
	}else {
		relatePos=0;
		recordpos=0;
	}
	m_dmarkPosRelatePos=relatePos;
	m_imarkPosAfterPtID=recordpos;
}

Processor* TaxiwayProc::CreateNewProc()
{
	Processor* pProc = new TaxiwayProc;
	return pProc;
}

bool TaxiwayProc::CopyOtherData(Processor* _pDestProc)
{
	TaxiwayProc* pProc = ((TaxiwayProc *)_pDestProc);

	pProc->SetWidth(GetWidth());
	pProc->SetMarking(GetMarking());

	return true;
}


TaxiwayProc::SegmentList& TaxiwayProc::GetSegmentList()
{
	if (m_bDirty)
		m_pTrafficGraph->RebuildGraph();

	m_bDirty = FALSE;

	return m_segmentList;
}
