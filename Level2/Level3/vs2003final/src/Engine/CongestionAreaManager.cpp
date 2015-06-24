
#include "StdAfx.h"

#include "congestionareamanager.h"

#include "congestionAreaEvent.h"
#include "CongestionAreaGrid.h"

#include "Inputs\PipeDataSet.h"
#include "engine\TERMINAL.H"
#include "../Common/ARCTracker.h"
#include "TerminalMobElementBehavior.h"


#define STEPDIST 140
#define PRECISION 0.00001
#define SAMEPOINT(pt1, pt2) (pt1.distance(pt2) < PRECISION)
#define SAMEFLOAT(f1, f2) ( abs(f1-f2) < PRECISION )

CCongestionAreaManager::CCongestionAreaManager(void) : DataSet(CongestionParamFile)
{
	m_vectAreas.clear();
	m_vectPipes.clear();
	m_pAreas = NULL;
	m_pPipes = NULL;
	m_vGrid.clear();
}

CCongestionAreaManager::~CCongestionAreaManager(void)
{
	if(m_vGrid.size() > 0)
	{
		for(UINT i = 0; i < m_vGrid.size(); i++)
		{
			delete m_vGrid[i];
		}
		m_vGrid.clear();
	}
}

void CCongestionAreaManager::clear()
{
	m_congParam.dLambda = 0.0;
	m_congParam.dTrigger = 0.0;
	m_vectAreas.clear();
	m_vectPipes.clear();
}

void CCongestionAreaManager::readData(ArctermFile& p_file)
{
	if(p_file.getLine() == 1)
	{
		// read the value of lambda
		p_file.getFloat(m_congParam.dLambda);

		// read the value of trigger
		p_file.getFloat(m_congParam.dTrigger);

		int nCount = 0;
		p_file.getInteger(nCount);
		int i = 0;
		for(i = 0; i < nCount; i++)
		{
			int nValue = 0;
			p_file.getInteger(nValue);
			m_vectAreas.push_back(nValue);
		}
		p_file.getInteger(nCount);
		for(i = 0; i < nCount; i++)
		{
			int nValue = 0;
			p_file.getInteger(nValue);
			m_vectPipes.push_back(nValue);
		}
	}
}

void CCongestionAreaManager::readObsoleteData(ArctermFile& p_file)
{

}

void CCongestionAreaManager::writeData(ArctermFile& p_file) const
{
	p_file.writeFloat((float)m_congParam.dLambda);
	p_file.writeFloat((float)m_congParam.dTrigger);

	int nSize = m_vectAreas.size();
	p_file.writeInt(nSize);
	int i = 0;
	for(i = 0; i < nSize; i++)
	{
		p_file.writeInt(m_vectAreas.at(i));
	}

	nSize = m_vectPipes.size();
	p_file.writeInt(nSize);
	for(i = 0; i < nSize; i++)
	{
		p_file.writeInt(m_vectPipes[i]);
	}

	p_file.writeLine();
}

//initialize congestion grid for every floor
void CCongestionAreaManager::InitGrid(Terminal* pTerminal, int nFloor)
{
	PLACE_METHOD_TRACK_STRING();
	m_pAreas = pTerminal->m_pAreas;
	m_pPipes = pTerminal->m_pPipeDataSet->GetPipeVector();
	if(m_pAreas == NULL && m_pPipes == NULL)
		return ;

	CAreas* vArea = new CAreas[nFloor];
	ZeroMemory((char*)vArea, sizeof(CAreas)*nFloor);
	UINT i = 0;
	CAreaList area = m_pAreas->m_vAreas;
	for(i = 0; i < m_vectAreas.size(); i++)
	{
		int nFloor = area[m_vectAreas[i]]->floor;
		vArea[nFloor].m_vAreas.push_back(area[m_vectAreas[i]]); 
	}

	typedef std::vector<CPipe*> CPipePtrList;
	CPipePtrList* pvPipe = new CPipePtrList[nFloor];
	for(i = 0; i < m_vectPipes.size(); i++)
	{
		int nCurFloor = (int)m_pPipes->at(m_vectPipes[i]).GetZ() / 100;
		pvPipe[nCurFloor].push_back(&(m_pPipes->at(m_vectPipes[i])));
	}

	if(m_vGrid.size() > 0)
	{
		for(i = 0; i < m_vGrid.size(); i++)
		{
			delete m_vGrid[i];
		}
		m_vGrid.clear();
	}

	for(int j = 0; j < nFloor; j++)
	{
		CCongestionAreaGrid* pGrid = new CCongestionAreaGrid;
        pGrid->InitGrid(&vArea[j], pvPipe[j]);
		m_vGrid.push_back(pGrid);
	}

	delete [] pvPipe;
}

//check if the line through the congestion area, 
BOOL CCongestionAreaManager::CheckAndHandleCongestion(Person* pPerson, ElapsedTime eventTime, BOOL bAreaCong)
{
	//check the data
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	Point ptStart;
	pPerson->getTerminalPoint(ptStart);
	int nFloor = (int)ptStart.getZ() / 100;
	if( !m_vGrid[nFloor]->IsValidCongestionArea() )
		return FALSE;

	Point ptEnd;
	pPerson->getTerminalDest(ptEnd);
	if(SAMEPOINT(ptStart, ptEnd) || !SAMEFLOAT(ptStart.getZ(), ptEnd.getZ()) )
		return FALSE;

	CCongestionAreaCell* pCell = NULL;
	Point ptDest = Point(0.0, 0.0, 0.0);

	//the line between start and end,
	//then get the intersect point between the line and the congestion area
	Point ptIntersect = Point(0.0, 0.0, ptStart.getZ());
	BOOL bExist = m_vGrid[nFloor]->GetInterSectPoint(ptStart, ptEnd, ptIntersect);
	if (!bExist)
		return FALSE;

//	pCell = m_vGrid[nFloor]->GetCell(ptIntersect);
	//		if(pCell == NULL)
	//			return FALSE;

	if(pCell = m_vGrid[nFloor]->GetCell(ptStart))
		pCell->EnterCell(pPerson->GetAdminGroupSize());//start cell

	if (pCell)
	{
		ptIntersect = m_vGrid[nFloor]->GetNearestPoint(ptStart,ptEnd);
	}
	ptDest = ptIntersect;

	//save the old destination point, old speed and old location

	
	if (spTerminalBehavior)
	{
		spTerminalBehavior->SetOldDestPoint(ptEnd);
		spTerminalBehavior->SetOldSpeed(pPerson->getSpeed());
		spTerminalBehavior->SetOldLocation(ptStart);
		spTerminalBehavior->processBillboard(eventTime);
		//set the new destination
		pPerson->setTerminalDestination(ptDest);
		//calculate the time and add event
		ElapsedTime time = eventTime + pPerson->moveTime();
		CongestionAreaEvent* event = new CongestionAreaEvent;
		event->init(pPerson, time,false);
		event->addEvent();
	}
	
	return TRUE;
}

//process congestion event and generate the next event
int CCongestionAreaManager::ProcessCongestionEvent(CongestionAreaEvent* pEvent, ElapsedTime eventTime)
{
	Person* pPerson = (Person*)pEvent->GetMobileElement();
	if(pPerson == NULL)
		return 0;
	
	// step1. Leave Cell
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return 0;
	}
	Point floorpt;
	pPerson->getTerminalPoint(floorpt);
	int nFloor = int(floorpt.getZ()) / 100;

	CCongestionAreaCell* pCell = m_vGrid[nFloor]->GetCell(spTerminalBehavior->GetOldLocation());
	if(pCell != NULL)// && pPerson->GetOldLocation() != pPerson->GetOldStartPoint()
	{
		if(pCell->GetPersonsCount()==0)
		{
			CString str;
			str.Format("1-%d", pPerson->getID());
			AfxMessageBox(str);
		}
		pCell->LeaveCell(pPerson->GetAdminGroupSize());
	}
	// step2. Enter Cell
	Point ptDest;
	pPerson->getTerminalDest(ptDest);
	pCell = m_vGrid[nFloor]->GetCell(ptDest);
//	ASSERT(pCell != NULL);//if the cell is null, last time has already exit
	if(pCell != NULL)
		pCell->EnterCell(pPerson->GetAdminGroupSize());
	// setp3. save the current point as old location
	spTerminalBehavior->SetOldLocation(ptDest);
	// setp4. Write log
	pPerson->writeLogEntry(eventTime, false,false);
	// step5. Next event.
	//calculate the next event destination;
    Point pt;
	pPerson->getTerminalPoint(pt);
	const Point& ptOld = spTerminalBehavior->GetOldDestPoint();
	Point ptNext = /*pt.GetDistancePoint(ptOld, STEPDIST)*/m_vGrid[nFloor]->GetNearestPoint(pt,ptOld);
	//if get null, leaving the congestion area, reset the old destionation and speed;
	//if the ptNext equal ptOld, the person arrived at the end next processor
	pCell = m_vGrid[nFloor]->GetCell(ptNext);
	if(pCell == NULL //the end point not in the congestion area
		|| SAMEPOINT(ptNext, ptOld) ) //the end point in the congestion area,but get the end
	{	
		//if the person leaving the congestion area, leave the current cell,
		CCongestionAreaCell* pPreCell = m_vGrid[nFloor]->GetCell(spTerminalBehavior->GetOldLocation());
		if(pPreCell != NULL)
		{
			if(pPreCell->GetPersonsCount()==0)
			{
				CString str;
				str.Format("2-%d", pPerson->getID());
				AfxMessageBox(str);
			}

			pPreCell->LeaveCell(pPerson->GetAdminGroupSize());
		}
		pPerson->setTerminalDestination(spTerminalBehavior->GetOldDestPoint());
		pPerson->setSpeed(spTerminalBehavior->GetOldSpeed());
		if(pCell == NULL && pPerson->getState() != WalkOnPipe)
		{
			//pPerson->setLocation(ptNext); 
			if(CheckAndHandleCongestion(pPerson, eventTime, TRUE))
				return 1;
			else
				spTerminalBehavior->processGeneralMovementHelp(eventTime);
		}
		else
			spTerminalBehavior->processGeneralMovementHelp(eventTime);
		return 0;
	}

	//reset the new speed to calculate the next event time;
	int nCount = pCell->GetPersonsCount();
	DistanceUnit dSpeed = GetNewSpeed(pPerson, nCount);
	pPerson->setSpeed(dSpeed);
	pPerson->setTerminalDestination(ptNext);
	//Add the next event to the event list;
	ElapsedTime time = eventTime + pPerson->moveTime();
	CongestionAreaEvent* event = new CongestionAreaEvent;
	event->init(pPerson, time,false);
	event->addEvent();
	
	return 1;
}
//get the new speed, nCount is the number of the person
DistanceUnit CCongestionAreaManager::GetNewSpeed(Person* pPerson, int nCount)
{
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	DistanceUnit dOldSpeed = spTerminalBehavior->GetOldSpeed();
	if(m_congParam.dTrigger <= (double)nCount)
	{
//		double d = m_congParam.dLambda*exp(m_congParam.dLambda*(nCount-m_congParam.dTrigger)) - m_congParam.dLambda;
		double d = m_congParam.dLambda*(nCount - m_congParam.dTrigger);
		d = min(d, 0.75);
		d = max(d, 0);
		return (dOldSpeed - dOldSpeed*d);
	}
	return dOldSpeed;
}

