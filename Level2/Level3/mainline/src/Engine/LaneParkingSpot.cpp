#include "stdafx.h"
#include "LaneParkingSpot.h"
#include "Common/CPPUtil.h"
#include "LandsideStretchInSim.h"
#include "LandsideVehicleInSim.h"
#include "LandsideIntersectionInSim.h"

LaneParkingSpot::LaneParkingSpot( LandsideLaneInSim * plane , DistanceUnit distf, DistanceUnit distT, LandsideResourceInSim* pParent )
{
	mpParent = pParent;
	mdistF = distf;
	mdistT = distT;
	mpOnLane = plane;
	pLeft = pRight = pHead = pBehind = NULL;
	mpOrderVehicle = NULL;
}

CPoint2008 LaneParkingSpot::GetEndPos() const
{
	return mpOnLane->GetDistPoint(mdistT);
}

CPoint2008 LaneParkingSpot::GetParkingPos() const
{
	return mpOnLane->GetDistPoint(GetDistInLane());
}


ARCVector3 LaneParkingSpot::GetParkingPosDir() const
{
	return mpOnLane->GetDistDir(GetDistInLane());
}

CPoint2008 LaneParkingSpot::GetStartPos() const
{
	return mpOnLane->GetDistPoint(mdistF);
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

LaneParkingSpotsGroup::~LaneParkingSpotsGroup()
{
	cpputil::deletePtrVector(mvParkingSpots);
}

void LaneParkingSpotsGroup::addLane( LandsideStretchLaneInSim* pLane, DistanceUnit distF, DistanceUnit distT,DistanceUnit spotlength,LandsideResourceInSim* pParent )
{
	pLane->SetHasParkingSpot(true);
	int nCount  =  MAX(1, static_cast<int>((distT - distF)/spotlength) );
	DistanceUnit distInc =  distF; 
	LaneParkingSpot* pSpot = NULL;
	for(int i=0;i<nCount;i++){
		pSpot = new LaneParkingSpot(pLane,distInc, distInc + spotlength, pParent);
		distInc += spotlength;
		pSpot->m_index = i;
		mvParkingSpots.push_back(pSpot);		
	}


}

LandsideResourceInSim* LaneParkingSpotsGroup::getLeft( LaneParkingSpot* pthis )const
{
	LandsideLaneInSim* LeftLane = pthis->mpOnLane->getLeft();
	if(LeftLane){
		if(LaneParkingSpot* pSpot = getSpot(LeftLane,pthis->m_index)){
			return pSpot;
		}
	}
	return LeftLane;
}

LaneParkingSpot* LaneParkingSpotsGroup::getSpot( LandsideLaneInSim* pLane, int idx ) const
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

LaneParkingSpot* LaneParkingSpotsGroup::getSpot( LandsideVehicleInSim* pVehicle )const
{
	for(size_t i=0;i<mvParkingSpots.size();i++){
		LaneParkingSpot* pspot = mvParkingSpots[i];
		if(pspot->GetPreOccupy() == pVehicle){
				return pspot;
			}
	}
	return NULL;
}

LandsideResourceInSim* LaneParkingSpotsGroup::getRight( LaneParkingSpot* pthis )const
{
	LandsideLaneInSim* rightlane = pthis->mpOnLane->getRight();
	if(rightlane){
		if(LaneParkingSpot* pSpot = getSpot(rightlane,pthis->m_index)){
			return pSpot;
		}
	}
	return rightlane;
}

LandsideResourceInSim* LaneParkingSpotsGroup::getHead( LaneParkingSpot* pthis )const
{
	if(pthis->m_index >0 ){
		return getSpot(pthis->mpOnLane,pthis->m_index-1);
	}
	return pthis->mpOnLane;
}

LaneParkingSpot* LaneParkingSpotsGroup::FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )const
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

LaneParkingSpot* LaneParkingSpotsGroup::FindParkingPosRandom( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* pLane , LandsideLaneNodeList& followPath ) const
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

LaneParkingSpot* LaneParkingSpotsGroup::FindParkingPosRandom( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath ) const
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

bool LaneParkingSpotsGroup::FindLeavePath( LandsideVehicleInSim* pVehicle, LaneParkingSpot* pSpot, LandsideLaneNodeList& followPath )
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

void LaneParkingSpotsGroup::ReleaseParkingPos( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	for(size_t i=0;i<mvParkingSpots.size();i++){
		LaneParkingSpot* pSpot = mvParkingSpots[i];
		if(pSpot->GetPreOccupy() == pVehicle){
			pSpot->SetPreOccupy(NULL);
		}
		if(pSpot->bHasVehicle(pVehicle)){			
			pSpot->OnVehicleExit(pVehicle,t);
		}		
	}
}

bool LaneParkingSpotsGroup::FindParkspotPath( LandsideLaneInSim* pFromlane, const CPoint2008& dFromPos, LaneParkingSpot* pSpot, LandsideLaneNodeList& followPath )const
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

LandsideResourceInSim* LaneParkingSpotsGroup::getBehind( LaneParkingSpot* pthis ) const
{
	if(pthis->m_index >0 ){
		if(LaneParkingSpot* pSpot = getSpot(pthis->mpOnLane,pthis->m_index+1) ){
			return pSpot;
		}
	}
	return pthis->mpOnLane;
}


LandsideLaneInSim* LaneParkingSpotsGroup::FindPosEntrySpot( LaneParkingSpot* pSpot, CPoint2008& pos )const
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

LandsideLaneInSim* LaneParkingSpotsGroup::FindPosExitToLane( LaneParkingSpot* pSpot,CPoint2008& pos )const
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
	bool operator()(LaneParkingSpot* p1, LaneParkingSpot*p2){
		return p1->GetDistInLane() > p2->GetDistInLane();
	}
};

void LaneParkingSpotsGroup::InitSpotRelations()
{
	for(size_t i=0;i<mvParkingSpots.size();i++){
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

LandsideLaneInSim* LaneParkingSpotsGroup::FindPosEntrySpotR( LaneParkingSpot* pSpot, CPoint2008& pos ) const
{
	//find right
	if(LandsideResourceInSim* pRes = pSpot->pRight){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetStartPos();
			return pLane;
		}
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot,pos) ){
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
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot,pos) ){
					return pNextLane;
				}
			}

		}
	}
	return NULL;
}

LandsideLaneInSim* LaneParkingSpotsGroup::FindPosEntrySpotL( LaneParkingSpot* pSpot, CPoint2008& pos ) const
{	
	//find left
	if(LandsideResourceInSim* pRes = pSpot->pLeft){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetStartPos();
			return pLane;
		}
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot,pos) ){
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
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){ //the spot is empty
				if(LandsideLaneInSim* pNextLane= FindPosEntrySpot(pNextSpot,pos) ){
					return pNextLane;
				}
			}

		}
	}
	return NULL;
}

LandsideLaneInSim* LaneParkingSpotsGroup::FindPosExitToLaneR( LaneParkingSpot* pSpot,CPoint2008& pos ) const
{
	
	//find right
	if(LandsideResourceInSim* pRes = pSpot->pRight){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetEndPos();
			return pLane;
		}
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot,pos) ){
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
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot,pos) ){
					return pNextLane;
				}
			}			
		}
	}
	return NULL;
}

LandsideLaneInSim* LaneParkingSpotsGroup::FindPosExitToLaneL( LaneParkingSpot* pSpot,CPoint2008& pos ) const
{
	
	//find left
	if(LandsideResourceInSim* pRes = pSpot->pLeft){
		if(LandsideLaneInSim* pLane = pRes->toLane()){
			pos = pSpot->GetEndPos();
			return pLane;
		}
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot,pos) ){
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
		else if(LaneParkingSpot* pNextSpot = pRes->toLaneSpot())
		{
			if(pNextSpot->GetPreOccupy()==NULL){
				if(LandsideLaneInSim* pNextLane= FindPosExitToLane(pNextSpot,pos) ){
					return pNextLane;
				}
			}			
		}
	}	
	return NULL;
}

LaneParkingSpot* LaneParkingSpotsGroup::FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* pLane , LandsideLaneNodeList& followPath ) const
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
