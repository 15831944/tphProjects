#pragma once

#include "ChildFrm.h"

//#include <gl/glut.h>
#include "Placement.h"
#include "../Results/AirsideEventLogBuffer.h"
#include "../InputAirside/TrafficLight.h"
#include "../Results/OutputAirside.h"
#include "../Engine/Airside/CommonInSim.h"
#include "RenderUtility.h"
#include ".\AnimationManager.h"
#include "Airport3D.h"
#include "Inputs/TerminalProcessorTypeSet.h"
void RenderEditPoint(Path * path,DistanceUnit dAlt){
	glColor3d(1.0,0.0,0.0);
	int nCount=path->getCount();
	for(int i=0;i<nCount;i++){
		Point & _p=path->getPoint(i);
		DrawFlatSquare(_p.getX(),_p.getY(),dAlt,200.0);
	}
	glEnd();
}

static void GetDefaultProcDispProperties(CDefaultDisplayProperties* pDDP, CProcessor2::CProcDispProperties* pPDP)
{
	pPDP->bDisplay[PDP_DISP_SHAPE] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcShapeOn);
	pPDP->bDisplay[PDP_DISP_SERVLOC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcServLocOn);
	pPDP->bDisplay[PDP_DISP_QUEUE] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcQueueOn);
	pPDP->bDisplay[PDP_DISP_INC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcInConstraintOn);
	pPDP->bDisplay[PDP_DISP_OUTC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcOutConstraintOn);
	pPDP->bDisplay[PDP_DISP_PROCNAME] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcNameOn);
	pPDP->color[PDP_DISP_SHAPE] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcShapeColor);
	pPDP->color[PDP_DISP_SERVLOC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcServLocColor);
	pPDP->color[PDP_DISP_QUEUE] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcQueueColor);
	pPDP->color[PDP_DISP_INC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcInConstraintColor);
	pPDP->color[PDP_DISP_OUTC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcOutConstraintColor);
	pPDP->color[PDP_DISP_PROCNAME] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcNameColor);
}

void GetDefaultStationDisplayProperties(CProcessor2::CProcDispProperties* pPDP)
{
	pPDP->bDisplay[PDP_DISP_CARAREA] = TRUE;
	pPDP->bDisplay[PDP_DISP_PREBOARD] = TRUE;
	pPDP->bDisplay[PDP_DISP_ENTRYDOORS] = TRUE;
	pPDP->bDisplay[PDP_DISP_INC] = TRUE;
	pPDP->bDisplay[PDP_DISP_EXITDOORS] = TRUE;
	pPDP->bDisplay[PDP_DISP_PROCNAME] = FALSE;
	pPDP->color[PDP_DISP_CARAREA] = StationProcColors[PDP_DISP_CARAREA];
	pPDP->color[PDP_DISP_PREBOARD] = StationProcColors[PDP_DISP_PREBOARD];
	pPDP->color[PDP_DISP_ENTRYDOORS] = StationProcColors[PDP_DISP_ENTRYDOORS];
	pPDP->color[PDP_DISP_INC] = StationProcColors[PDP_DISP_INC];
	pPDP->color[PDP_DISP_EXITDOORS] = StationProcColors[PDP_DISP_EXITDOORS];
	pPDP->color[PDP_DISP_PROCNAME] = StationProcColors[PDP_DISP_PROCNAME];
}



inline
static void DrawGrids(CTermPlanDoc * pDoc,C3DView * pView)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	if(pView->GetCamera()-> GetProjectionType() == C3DCamera::perspective)
	{                      //3D
		if( EnvMode_AirSide == pDoc->GetCurrentMode() ){
			CAirside3D * pAirside = pDoc->Get3DViewParent()->GetAirside3D();
			for(int i =0;i< (int)pAirside->GetAirportList().size();i++)
			{
				CAirport3D * pAirport = pAirside->GetActiveAirport();
				ARCVector3 vWorld = pAirport->m_altAirport.getRefPoint();
				glPushMatrix();
				//glTranslated(vWorld[VX],vWorld[VY],vWorld[VZ]);
				for(int i=0;i< pAirport->GetLevelList().GetCount();i++)
				{
					if(pAirport->GetLevelList().GetLevel(i).IsVisible())
					{
						double dAlt = pAirport->GetLevelList().GetLevel(i).RealAltitude()*pDoc->m_iScale;
						pAirport->GetLevelList().GetLevel(i).DrawGrid(dAlt);
					}
				}
				//pAirport->GetGround()->DrawGrid(0);
				glPopMatrix();
			}
				
			return ;
		}
		else
		{
			const CFloorList& vTermFloorList = pDoc->GetCurModeFloor().m_vFloors;
			int nFloorCount = vTermFloorList.size();
			for(int i=0;i<nFloorCount;i++)
			{
				if( vTermFloorList[i]->IsVisible() )
					vTermFloorList[i]->DrawGrid(vTermFloorList[i]->Altitude());
			}
		}

	}
	else    //2D
	{
		
		if(EnvMode_AirSide == pDoc->GetCurrentMode())
		{
			//
			int nActiveAirport = 0;
			int nActiveFloor = 0;
			CAirside3D * pAirside = pDoc->Get3DViewParent()->GetAirside3D();
			
			if( pAirside->GetAirportList().size() )
			{
				CAirport3D * pAirport = pAirside->GetActiveAirport();
				if(pAirport)
				{
					if( pAirport->GetLevelList().GetCount() )
					{
						CAirsideGround * pAirsideLevel = &pAirport->GetLevelList().GetLevel(0);
						if(pAirsideLevel && pAirsideLevel->IsVisible() )
						{
							pAirsideLevel->DrawGrid(0);
						}
					}
				}
			}	
		}
		else
		{
			CFloor2 * pFloor = pDoc->GetCurModeFloor().GetActiveFloor();
			if( pFloor->IsVisible() ) 
				pFloor->DrawGrid(0.0);
		}
	}
}

void GetFloorProperty(const CFloors& _floors, double dAlt[], BOOL bOn[],CTermPlanDoc* pDoc){
	if(pDoc->Get3DView()->GetCamera()->GetProjectionType()==C3DCamera::perspective)
	{
		/*if (pDoc->m_systemMode == EnvMode_AirSide)
		{
			for(int i=0; i<static_cast<int>(_floors.m_vFloors.size()); i++) 
			{
				dAlt[i] = _floors.m_vFloors[i]->RealAltitude()*pDoc->m_iScale;
				bOn[i] = _floors.m_vFloors[i]->IsVisible();
			}
		}
		else*/
		{
			for(int i=0; i<static_cast<int>(_floors.m_vFloors.size()); i++) 
			{
				dAlt[i] = _floors.m_vFloors[i]->Altitude();
				bOn[i] = _floors.m_vFloors[i]->IsVisible();
			}
		}
	}
	else
	{
		for(int i=0; i<static_cast<int>(_floors.m_vFloors.size()); i++) 
		{
			dAlt[i] = 0.0;
			bOn[i] = _floors.m_vFloors[i]->IsVisible();
		}
	}
}


//returns the number of lines
//inline
//static void BuildAircraftTag(const FlightDescStruct& _fds, CString& _sTag, bool bExpanded, int& nNumLines, int& nMaxCharsInLine )
//{
//	ElapsedTime t;
//	char* s = _sTag.GetBuffer(256);
//	nNumLines=0;
//	nMaxCharsInLine=0;
//	//if arriving
//	if(_fds.arrID.HasValue()) {	//arriving
//		sprintf(s, "\n%s %s", _fds.airline.GetValue(), _fds.arrID.GetValue());
//		int slen = strlen(s);
//		nMaxCharsInLine = max(nMaxCharsInLine, slen);
//		s = s+slen;
//		nNumLines++;
//		if(bExpanded) {
//			//show origin, arr time
//			sprintf(s, "\n%s  ",_fds.origin.GetValue());
//			t.setPrecisely(_fds.arrTime);
//			t.printTime(s+strlen(s),FALSE);
//			slen = strlen(s);
//			nMaxCharsInLine = max(nMaxCharsInLine, slen);
//			s = s+slen;
//			nNumLines++;
//		}
//	}
//	//if departing
//	if(_fds.depID.HasValue()) {		//departing
//		sprintf(s, "\n%s %s", _fds.airline.GetValue(), _fds.depID.GetValue());
//		int slen = strlen(s);
//		nMaxCharsInLine = max(nMaxCharsInLine, slen);
//		s = s+slen;
//		nNumLines++;
//		if(bExpanded) {
//			//show destination, dep time
//			sprintf(s, "\n%s  ",_fds.destination.GetValue());
//			t.setPrecisely(_fds.depTime);
//			t.printTime(s+strlen(s),FALSE);
//			slen = strlen(s);
//			nMaxCharsInLine = max(nMaxCharsInLine, slen);
//			s = s+slen;
//			nNumLines++;
//		}
//	}
//	if(bExpanded) {
//		//show ac type, # of PAX
//		sprintf(s, "\n%s\n%d PAX", _fds.acType.GetValue(), (int)(_fds.capacity*_fds.loadFactor));
//		nNumLines+=2;
//	}
//	_sTag.ReleaseBuffer();
//}
//
//
inline
static void DrawPaxTags(CTermPlanDoc* pDoc, C3DView* pView, BOOL* bOn, double* dAlt)
{
	///draw pax tags
	AnimationData* pAnimData = &(pDoc->m_animData);
	CPaxTags& paxtagprops = pDoc->m_paxTags;
	CPaxDispProps& paxdispprops = pDoc->m_paxDispProps;
	std::vector<int>& paxtagpropidx = pDoc->m_vPaxTagPropIdx;
	std::vector<int>& paxdisppropidx = pDoc->m_vPaxDispPropIdx;
	MobLogEntry element;
	PaxLog* pPaxLog = pDoc->GetTerminal().paxLog;

	if(pDoc->m_eAnimState==CTermPlanDoc::anim_pause) {
		if(pView->m_bAnimDirectionChanged) {
			pDoc->GetTerminal().m_pLogBufManager->InitBuffer();
			pView->m_bAnimDirectionChanged = FALSE;
		}
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( pAnimData->nLastAnimTime, (CEventLogBufManager::ANIMATION_MODE)pView->m_nAnimDirection,pDoc );
	}
	else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playF) {
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( pAnimData->nLastAnimTime, CEventLogBufManager::ANIMATION_FORWARD,pDoc );
	}
	else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playB) {
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( pAnimData->nLastAnimTime, CEventLogBufManager::ANIMATION_BACKWARD,pDoc );
	}

	//pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( pAnimData->nLastAnimTime, (pDoc->m_eAnimState==CTermPlanDoc::anim_playF||pDoc->m_eAnimState==CTermPlanDoc::anim_pause) ? CEventLogBufManager::ANIMATION_FORWARD : CEventLogBufManager::ANIMATION_BACKWARD  );
	const PaxEventBuffer& logData = pDoc->GetTerminal().m_pLogBufManager->getLogDataBuffer();

	for( PaxEventBuffer::const_iterator it = logData.begin(); it!= logData.end(); ++it )
	{
		CPaxDispPropItem* pPDPI = paxdispprops.GetDispPropItem(paxdisppropidx[it->first]);
		CPaxTagItem* pPTI = paxtagprops.GetPaxTagItem(paxtagpropidx[it->first]);
		pPaxLog->getItem(element, it->first);

		const MobDescStruct& pds = element.GetMobDesc();

		long nTime = pAnimData->nLastAnimTime;
		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();
		if(nStartTime <= nTime && nEndTime >= nTime) {

			const part_event_list& part_list = it->second;
			int pesAIdx=0;
			AnimationPaxGeometryData geoData;
			PaxAnimationData animaPaxData;
			//draw pax
			if(CAnimationManager::GetPaxShow(pDoc,part_list,nTime,bOn,dAlt,geoData,animaPaxData,pesAIdx) )
			{
				const MobEventStruct& pesA = part_list[pesAIdx];
				if(pPDPI->IsVisible() && pPTI->IsShowTags() ) {
					CString sPaxTag;
					CAnimationManager::BuildPaxTag(pDoc,pPTI->GetTagInfo(), pds, pesA, sPaxTag);
					glColor3ubv(ARCColor3::BLACK);
					TEXTMANAGER3D->DrawFormattedBitmapText3D(sPaxTag, geoData.pos);
					//DrawPAXShape(geoData.pos[VX],geoData.pos[VY],geoData.pos[VZ],geoData.dAngleFromX, pView->GetParentFrame()->PaxShapeDLIDs()[paxdisppropidx[it->first]]);	
					//TEXTMANAGER3D->DrawBitmapText(sPaxTag,geoData.pos);
				}
			}			
		}
	}
}


//inline
//static void DrawAircraftTags(C3DView* pView, CTermPlanDoc* pDoc, const BOOL* bOn, const double* dAlt, bool bSelectMode, const double* mvmatrix, const double* projmatrix, const int* viewport)
//{
//	if(pDoc->m_bHideACTags)
//		return;
//	//static const ARCVector3 farlocation(1e6, 1e6, 1e5);
//	static const long transittime(1200l);
//	AnimationData* pAnimData = &(pDoc->m_animData);
//	long nTime = pAnimData->nLastAnimTime;
//
//	FlightLog* flog = pDoc->GetTerminal().flightLog;
//
//	long nFlightCount = flog->getCount();
//	FlightLogEntry element;
//
//	GLuint nSphereDLID = pView->GetParentFrame()->SphereDLID();
//
//	for(long ii=0; ii<nFlightCount; ii++)
//	{
//		flog->getItem(element, ii);
//		FlightDescStruct fds;
//		element.initStruct(fds);
//
//		long nStartTime, nEndTime;
//		long nShowTime, nHideTime;
//		if(!fds.arrID.HasValue()) //departure only
//		{
//			nStartTime = fds.depTime-(long)fds.gateOccupancy;
//			nEndTime = fds.depTime;
//			nShowTime = nStartTime;
//			nHideTime = nEndTime+(long)ElapsedTime(transittime); //500 seconds later
//		}
//		else if(!fds.depID.HasValue()) { //arrival only
//			nStartTime = fds.arrTime;
//			nEndTime = fds.arrTime+(long)fds.gateOccupancy;
//			nShowTime = nStartTime-(long)ElapsedTime(transittime); //500 seconds earlier
//			nHideTime = nEndTime;
//		}
//		else { //arrival and departure
//			nStartTime = fds.arrTime;
//			nEndTime = fds.depTime;
//			nShowTime = nStartTime-(long)ElapsedTime(transittime); //500 seconds earlier
//			nHideTime = nEndTime+(long)ElapsedTime(transittime); //500 seconds later
//		}
//
//
//		if(nShowTime <= nTime && nHideTime >= nTime) {
//			int floor;
//			ARCVector3 worldPos;
//			if(fds.gate == -1) { //no assigned gate
//				floor = 0;
//				worldPos[VX]=0.0; worldPos[VY]=0.0; worldPos[VZ]=dAlt[floor];
//			}
//			else {
//				floor = static_cast<int>( fds.nStandGateZ/SCALE_FACTOR );
//				worldPos[VX]=fds.nStandGateX; worldPos[VY]=fds.nStandGateY; worldPos[VZ]=dAlt[floor];
//			} 
//			if(bOn[floor]) {
//
//				ARCVector3 farloc(worldPos);
//				farloc[VZ] = 0.0;
//				farloc*=(1e6/worldPos.Magnitude());
//				farloc[VZ] = 1e5;
//
//				if(nStartTime > nTime) {//aircraft is in transit arriving
//					double d = static_cast<double>(nTime-nShowTime)/(nStartTime-nShowTime);
//					worldPos = farloc + d * (worldPos-farloc);
//				}
//				else if(nEndTime < nTime) { //ac in transit departing
//					double d = static_cast<double>(nTime-nEndTime)/(nHideTime-nEndTime);
//					worldPos = worldPos + d * (farloc-worldPos);
//				}
//
//				CString sTag;
//				int h, w;
//				bool bExpanded = false;
//				if(pDoc->m_vSelectedACTags.size() > 0 && ii==pDoc->m_vSelectedACTags[0])
//					bExpanded = true;
//				BuildAircraftTag(fds, sTag, bExpanded, h, w); //h now holds num lines, w holds max line char count
//				h*=TEXTMANAGER3D->GetCharHeight();
//				h+=5;
//				w*=TEXTMANAGER3D->GetCharWidth();
//				w+=30;
//
//				worldPos[VZ] += 100.0;
//				glEnable(GL_LIGHTING);
//				glColor3ubv(accolor);
//				DrawSphere(nSphereDLID, worldPos, 100.0);
//				glDisable(GL_LIGHTING);
//
//				worldPos[VZ] += 100.0;
//				//get screen coords of ac stand world pos (project)
//				ARCVector3 winPos;
//				gluProject(worldPos[VX], worldPos[VY], worldPos[VZ], mvmatrix, projmatrix, viewport, &(winPos[VX]), &(winPos[VY]), &(winPos[VZ]));
//				//now get world coords of screen pos corners (unproject)
//				ARCVector3 worldPosBL, worldPosBR, worldPosTR, worldPosTL;
//				worldPosBL = worldPos;
//				gluUnProject(winPos[VX]+20, winPos[VY]+5, winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPos[VX]), &(worldPos[VY]), &(worldPos[VZ]));
//				gluUnProject(winPos[VX]+w, winPos[VY], winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPosBR[VX]), &(worldPosBR[VY]), &(worldPosBR[VZ]));
//				gluUnProject(winPos[VX]+w, winPos[VY]+h, winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPosTR[VX]), &(worldPosTR[VY]), &(worldPosTR[VZ]));
//				gluUnProject(winPos[VX], winPos[VY]+h, winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPosTL[VX]), &(worldPosTL[VY]), &(worldPosTL[VZ]));
//
//				if(bSelectMode)
//					glLoadName( LANDSIDEACTAGSOFFSET + ii );
//
//				//ARCVector3 lookAt;
//				//pView->GetCamera()->GetLookAtVector(lookAt);
//				//lookAt.Normalize();
//				//lookAt = -lookAt;
//
//				glEnable(GL_POLYGON_OFFSET_FILL);
//				glPolygonOffset(4.0,4.0);
//				glBegin(GL_QUADS);
//				//glNormal3dv(lookAt);
//				glColor3ubv(actagcolor2);
//				glVertex3dv(worldPosBL);
//				glVertex3dv(worldPosBR);
//				glColor3ubv(actagcolor1);
//				glVertex3dv(worldPosTR);
//				glVertex3dv(worldPosTL);
//				glEnd();
//				glDisable(GL_POLYGON_OFFSET_FILL);
//
//				ARCVector3 pt1, pt2, pt3;
//
//				int base = 0;
//
//				if(fds.arrID) {
//					gluUnProject(winPos[VX]+5, winPos[VY]+h-7, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt1[VX]), &(pt1[VY]), &(pt1[VZ]));
//					gluUnProject(winPos[VX]+15, winPos[VY]+h-7, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt2[VX]), &(pt2[VY]), &(pt2[VZ]));
//					gluUnProject(winPos[VX]+10, winPos[VY]+h-12, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt3[VX]), &(pt3[VY]), &(pt3[VZ]));
//					glBegin(GL_TRIANGLES);
//					//glNormal3dv(lookAt);
//					glColor3ubv(ARCColor3(255,0,0));
//					glVertex3dv(pt1);
//					glVertex3dv(pt2);
//					glVertex3dv(pt3);
//					glEnd();
//					base += TEXTMANAGER3D->GetCharHeight();
//					if(bExpanded)
//						base += TEXTMANAGER3D->GetCharHeight();
//				}
//
//				if(fds.depID) {
//					gluUnProject(winPos[VX]+5, winPos[VY]+h-base-12, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt1[VX]), &(pt1[VY]), &(pt1[VZ]));
//					gluUnProject(winPos[VX]+15, winPos[VY]+h-base-12, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt2[VX]), &(pt2[VY]), &(pt2[VZ]));
//					gluUnProject(winPos[VX]+10, winPos[VY]+h-base-7, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt3[VX]), &(pt3[VY]), &(pt3[VZ]));
//					glBegin(GL_TRIANGLES);
//					//glNormal3dv(lookAt);
//					glColor3ubv(ARCColor3(0,255,0));
//					glVertex3dv(pt1);
//					glVertex3dv(pt2);
//					glVertex3dv(pt3);
//					glEnd();
//				}
//
//				glColor3ubv(processortagcolor);
//				glBegin(GL_LINE_LOOP);
//				glVertex3dv(worldPosBL);
//				glVertex3dv(worldPosBR);
//				glVertex3dv(worldPosTR);
//				glVertex3dv(worldPosTL);
//				glEnd();
//				glColor3ubv(ARCColor3::BLACK);
//				TEXTMANAGER3D->DrawFormattedBitmapText(sTag, worldPos);
//
//			}
//		}
//	}
//
//	CHECK_GL_ERRORS("Aircraft Tags Rendering in 3DView.cpp...");
//}
//
static void Draw3DVectorList(const std::vector<ARCVector3>& vList, double dAlt, BOOL bLoop = FALSE)
{
	int c = vList.size();
	if(c == 1) {
		const double delta = 10.0;
		glBegin(GL_LINE_LOOP);
		glVertex3d(vList[0][VX]-delta, vList[0][VY]-delta, dAlt);
		glVertex3d(vList[0][VX]-delta, vList[0][VY]+delta, dAlt);
		glVertex3d(vList[0][VX]+delta, vList[0][VY]+delta, dAlt);
		glVertex3d(vList[0][VX]+delta, vList[0][VY]-delta, dAlt);
		glEnd();
	}
	else {
		if(bLoop)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_LINE_STRIP);
		for(int i=0; i<c; i++) {
			glVertex3d(vList[i][VX], vList[i][VY],dAlt);
		}
		glEnd();
	}
}


static double GetHeading(const ARCVector3& vFrom, const ARCVector3& vTo)
{
	double x = vTo[VX] - vFrom[VX];
	double y = vTo[VY] - vFrom[VY];
	if (x!=0.0 && y!=0.0) {
		double beta = atan (y / x);
		double theta = ARCMath::RadiansToDegrees(beta);
		return (x > 0)? (90.0 - theta): (270.0 - theta);
	}
	else {
		if (x!=0.0)          // y == 0.0
			return (x > 0.0)? 90.0: 270.0;
		else if (y!=0.0)     // x == 0.0
			return (y > 0.0)? 0.0: 180.0;
		else            // 0.0, 0.0 vector
			return -1.0;
	}
}

static void DrawHeadArrow(const std::vector<ARCVector3>& vList, double dAlt)
{
	static const double dx = 20.0;
	static const double dy = 40.0;
	int c = vList.size();
	if(c > 1) {
		glPushMatrix();
		glTranslated(vList[c-1][VX], vList[c-1][VY], dAlt);
		glRotated(-GetHeading(vList[c-2], vList[c-1]), 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3d(-dx, -dy, 0.0);
		glVertex3d(dx, -dy, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}

static void DrawTailArrow(const std::vector<ARCVector3>& vList, double dAlt)
{
	static const double dx = 20.0;
	static const double dy = 40.0;
	int c = vList.size();
	if(c > 1) {
		glPushMatrix();
		glTranslated(vList[0][VX], vList[0][VY], dAlt);
		glRotated(-GetHeading(vList[1], vList[0]), 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3d(-dx, -dy, 0.0);
		glVertex3d(dx, -dy, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}

static void DrawPath(const Path& _path, double* dAlt)
{
	int c =_path.getCount();
	glBegin(GL_LINE_STRIP);
	Point* pts = _path.getPointList();
	for(int i=0; i<c; i++) {
		glVertex3d(pts[i].getX(), pts[i].getY(),dAlt[(int)(pts[i].getZ()/SCALE_FACTOR)]);
	}
	glEnd();	
}

static void DrawPath(const Path* pPathToDraw, double dAlt, BOOL bLoop = FALSE)
{
	int c = pPathToDraw->getCount();
	if(c == 1) {
		Point pt = pPathToDraw->getPoint(0);
		const double delta = 10.0;
		glBegin(GL_LINE_LOOP);
		glVertex3d(pt.getX()-delta, pt.getY()-delta, dAlt);
		glVertex3d(pt.getX()-delta, pt.getY()+delta, dAlt);
		glVertex3d(pt.getX()+delta, pt.getY()+delta, dAlt);
		glVertex3d(pt.getX()+delta, pt.getY()-delta, dAlt);
		glEnd();
	}
	else {
		if(bLoop)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_LINE_STRIP);
		Point* pts = pPathToDraw->getPointList();
		for(int i=0; i<c; i++) {
			glVertex3d(pts[i].getX(), pts[i].getY(),dAlt);
		}
		glEnd();
	}
}

void GetDirectionPoint(int nStart,const Path& path,Point& pt)
{
	Point ptStart = path.getPoint(nStart);
	for (int i = nStart + 1; i < path.getCount(); i++)
	{
		Point ptNext = path.getPoint(i);
		if (ptNext.distance(ptStart) > (std::numeric_limits<double>::min)())
		{
			pt = Point(ptStart,ptNext);
			return;
		}
	}
	pt = Point(1.0,0.0,0.0);
}

inline
static void RenderTexturedPath(
							   const Path* pPath,		//centerline
							   double dWidth,			//width of the path
							   double dSideHeight,		//height of "handrails"
							   const ARCColor3& cColor,//color
							   int nTextureID,			//textureId to apply
							   double dTextureOffset,	//texture offset (for animated textures)
							   double* dFloorAlts,		//floor altitudes
							   bool bFadeFromActiveFloor = false, //true if path should fade out as it moves away from active floor
							   int nActiveFloor = -1 ) //the active floor (only needed if bFadeFromActiveFloor is true)	
{
	int c = pPath->getCount();
	if(c >= 2) {

		double dHalfWidth = dWidth/2.0;

		Point* pts = pPath->getPointList();

		std::vector<Point> vBisect1;
		std::vector<Point> vBisect2;
		std::vector<double> vBisectOffsets;
		vBisect1.reserve(c);
		vBisect2.reserve(c);
		vBisectOffsets.reserve(c-1);

		//calculate centerline point altitudes
		std::vector<double> vPtAlts;
		vPtAlts.reserve(c);
		for(int j=0; j<c; j++) {
			double dFloorIdx, dFraction;
			dFraction = modf( pts[j].getZ()/SCALE_FACTOR, &dFloorIdx );
			int nFloorIdx = static_cast<int>(dFloorIdx);
			vPtAlts.push_back(
				dFloorAlts[nFloorIdx] + dFraction*( dFloorAlts[nFloorIdx+1] - dFloorAlts[nFloorIdx] ) );
		}

		//if fading is enabled, calculate alpha vals
		std::vector<COLORUNIT> vPtAlpha;
		if(bFadeFromActiveFloor) {
			vPtAlpha.reserve(c);
			for(int j=0; j<c; j++) {
				double dFraction = 
					1.0 - min( 1.0, fabs( (pts[j].getZ()/SCALE_FACTOR) - static_cast<double>(nActiveFloor) ) );
				vPtAlpha.push_back( static_cast<COLORUNIT>( dFraction * 255.0 ) );
			}
		}

		//calculate the bisect line for the first segment
		{
			//Point c0(pts[0], pts[1]);
			Point c0;
			GetDirectionPoint(0,*pPath,c0);
			Point p0 = c0.perpendicular();
			p0.length( dHalfWidth );
			vBisect1.push_back(p0 + pts[0]);
			p0 = c0.perpendicularY();
			p0.length( dHalfWidth );
			vBisect2.push_back(p0 + pts[0]);
			vBisectOffsets.push_back(0.0);
		}

		//calculate bisect lines for middle segements
		for(j=1; j<c-1; j++) {
// 			Point c0(pts[j-1], pts[j]);
// 			Point c1(pts[j], pts[j+1]);
			Point c0;
			GetDirectionPoint(j-1,*pPath,c0);
			Point c1;
			GetDirectionPoint(j,*pPath,c1);
			Point p0 = c0.perpendicular();
			p0.length( 1.0 );
			Point p1 = c1.perpendicular();
			p1.length( 1.0 );
			p1+=p0;
			p1*=0.5; //avg

			//find angle between bisect line and c1
			double dCosVal = p1.GetCosOfTwoVector( c1 );
			double dAngle = acos( dCosVal );
			double dAdjustedWidth = dHalfWidth / sin( dAngle );
			vBisectOffsets.push_back(0.01 * dHalfWidth / tan( dAngle ));
			p1.length( dAdjustedWidth );
			vBisect1.push_back(pts[j] + p1);

			p0 = c0.perpendicularY();
			p0.length( 1.0 );
			p1 = c1.perpendicularY();
			p1.length( 1.0 );
			p1+=p0;
			p1*=0.5;
			p1.length( dAdjustedWidth );
			vBisect2.push_back(pts[j] + p1);
			
		}

		//calculate the bisect line for the last segment	 
		{
			//Point c0(pts[c-2], pts[c-1]);
			Point c0;
			GetDirectionPoint(c-2,*pPath,c0);
			Point p0 = c0.perpendicular();
			p0.length( dHalfWidth );
			vBisect1.push_back(p0 + pts[c-1]);
			p0 = c0.perpendicularY();
			p0.length( dHalfWidth );
			vBisect2.push_back(p0 + pts[c-1]);
		}

		//now draw
		if(bFadeFromActiveFloor) {
			glLineWidth(2.0);
			glBegin(GL_LINE_LOOP);
			glNormal3f(0.0,0.0,1.0);
			for(j=0; j<c; j++) {
				glColor4ub(msoutlinecolor[RED], msoutlinecolor[GREEN], msoutlinecolor[BLUE], vPtAlpha[j]);
				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
			}
			for(j=c-1; j>=0; j--) {
				glColor4ub(msoutlinecolor[RED], msoutlinecolor[GREEN], msoutlinecolor[BLUE], vPtAlpha[j]);
				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
			}
			glEnd();
			glLineWidth(1.0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, nTextureID);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

			glBegin(GL_QUADS);
			glNormal3f(0.0,0.0,1.0);

			double dPrevLen1=0.0, dPrevLen2=0.0, dPrevLenC=dTextureOffset;
			for(j=0; j<c-1; j++) {
				dPrevLen1 = dPrevLenC+vBisectOffsets[j];
				dPrevLen2 = dPrevLenC-vBisectOffsets[j];
// 				double dLen1 = Point(vBisect1[j], vBisect1[j+1]).length()/100.0;
// 				double dLen2 = Point(vBisect2[j], vBisect2[j+1]).length()/100.0;
// 				double dLenC = Point(pts[j], pts[j+1]).length()/100.0;
				double dLen1 = (vBisect1[j].distance3D(vBisect1[j+1]))/100.0;
				double dLen2 = (vBisect2[j].distance3D(vBisect2[j+1]))/100.0;
				double dLenC = (pts[j].distance3D(pts[j+1]))/100.0;

				glColor4ub(cColor[RED], cColor[GREEN], cColor[BLUE], vPtAlpha[j]);
				glTexCoord2d(0.0,dPrevLen1);
				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);

				glColor4ub(cColor[RED], cColor[GREEN], cColor[BLUE], vPtAlpha[j+1]);
				glTexCoord2d(0.0,dPrevLen1+dLen1);
				glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]);
				glTexCoord2d(1.0,dPrevLen2+dLen2);
				glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]);

				glColor4ub(cColor[RED], cColor[GREEN], cColor[BLUE], vPtAlpha[j]);
				glTexCoord2d(1.0,dPrevLen2);
				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);

				dPrevLenC += dLenC;
			}

			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		else {
			glColor3ubv(msoutlinecolor);//grey
			glLineWidth(2.0);
			glBegin(GL_LINE_LOOP);
			glNormal3f(0.0,0.0,1.0);
			for(j=0; j<c; j++) {
				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
			}
			for(j=c-1; j>=0; j--) {
				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
			}
			glEnd();
			glLineWidth(1.0);
 			glEnable(GL_TEXTURE_2D);
 			glBindTexture( GL_TEXTURE_2D, nTextureID);
 			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

 			glColor3ubv(cColor);
 
 			glBegin(GL_QUADS);
 			glNormal3f(0.0,0.0,1.0);
 
 			double dPrevLen1=0.0, dPrevLen2=0.0, dPrevLenC=dTextureOffset;
 			for(j=0; j<c-1; j++) {
 				dPrevLen1 = dPrevLenC+vBisectOffsets[j];
 				dPrevLen2 = dPrevLenC-vBisectOffsets[j];
 				//double dLen1 = Point(vBisect1[j], vBisect1[j+1]).length()/100.0;
 				//double dLen2 = Point(vBisect2[j], vBisect2[j+1]).length()/100.0;
 				//double dLenC = Point(pts[j], pts[j+1]).length3D()/100.0;

				double dLen1 = (vBisect1[j].distance3D(vBisect1[j+1]))/100.0;
				double dLen2 = (vBisect2[j].distance3D(vBisect2[j+1]))/100.0;
				double dLenC = (pts[j].distance3D(pts[j+1]))/100.0;
 
 				glTexCoord2d(0.0,dPrevLen1);
 				glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
 				glTexCoord2d(0.0,dPrevLen1+dLen1);
 				glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]);
 				glTexCoord2d(1.0,dPrevLen2+dLen2);
 				glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]);
 				glTexCoord2d(1.0,dPrevLen2);
 				glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
 
 				dPrevLenC += dLenC;
 			}
 
 			glEnd();
 			glDisable(GL_TEXTURE_2D);
 
 			if(dSideHeight > 0.1) {
 				glBegin(GL_QUADS);
 				glColor3ubv(cColor);
 				for(j=0; j<c-1; j++) {
 					glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]);
 					glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]);
 					glVertex3d(vBisect1[j+1].getX(), vBisect1[j+1].getY(), vPtAlts[j+1]+dSideHeight);
 					glVertex3d(vBisect1[j].getX(), vBisect1[j].getY(), vPtAlts[j]+dSideHeight);
 
 					glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]);
 					glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]);
 					glVertex3d(vBisect2[j+1].getX(), vBisect2[j+1].getY(), vPtAlts[j+1]+dSideHeight);
 					glVertex3d(vBisect2[j].getX(), vBisect2[j].getY(), vPtAlts[j]+dSideHeight);
 				}
 				glEnd();
 			}
		}
	}
}




inline
static void RenderPipes(CTermPlanDoc* pDoc, CTextureResource* pTextureRes, int nSelectedPipe, BOOL bSelPipeEdit, double* dAlt, BOOL* bOn)
{
	CPipeDataSet* pPipeDS = pDoc->GetTerminal().m_pPipeDataSet;
	for(int i=0; i<pPipeDS->GetPipeCount(); i++) {
		CPipe* pPipe = pPipeDS->GetPipeAt(i);
		if(!pPipe->GetPipePointCount())
			continue;
		int nFloorIndex = pPipe->GetFloorIndex();
		if(!pPipe->GetDisplayFlag() || !bOn[nFloorIndex])
			continue;

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-4.0f,-4.0f);
		glEnable(GL_BLEND);
		POLLYGONVECTOR polyVect;
		pPipe->GetPipeCoveredRegion(polyVect);
		glColor4ubv(PipeColors[i%16]);
		for(int j=0; j<static_cast<int>(polyVect.size()); j++) {
			Pollygon poly = polyVect[j];
			int c = poly.getCount();
			Point* pts = poly.getPointList();
			glBegin(GL_POLYGON);
			glNormal3f(0.0,0.0,1.0);
			for(int k=0; k<c; k++) {
				glVertex3d(pts[k].getX(), pts[k].getY(),dAlt[nFloorIndex]);
			}
			glEnd();
		}

		//draw cross walk attach pipe
		CTexture* pTexture = pTextureRes->getTexture("crosswalk");
		if(pTexture)pTexture->Apply();

		POLLYGONVECTOR polyCrossVect;
		POLLYGONVECTOR polyTexVect;
		GLubyte Colors[4] = {255,255,255,132};
		pPipe->GetPipeCrosswalkRegion(polyCrossVect,polyTexVect);
		glColor4ubv(Colors);
		for(int j=0; j<static_cast<int>(polyCrossVect.size()); j++) {
			const Pollygon& poly = polyCrossVect[j];
			const Pollygon& texpoly  = polyTexVect[j];
			int c = poly.getCount();
			Point* pts = poly.getPointList();
			Point* tex = texpoly.getPointList();
			glBegin(GL_POLYGON);
			glNormal3f(0.0,0.0,1.0);
			for(int k=0; k<c; k++) {	
				glTexCoord2d(tex[k].getX()/500,tex[k].getY()/500);
				glVertex3d(pts[k].getX(), pts[k].getY(),dAlt[nFloorIndex]);
			}
			glEnd();
		}
		if(pTexture)pTexture->UnApply();
		//glDisable(GL_BLEND);
		glDisable(GL_POLYGON_OFFSET_FILL);


		if(i==nSelectedPipe && bSelPipeEdit) 
		{
			//draw pipe line and points
			glColor4ub(PipeColors[i%16][0], PipeColors[i%16][1], PipeColors[i%16][2], 255);
			int c = pPipe->GetPipePointCount();
			glBegin(GL_LINE_STRIP);
			for(j=0; j<c; j++) 
			{
				const PIPEPOINT& pt = pPipe->GetPipePointAt(j);
				glVertex3f(static_cast<GLfloat>(pt.m_point.getX()), static_cast<GLfloat>(pt.m_point.getY()), static_cast<GLfloat>(dAlt[nFloorIndex]));
			}
			glEnd();

			for(j=0; j<c; j++) 
			{
				const PIPEPOINT& pt = pPipe->GetPipePointAt(j);
				DrawFlatSquare(pt.m_point.getX(), pt.m_point.getY(), dAlt[nFloorIndex], 100.0);
				DrawFlatSquare(pt.m_bisectPoint1.getX(), pt.m_bisectPoint1.getY(), dAlt[nFloorIndex], 75.0);
				DrawFlatSquare(pt.m_bisectPoint2.getX(), pt.m_bisectPoint2.getY(), dAlt[nFloorIndex], 75.0);
				glBegin(GL_LINES);
				glVertex3f(static_cast<GLfloat>(pt.m_bisectPoint1.getX()), static_cast<GLfloat>(pt.m_bisectPoint1.getY()), static_cast<GLfloat>(dAlt[nFloorIndex]));
				glVertex3f(static_cast<GLfloat>(pt.m_bisectPoint2.getX()), static_cast<GLfloat>(pt.m_bisectPoint2.getY()), static_cast<GLfloat>(dAlt[nFloorIndex]));
				glEnd();
			}

		}
	}
}
//2005-9-1by ben
inline static void RenderStructures(CTermPlanDoc * pDoc,C3DView* pView,int nSelectedStructure, BOOL bSelStructureEdit,double * dAlt,BOOL * bOn){
	int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
	int nActiveFloor= pDoc->m_nActiveFloor;
	CStructureList & cslist=pDoc->GetStructurelistByMode(EnvMode_AirSide);
	CStructureList& StructureList = pDoc->GetStructurelistByMode(EnvMode_Terminal);
	glDisable(GL_LIGHTING);
	GLfloat _alpha=1.0f;
	if(pView->getColorMode()==_vivid){
		_alpha=1.0f;
	}else if(pView->getColorMode()==_de_saturated){
		_alpha=0.5f;
	}
	//draw to depth only;
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f,2.0f);
	for (size_t i = 0; i < StructureList.getStructureNum(); i++)
	{	
		CStructure * cs=StructureList.getStructureAt(i);
		int iFloor=cs->getFloorIndex();
		if(bOn[iFloor]){							
			StructureList.getStructureAt(i)->DrawOGL(pView->getTextureResource(),pView->GetDocument()->GetSurfaceMaterialLib(),dAlt[cs->getFloorIndex()],FALSE);
		}
	}	
	for(size_t i=0;i<cslist.getStructureNum();i++){		
		if(bOn[nFloorCount]){			
			cslist.getStructureAt(i)->DrawOGL(pView->getTextureResource(),pView->GetDocument()->GetSurfaceMaterialLib(),dAlt[nFloorCount],FALSE);
		}		
	}
	glDisable(GL_POLYGON_OFFSET_FILL);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	//render
	//glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-2.0f,-2.0f);	
	for (size_t i = 0; i < StructureList.getStructureNum(); i++)
	{		
		CStructure * cs=StructureList.getStructureAt(i);
		int iFloor=cs->getFloorIndex();
		if(bOn[iFloor])
		{
			glColor4f(1.0f, 1.0f, 1.0f, _alpha);					// Full Brightness.  50% Alpha
			if (i == nSelectedStructure&&iFloor==nActiveFloor&&pDoc->m_systemMode==EnvMode_Terminal)
				StructureList.getStructureAt(i)->DrawOGL(pView->getTextureResource(),pView->GetDocument()->GetSurfaceMaterialLib(),dAlt[cs->getFloorIndex()],bSelStructureEdit);
			else
				StructureList.getStructureAt(i)->DrawOGL(pView->getTextureResource(),pView->GetDocument()->GetSurfaceMaterialLib(),dAlt[cs->getFloorIndex()],FALSE);
		}		
	}	
	for(size_t i=0;i<cslist.getStructureNum();i++){
		if(bOn[nFloorCount]){
			glColor4f(1.0f, 1.0f, 1.0f, _alpha);					// Full Brightness.  50% Alpha
			if(i==nSelectedStructure&&pDoc->m_systemMode==EnvMode_AirSide){
				cslist.getStructureAt(i)->DrawOGL(pView->getTextureResource(),pView->GetDocument()->GetSurfaceMaterialLib(),dAlt[nFloorCount],bSelStructureEdit);
			}
			else
			{
				cslist.getStructureAt(i)->DrawOGL(pView->getTextureResource(),pView->GetDocument()->GetSurfaceMaterialLib(),dAlt[nFloorCount],FALSE);
			}
		}		
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);
	//glEnable(GL_DEPTH_TEST);	
	glEnable(GL_LIGHTING);
}
inline static void RenderAirsideNodes(CTermPlanDoc *pDoc,double  dAlt,BOOL  bOn)
{
	/*CAirsideNodeList * pNodesList=pDoc->GetTerminal().m_AirsideInput->getAirsideNodeList();
	pNodesList->SetDirty();
	std::vector<CAirsideNode> & pNodes=pNodesList->GetNodeList(pDoc->GetTerminal().GetProcessorList(EnvMode_AirSide));
	int nNodeCount=pNodes.size();
	glColor4f(1.0f,1.0f,0.0f,1.0f);
	glPointSize(100.0);
	glBegin(GL_POINTS);
	for(int i=0;i<nNodeCount;++i){	
	glVertex3d(pNodes[i].GetPoint().getX(),pNodes[i].GetPoint().getY(),pNodes[i].GetPoint().getZ());
	}
	glEnd();
	glPointSize(1.0);*/
	glDepthMask(GL_FALSE);
	if(bOn)
	{
		CAirsideNodeList * pNodesList=pDoc->GetTerminal().m_AirsideInput->getAirsideNodeList();
		pNodesList->SetDirty();
		std::vector<CAirsideNode> & pNodes=pNodesList->GetNodeList(pDoc->GetTerminal().GetAirsideProcessorList());
		int nNodeCount=pNodes.size();

		ARCVector3 worldpos;
		worldpos[VZ]=dAlt;
		GLuint GLDILdisk=glGenLists(1);
		GLUquadricObj * pqud=gluNewQuadric();
		glNewList(GLDILdisk,GL_COMPILE);			
		gluQuadricDrawStyle(pqud,GLU_FILL);
		gluDisk(pqud,0,200,20,4);
		glEndList();		
		gluDeleteQuadric(pqud);
		for(int i=0;i<nNodeCount;i++){
			worldpos[VX]=pNodes[i].GetPoint().getX();worldpos[VY]=pNodes[i].GetPoint().getY();
			glColor3ub(124,252,0);
			TEXTMANAGER3D->DrawBitmapText(pNodes[i].GetLable(),worldpos);
			glColor3ub(255,255,255);
			glPushMatrix();
			glTranslated(worldpos[VX],worldpos[VY],worldpos[VZ]);
			glCallList(GLDILdisk);
			glPopMatrix();
		}
		glDeleteLists(GLDILdisk ,1);

	}
	glDepthMask(GL_TRUE);

}
inline static void RenderWallShapes(CTermPlanDoc* pDoc, double* dAlt, BOOL* bOn)
{


	WallShapeList *termWallList=NULL;
	WallShapeList *airWallList=NULL;
	int AirFloorNum= pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide );

	termWallList=&(pDoc->GetWallShapeListByMode(EnvMode_Terminal)) ;
	airWallList= &(pDoc->GetWallShapeListByMode(EnvMode_AirSide)) ;
	//render Terminal wall

	if(pDoc->m_systemMode == EnvMode_Terminal){
		int nCount=termWallList->getShapeNum();	
		for(int i=0;i<nCount;++i){
			WallShape* termwall=termWallList->getShapeAt(i);
			if(bOn[termwall->GetFloorIndex()]){
				glEnable(GL_LIGHTING);
				glPushMatrix();
				glTranslated(0.0,0.0,dAlt[termwall->GetFloorIndex()]);
				glColor4f(1.0f,1.0f,1.0f,1.0f);	
				termwall->DrawOGL();
				glPopMatrix();
			}		
		}
		return;
	}

	//render airsidewall
	if(pDoc->m_systemMode == EnvMode_AirSide){
		int nCount=airWallList->getShapeNum();
		for(int i=0;i<nCount;++i){
			WallShape* airWall=airWallList->getShapeAt(i);
			if(bOn[airWall->GetFloorIndex()+AirFloorNum]){
				WallShape *Airwall =airWallList->getShapeAt(i);
				glEnable(GL_LIGHTING);
				glPushMatrix();
				glTranslated(0.0,0.0,dAlt[Airwall->GetFloorIndex()+AirFloorNum]);
				glColor4f(0.8f,0.8f,0.8f,1.0f);	
				Airwall->DrawOGL();
				glPopMatrix();
			}

		}
	}


}
inline
static void RenderResourcePaths(CTermPlanDoc* pDoc, double* _dAlt)
{
	int nResPoolCount = pDoc->GetTerminal().m_pResourcePoolDB->getResourcePoolCount();
	for( int i=0; i<nResPoolCount; i++ ) {
		CResourcePool* pPool = pDoc->GetTerminal().m_pResourcePoolDB->getResourcePoolByIndex( i );
		ASSERT( pPool );
		const Path& path = pPool->getPoolBasePath();
		DrawPath(path, _dAlt);
	}
}

inline
static void RenderRailways(CTermPlanDoc* pDoc, int nSelectedRailWay, BOOL bSelRailEdit, double* dAlt, BOOL* bOn)
{
	std::vector<RailwayInfo*> railWayVect;
	pDoc->m_trainTraffic.GetAllRailWayInfo(railWayVect);
	for(int i=0; i<static_cast<int>(railWayVect.size()); i++) {
		if(railWayVect[i]->IsPathDefined()) {
			if(i==nSelectedRailWay && bSelRailEdit)
				glColor3ubv(railwaycolorselected);
			else
				glColor3ubv(railwaycolor);
			const Path& path = railWayVect[i]->GetRailWayPath();
			Point* pts = path.getPointList();
			int nIndexInFloor=static_cast<int>(pts[0].getZ()/SCALE_FACTOR);
			if(!bOn[nIndexInFloor]) continue;
			int c = path.getCount();

			double dRealAlt,dAltDiff1;
			int nFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
			glBegin(GL_LINE_STRIP);
			for(int j=0; j<c; j++) {
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				if(nIndexInFloor1<=nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				dRealAlt=dAlt[nIndexInFloor1]+mult*dAltDiff1;
				glVertex3d(pts[j].getX(), pts[j].getY(), dRealAlt);				
			}
			glEnd();
			for(int j=0; j<c; j++)
			{
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				if(nIndexInFloor1<=nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				dRealAlt=dAlt[nIndexInFloor1]+mult*dAltDiff1;
				DrawFlatSquare(pts[j].getX(), pts[j].getY(), dRealAlt, 100.0);
			}

		}
	}
}

static CString GetSelectMenuEntryName( GLuint idx )
{
	BYTE seltype = GetSelectType( idx );
	BYTE selsubtype = GetSelectSubType( idx );
	unsigned short index = GetSelectIdx( idx );

	CString sRet;

	if( SELTYPE_PROCESSOR == seltype ) {
		sRet.Format("Processor %d", index);
	}
	else if( SELTYPE_TERMINALTRACER == seltype ) {
		sRet.Format("Passenger Tracer %d", index);
	}
	else if( SELTYPE_AIRSIDETRACER == seltype ) {
		sRet.Format("Aircraft Tracer %d", index);
	}
	else {
		sRet.Format("Unknown [%d]", idx);
	}

	return sRet;
}


inline
static void RenderTempProcInfo(CTermPlanDoc* pDoc, double dAltActive)
{
	glColor3ubv(pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::ProcServLocColor));
	Draw3DVectorList(pDoc->m_tempProcInfo.GetTempServicLocation(),dAltActive);
	glColor3ubv(pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::ProcInConstraintColor));
	Draw3DVectorList(pDoc->m_tempProcInfo.GetTempInConstraint(),dAltActive);
	DrawHeadArrow(pDoc->m_tempProcInfo.GetTempInConstraint(),dAltActive);
	glColor3ubv(pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::ProcOutConstraintColor));
	Draw3DVectorList(pDoc->m_tempProcInfo.GetTempOutConstraint(),dAltActive);
	DrawHeadArrow(pDoc->m_tempProcInfo.GetTempOutConstraint(),dAltActive);
	glColor3ubv(pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::ProcQueueColor));
	Draw3DVectorList(pDoc->m_tempProcInfo.GetTempQueue(),dAltActive);
	//DrawHeadArrow(pDoc->m_tempProcInfo.GetTempQueue(),dAltActive);
    DrawTailArrow(pDoc->m_tempProcInfo.GetTempQueue(),dAltActive);
	glColor3ubv(pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::ProcServLocColor));
	Draw3DVectorList(pDoc->m_tempProcInfo.GetTempBaggageBarrier(),dAltActive);

	glColor3ubv(pDoc->m_defDispProp.GetColorValue(CDefaultDisplayProperties::ProcServLocColor));
	Draw3DVectorList(pDoc->m_tempProcInfo.GetTempBaggageMovingPath(),dAltActive);

	//
	if(pDoc->m_tempProcInfo.GetProcType() == StandProcessor){
		ARCVector3 pos;
		if( pDoc->m_tempProcInfo.GetTempServicLocation().size()> 0 )
			pos = pDoc->m_tempProcInfo.GetTempServicLocation()[0];		

		glPushMatrix();
		glTranslated(pos[VX],pos[VY],dAltActive);
		glScalef(5,5,5);
		CShape* pShape = SHAPESMANAGER->FindShapeByName(_T("Large Disk"));
		if(pShape)pShape->DrawOGL();
		glPopMatrix();
	}
}

/*
static COleDateTime GetOleDateTime(long _nTime)
{
COleDateTime ret = COleDateTime::GetCurrentTime();
int nHour = _nTime/TIMETOHOURDIV;
int nMin = (_nTime-nHour*TIMETOHOURDIV)/TIMETOMINDIV;
int nSec = (_nTime-nHour*TIMETOHOURDIV-nMin*TIMETOMINDIV)/TIMETOSECDIV;
ret.SetTime(nHour,nMin,nSec);
return ret;
}
*/


inline
static void RenderProcessors(CTermPlanDoc* pDoc, C3DView* pView, long nTime, double* dAlt, BOOL* bOn)
{

	START_CODE_TIME_TEST
		const Path* path = NULL;
	int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
	int i,j;
	//end draw ;
	glPushMatrix();	


	//draw under construction procs (just shapes)
	CPROCESSOR2LIST* vProcList = &(pDoc->GetCurrentPlacement(EnvMode_All)->m_vUndefined);
	int nProcCount = vProcList->size();
	int nTerCount = 0;
	int nAirCount = 0;
	int nLandCount = 0;

	if( pDoc->GetCurrentPlacement(EnvMode_Terminal) )
		nTerCount = pDoc->GetCurrentPlacement(EnvMode_Terminal)->m_vUndefined.size();
	//if( pDoc->GetCurrentPlacement(EnvMode_AirSide) )
	//	nAirCount = pDoc->GetCurrentPlacement(EnvMode_AirSide)->m_vUndefined.size();
	if( pDoc->GetCurrentPlacement(EnvMode_LandSide) )
		nLandCount = pDoc->GetCurrentPlacement(EnvMode_LandSide)->m_vUndefined.size();


	static int tstep = 0;
	for(i=0; i<nProcCount; i++) 
	{
		int nProcFloor;
		if( i < nTerCount )
			nProcFloor = vProcList->at(i)->GetFloor();
		else if(i < nTerCount + nAirCount)
		{ // airside.
			tstep = 0;
			nProcFloor = pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide );
		}
		else
		{ // landside.
			nProcFloor = pDoc->GetDrawFloorsNumberByMode( EnvMode_LandSide ) + tstep;
			tstep ++;
		}

		if(bOn[nProcFloor])
			vProcList->at(i)->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALL);
	}

	//draw defined procs
	vProcList = &(pDoc->GetCurrentPlacement(EnvMode_All)->m_vDefined);
	nProcCount = vProcList->size();

	if( pDoc->GetCurrentPlacement(EnvMode_Terminal) )
		nTerCount = pDoc->GetCurrentPlacement(EnvMode_Terminal)->m_vDefined.size();
	//if( pDoc->GetCurrentPlacement(EnvMode_AirSide) )
	//	nAirCount = pDoc->GetCurrentPlacement(EnvMode_AirSide)->m_vDefined.size();
	if( pDoc->GetCurrentPlacement(EnvMode_LandSide) )
		nLandCount = pDoc->GetCurrentPlacement(EnvMode_LandSide)->m_vDefined.size();
	glEnable(GL_LIGHTING);

	for(i=0; i<nProcCount; i++) 
	{
		CProcessor2* pProc2 = vProcList->at(i);

		int nProcFloor ;//= pProc2->GetFloor();
		if( i < nTerCount )
			nProcFloor = pProc2->GetFloor();
		else if(i < nTerCount + nAirCount)
		{ // airside.
			tstep = 0;
			nProcFloor = pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide );
		}
		else
		{ // landside.
			nProcFloor = pDoc->GetDrawFloorsNumberByMode( EnvMode_LandSide ) + tstep;
			tstep ++;
		}

		Processor* pProcessor = pProc2->GetProcessor();

		CString id = pProcessor->getID()->GetIDString();
		ASSERT(pProcessor);
		int nProcType = pProc2->GetProcessor()->getProcessorType();
	

		if (!pDoc->GetTerminal().m_pTerminalProcessorTypeSet->IsProcessorTypeVisiable(nProcType))
		{
			continue;
		}
		//check whether display state off tag color with light
		bool bShowStateOnTag = CAnimationManager::DisplayProcessorStateOnTags(pDoc,pProcessor,nTime);
		ARCColor3 stateOffColor(128,128,128);
		pProc2->SetProcessorStateOff(bShowStateOnTag);
		
		if(nProcType == StairProc) {
			if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
				Stair* pStair = (Stair*)( pProc2->GetProcessor() );
				Path* path = pStair->serviceLocationPath();

				ARCColor3 cDisplay;
				cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;
				int nActiveFloor ;
				nActiveFloor = pDoc->m_nActiveFloor;
				if(path) {
					RenderTexturedPath( 
						path, 
						pStair->GetWidth(),
						76.2, //30 inch sides
						/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay, 
						pView->GetParentFrame()->RampTextureID(),
						0.0,
						dAlt,
						C3DCamera::parallel == pView->GetCamera()->GetProjectionType(),
						nActiveFloor);
				}
			}

			pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALLNOSHAPENOSERVLOC);
		}
		else if(nProcType == EscalatorProc) {
			if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
				Escalator* pEsc = (Escalator*)( pProc2->GetProcessor() );
				Path* path = pEsc->serviceLocationPath();
				int nActiveFloor ;
				nActiveFloor = pDoc->m_nActiveFloor;

				ARCColor3 cDisplay;
				cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;
				if(path) {
					RenderTexturedPath( 
						path, 
						pEsc->GetWidth(),
						76.2, //30 inch sides
						/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay, 
						pView->GetParentFrame()->MSTextureID(),
						0.0,
						dAlt,
						C3DCamera::parallel == pView->GetCamera()->GetProjectionType(),
						nActiveFloor);
				}
			}

			pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALLNOSHAPENOSERVLOC);
		}
		else if(nProcType == ConveyorProc) {
			if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
				Conveyor* pConveyor = (Conveyor*)( pProc2->GetProcessor() );
				//MiscProcessorData* pMiscDB = pDoc->GetTerminal().miscData->getDatabase( ConveyorProc );
				//ASSERT(pMiscDB);
				//int nProcIdx = pDoc->GetTerminal().procList->findProcessor(*(pProc2->GetProcessor()->getID()));
				//ASSERT(nProcIdx!=-1);
				//MiscDataElement* pMiscElem = (MiscDataElement*)pMiscDB->getItem( nProcIdx );
				double dSpeed = MiscConveyorData::DefaultSpeed;
				//if(pMiscElem!=NULL) {
				//	MiscConveyorData* pMiscData = (MiscConveyorData*) pMiscElem->getData();
				//	ASSERT(pMiscData);
				//	dSpeed = pMiscData->GetSpeed();
				//}
				int nSteps;
				if(nTime>=0)
					nSteps = static_cast<int>(1000000/(pDoc->m_animData.nAnimSpeed*dSpeed));
				else
					nSteps = 1000;
				if(nSteps==0)
					nSteps=1;
				double dOffset = -(nTime%nSteps)/(double)nSteps; //between 0.0 and 1.0
				//// TRACE("Offset=%.2f; Speed=%.2f; Width=%.2f\n", dOffset,dSpeed,pConveyor->GetWidth());
				int nActiveFloor ;
				nActiveFloor = pDoc->m_nActiveFloor;

				ARCColor3 cDisplay;
				cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;

				path = pConveyor->serviceLocationExceedTwoFloorPath();
				if(path) {
 					RenderTexturedPath( 
 						path, 
 						pConveyor->GetWidth(),
 						30.0, //30 cm sides
 						/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay, 
 						pView->GetParentFrame()->MSTextureID(),
 						dOffset,
 						dAlt,
 						C3DCamera::parallel == pView->GetCamera()->GetProjectionType(),
 						nActiveFloor);
				}
			}

			pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALLNOSHAPENOSERVLOC);
		}

		else if(bOn[nProcFloor]) {				
			if(nProcType == MovingSideWalkProc) {
				START_CODE_TIME_TEST
					if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
						//setup data;
						MovingSideWalk* pSidewalk = (MovingSideWalk*) pProc2->GetProcessor();
						CSideMoveWalkProcData * pData=pDoc->GetTerminal().m_pMovingSideWalk->GetLinkedSideWalkProcData(*pSidewalk->getID());
						double dSpeed =  pData->GetMoveSpeed();
						int nSteps;
						if(nTime>=0)
							nSteps = static_cast<int>(1000000/(pDoc->m_animData.nAnimSpeed*dSpeed));
						else
							nSteps = 1000;
						if(nSteps==0)
							nSteps=1;
						double dOffset = -(nTime%nSteps)/(double)nSteps; //between 0.0 and 1.0
						dOffset*=10.0;
						//// TRACE("Offset=%.2f; Speed=%.2f; Width=%.2f\n", dOffset,dSpeed,pData->GetWidth());

						ARCColor3 cDisplay;
						cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;

						path = pSidewalk->serviceLocationPath();
						if(path) {
							RenderTexturedPath( 
								path, 
								pData->GetWidth(),
								76.2, //30 inch handrails
								/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay, 
								pView->GetParentFrame()->MSTextureID(),
								dOffset,
								dAlt );
						}
					}
					pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALLNOSHAPENOSERVLOC);
			}
			else if(nProcType == BridgeConnectorProc)
			{
				BridgeConnector* pConnector = (BridgeConnector*)pProc2->GetProcessor();
				for(int ii=0;ii<pConnector->GetConnectPointCount();ii++)
				{
					const BridgeConnector::ConnectPoint& pConectPoint =  pConnector->GetConnectPointByIdx(ii);
					if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAPE])
					{
						ARCColor3 cDisplay;
						cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;

						CRenderUtility::DrawBridgeConPtr(ii,pConnector,pDoc,/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay,dAlt[nProcFloor] );
					}
				}
				pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALLNOSHAPENOSERVLOC);
			}			
			else if(nProcType == Elevator) {
				START_CODE_TIME_TEST
					ElevatorProc* pElevator = ( ElevatorProc* ) pProc2->GetProcessor();
				int nMinFloor = pElevator->GetMinFloor();
				int nMaxFloor = pElevator->GetMaxFloor();
				for(int nFloor=nMinFloor; nFloor<=nMaxFloor; nFloor++) {
					std::vector<Pollygon> vAreas;
					std::vector<Point> vDoors;
					pElevator->GetDataAtFloor(nFloor).GetLayoutData(vAreas);
					pElevator->GetDataAtFloor(nFloor).GetLiftDoors(vDoors);
					if(pProc2->m_dispProperties.bDisplay[PDP_DISP_LIFTAREA]) {
						int nAreaCount = vAreas.size();
						ARCColor3 cDisplay;
						cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_LIFTAREA] : stateOffColor;
						glColor3ubv(/*pProc2->m_dispProperties.color[PDP_DISP_LIFTAREA]*/cDisplay);
						for(int i=0; i<nAreaCount; i++) {
							DrawPath(&(vAreas[i]), dAlt[nFloor], TRUE);
						}
					}
					if(pProc2->m_dispProperties.bDisplay[PDP_DISP_DOORS]) {
						int nDoorCount = vDoors.size();
						const double delta = 70.0;
						ARCColor3 cDisplay;
						cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_DOORS] : stateOffColor;
						glColor3ubv(/*pProc2->m_dispProperties.color[PDP_DISP_DOORS]*/cDisplay);
						for(int iDoorIdx=0; iDoorIdx<nDoorCount; iDoorIdx++) {
							//float r1 = static_cast<float>(pElevator->GetOrientation());
							//float r2 = static_cast<float>(pProc2->GetRotation());
							glPushMatrix();
							glDisable(GL_LIGHTING);
							glTranslated(vDoors[iDoorIdx].getX(), vDoors[iDoorIdx].getY(),dAlt[nFloor]);
							glRotated(pProc2->GetRotation(),0.0,0.0,1.0);
							glBegin(GL_QUADS);
							glNormal3f(0.0,1.0,0.0);
							glVertex3d(-delta, 0.0, 0.0);
							glVertex3d(delta, 0.0, 0.0);
							glVertex3d(delta, 0.0, 200.0);
							glVertex3d(-delta, 0.0,200.0);					
							glEnd();
							glEnable(GL_LIGHTING);
							glPopMatrix();
						}
					}
				}
				if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAFTLINES]) {
					//draw vertical lines between 1st floor and last floor
					glDisable(GL_LIGHTING);
					ARCColor3 cDisplay;
					cDisplay = bShowStateOnTag ? ARCColor3(122,122,122) : stateOffColor;
					glColor3ubv(/*122,122,122*/cDisplay);
					std::vector<Pollygon>& liftAreas = pElevator->GetDataAtFloor(nMinFloor).GetLiftAreas();
					int nLiftAreaCount = liftAreas.size();
					for(int iLiftArea=0; iLiftArea<nLiftAreaCount; iLiftArea++) {
						int ptCount = liftAreas[iLiftArea].getCount();
						Point* ptList = liftAreas[iLiftArea].getPointList();
						for(j=0; j<ptCount; j++) {
							glBegin(GL_LINES);
							glVertex3d(ptList[j].getX(),ptList[j].getY(),dAlt[nMinFloor]);
							glVertex3d(ptList[j].getX(),ptList[j].getY(),dAlt[nMaxFloor]);
							glEnd();
						}
					}
					glEnable(GL_LIGHTING);
				}
				//pProc2->DrawOGL(dAlt[pProc2->GetFloor()], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALLNOSHAPENOSERVLOC);
			}
			else if(nProcType == FloorChangeProc) {
				START_CODE_TIME_TEST
					//draw ramp from this floor change proc to all connections
					if(pProc2->m_dispProperties.bDisplay[PDP_DISP_SHAPE]) {
						FloorChangeProcessor* pFlrChangeProc = (FloorChangeProcessor*)pProc2->GetProcessor();
						Point pt1 = pFlrChangeProc->serviceLocationPath()->getPoint(0);
						double dAlt1 = dAlt[(int)pt1.getZ()/100];

						glEnable(GL_TEXTURE_2D);
						glBindTexture( GL_TEXTURE_2D, pView->GetParentFrame()->RampTextureID());
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

						ARCColor3 cDisplay;
						cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;
						glColor3ubv(/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay);

						int nConnections = pFlrChangeProc->getConnectionCount();
						for(int nCon=0; nCon<nConnections; nCon++) {
							Processor* pConProc = pDoc->GetTerminal().procList->getProcessor(pFlrChangeProc->getConnection(nCon).start);
							ASSERT(pConProc);
							Point pt2 = pConProc->serviceLocationPath()->getPoint(0);
							double dAlt2 = dAlt[(int)pt2.getZ()/100];
							ARCVector3 v1(pt2.getX()-pt1.getX(), pt2.getY()-pt1.getY(), dAlt2-dAlt1);
							ARCVector3 v2(v1);
							v2[VZ] = 0.0;
							double dLength = v1.Magnitude()/100.0; //length of ramp in meters
							ARCVector3 vPerp(v1^v2);
							vPerp.Normalize();
							vPerp*=35.0; //with = 70cm = 2 ft;
							v2.Normalize();
							v2*=50.0;
							glBegin(GL_QUADS);
							glTexCoord2d(0.0,0.0);
							glVertex3d(pt1.getX()-vPerp[VX], pt1.getY()-vPerp[VY], dAlt1);
							glTexCoord2d(1.0,0.0);
							glVertex3d(pt1.getX()+vPerp[VX], pt1.getY()+vPerp[VY], dAlt1);
							glTexCoord2d(1.0,dLength);
							glVertex3d(pt2.getX()+vPerp[VX], pt2.getY()+vPerp[VY], dAlt2);
							glTexCoord2d(0.0,dLength);
							glVertex3d(pt2.getX()-vPerp[VX], pt2.getY()-vPerp[VY], dAlt2);
							glEnd();
						}
						glDisable(GL_TEXTURE_2D);
					}

					pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides, DRAWALL);
			}					
			else { //point, line, hold, dep src, dep snk, station, gate
				//add fructurm test here; ben 2005-11-11	
				Point pMin = pProc2->GetProcessor()->m_min;
				Point pMax = pProc2->GetProcessor()->m_max;
                /*if (pDoc->m_systemMode == EnvMode_AirSide)
                {
					pMin.setZ( pDoc->GetFloorByMode(EnvMode_Terminal).getRealAltitude(pMin.getZ())*pDoc->m_iScale );
					pMax.setZ( pDoc->GetFloorByMode(EnvMode_Terminal).getRealAltitude(pMax.getZ())*pDoc->m_iScale );
                }
				else
				{*/
					if (pView->GetCamera()->GetProjectionType()==C3DCamera::perspective)
					{
						pMin.setZ( pDoc->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(pMin.getZ()) );
						pMax.setZ( pDoc->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(pMax.getZ()) );
					}
					else
					{
						pMin.setZ(0);
						pMax.setZ(0);
					}
				//}
                				


				Point location = ( pMax + pMin )*0.5;
				double radius = pMin.distance(pMax)*0.5;

				//shape bounding sphere
				ARCVector3 vshapeLoc = pProc2->GetLocation();
				Point shapeLoc;				
				shapeLoc.setPoint(vshapeLoc[VX],vshapeLoc[VY],location.getZ() + vshapeLoc[VZ]);
				double dShapeRad = pProc2->GetScale().Length() * SCALE_FACTOR;

				////
				//glPushMatrix();
				//glTranslated(location.getX(),location.getY(),location.getZ());
				//glutWireSphere(radius,12,12);
				//glPopMatrix();
				//glPushMatrix();
				//glTranslated(shapeLoc.getX(),shapeLoc.getY(),shapeLoc.getZ());
				//glutWireSphere(dShapeRad,12,12);
				//glPopMatrix();
				
				//
				if ( pView->GetCamera()->SphereInCamera(location,radius) || pView->GetCamera()->SphereInCamera(shapeLoc,dShapeRad) ) 
				{
					//
					int nTPI = pDoc->m_tempProcInfo.GetProcIndex(); //index into procList
					BOOL bIsSpecialProc = !pDoc->m_tempProcInfo.GetProcessorGroup();

					//drawn normal proc
					if(nTPI != pProc2->GetProcessor()->getIndex())
					{
						START_CODE_TIME_TEST

							pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides,DRAWALL);
					}
					else 
					{
						START_CODE_TIME_TEST

							pProc2->DrawOGL(pView,dAlt[nProcFloor], pDoc->m_displayoverrides.m_pbProcDisplayOverrides,DRAWSHAPE);
					}	
				
				};
				
			}
		}
	}



	glDisable(GL_LIGHTING);

	//draw anim only stuff
	if(nTime>=0) {

		long ii;
		glEnable(GL_LIGHTING);
		{
			START_CODE_TIME_TEST
				//draw moving elevators
				CElevatorLog* pELog = pDoc->GetTerminal().m_pElevatorLog;
			CElevatorLogEntry elevatorElement;
			ElevatorDescStruct eds;
			int nECount = pELog->getCount();
			for(ii=0; ii<nECount; ii++)
			{
				pELog->getItem(elevatorElement, ii);
				elevatorElement.SetOutputTerminal(&(pDoc->GetTerminal()));
				elevatorElement.SetEventLog(pDoc->GetTerminal().m_pElevatorEventLog);
				elevatorElement.initStruct(eds);
				long nStartTime = (long) elevatorElement.GetStartTime();
				long nEndTime = (long) elevatorElement.GetEndTime();
				if(nStartTime <= nTime && nEndTime >= nTime)
				{
					long nEventCount = elevatorElement.getCount();
					long nextIdx = -1;
					for(long jj=0; jj<nEventCount; jj++) 
					{
					//ElevatorEventStruct ees = elevatorElement.getEvent(jj);
					//long attime = ees.time;
					if(elevatorElement.getEvent(jj).time >= nTime)
					{
					nextIdx = jj;
					break;
					}
					}
					if(nextIdx > 0) 
					{
					const ElevatorEventStruct& eesB = elevatorElement.getEvent(nextIdx);
					const ElevatorEventStruct& eesA = elevatorElement.getEvent(nextIdx-1);

					//3)interpolate position

					//ElevatorEventStruct eesB ;
					//ElevatorEventStruct eesA;

					//bool bRet = elevatorElement.getEventsAtTime(nTime,eesA,eesB);
					//pELog->updateItem(elevatorElement,ii);					

						int w = eesB.time - eesA.time;
						int d = eesB.time - nTime;
						double r = ((double) d) / w;
						float xpos = static_cast<float>((1-r)*eesB.x + r*eesA.x);// - eds.liftLength/2.0;
						float ypos = static_cast<float>((1-r)*eesB.y + r*eesA.y);// + eds.liftWidth/2.0;
						float altA = static_cast<float>(dAlt[(int)(eesA.z/SCALE_FACTOR)]);
						float altB = static_cast<float>(dAlt[(int)(eesB.z/SCALE_FACTOR)]);
						float zpos = static_cast<float>((1-r)*altB + r*altA);

						//draw elevator "car"
						glPushMatrix();
						glColor3ubv(traincolor);
						DrawElevTray(xpos,ypos,zpos,eds.liftLength,eds.liftWidth,360.0f-eds.orientation);
						glPopMatrix();
					}
				}

			}
		}

		{
			START_CODE_TIME_TEST

				//draw train
				glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-2.0,-2.0);

			CTrainLog* pTrainLog = pDoc->GetTerminal().m_pTrainLog;
			CTrainLogEntry trainElement;
			TrainDescStruct tds;
			int nTrainCount = pTrainLog->getCount();
			for(ii=0; ii<nTrainCount; ii++)
			{
				pTrainLog->getItem(trainElement,ii);
				trainElement.SetOutputTerminal(&(pDoc->GetTerminal()));
				trainElement.SetEventLog(pDoc->GetTerminal().m_pTrainEventLog);
				trainElement.initStruct(tds);
				long nStartTime = (long) trainElement.GetStartTime();
				long nEndTime = (long) trainElement.GetEndTime();
				if(nStartTime <= nTime && nEndTime >= nTime)
				{
					TrainEventStruct tesB;
					TrainEventStruct tesA;

					bool bRet = trainElement.getEventsAtTime(nTime,tesA,tesB);
					pTrainLog->updateItem(trainElement,ii);
					if(bRet) {

						//interpolate position based on the two PaxEventStructs and the time
						int w = tesB.time - tesA.time;
						int d = tesB.time - nTime;
						double r = ((double) d) / w;
						float xpos = static_cast<float>((1-r)*tesB.x + r*tesA.x);
						float ypos = static_cast<float>((1-r)*tesB.y + r*tesA.y);
						//get tesA altitude
						int nIndexInFloor=static_cast<int>((int)(tesA.z/SCALE_FACTOR)); 
						float dAltDiff;
						if(nIndexInFloor<nFloorCount-1) {
							dAltDiff=static_cast<float> (dAlt[nIndexInFloor+1]-dAlt[nIndexInFloor]);
						}
						else {
							dAltDiff=1000.0f;
						}
						float mult = static_cast<float>(tesA.z/SCALE_FACTOR)-nIndexInFloor;
						float altA = dAltDiff * mult + static_cast<float> (dAlt[nIndexInFloor]);
						//get tezB altitude
						nIndexInFloor=static_cast<int>((int)(tesB.z/SCALE_FACTOR)); 
						if(nIndexInFloor<nFloorCount-1) {
							dAltDiff=static_cast<float>(dAlt[nIndexInFloor+1]-dAlt[nIndexInFloor]);
						}
						else {
							dAltDiff=1000.0f;
						}
						mult = static_cast<float>(tesB.z/SCALE_FACTOR)-nIndexInFloor;
						float altB = dAltDiff * mult + static_cast<float>(dAlt[nIndexInFloor]);


						float zpos = static_cast<float>((1-r)*altB + r*altA);

						//					int zpos = static_cast<int>( tesB.z/SCALE_FACTOR );
						//					if(!bOn[zpos]) break;//suppose all cars in same floor
						//float rot = (1-r)*tesB.heading + r*tesA.heading;
						float rot = tesA.heading;
						rot = static_cast<float>(90.0-rot);

						//draw train
						//					static const float delta=500.0;
						glColor3ubv(traincolor);
						glPushMatrix();
						DrawCar(xpos,ypos,zpos,tds.carLength,tds.carWidth,rot,tds.numCars);//dAlt[zpos]
						glPopMatrix();
					}
				}
			}

			glDisable(GL_POLYGON_OFFSET_FILL);

		}

		{
			START_CODE_TIME_TEST


				//draw resource elements
				ResourceElementLog* pResLog = pDoc->GetTerminal().resourceLog;
			ResourceElementLogEntry resElement;
			ResDescStruct rds;
			int nResCount = pResLog->getCount();
			for(ii=0; ii<nResCount; ii++)
			{
				pResLog->getItem(resElement,ii);
				resElement.SetOutputTerminal(&(pDoc->GetTerminal()));
				resElement.SetEventLog(pDoc->GetTerminal().m_pResourceEventLog);
				resElement.initStruct(rds);


				long nStartTime = (long) resElement.getStartTime();
				long nEndTime = (long) resElement.getEndTime();
				if(nStartTime <= nTime && nEndTime >= nTime)
				{
					//interpolate position based on the two PaxEventStructs and the time

					ResEventStruct resA;
					ResEventStruct resB;

					bool bRet = resElement.getEventsAtTime(nTime,resA,resB);
					if(bRet)
					{
						int w = resB.time - resA.time;
						int d = resB.time - nTime;
						double r = ((double) d) / w;
						float xpos = static_cast<float>((1-r)*resB.x + r*resA.x);
						float ypos = static_cast<float>((1-r)*resB.y + r*resA.y);
						int zpos = static_cast<int>( resB.z/SCALE_FACTOR );
						if(!bOn[zpos]) break;
						//float rot = resA.heading;
						///rot = 90.0-rot;

						//glColor3ubv(traincolor);
						//DrawSphere( pView->GetParentFrame()->SphereDLID(), ARCVector3(xpos, ypos, dAlt[zpos]), 100.0 );


						GLuint nDLID = pView->GetParentFrame()->ResShapeDLIDs()[rds.pool_idx];

						if( pView->GetCamera()->PointInCamera(xpos, ypos, dAlt[zpos]))
							DrawPAXShape(xpos, ypos, dAlt[zpos],1.0f, 0.0, nDLID);


					}
				}	
			}
		}
	}
	else { //no animation
		if(pDoc->m_bShowTracers) {
			int nTrackCount = pDoc->m_tempTracerData.GetTrackCount();
			if(nTrackCount > 0) {
				glEnable(GL_LINE_STIPPLE);
				for(int nTrackIdx=0; nTrackIdx<nTrackCount; nTrackIdx++) {
					std::vector<CTrackerVector3>& track = pDoc->m_tempTracerData.GetTrack(nTrackIdx);
					CPaxDispPropItem* pPDPI = pDoc->m_paxDispProps.GetDispPropItem(pDoc->m_tempTracerData.GetDispPropIdx(nTrackIdx));
					ARCColor3 pdpcol(pPDPI->GetColor());
					int pdplt = (int) pPDPI->GetLineType();
					glColor3ubv(pdpcol);
					glLineWidth(static_cast<GLfloat>((pdplt % 3) +1));
					glLineStipple((pdplt % 3) +1, StipplePatterns[pdplt / 3]);
					glBegin(GL_LINE_STRIP);
					int nHitCount = track.size();
					bool bBeginVertex = false;
					for(int nHit=1; nHit<nHitCount; nHit++) {					
						CTrackerVector3& hit = track[nHit];
						int nFloorIdx = static_cast<int>(hit[VZ] / 100);
						double dAltitude = 0.0;
						if (hit.IsRealZ())
						{
							dAltitude = hit[VZ];
						}
						else
						{
							double dOffset = (hit[VZ]/100.0 - (double)nFloorIdx);
							dAltitude = dAlt[nFloorIdx]*(1.0-dOffset)  + dAlt[nFloorIdx+1]*dOffset;	
						}

						if(bOn[nFloorIdx]){
							glVertex3f(static_cast<GLfloat>(hit[VX]), static_cast<GLfloat>(hit[VY]),(GLfloat)dAltitude);
							bBeginVertex = true;
						}
						else if(bBeginVertex)
						{
							glEnd();
							glBegin(GL_LINE_STRIP);
							bBeginVertex = false;
						}
					}
					glEnd();
				}
				glDisable(GL_LINE_STIPPLE);
			}

			//airside
			nTrackCount = pDoc->m_tempAirsideTracerData.GetTrackCount();
			if(nTrackCount > 0) {
				glEnable(GL_LINE_STIPPLE);
				for(int nTrackIdx=0; nTrackIdx<nTrackCount; nTrackIdx++) {
					const std::vector<CTrackerVector3>& track = pDoc->m_tempAirsideTracerData.GetTrack(nTrackIdx);
					CAircraftDispPropItem* pADPI = pDoc->m_aircraftDispProps.GetDispPropItem(pDoc->m_tempAirsideTracerData.GetDispPropIdx(nTrackIdx));
					ARCColor3 adpcol(pADPI->GetColor());
					int adplt = (int) pADPI->GetLineType();
					glColor3ubv(adpcol);
					glLineWidth(static_cast<GLfloat>((adplt % 3) +1));
					glLineStipple((adplt % 3) +1, StipplePatterns[adplt / 3]);
					glBegin(GL_LINE_STRIP);
					int nHitCount = track.size();
					for(int nHit=1; nHit<nHitCount; nHit++) {					
						const CTrackerVector3& hit = track[nHit];
						glVertex3f(static_cast<GLfloat>(hit[VX]), static_cast<GLfloat>(hit[VY]), static_cast<GLfloat>(hit[VZ]));
					}
					glEnd();
				}
				glDisable(GL_LINE_STIPPLE);
			}
		}
		glLineWidth(1.0); 
	}

	glPopMatrix();

}



inline static void DrawFlightTag(const AirsideFlightDescStruct& fds, const AirsideFlightEventStruct& pesA, double dSpd, double dAlt, const CAircraftTagItem& _ati, ARCVector3& worldPos, const double* mvmatrix, const double* projmatrix, const int* viewport)
{

	CString sTag;
	int h, w;
	bool bExpanded = false;
	std::vector<C3DTextManager::TextFormat> fmts;
	CAnimationManager::BuildFlightTag(fds, pesA, _ati, bExpanded, sTag, fmts, h, w,dSpd,dAlt);

// 	h*=TEXTMANAGER3D->GetCharHeight();
// 	h+=5;
// 	w*=TEXTMANAGER3D->GetCharWidth();
// 	w+=30;

	
	////get screen coords of ac stand world pos (project)
	//ARCVector3 winPos;
	//gluProject(worldPos[VX], worldPos[VY], worldPos[VZ], mvmatrix, projmatrix, viewport, &(winPos[VX]), &(winPos[VY]), &(winPos[VZ]));
	////now get world coords of screen pos corners (unproject)
	//ARCVector3 worldPosBL, worldPosBR, worldPosTR, worldPosTL;
	//worldPosBL = worldPos;
	//gluUnProject(winPos[VX]+20, winPos[VY]+5, winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPos[VX]), &(worldPos[VY]), &(worldPos[VZ]));
	//gluUnProject(winPos[VX]+w, winPos[VY], winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPosBR[VX]), &(worldPosBR[VY]), &(worldPosBR[VZ]));
	//gluUnProject(winPos[VX]+w, winPos[VY]+h, winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPosTR[VX]), &(worldPosTR[VY]), &(worldPosTR[VZ]));
	//gluUnProject(winPos[VX], winPos[VY]+h, winPos[VZ], mvmatrix, projmatrix, viewport, &(worldPosTL[VX]), &(worldPosTL[VY]), &(worldPosTL[VZ]));

	//glEnable(GL_POLYGON_OFFSET_FILL);

	//glPolygonOffset(4.0,4.0);
	//glBegin(GL_QUADS);
	//glColor3ubv(actagcolor2);
	//glVertex3dv(worldPosBL);
	//glVertex3dv(worldPosBR);
	//glColor3ubv(actagcolor1);
	//glVertex3dv(worldPosTR);
	//glVertex3dv(worldPosTL);
	//glEnd();

	//ARCVector3 pt1, pt2, pt3;

	//if('A' == pesA.state) {
	//	glColor3ubv(ARCColor3(255,0,0));
	//	gluUnProject(winPos[VX]+5, winPos[VY]+h-7, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt1[VX]), &(pt1[VY]), &(pt1[VZ]));
	//	gluUnProject(winPos[VX]+15, winPos[VY]+h-7, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt2[VX]), &(pt2[VY]), &(pt2[VZ]));
	//	gluUnProject(winPos[VX]+10, winPos[VY]+h-12, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt3[VX]), &(pt3[VY]), &(pt3[VZ]));

	//}
	//else if('D' == pesA.state) {
	//	glColor3ubv(ARCColor3(0,255,0));
	//	gluUnProject(winPos[VX]+5, winPos[VY]+h-12, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt1[VX]), &(pt1[VY]), &(pt1[VZ]));
	//	gluUnProject(winPos[VX]+15, winPos[VY]+h-12, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt2[VX]), &(pt2[VY]), &(pt2[VZ]));
	//	gluUnProject(winPos[VX]+10, winPos[VY]+h-7, winPos[VZ], mvmatrix, projmatrix, viewport, &(pt3[VX]), &(pt3[VY]), &(pt3[VZ]));
	//}

	//glPolygonOffset(-4.0,-4.0);
	//glBegin(GL_TRIANGLES);
	//glVertex3dv(pt1);
	//glVertex3dv(pt2);
	//glVertex3dv(pt3);
	//glEnd();

	/*
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-4.0,-4.0);
	glBegin(GL_QUADS);
	glVertex3dv(worldPosBL);
	glVertex3dv(worldPosBR);
	glVertex3dv(worldPosTR);
	glVertex3dv(worldPosTL);
	glEnd();
	glDisable(GL_POLYGON_OFFSET_LINE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	*/

	TEXTMANAGER3D->DrawFormattedBitmapText3D(sTag, worldPos, &fmts[0], fmts.size());
	glDisable(GL_POLYGON_OFFSET_FILL);
}

inline static void DrawVehicleTag(const AirsideVehicleDescStruct& fds, const AirsideVehicleEventStruct& pesA, const DWORD& dwTagInfo, ARCVector3& worldPos)
{
	CString sTag;
	int h, w;
	bool bExpanded = false;
	glColor3ubv(ARCColor3::BLACK);
	CAnimationManager::BuildVehicleTag(fds, pesA, dwTagInfo, bExpanded, sTag, h, w);
	h*=TEXTMANAGER3D->GetCharHeight();
	h+=5;
	w*=TEXTMANAGER3D->GetCharWidth();
	w+=30;
	glColor3ubv(ARCColor3::BLACK);
	TEXTMANAGER3D->DrawFormattedBitmapText3D(sTag, worldPos);
	glDisable(GL_POLYGON_OFFSET_FILL);
}


inline
static void DrawAirsideTags2(C3DView* pView, CTermPlanDoc* pDoc, const double* dAlt,const double* mvmatrix, const double* projmatrix, const int* viewport)
{
	
	DistanceUnit dAirportAlt = *dAlt;
	//flight tag
	CAirsideSimLogs& airsideSimLogs = pDoc->GetAirsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	CAircraftDispProps& aircraftdispprops = pDoc->m_aircraftDispProps;
	CAircraftTags& aircrafttags = pDoc->m_aircraftTags;
	std::vector< std::pair<int,int> >& aircraftdisppropidx = pDoc->m_vACDispPropIdx;
	std::vector< std::pair<int,int> >& aircrafttagpropidx = pDoc->m_vACTagPropIdx;

	AirsideFlightLogEntry element;

	AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
	element.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));
	
	//int nCount = airsideFlightLog.getCount();
	CAirsideEventLogBuffer<AirsideFlightEventStruct> &airsideFlightEventLog = pDoc->GetOutputAirside()->GetLogBuffer()->m_flightLog;
	size_t nCount = airsideFlightEventLog.m_lstElementEvent.size();

	for (size_t i = 0; i < nCount; i++)
	{

		airsideFlightLog.getItem(element, airsideFlightEventLog.m_lstElementEvent[i].first);
		//airsideFlightLog.getItem(element, i);

		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();
		AirsideFlightDescStruct fds  = element.GetAirsideDescStruct();

		if (nStartTime <= nCurTime && nEndTime >= nCurTime) 
		{
			//long nEventCount = element.getCount();
			size_t nEventCount = airsideFlightEventLog.m_lstElementEvent[i].second.size();

			long nextIdx = -1;
			for (size_t j = 0; j < nEventCount; j++)
			{
//				if (element.getEvent(j).time >= nCurTime )
				if (airsideFlightEventLog.m_lstElementEvent[i].second.at(j).time >= nCurTime )
				{
					nextIdx = j;
					break;
				}
			}
			if (nextIdx > 0)
			{
				//AirsideFlightEventStruct& pesB = element.getEvent(nextIdx);
				//AirsideFlightEventStruct& pesA = element.getEvent(nextIdx - 1);
				AirsideFlightEventStruct& pesB = airsideFlightEventLog.m_lstElementEvent[i].second.at(nextIdx);
				AirsideFlightEventStruct& pesA = airsideFlightEventLog.m_lstElementEvent[i].second.at(nextIdx - 1);

				CAircraftDispPropItem* pADPI = NULL; 
				CAircraftTagItem* pATI =NULL;
				if(pesA.state = 'A')
				{
					pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i].first);		
					pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i].first);

				}
				else
				{
					pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i].second);
					pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i].second);
				}

				if( !(pADPI && pADPI->IsVisible()) )
					continue;

				if( !(pATI&& pATI->IsShowTags()) )
					continue;


				// calculate the current location
				//int a = pesB.time - pesA.time;
				//int b = pesB.time - nCurTime;
				//float r = ((float) b) / a;
				float r=0.0;
				if(pesA.speed==0&&pesB.speed==0)
				{
					r = static_cast<float>(nCurTime-pesA.time)/static_cast<float>(pesB.time-pesA.time);
				}
				else
				{	
					double rate = static_cast<double>(nCurTime-pesA.time)/static_cast<double>(pesB.time-pesA.time);				
					double nCurSpeed = (1-rate)*pesA.speed+rate*pesB.speed;				
					double flyedLength = (pesA.speed+nCurSpeed)*(nCurTime-pesA.time);//pesA.speed*(nCurTime-pesA.time)+accel*(nCurTime-pesA.time)*(nCurTime-pesA.time)/2;
					double flyingLength  = (pesA.speed+pesB.speed)*(pesB.time-pesA.time);//pesA.speed*(pesB.time-pesA.time)+accel*(pesB.time-pesA.time)*(pesB.time-pesA.time)/2;			
					r = static_cast<float>(flyedLength/flyingLength);
				}

					
				double dSpd = pesA.speed * (1.0-r) + pesB.speed * r;

				ARCVector3 worldPos;
				worldPos[VX] = (1.0f - r) * pesA.x + r * pesB.x;
				worldPos[VY] = (1.0f - r) * pesA.y + r * pesB.y;

				double zA = pesA.z;
				double zB = pesB.z;
				worldPos[VZ] = static_cast<float>((1.0 - r)*zA + r*zB) - dAirportAlt;
				worldPos[VZ] += 440;				
				
				DrawFlightTag(fds,pesA, dSpd,worldPos[VZ], *pATI,worldPos,mvmatrix,projmatrix,viewport);

			}
		}
	}

	// Vehicle Tag
	CVehicleDispProps& vehicleDispprops = pDoc->m_vehicleDispProps;
	CVehicleTag& vehicleTag = pDoc->m_vehicleTags;
	std::vector<int>& vehicleDisppropidx = pDoc->m_vAVehicleDispPropIdx;

	AirsideVehicleLogEntry vehicleLogEntry;
	AirsideVehicleLog& airsideVehiclelog = airsideSimLogs.m_airsideVehicleLog;
	
	vehicleLogEntry.SetEventLog(&(airsideSimLogs.m_airsideVehicleEventLog));
	CAirsideEventLogBuffer<AirsideVehicleEventStruct>& airsideVehicleEventLog =  pDoc->GetOutputAirside()->GetLogBuffer()->m_vehicleLog;
	//int vehicleNodeCount = vehicleTag.GetNodeCount();
	BOOL nVehicleFlags=FALSE;
	DWORD dwTagInfo;
	size_t vehicleCount = airsideVehicleEventLog.m_lstElementEvent.size();
 
	for(size_t k=0; k<vehicleCount; k++)
	{
		CVehicleDispPropItem* pVDPI = vehicleDispprops.FindBestMatch(vehicleDisppropidx[k]);
		//if(!pVDPI->IsVisible())
		//	continue;
		int CurNodeIndex = vehicleTag.GetCurSelNodeIndex();
		int CurItemIndex = vehicleTag.GetCurSelItemIndexInCurSelNode();

//		CVehicleTagItem* pVTI = vehicleTag.GetItemShowTagsFlagInNode(CurNodeIndex,CurItemIndex,nVehicleFlags);

		if(vehicleTag.GetItemShowTagsFlagInNode(CurNodeIndex,CurItemIndex,nVehicleFlags))
		{
			if (nVehicleFlags!=TRUE)
				continue;
		}
		else
		{
			continue;
		}
		airsideVehiclelog.getItem(vehicleLogEntry,airsideVehicleEventLog.m_lstElementEvent[k].first);

		long vehicleStartTime = (long)vehicleLogEntry.getEntryTime();
		long vehicleEndTime = (long)vehicleLogEntry.getExitTime();
		const AirsideVehicleDescStruct& getdesc  = vehicleLogEntry.GetAirsideDesc();

		if(vehicleStartTime<=nCurTime && vehicleEndTime>=nCurTime)
		{
			size_t vEventCount = airsideVehicleEventLog.m_lstElementEvent[k].second.size();
			long vnextIdx = -1;
			for (size_t m = 0; m < vEventCount; m++)
			{
				//				if (element.getEvent(j).time >= nCurTime )
				if (airsideVehicleEventLog.m_lstElementEvent[k].second.at(m).time >= nCurTime )
				{
					vnextIdx = m;
					break;
				}
			}
			if (vnextIdx > 0)
			{
				AirsideVehicleEventStruct& vpesB = airsideVehicleEventLog.m_lstElementEvent[k].second.at(vnextIdx);
				AirsideVehicleEventStruct& vpesA = airsideVehicleEventLog.m_lstElementEvent[k].second.at(vnextIdx - 1);
    			// calculate the current location
				float r=0.0;
				if(vpesA.speed==0&&vpesB.speed==0)
				{
					r = static_cast<float>(nCurTime-vpesA.time)/static_cast<float>(vpesB.time-vpesA.time);
				}
				else
				{	
					double rate = static_cast<double>(nCurTime-vpesA.time)/static_cast<double>(vpesB.time-vpesA.time);				
					double nCurSpeed = (1-rate)*vpesA.speed+rate*vpesB.speed;				
					double flyedLength = (vpesA.speed+nCurSpeed)*(nCurTime-vpesA.time);//pesA.speed*(nCurTime-pesA.time)+accel*(nCurTime-pesA.time)*(nCurTime-pesA.time)/2;
					double flyingLength  = (vpesA.speed+vpesB.speed)*(vpesB.time-vpesA.time);//pesA.speed*(pesB.time-pesA.time)+accel*(pesB.time-pesA.time)*(pesB.time-pesA.time)/2;			
					r = static_cast<float>(flyedLength/flyingLength);
				}



				ARCVector3 vworldPos;
				vworldPos[VX] = (1.0f - r) * vpesA.x + r * vpesB.x;
				vworldPos[VY] = (1.0f - r) * vpesA.y + r * vpesB.y;

				double zA = vpesA.z;
				double zB = vpesB.z;
				double zZ = static_cast<double>(pDoc->m_iScale);
				vworldPos[VZ] = static_cast<float>((1.0 - r)*zA*zZ + r*zB*zZ);
				vworldPos[VZ] += 440;
                if(vehicleTag.GetItemTagInfoInNode(CurNodeIndex,CurItemIndex,dwTagInfo))
				{
					DrawVehicleTag(getdesc,vpesA,dwTagInfo,vworldPos);
				}

			}
		}		
	}
}

static void RenderFloors(C3DView* pView, const CFloors& vFloor )
{
	
	if(pView->GetCamera()->GetProjectionType() == C3DCamera::perspective)
	{
		std::vector<CFloor2*> vSortedFloors(vFloor.m_vFloors);
		std::sort(vSortedFloors.begin(), vSortedFloors.end(), CBaseFloor::CompareFloors);
		
		for(int i =0 ;i< (int)vSortedFloors.size();i++)
		{
			CFloor2 * pFloor = vSortedFloors.at(i);
			pFloor->DrawOGL(pView, pFloor->Altitude());
		}
	}
	else
	{
		CFloor2 * pFloor = ((CFloors&)vFloor).GetActiveFloor();
		if(pFloor)
			pFloor->DrawOGL(pView,pFloor->Altitude());
	}
}

inline static ARCRay GetMousePickRay(C3DView * pView, const CPoint& p)
{	
	// here we get the 3d point in space under the mouse by using the line between the points at depth = 0 and depth =1
	::wglMakeCurrent(pView->GetClientDC()->GetSafeHdc(), pView->GetHGLRC() );
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	GLint realy = viewport[3] - p.y;

	ARCVector3 pt1, pt2;
	gluUnProject((GLdouble) p.x, (GLdouble) realy, 0.0, mvmatrix, projmatrix, viewport, &pt1[VX], &pt1[VY], &pt1[VZ]);
	gluUnProject((GLdouble) p.x, (GLdouble) realy, 1.0, mvmatrix, projmatrix, viewport, &pt2[VX], &pt2[VY], &pt2[VZ]);
	::wglMakeCurrent(NULL, NULL);
	pt2 = (pt2 - pt1).Normalize();
	
	
	return ARCRay(Point(pt1[VX],pt1[VY],pt1[VZ]),Point(pt2[VX],pt2[VY],pt2[VZ]));
}
