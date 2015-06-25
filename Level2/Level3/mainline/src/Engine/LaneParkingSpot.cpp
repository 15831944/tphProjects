#include "stdafx.h"
#include "LaneParkingSpot.h"
#include "Common/CPPUtil.h"
#include "LandsideStretchInSim.h"
#include "LandsideVehicleInSim.h"
#include "LandsideIntersectionInSim.h"
#include "LandsideResourceManager.h"
#include "Landside/LandsideLinkStretchObject.h"
#include "LaneSegInSim.h"

LaneParkingSpot::LaneParkingSpot( LandsideLaneInSim * plane , DistanceUnit distf, DistanceUnit distT, LandsideResourceInSim* pParent )
{
	m_pParent = pParent;
	mdistF = distf;
	mdistT = distT;
	mpOnLane = plane;
	pLeft = pRight = pHead = pBehind = NULL;

	double dIndexInLane = mpOnLane->GetPath().GetDistIndex( 0.5*(distf+distT));
	m_pos = mpOnLane->GetPath().GetIndexPoint(dIndexInLane);
	m_dir = mpOnLane->GetPath().GetIndexDir(dIndexInLane);
	m_dir.Normalize();

	m_startPos = mpOnLane->GetPath().GetDistPoint(distf);
	m_endPos = mpOnLane->GetPath().GetDistPoint(distT);
}


void LaneParkingSpot::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t )
{
	GetLane()->OnVehicleEnter(pVehicle,dist,t);
	__super::OnVehicleEnter(pVehicle,dist,t);
}

void LaneParkingSpot::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	GetLane()->OnVehicleExit(pVehicle,t);
	__super::OnVehicleExit(pVehicle,t);
}


InLaneParkingSpotsGroup::~InLaneParkingSpotsGroup()
{
	cpputil::deletePtrVector(mvParkingSpots);
}

void InLaneParkingSpotsGroup::addLane( LandsideStretchLaneInSim* pLane, DistanceUnit distF, DistanceUnit distT,DistanceUnit spotlength,LandsideResourceInSim* pParent )
{
	pLane->SetHasParkingSpot(true);
	int nCount  =  MAX(1, static_cast<int>((distT - distF)/spotlength) );
	DistanceUnit distInc =  distF; 
	LaneParkingSpot* pSpot = NULL;
	for(int i=0;i<nCount;i++)
	{
		pSpot = new LaneParkingSpot(pLane,distInc, distInc + spotlength, pParent);
		distInc += spotlength;
		pSpot->m_index = i;
		mvParkingSpots.push_back(pSpot);		
	}

	pLane->SetHasParkingSpot(true);
}

LandsideResourceInSim* InLaneParkingSpotsGroup::getLeft( LaneParkingSpot* pthis )const
{
	LandsideLaneInSim* LeftLane = pthis->mpOnLane->getLeft();
	if(LeftLane){
		if(LaneParkingSpot* pSpot = getSpot(LeftLane,pthis->m_index)){
			return pSpot;
		}
	}
	return LeftLane;
}

LaneParkingSpot* InLaneParkingSpotsGroup::getSpot( LandsideLaneInSim* pLane, int idx ) const
{
	for(size_t i=0;i<mvParkingSpots.size();i++){
		LaneParkingSpot* pspot = mvParkingSpots[i];
		if(pspot->mpOnLane == pLane
			&& pspot->m_index == idx){
				return pspot;
			}
	}
	return NULL;
}

LaneParkingSpot* InLaneParkingSpotsGroup::getSpot( LandsideVehicleInSim* pVehicle )const
{
	for(size_t i=0;i<mvParkingSpots.size();i++){
		LaneParkingSpot* pspot = mvParkingSpots[i];
		if(pspot->GetPreOccupy() == pVehicle){
				return pspot;
			}
	}
	return NULL;
}

LandsideResourceInSim* InLaneParkingSpotsGroup::getRight( LaneParkingSpot* pthis )const
{
	LandsideLaneInSim* rightlane = pthis->mpOnLane->getRight();
	if(rightlane){
		if(LaneParkingSpot* pSpot = getSpot(rightlane,pthis->m_index)){
			return pSpot;
		}
	}
	return rightlane;
}

LandsideResourceInSim* InLaneParkingSpotsGroup::getHead( LaneParkingSpot* pthis )const
{
	if(pthis->m_index >0 ){
		return getSpot(pthis->mpOnLane,pthis->m_index-1);
	}
	return pthis->mpOnLane;
}

LaneParkingSpot* InLaneParkingSpotsGroup::FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )const
{
	LandsideResourceInSim* pAtRes = pVehicle->getLastState().getLandsideRes();
	if(!pAtRes) return NULL;
	
	if(LandsideLaneInSim* pAtLane = pAtRes->toLane())
	{
		return FindParkingPos(pVehicle,pAtLane,followPath);

	}
	else if( LandsideIntersectLaneLinkInSim* pLaneLink = pAtRes->toIntersectionLink() )
	{
		LandsideLaneInSim* plane = pLaneLink->getToNode()->mpLane;
		return FindParkingPos(pVehicle,plane,followPath);
	}	
	return NULL;
}

LaneParkingSpot* InLaneParkingSpotsGroup::FindParkingPosRandom( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* pLane , LandsideLaneNodeList& followPath ) const
{
	ASSERT(pLane);

	const CPoint2008& dFromPos =  pVehicle->getLastState().pos;	
	std::vector<LaneParkingSpot*> SpotsCopy = mvParkingSpots;
	std::random_shuffle(SpotsCopy.begin(),SpotsCopy.end());

	for(size_t i=0;i<SpotsCopy.size();i++)
	{
		LaneParkingSpot* pSpot = SpotsCopy[i];
		if(pSpot->GetPreOccupy()==NULL)
		{
			ASSERT(pSpot->GetPreOccupy()!=pVehicle);
			if(FindParkspotPath(pLane,dFromPos,pSpot,followPath))
			{
				return pSpot;
			}
		}		
	}
	return NULL;
}

LaneParkingSpot* InLaneParkingSpotsGroup::FindParkingPosRandom( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath ) const
{
	LandsideResourceInSim* pAtRes = pVehicle->getLastState().getLandsideRes();
	if(!pAtRes) return NULL;
	LandsideLaneInSim* pAtLane = pAtRes->toLane();
	if(!pAtLane) return NULL;
	return FindParkingPosRandom(pVehicle,pAtLane,followPath);
}

//
//bool LaneParkingSpotsGroup::FindLeavePath( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )
//{
//	LaneParkingSpot* pSpot = getSpot(pVehicle);
//	//ASSERT(pSpot);
//	if(!pSpot)
//		return false;
//	return FindLeavePath(pVehicle,pSpot,followPath);	
//}

bool InLaneParkingSpotsGroup::FindLeavePath( LandsideVehicleInSim* pVehicle, LaneParkingSpot* pSpot, LandsideLaneNodeList& followPath )
{
	LandsideLaneInSim* pAtLane = pSpot->GetLane();
	CPoint2008 dAtPos = pVehicle->getLastState().pos;

	CPoint2008 extPos;
	if(LandsideLaneInSim* pExitLane = FindPosExitToLane(pSpot,extPos))
	{

		//////////////////////////////////////////////////////////////////////////
		LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();		
		pLaneEntry->SetPosition(pAtLane,dAtPos);
		pLaneEntry->SetFromRes(pSpot);
		followPath.push_back(pLaneEntry);

		if(pAtLane!=pExitLane)
		{
			LandsideLaneExit* pLaneExit = new LandsideLaneExit();
			pLaneExit->SetPosition(pAtLane, pLaneEntry->m_distInlane + 100 );
			pLaneExit->SetToRes(pExitLane);
			followPath.push_back(pLaneExit);		

			LandsideLaneEntry* pLastEntry = new LandsideLaneEntry();
			pLastEntry->SetPosition(pExitLane,extPos);			
			pLastEntry->SetFromRes(pAtLane);
			followPath.push_back(pLastEntry);
		}

		LandsideLaneExit* pLastLaneExit = new LandsideLaneExit();
		pLastLaneExit->SetPosition(pExitLane,extPos);
		pLastLaneExit->Offset(400);
		pLastLaneExit->SetToRes(pExitLane);
		followPath.push_back(pLastLaneExit);	
		//////////////////////////////////////////////////////////////////////////
		return true;
	}
	//ASSERT(FALSE);
	return false;
}

void InLaneParkingSpotsGroup::ReleaseParkingPos( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	for(size_t i=0;i<mvParkingSpots.size();i++)
	{
		LaneParkingSpot* pSpot = mvParkingSpots[i];
		if(pSpot->GetPreOccupy() == pVehicle)
		{
			pSpot->SetPreOccupy(NULL);
		}
		if(pSpot->bHasVehicle(pVehicle))
		{			
			pSpot->OnVehicleExit(pVehicle,t);
		}		
	}
}

bool InLaneParkingSpotsGroup::FindParkspotPath( LandsideLaneInSim* pFromlane, const CPoint2008& dFromPos, LaneParkingSpot* pSpot, LandsideLaneNodeList& followPath )const
{
	
	CPoint2008 entryPos;
	if(LandsideLaneInSim* pLane = FindPosEntrySpot(pSpot,entryPos) )
	{
		//DistanceUnit distFrom = pLane->GetPointDist(dFromPos);
		//DistanceUnit distEntry = pLane->GetPointDist(entryPos);
		//if(distFrom<distEntry){
			//build path to the lane entry//////////////////////////////////////////////////////////////////////////
			LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();		
			pLaneEntry->SetPosition(pFromlane,dFromPos);
			pLaneEntry->SetFromRes(pFromlane);
			followPath.push_back(pLaneEntry);

			if(pLane!=pFromlane)
			{
				LandsideLaneExit* pLaneExit = new LandsideLaneExit();
				pLaneExit->SetPosition(pFromlane, dFromPos );
				pLaneExit->SetToRes(pLane);
				followPath.push_back(pLaneExit);		

				LandsideLaneEntry* pLastEntry = new LandsideLaneEntry();
				pLastEntry->SetPosition(pLane,dFromPos);
				pLastEntry->SetFromRes(pFromlane);
				followPath.push_back(pLastEntry);
			}

			LandsideLaneExit* pLastLaneExit = new LandsideLaneExit();
			pLastLaneExit->SetPosition(pLane,entryPos);
			pLastLaneExit->SetToRes(pSpot);
			followPath.push_back(pLastLaneExit);	
			//////////////////////////////////////////////////////////////////////////

			return true;
		//}
	}
	return false;
}

LandsideResourceInSim* InLaneParkingSpotsGroup::getBehind( LaneParkingSpot* pthis ) const
{
	if(pthis->m_index >0 ){
		if(LaneParkingSpot* pSpot = getSpot(pthis->mpOnLane,pthis->m_index+1) ){
			return pSpot;
		}
	}
	return pthis->mpOnLane;
}


LandsideLaneInSim* InLaneParkingSpotsGroup::FindPosEntrySpot( LaneParkingSpot* pSpot, CPoint2008& pos )const
{
	if(m_bLeftDrive)
	{
		
		if(LandsideLaneInSim* pLane = FindPosEntrySpotR(pSpot,pos))
		{
			return pLane;
		}
		if(LandsideLaneInSim* pLane  = FindPosEntrySpotL(pSpot,pos) )
		{
			return pLane;
		}
	}
	else
	{
		
		if(LandsideLaneInSim* pLane  = FindPosEntrySpotL(pSpot,pos) )
		{
			return pLane;
		}	
		if(LandsideLaneInSim* pLane = FindPosEntrySpotR(pSpot,pos))
		{
			return pLane;
		}
	}	
	return NULL;	
}

LandsideLaneInSim* InLaneParkingSpotsGroup::FindPosExitToLane( LaneParkingSpot* pSpot,CPoint2008& pos )const
{	
	if(m_bLeftDrive)
	{
		if(LandsideLaneInSim* pLane = FindPosExitToLaneR(pSpot,pos))
		{
			return pLane;
		}
		if(LandsideLaneInSim* pLane = FindPosExitToLaneL(pSpot,pos) )
		{
			return pLane;
		}
	}
	else 
	{	
		if(LandsideLaneInSim* pLane = FindPosExitToLaneL(pSpot,pos) )
		{
			return pLane;
		}
		if(LandsideLaneInSim* pLane = FindPosExitToLaneR(pSpot,pos))
		{
			return pLane;
		}
	}
	
	return NULL;
}

struct ParkingSpotOrder
{
public:
	bool operator()(LaneParkingSpot* p1, LaneParkingSpot*p2)
	{
		return p1->GetDistInLane() > p2->GetDistInLane();
	}
};

void InLaneParkingSpotsGroup::InitSpotRelations()
{
	for(size_t i=0;i<mvParkingSpots.size();i++)
	{
		LaneParkingSpot* pspot = mvParkingSpots[i];
		pspot->pHead = getHead(pspot);
		ASSERT(pspot->pHead!=pspot);

		pspot->pLeft = getLeft(pspot);
		ASSERT(pspot->pLeft!=pspot);

		pspot->pRight = getRight(pspot);
		ASSERT(pspot->pRight!=pspot);

		pspot->pBehind = getBehind(pspot);
		ASSERT(pspot->pBehind!=pspot);
	}
	std::sort(mvParkingSpots.begin(),mvParkingSpots.end(),ParkingSpotOrder());
}

LandsideLaneInSim* InLaneParkingSpotsGroup::FindPosEntrySpotR( LaneParkingSpot* pSpot, CPoint2008& pos ) const
{
	//find right
	if(LandsideResourceInSim* pRes = pSpot->pRight)
	{
		if(LandsideLaneInSim* pLane = pRes->toLane())
		{
			pos = pSpot->GetStartPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL)
			{ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot->toInLanePark(),pos) )
				{
					return pNextLane;
				}
			}
		}
	}
	//find head
	if(LandsideResourceInSim* pRes = pSpot->pHead){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetStartPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot->toInLanePark(),pos) ){
					return pNextLane;
				}
			}

		}
	}
	return NULL;
}

LandsideLaneInSim* InLaneParkingSpotsGroup::FindPosEntrySpotL( LaneParkingSpot* pSpot, CPoint2008& pos ) const
{	
	//find left
	if(LandsideResourceInSim* pRes = pSpot->pLeft)
	{
		if(LandsideLaneInSim* pLane = pRes->toLane())
		{
			pos = pSpot->GetStartPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL)
			{ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot->toInLanePark(),pos) )
				{
					return pNextLane;
				}
			}
		}
	}

	//find head
	if(LandsideResourceInSim* pRes = pSpot->pHead)
	{
		if(LandsideLaneInSim* pLane = pRes->toLane())
		{
			pos = pSpot->GetStartPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL)
			{ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot->toInLanePark(),pos) )
				{
					return pNextLane;
				}
			}

		}
	}
	return NULL;
}

LandsideLaneInSim* InLaneParkingSpotsGroup::FindPosExitToLaneR( LaneParkingSpot* pSpot,CPoint2008& pos ) const
{
	
	//find right
	if(LandsideResourceInSim* pRes = pSpot->pRight){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetEndPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot->toInLanePark(),pos) ){
					return pNextLane;
				}
			}	
		}
	}	
	//find behind
	if(LandsideResourceInSim* pRes = pSpot->pBehind)
	{
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetEndPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot->toInLanePark(),pos) ){
					return pNextLane;
				}
			}			
		}
	}
	return NULL;
}

LandsideLaneInSim* InLaneParkingSpotsGroup::FindPosExitToLaneL( LaneParkingSpot* pSpot,CPoint2008& pos ) const
{
	
	//find left
	if(LandsideResourceInSim* pRes = pSpot->pLeft){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetEndPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot->toInLanePark(),pos) ){
					return pNextLane;
				}
			}	
		}
	}
//find behind
	if(LandsideResourceInSim* pRes = pSpot->pBehind){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetEndPos();
			return pLane;
		}
		else if(IParkingSpotInSim* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot->toInLanePark(),pos) ){
					return pNextLane;
				}
			}			
		}
	}	
	return NULL;
}

LaneParkingSpot* InLaneParkingSpotsGroup::FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* pLane , LandsideLaneNodeList& followPath ) const
{
	ASSERT(pLane);

	const CPoint2008& dFromPos =  pVehicle->getLastState().pos;	
	std::vector<LaneParkingSpot*> SpotsCopy = mvParkingSpots;

	for(size_t i=0;i<SpotsCopy.size();i++){
		LaneParkingSpot* pSpot = SpotsCopy[i];
		if(pSpot->GetPreOccupy()==NULL)
		{
			ASSERT(pSpot->GetPreOccupy()!=pVehicle);
			if(FindParkspotPath(pLane,dFromPos,pSpot,followPath)){
				return pSpot;
			}
		}		
	}
	return NULL;
}

void InLaneParkingSpotsGroup::Init( LandsideLinkStretchObject * pLinkStretch,LandsideStretchInSim* pStretch, LandsideResourceInSim* parent )
{
	//CPoint2008 ptFrom = pStretch->m_Path.GetDistPoint(pLinkStretch->getDistFrom());
	//CPoint2008 ptTo = pStretch->m_Path.GetDistPoint(pLinkStretch->getDistTo());
	
	double dIndexFrom = pStretch->m_Path.GetDistIndex(pLinkStretch->getDistFrom());
	double dIndexTo = pStretch->m_Path.GetDistIndex(pLinkStretch->getDistTo());


	int iLaneForm = MIN(pLinkStretch->m_nLaneTo-1,pLinkStretch->m_nLaneFrom-1);
	int iLaneTo = MAX(pLinkStretch->m_nLaneTo-1,pLinkStretch->m_nLaneFrom-1);
	iLaneForm = MAX(0,iLaneForm); 
	iLaneForm = MIN(iLaneForm,pStretch->GetLaneCount()-1);
	iLaneTo = MAX(0,iLaneTo); 
	iLaneTo = MIN(iLaneTo,pStretch->GetLaneCount()-1);

	for(int i=iLaneForm;i<=iLaneTo;i++)
	{
		LandsideStretchLaneInSim* pLane = pStretch->GetLane(i);
		if(pLane)
		{
			DistanceUnit distF = pLane->GetPath().GetIndexDist(dIndexFrom);
			DistanceUnit distT = pLane->GetPath().GetIndexDist(dIndexTo);

			m_vLaneOccupy.push_back(new LaneSegInSim(pLane, distF,distT));
			addLane(pLane,distF,distT,pLinkStretch->m_dSpotLength,parent);
		}
	}
	InitSpotRelations();
}

void LandsideInterfaceParkingSpotsGroup::Init(LandsideLinkStretchObject * pLinkStretch,LandsideResourceManager* allRes,LandsideResourceInSim* parent )
{

	LandsideStretchInSim*pStretch = allRes->getStretchByID(pLinkStretch->getStrech()->getID());
	if(!pStretch)
		return;

	double  indexEntry = pStretch->m_Path.getCount();
	double indexExit = 0;

	if(pLinkStretch->m_bUseInStretchParking)
	{
		m_inlaneGroup.Init(pLinkStretch, pStretch ,parent);

		indexEntry = pStretch->m_Path.GetDistIndex(pLinkStretch->getDistFrom());
		indexExit  = pStretch->m_Path.GetDistIndex(pLinkStretch->getDistTo());
	}
	if(true)
	{
		m_outLaneGroup.Init(pLinkStretch,pStretch,parent);
		m_outLaneGroup.getMinEntryExit(indexEntry, indexExit);
	}
	
	m_dEntryIndexInStretch = indexEntry;
	m_dExitIndexInStretch = indexExit;
}


IParkingSpotInSim * LandsideInterfaceParkingSpotsGroup::FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath ) const
{

	if(IParkingSpotInSim * pret = m_inlaneGroup.FindParkingPos(pVehicle, followPath))
		return pret;

	//find in the out lane parking
	if(IParkingSpotInSim* pret = m_outLaneGroup.FindParkingSpot(pVehicle,followPath))
		return pret;

	return NULL;

}

bool LandsideInterfaceParkingSpotsGroup::FindLeavePosition( LandsideVehicleInSim* pVehicle, IParkingSpotInSim* pSpot, LandsidePosition& pos )
{
	if(OutLaneParkingSpot* outLaneSpot = pSpot->toOutLanePark())
	{
		pos.pRes = outLaneSpot->getLinkLane();
		pos.distInRes = outLaneSpot->getExitDistInLane();
		pos.pos = outLaneSpot->getLinkLane()->GetDistPoint(outLaneSpot->getExitDistInLane());
		return true;
	}
	else if(LaneParkingSpot* inlaneSpot = pSpot->toInLanePark() )
	{
		LandsideLaneNodeList retPath;
		if( m_inlaneGroup.FindLeavePath(pVehicle, inlaneSpot, retPath) )
		{
			if(!retPath.empty())
			{
				LandsideLaneNodeInSim* pNode = retPath.back();
				LandsideLaneInSim* plane =  pNode->mpLane;
				pos.pRes = pNode->mpLane;
				pos.pos = pNode->m_pos;
				pos.distInRes = pNode->m_distInlane;
				return true;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}
	return false;
}

void OutLaneParkingSpotGroup::addParkingSpace( ParkingSpace & space,LandsideStretchInSim* pLinkStretch,double dHeight, LandsideResourceInSim* parent)
{
	int i=0;
	CPoint2008 pos;
	ARCVector3 dir;
	CPoint2008 entrypos;

	while(space.GetSpotDirPos(i,pos,dir,entrypos))
	{
		pos.setZ(dHeight);
		entrypos.setZ(dHeight);

		DistanceUnit distInLane ;
		if(LandsideStretchLaneInSim* pLane = pLinkStretch->GetNearestLane(pos, distInLane))
		{
			OutLaneParkingSpot* pSpot = new OutLaneParkingSpot(pLane, parent);		

			pSpot->SetPosDir(pos,dir);
			pSpot->SetEntryExitDist(distInLane, distInLane);
			m_vSpots.push_back(pSpot);
		}		
		++i;
	}
}

void OutLaneParkingSpotGroup::Init( LandsideLinkStretchObject * pLinkStretch,LandsideStretchInSim* pStretch,LandsideResourceInSim* parent )
{
	CPoint2008 linkStetchPos  = pStretch->m_Path.GetDistPoint(pLinkStretch->getDistFrom());

	for(int i=0;i< pLinkStretch->m_outStretchParkingspace.getCount();i++)
	{
		addParkingSpace(pLinkStretch->m_outStretchParkingspace.getSpace(i), pStretch, linkStetchPos.z, parent);
	}
}

void OutLaneParkingSpotGroup::getMinEntryExit( double& dentrydistinStertch, double& dexitinStretch ) const
{
	for(size_t i=0;i<m_vSpots.size();i++)
	{
		OutLaneParkingSpot* pSpot = m_vSpots.at(i);
		dentrydistinStertch = MIN( pSpot->getEntryIndexInlane(), dentrydistinStertch);
		dexitinStretch = MAX(pSpot->getExitIndexInlane(), dexitinStretch);
	}
}

bool OutLaneParkingSpotGroup::_getParkspotPath( LandsideLaneInSim* pFromlane, const CPoint2008& dFromPos, OutLaneParkingSpot* pSpot, LandsideLaneNodeList& followPath ) const
{
	LandsideLaneInSim* pEntrySpotLane = pSpot->getLinkLane();
	DistanceUnit entrypos = pSpot->getEntryDistInLane();		

	//build path to the lane entry//////////////////////////////////////////////////////////////////////////
	LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();		
	pLaneEntry->SetPosition(pFromlane,dFromPos);
	pLaneEntry->SetFromRes(pFromlane);
	followPath.push_back(pLaneEntry);

	if(pEntrySpotLane!=pFromlane)
	{
		LandsideLaneExit* pLaneExit = new LandsideLaneExit();
		pLaneExit->SetPosition(pFromlane, dFromPos );
		pLaneExit->SetToRes(pEntrySpotLane);
		followPath.push_back(pLaneExit);		

		LandsideLaneEntry* pLastEntry = new LandsideLaneEntry();
		pLastEntry->SetPosition(pEntrySpotLane,dFromPos);
		pLastEntry->SetFromRes(pFromlane);
		followPath.push_back(pLastEntry);
	}

	LandsideLaneExit* pLastLaneExit = new LandsideLaneExit();
	pLastLaneExit->SetPosition(pEntrySpotLane,entrypos);
	pLastLaneExit->SetToRes(pSpot);
	followPath.push_back(pLastLaneExit);	
	//////////////////////////////////////////////////////////////////////////

	return true;	//}
}

OutLaneParkingSpot* OutLaneParkingSpotGroup::_findParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* patLane, LandsideLaneNodeList& followPath )const
{
	const CPoint2008& dFromPos =  pVehicle->getLastState().pos;

	std::vector<OutLaneParkingSpot*> vSpots = m_vSpots;
	std::random_shuffle(vSpots.begin(),vSpots.end());

	for(size_t i=0;i<vSpots.size();i++)
	{
		OutLaneParkingSpot* pSpot = vSpots[i];
		if(pSpot->GetPreOccupy()==NULL)
		{
			ASSERT(pSpot->GetPreOccupy()!=pVehicle);
			if(_getParkspotPath(patLane,dFromPos,pSpot,followPath))
			{
				return pSpot;
			}
		}		
	}
	return NULL;
}

OutLaneParkingSpot* OutLaneParkingSpotGroup::FindParkingSpot( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )const
{
	LandsideResourceInSim* pAtRes = pVehicle->getLastState().getLandsideRes();
	if(!pAtRes) return NULL;

	LandsideLaneInSim* pAtLane = NULL; pAtRes->toLane();

	pAtLane = pAtRes->toLane();
	if(!pAtLane)
	{
		LandsideIntersectLaneLinkInSim* pLaneLink = pAtRes->toIntersectionLink();
		if(pLaneLink)
		{
			pAtLane = pLaneLink->getToNode()->mpLane;
		}
	}
	if(pAtLane)
	{
		return _findParkingPos(pVehicle,pAtLane,followPath);
	}

	return NULL;
}

void IParkingSpotInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	if(GetPreOccupy() == pVehicle)
	{
		SetPreOccupy(NULL);
	}
	__super::OnVehicleExit(pVehicle,t);
}

void IParkingSpotInSim::SetPosDir( const ARCPoint3& pos ,const ARCVector3& dir )
{
	m_pos = pos;
	m_dir = dir;
	m_dir.Normalize();
}

LandsideLayoutObjectInSim* IParkingSpotInSim::getLayoutObject() const
{
	return m_pParent->getLayoutObject();
}

IParkingSpotInSim::IParkingSpotInSim()
{
	mpOrderVehicle = NULL;
	m_pParent = NULL;
}

OutLaneParkingSpot::OutLaneParkingSpot( LandsideLaneInSim* pLinkLane, LandsideResourceInSim* parent )
{
	m_pLinkLane = pLinkLane;
	m_pParent = parent;
}

void OutLaneParkingSpot::SetEntryExitDist( DistanceUnit entryDist, DistanceUnit exitDist )
{
	m_entryDistInLane = entryDist;
	m_exitDistInlane = exitDist;
	m_dIndexEntry = m_pLinkLane->GetPath().GetDistIndex(m_entryDistInLane);
	m_dIndexExit = m_pLinkLane->GetPath().GetDistIndex(m_exitDistInlane);
}

