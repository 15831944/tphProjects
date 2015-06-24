// ProcessorTagWnd.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "inputs\con_db.h"
#include "inputs\srvctime.h"
#include "inputs\assigndb.h"
#include "inputs\AllPaxTypeFlow.h"
#include "inputs\SubFlowList.h"
#include "inputs\SubFlow.h"
#include "inputs\ProcData.h"
#include "TermPlanDoc.h"
#include "ProcessorTagWnd.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const LOGFONT lfTitle = {
	14, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial"
};
static const LOGFONT lfText = {
	14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial"
};

/////////////////////////////////////////////////////////////////////////////
// CProcessorTagWnd dialog


CProcessorTagWnd::CProcessorTagWnd(UINT idx, CTermPlanDoc* pDoc)
{
	m_nProcIdx = idx;
	m_pDoc = pDoc;
}

BEGIN_MESSAGE_MAP(CProcessorTagWnd, CWnd)
	//{{AFX_MSG_MAP(CProcessorTagWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessorTagWnd message handlers

void CProcessorTagWnd::OnPaint() 
{	
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(&rcClient);

	COLORREF colWindow = GetSysColor(COLOR_WINDOW);
	//COLORREF colMenuBar = GetSysColor(COLOR_MENUBAR);
	COLORREF colMenu = 0x00fcfbfa;//MixedColor(colWindow,colMenuBar);
	COLORREF colActiveCaption = 0x00c0c0c0;//GetSysColor(COLOR_ACTIVECAPTION);

	dc.FillSolidRect(&rcClient, colWindow);

	CFont fTitle; 
    fTitle.CreateFontIndirect(&lfTitle);
	CFont* pOldFont = dc.SelectObject(&fTitle);

	SIZE size = {0,0};
	VERIFY(::GetTextExtentPoint32(dc.GetSafeHdc(),m_sTitle,m_sTitle.GetLength(),&size));

	CRect rcTitle(rcClient.left, rcClient.top, rcClient.right, rcClient.top+size.cy+4);

	COLOR16 r = (COLOR16) ((colActiveCaption & 0x000000FF)<<8);
    COLOR16 g = (COLOR16) (colActiveCaption & 0x0000FF00);
    COLOR16 b = (COLOR16) ((colActiveCaption & 0x00FF0000)>>8);

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = rcTitle.left;
	vert [0] .y      = rcTitle.top;
	vert [0] .Red    = r;
	vert [0] .Green  = g;
	vert [0] .Blue   = b;
	vert [0] .Alpha  = 0x0000;

	r = (COLOR16) ((colMenu & 0x000000FF)<<8);
    g = (COLOR16) (colMenu & 0x0000FF00);
    b = (COLOR16) ((colMenu & 0x00FF0000)>>8);

	vert [1] .x      = rcTitle.right;
	vert [1] .y      = rcTitle.bottom; 
	vert [1] .Red    = r;
	vert [1] .Green  = g;
	vert [1] .Blue   = b;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(dc.GetSafeHdc(),vert,2,&gRect,1,GRADIENT_FILL_RECT_H);

	dc.SetBkMode(TRANSPARENT);
	rcTitle.DeflateRect(10,0,0,0);
	dc.DrawText(m_sTitle, rcTitle, DT_LEFT|DT_NOCLIP|DT_VCENTER|DT_SINGLELINE);

	CFont fText; 
    fText.CreateFontIndirect(&lfText);
	dc.SelectObject(&fText);

	CRect rcTextArea(rcClient);
	rcTextArea.DeflateRect(5, rcTitle.Height(), 5, 0);

	dc.DrawText(m_sTagNames, &rcTextArea, DT_LEFT|DT_NOCLIP);

	rcTextArea.DeflateRect(120, 0, 0, 0);

	//CRect rcTest(rcTextArea);
	//dc.DrawText(m_sWidestLineValue, &rcTest, DT_LEFT|DT_NOCLIP|DT_CALCRECT);
	//// TRACE("Widest line %d pix\n", rcTest.Width());
	
	dc.DrawText(m_sTagValues, &rcTextArea, DT_LEFT|DT_NOCLIP);

	dc.SelectObject(pOldFont);

	dc.Draw3dRect(&rcClient, EDGE_ETCHED, BF_BOTTOMRIGHT);

	// Do not call CDialog::OnPaint() for painting messages
}

void CProcessorTagWnd::DrawTitle(const CRect& r)
{
	
}

static int BuildTitleString(CTermPlanDoc* pDoc, UINT nProcIdx, CString& s)
{
	static char buf[256];
	CProcessor2* pProc2 = pDoc->GetProcWithSelName(nProcIdx);
	ASSERT(pProc2 && pProc2->GetProcessor());
	pProc2->GetProcessor()->getID()->printID(buf);
	s.Empty();
	s = buf;
	return s.GetLength();
}

CSize CProcessorTagWnd::BuildProcTagStrings(CDC* pDC, CString& _sTagNames, CString& _sTagValues)
{
	static const char* _YES = "Yes\n";
	static const char* _NO = "No\n";
	const CProcessorTags& proctags = m_pDoc->m_procTags;
	static char szProcName[256];
	static char tempbuf[1024];
	int len;

	CSize szRet(0,0); //extents of the text

	CProcessor2* pProc2 = m_pDoc->GetProcWithSelName(m_nProcIdx);
	ASSERT(pProc2 && pProc2->GetProcessor());
	pProc2->GetProcessor()->getID()->printID(szProcName);

	_sTagValues.Empty();
	_sTagNames.Empty();
	m_sWidestLineValue = "placement";

	int _nMaxWidthValues = pDC->GetTextExtent("placement").cx; //placement
	int _nNumLines = 0;

	if(proctags.Get(0)) {
		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += CProcessorTags::PROCTAGNAMES[0];
		if(proctags.Get(1)) {
			_sTagValues+="placement\n"; _nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[1];
		}
		if(proctags.Get(2)) {
			_sTagValues+=szProcName;
			_sTagValues+="\n"; _nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[2];
			if((len = pDC->GetTextExtent(szProcName).cx) > _nMaxWidthValues)
				{ _nMaxWidthValues = len; m_sWidestLineValue = szProcName; }
		}
		if(proctags.Get(3)) {
			_sTagValues+=PROC_TYPE_NAMES[pProc2->GetProcessor()->getProcessorType()];
			_sTagValues+="\n"; _nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[3];
			if((len = pDC->GetTextExtent(PROC_TYPE_NAMES[pProc2->GetProcessor()->getProcessorType()]).cx) > _nMaxWidthValues)
				{ _nMaxWidthValues = len; m_sWidestLineValue = PROC_TYPE_NAMES[pProc2->GetProcessor()->getProcessorType()]; }
		}
		if(proctags.Get(4)) {
			if(pProc2->GetProcessor()->queueLength() > 0)
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[4];
		}
		if(proctags.Get(5)) {
			if(pProc2->GetProcessor()->queueIsFixed())
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[5];
		}
		if(proctags.Get(6)) {
			if(pProc2->GetProcessor()->inConstraintLength() > 0)
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[6];
		}
		if(proctags.Get(7)) {
			if(pProc2->GetProcessor()->outConstraintLength() > 0)
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[7];
		}
		if(proctags.Get(8)) {
			_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[8];
		}
		if(proctags.Get(9)) {
			_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[9];
		}
	}
	if(proctags.Get(10)) {
		_sTagValues+="\n\n";
		_sTagNames += "\n";
		_sTagNames += CProcessorTags::PROCTAGNAMES[10];
		_nNumLines+=2;
		if(proctags.Get(11)) {
			ProcessorDatabase* pSTDB = (ProcessorDatabase*) m_pDoc->GetTerminal().serviceTimeDB;
			int nCount = pSTDB->getCount();
			int nDBIdx = -1;
			for(int i=0; i<nCount; i++) {
				const ProcessorID* pProcID = pSTDB->getProcName( i );
				if(pProc2->GetProcessor()->getID()->isChildOf(*pProcID)) {	
					nDBIdx = i;
				}
				if(pProc2->GetProcessor()->getID()->compare(*pProcID) == 0) {
					nDBIdx = i;
					break;
				}
			}
			if(nDBIdx == -1) {
				strcpy(tempbuf, "DEFAULT - CONSTANT(0)\n");
				_sTagValues += tempbuf;
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[11];
				if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
			}
			else {
				ConstraintDatabase* pConDB = m_pDoc->GetTerminal().serviceTimeDB->getDatabase(nDBIdx);
				int nCDBCount = pConDB->getCount();
				for( i=0; i<nCDBCount; i++ ) {
					ConstraintEntry* pEntry = pConDB->getItem( i );
					const Constraint* pCon = pEntry->getConstraint();
					//char szName[256];
					CString szName;
					pCon->screenPrint( szName, 0, 256 );
					const ProbabilityDistribution* pProbDist = pEntry->getValue();
					char szDist[1024];
					pProbDist->screenPrint( szDist );
					sprintf(tempbuf, "%s - %s\n", szName.GetBuffer(0), szDist);
					_sTagValues+=tempbuf;
					_nNumLines++;
					if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					if(i==0)
						_sTagNames += CProcessorTags::PROCTAGNAMES[11];
					else
						_sTagNames += "\n";
				}
			}
		}
		if(proctags.Get(12)) { //process
			int iCount = m_pDoc->GetTerminal().m_pSubFlowList->GetProcessUnitCount();
			bool bInProcess = false;
			if(iCount>0) {
				for(int i=0; i<iCount; i++ ) {
					CSubFlow* pProcessFlow = m_pDoc->GetTerminal().m_pSubFlowList->GetProcessUnitAt(i);
					CSinglePaxTypeFlow* pFlow = pProcessFlow->GetInternalFlow();
					if(pFlow->IfExist(*(pProc2->GetProcessor()->getID()))) {
						sprintf(tempbuf, "%s\n", pProcessFlow->GetProcessUnitName());
						_sTagValues+=tempbuf;
						_nNumLines++;
						if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
							{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					}
				}
			}
			
			if(!bInProcess) {
				_sTagValues+="NONE\n";
				_nNumLines++;
			}

			_sTagNames += CProcessorTags::PROCTAGNAMES[12];

		}
		if(proctags.Get(13)) { //flows
			_sTagValues+="\n";
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[13];
		}
		if(proctags.Get(14)) { //roster

			ProcessorDatabase* pRDB = (ProcessorDatabase*) m_pDoc->GetTerminal().procAssignDB;
			int nCount = pRDB->getCount();
			int nDBIdx = -1;
			for(int i=0; i<nCount; i++) {
				const ProcessorID* pProcID = pRDB->getProcName( i );
				if(pProc2->GetProcessor()->getID()->isChildOf(*pProcID)) {	
					nDBIdx = i;
				}
				if(pProc2->GetProcessor()->getID()->compare(*pProcID) == 0) {
					nDBIdx = i;
					break;
				}
			}
			if(nDBIdx == -1) {
				strcpy(tempbuf, "DEFAULT - (0:00 to 24:00)\n");
				_sTagValues += tempbuf;
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[14];
				if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
			}
			else {
				ProcessorRosterSchedule* pProcSchd = m_pDoc->GetTerminal().procAssignDB->getDatabase(nDBIdx);
				int nRDBCount = pProcSchd->getCount();
				for( i=0; i<nRDBCount; i++ ) {

					ProcessorRoster* pPA = pProcSchd->getItem(i);
					char szOpen[128], szClose[128];
					CString szName;//[1024];
					pPA->getAbsOpenTime().printTime(szOpen,false);
					pPA->getAbsCloseTime().printTime(szClose,false);
					pPA->getAssignment()->getConstraint(0)->screenPrint(szName, 0, 128);					
					sprintf(tempbuf, "%s - (%s to %s)\n", szName.GetBuffer(0), szOpen, szClose);
					_sTagValues+=tempbuf;
					_nNumLines++;
					if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					if(i==0)
						_sTagNames += CProcessorTags::PROCTAGNAMES[14];
					else
						_sTagNames += "\n";
				}
			}
		}
		if(proctags.Get(15)) { //bag device
			if(pProc2->GetProcessor()->getProcessorType() != BaggageProc) {
				_sTagValues+="N/A\n";
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[15];
			}
			else {
				//(flight type, delivery dist) pairs
				_sTagValues+="YES\n";
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[15];
			}
		}
		if(proctags.Get(16)) { //behaviours
			MiscProcessorData* pMiscDB = m_pDoc->GetTerminal().miscData->getDatabase(pProc2->GetProcessor()->getProcessorType());
			int nProcIdx = pMiscDB->findBestMatch(*(pProc2->GetProcessor()->getID()));
			if(nProcIdx != -1) {
				//linkages
				MiscData* pData = ((MiscDataElement*) pMiscDB->getItem(nProcIdx))->getData();
				ProcessorIDList* pGateIDList = pData->getGateList();
				MiscProcessorIDList* pWaitIDList = pData->getWaitAreaList();
				bool bLinked = false;

				_sTagNames += CProcessorTags::PROCTAGNAMES[16];

				if(pProc2->GetProcessor()->getProcessorType()==DepSourceProc) {
					//DEP SOURCE
					ProcessorIDList* pDepIDList = ((MiscDepSourceData*)pData)->getDependentList();
					if(pDepIDList->getCount()>0) {
						_sTagValues+="Dependents:\n";  _nNumLines++;
						for(int i=0; i<pDepIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pDepIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames+="\n";
							_nNumLines++;
							if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}

				}
				else if(pProc2->GetProcessor()->getProcessorType()==DepSinkProc) {
					//DEP SINK
					ProcessorIDList* pDepIDList = ((MiscDepSinkData*)pData)->getDependentList();
					if(pDepIDList->getCount()>0) {
						_sTagValues+="Dependents:\n";  _nNumLines++;
						for(int i=0; i<pDepIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pDepIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames+="\n";
							_nNumLines++;
							if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}
				}
				else if(pProc2->GetProcessor()->getProcessorType() == GateProc) {
					//GATE PROC
					ProcessorIDList* pHoldIDList = ((MiscGateData*)pData)->getHoldAreaList();
					if(pHoldIDList->getCount()>0) {
						_sTagValues+="Holding Areas:\n"; _nNumLines++;
						for(int i=0; i<pHoldIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pHoldIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames +="\n";
							_nNumLines++;
							if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}
				}
				else if(pProc2->GetProcessor()->getProcessorType() == BaggageProc) {
					//BAGGAGE PROC
					ProcessorIDList* pBypassIDList = ((MiscBaggageData*)pData)->getBypassList();
					if(pBypassIDList->getCount()>0) {
						_sTagValues+="Bypass Processors:\n"; _nNumLines++;
						for(int i=0; i<pBypassIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pBypassIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames +="\n";
							_nNumLines++;
							if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}
				}

				if(pGateIDList->getCount()>0) {
					//LINKED GATES
					_sTagValues+="Linked Gates:\n"; _nNumLines++;
					for(int i=0; i<pGateIDList->getCount(); i++) {
						strcpy(tempbuf, "  ");
						pGateIDList->getItem(i)->printID(tempbuf+2);
						_sTagValues+=tempbuf;
						_sTagValues+="\n";
						_sTagNames+="\n";
						_nNumLines++;
						if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
							{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					}
					bLinked = true;
				}

				if(pWaitIDList->getCount()>0) {
					//LINKED WAIT AREAS
					_sTagValues+="Linked Wait Areas:\n"; _nNumLines++;
					for(int i=0; i<pWaitIDList->getCount(); i++) {
						strcpy(tempbuf, "  ");
						pWaitIDList->getItem(i)->printID(tempbuf+2);
						_sTagValues+=tempbuf;
						_sTagValues+="\n";
						_sTagNames+="\n";
						_nNumLines++;
						if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
							{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					}
					bLinked = true;	
				}
				

				if(!bLinked) {
					_sTagValues+="NOT LINKED\n";
					_nNumLines++;
				}			
				
				//disallow groups
				_sTagValues+= "Disallow groups - ";
				_sTagValues+= pData->GetDisallowGroupFlag()?_YES:_NO;
				_sTagNames +="\n";
				_nNumLines++;

				//pop cap
				if(pData->GetQueueCapacity() > 0)
					sprintf(tempbuf, "Population capacity - %d\n", pData->GetQueueCapacity());
				else
					strcpy(tempbuf, "Population capacity - None\n");
				_sTagValues+= tempbuf;
				_nNumLines++;
				_sTagNames +="\n";
				if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

				//allows visitors
				_sTagValues+= "Allows visitors - ";
				_sTagValues+= pData->allowsVisitors()?_YES:_NO;
				_sTagNames +="\n";
				_nNumLines++;
				if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

				//recycle freq
				sprintf(tempbuf, "Recycle frquency - %d\n", pData->GetCyclicFreq());
				_sTagValues+= tempbuf;
				_sTagNames +="\n";
				_nNumLines++;
				if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
				
			}
			else {
				_sTagValues+="NO BEHAVIOURS DATA\n";
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[16];
			}
		}
		if(proctags.Get(17)) { //train sched
			_sTagValues+="\n";
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[17];
		}
		if(proctags.Get(18)) { //moving walkway
			_sTagValues+="\n";
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[18];
		}
		if(proctags.Get(19)) { //elevator
			_sTagNames += CProcessorTags::PROCTAGNAMES[19];
			if(pProc2->GetProcessor()->getProcessorType() == Elevator) {
				MiscProcessorData* pMiscDB = m_pDoc->GetTerminal().miscData->getDatabase(Elevator);
				int nProcIdx = pMiscDB->findBestMatch(*(pProc2->GetProcessor()->getID()));
				if(nProcIdx != -1) {
					MiscElevatorData* pData = (MiscElevatorData*) ((MiscDataElement*) pMiscDB->getItem(nProcIdx))->getData();
					sprintf(tempbuf, "Speed - %.2f m/s\n", pData->getSpeed(MiscElevatorData::EST_NORMAL));
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					sprintf(tempbuf, "Accel - %.2f m/s\n", pData->getSpeed(MiscElevatorData::EST_ACCELERATION));
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					sprintf(tempbuf, "Decel - %.2f m/s\n", pData->getSpeed(MiscElevatorData::EST_DECELERATION));
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					sprintf(tempbuf, "Capacity - %d", pData->getCapacity());
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					_sTagNames+="\n\n\n\n";
				}
			}
			else {
				_sTagValues+="N/A\n";
				_nNumLines++;	
			}
		}
	}

	szRet.cx = 125 + _nMaxWidthValues + 15;
	szRet.cy = _nNumLines*14+20;
	return szRet;
}

void CProcessorTagWnd::BuildProcTagStrings(CString& _sTagNames, CString& _sTagValues, int& _nMaxWidthNames, int& _nMaxWidthValues, int& _nNumLines)
{
	static const char* _YES = "Yes\n";
	static const char* _NO = "No\n";
	const CProcessorTags& proctags = m_pDoc->m_procTags;
	static char szProcName[256];
	static char tempbuf[1024];
	int len;

	CProcessor2* pProc2 = m_pDoc->GetProcWithSelName(m_nProcIdx);
	ASSERT(pProc2 && pProc2->GetProcessor());
	pProc2->GetProcessor()->getID()->printID(szProcName);

	_sTagValues.Empty();
	_sTagNames.Empty();
	m_sWidestLineValue = "placement";

	_nMaxWidthValues = 9; //placement
	_nMaxWidthNames = 21; //Geographic Properties
	_nNumLines = 0;

	if(proctags.Get(0)) {
		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += CProcessorTags::PROCTAGNAMES[0];
		if(proctags.Get(1)) {
			_sTagValues+="placement\n"; _nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[1];
		}
		if(proctags.Get(2)) {
			_sTagValues+=szProcName;
			_sTagValues+="\n"; _nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[2];
			if((len = strlen(szProcName)) > _nMaxWidthValues)
				{ _nMaxWidthValues = len; m_sWidestLineValue = szProcName; }
		}
		if(proctags.Get(3)) {
			_sTagValues+=PROC_TYPE_NAMES[pProc2->GetProcessor()->getProcessorType()];
			_sTagValues+="\n"; _nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[3];
			if((len = strlen(PROC_TYPE_NAMES[pProc2->GetProcessor()->getProcessorType()])) > _nMaxWidthValues)
				{ _nMaxWidthValues = len; m_sWidestLineValue = PROC_TYPE_NAMES[pProc2->GetProcessor()->getProcessorType()]; }
		}
		if(proctags.Get(4)) {
			if(pProc2->GetProcessor()->queueLength() > 0)
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[4];
		}
		if(proctags.Get(5)) {
			if(pProc2->GetProcessor()->queueIsFixed())
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[5];
		}
		if(proctags.Get(6)) {
			if(pProc2->GetProcessor()->inConstraintLength() > 0)
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[6];
		}
		if(proctags.Get(7)) {
			if(pProc2->GetProcessor()->outConstraintLength() > 0)
				_sTagValues+=_YES;
			else
				_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[7];
		}
		if(proctags.Get(8)) {
			_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[8];
		}
		if(proctags.Get(9)) {
			_sTagValues+=_NO;
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[9];
		}
	}
	if(proctags.Get(10)) {
		_sTagValues+="\n\n";
		_sTagNames += "\n";
		_sTagNames += CProcessorTags::PROCTAGNAMES[10];
		_nNumLines+=2;
		if(proctags.Get(11)) {
			ProcessorDatabase* pSTDB = (ProcessorDatabase*) m_pDoc->GetTerminal().serviceTimeDB;
			int nCount = pSTDB->getCount();
			int nDBIdx = -1;
			for(int i=0; i<nCount; i++) {
				const ProcessorID* pProcID = pSTDB->getProcName( i );
				if(pProc2->GetProcessor()->getID()->isChildOf(*pProcID)) {	
					nDBIdx = i;
				}
				if(pProc2->GetProcessor()->getID()->compare(*pProcID) == 0) {
					nDBIdx = i;
					break;
				}
			}
			if(nDBIdx == -1) {
				strcpy(tempbuf, "DEFAULT - CONSTANT(0)\n");
				_sTagValues += tempbuf;
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[11];
				if((len = strlen(tempbuf)) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
			}
			else {
				ConstraintDatabase* pConDB = m_pDoc->GetTerminal().serviceTimeDB->getDatabase(nDBIdx);
				int nCDBCount = pConDB->getCount();
				for( i=0; i<nCDBCount; i++ ) {
					ConstraintEntry* pEntry = pConDB->getItem( i );
					const Constraint* pCon = pEntry->getConstraint();
					//char szName[256];
					CString szName;
					pCon->screenPrint( szName, 0, 256 );
					const ProbabilityDistribution* pProbDist = pEntry->getValue();
					char szDist[1024];
					pProbDist->screenPrint( szDist );
					sprintf(tempbuf, "%s - %s\n", szName.GetBuffer(0), szDist);
					_sTagValues+=tempbuf;
					_nNumLines++;
					if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					if(i==0)
						_sTagNames += CProcessorTags::PROCTAGNAMES[11];
					else
						_sTagNames += "\n";
				}
			}
		}
		if(proctags.Get(12)) { //process
			int iCount = m_pDoc->GetTerminal().m_pSubFlowList->GetProcessUnitCount();
			bool bInProcess = false;
			if(iCount>0) {
				for(int i=0; i<iCount; i++ ) {
					CSubFlow* pProcessFlow = m_pDoc->GetTerminal().m_pSubFlowList->GetProcessUnitAt(i);
					CSinglePaxTypeFlow* pFlow = pProcessFlow->GetInternalFlow();
					if(pFlow->IfExist(*(pProc2->GetProcessor()->getID()))) {
						sprintf(tempbuf, "%s\n", pProcessFlow->GetProcessUnitName());
						_sTagValues+=tempbuf;
						_nNumLines++;
						if((len = strlen(tempbuf)) > _nMaxWidthValues)
							{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					}
				}
			}
			
			if(!bInProcess) {
				_sTagValues+="NONE\n";
				_nNumLines++;
			}

			_sTagNames += CProcessorTags::PROCTAGNAMES[12];

		}
		if(proctags.Get(13)) { //flows
			_sTagValues+="\n";
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[13];
		}
		if(proctags.Get(14)) { //roster

			ProcessorDatabase* pRDB = (ProcessorDatabase*) m_pDoc->GetTerminal().procAssignDB;
			int nCount = pRDB->getCount();
			int nDBIdx = -1;
			for(int i=0; i<nCount; i++) {
				const ProcessorID* pProcID = pRDB->getProcName( i );
				if(pProc2->GetProcessor()->getID()->isChildOf(*pProcID)) {	
					nDBIdx = i;
				}
				if(pProc2->GetProcessor()->getID()->compare(*pProcID) == 0) {
					nDBIdx = i;
					break;
				}
			}
			if(nDBIdx == -1) {
				strcpy(tempbuf, "DEFAULT - (0:00 to 24:00)\n");
				_sTagValues += tempbuf;
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[14];
				if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
			}
			else {
				ProcessorRosterSchedule* pProcSchd = m_pDoc->GetTerminal().procAssignDB->getDatabase(nDBIdx);
				int nRDBCount = pProcSchd->getCount();
				for( i=0; i<nRDBCount; i++ ) {

					ProcessorRoster* pPA = pProcSchd->getItem(i);
					char szOpen[128], szClose[128];
					CString szName;//[128];
					pPA->getAbsOpenTime().printTime(szOpen,false);
					pPA->getAbsCloseTime().printTime(szClose,false);
					pPA->getAssignment()->getConstraint(0)->screenPrint(szName, 0, 128);					
					sprintf(tempbuf, "%s - (%s to %s)\n", szName.GetBuffer(0), szOpen, szClose);
					_sTagValues+=tempbuf;
					_nNumLines++;
					if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					if(i==0)
						_sTagNames += CProcessorTags::PROCTAGNAMES[14];
					else
						_sTagNames += "\n";
				}
			}
		}
		if(proctags.Get(15)) { //bag device
			if(pProc2->GetProcessor()->getProcessorType() != BaggageProc) {
				_sTagValues+="N/A\n";
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[15];
			}
			else {
				//(flight type, delivery dist) pairs
				_sTagValues+="YES\n";
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[15];
			}
		}
		if(proctags.Get(16)) { //behaviours
			MiscProcessorData* pMiscDB = m_pDoc->GetTerminal().miscData->getDatabase(pProc2->GetProcessor()->getProcessorType());
			int nProcIdx = pMiscDB->findBestMatch(*(pProc2->GetProcessor()->getID()));
			if(nProcIdx != -1) {
				//linkages
				MiscData* pData = ((MiscDataElement*) pMiscDB->getItem(nProcIdx))->getData();
				ProcessorIDList* pGateIDList = pData->getGateList();
				MiscProcessorIDList* pWaitIDList = pData->getWaitAreaList();
				bool bLinked = false;

				_sTagNames += CProcessorTags::PROCTAGNAMES[16];

				if(pProc2->GetProcessor()->getProcessorType()==DepSourceProc) {
					//DEP SOURCE
					ProcessorIDList* pDepIDList = ((MiscDepSourceData*)pData)->getDependentList();
					if(pDepIDList->getCount()>0) {
						_sTagValues+="Dependents:\n";  _nNumLines++;
						for(int i=0; i<pDepIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pDepIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames+="\n";
							_nNumLines++;
							if((len = strlen(tempbuf)) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}

				}
				else if(pProc2->GetProcessor()->getProcessorType()==DepSinkProc) {
					//DEP SINK
					ProcessorIDList* pDepIDList = ((MiscDepSinkData*)pData)->getDependentList();
					if(pDepIDList->getCount()>0) {
						_sTagValues+="Dependents:\n";  _nNumLines++;
						for(int i=0; i<pDepIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pDepIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames+="\n";
							_nNumLines++;
							if((len = strlen(tempbuf)) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}
				}
				else if(pProc2->GetProcessor()->getProcessorType() == GateProc) {
					//GATE PROC
					ProcessorIDList* pHoldIDList = ((MiscGateData*)pData)->getHoldAreaList();
					if(pHoldIDList->getCount()>0) {
						_sTagValues+="Holding Areas:\n"; _nNumLines++;
						for(int i=0; i<pHoldIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pHoldIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames +="\n";
							_nNumLines++;
							if((len = strlen(tempbuf)) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}
				}
				else if(pProc2->GetProcessor()->getProcessorType() == BaggageProc) {
					//BAGGAGE PROC
					ProcessorIDList* pBypassIDList = ((MiscBaggageData*)pData)->getBypassList();
					if(pBypassIDList->getCount()>0) {
						_sTagValues+="Bypass Processors:\n"; _nNumLines++;
						for(int i=0; i<pBypassIDList->getCount(); i++) {
							strcpy(tempbuf, "  ");
							pBypassIDList->getItem(i)->printID(tempbuf+2);
							_sTagValues+=tempbuf;
							_sTagValues+="\n";
							_sTagNames +="\n";
							_nNumLines++;
							if((len = strlen(tempbuf)) > _nMaxWidthValues)
								{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
						}
						bLinked = true;
					}
				}

				if(pGateIDList->getCount()>0) {
					//LINKED GATES
					_sTagValues+="Linked Gates:\n"; _nNumLines++;
					for(int i=0; i<pGateIDList->getCount(); i++) {
						strcpy(tempbuf, "  ");
						pGateIDList->getItem(i)->printID(tempbuf+2);
						_sTagValues+=tempbuf;
						_sTagValues+="\n";
						_sTagNames+="\n";
						_nNumLines++;
						if((len = strlen(tempbuf)) > _nMaxWidthValues)
							{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					}
					bLinked = true;
				}

				if(pWaitIDList->getCount()>0) {
					//LINKED WAIT AREAS
					_sTagValues+="Linked Wait Areas:\n"; _nNumLines++;
					for(int i=0; i<pWaitIDList->getCount(); i++) {
						strcpy(tempbuf, "  ");
						pWaitIDList->getItem(i)->printID(tempbuf+2);
						_sTagValues+=tempbuf;
						_sTagValues+="\n";
						_sTagNames+="\n";
						_nNumLines++;
						if((len = strlen(tempbuf)) > _nMaxWidthValues)
							{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					}
					bLinked = true;	
				}
				

				if(!bLinked) {
					_sTagValues+="NOT LINKED\n";
					_nNumLines++;
				}			
				
				//disallow groups
				_sTagValues+= "Disallow groups - ";
				_sTagValues+= pData->GetDisallowGroupFlag()?_YES:_NO;
				_sTagNames +="\n";
				_nNumLines++;

				//pop cap
				if(pData->GetQueueCapacity() > 0)
					sprintf(tempbuf, "Population capacity - %d\n", pData->GetQueueCapacity());
				else
					strcpy(tempbuf, "Population capacity - None\n");
				_sTagValues+= tempbuf;
				_nNumLines++;
				_sTagNames +="\n";
				if((len = strlen(tempbuf)) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

				//allows visitors
				_sTagValues+= "Allows visitors - ";
				_sTagValues+= pData->allowsVisitors()?_YES:_NO;
				_sTagNames +="\n";
				_nNumLines++;
				if((len = strlen(tempbuf)) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

				//recycle freq
				sprintf(tempbuf, "Recycle frquency - %d\n", pData->GetCyclicFreq());
				_sTagValues+= tempbuf;
				_sTagNames +="\n";
				_nNumLines++;
				if((len = strlen(tempbuf)) > _nMaxWidthValues)
					{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
				
			}
			else {
				_sTagValues+="NO BEHAVIOURS DATA\n";
				_nNumLines++;
				_sTagNames += CProcessorTags::PROCTAGNAMES[16];
			}
		}
		if(proctags.Get(17)) { //train sched
			_sTagValues+="\n";
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[17];
		}
		if(proctags.Get(18)) { //moving walkway
			_sTagValues+="\n";
			_nNumLines++;
			_sTagNames += CProcessorTags::PROCTAGNAMES[18];
		}
		if(proctags.Get(19)) { //elevator
			_sTagNames += CProcessorTags::PROCTAGNAMES[19];
			if(pProc2->GetProcessor()->getProcessorType() == Elevator) {
				MiscProcessorData* pMiscDB = m_pDoc->GetTerminal().miscData->getDatabase(Elevator);
				int nProcIdx = pMiscDB->findBestMatch(*(pProc2->GetProcessor()->getID()));
				if(nProcIdx != -1) {
					MiscElevatorData* pData = (MiscElevatorData*) ((MiscDataElement*) pMiscDB->getItem(nProcIdx))->getData();
					sprintf(tempbuf, "Speed - %.2f m/s\n", pData->getSpeed(MiscElevatorData::EST_NORMAL));
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					sprintf(tempbuf, "Accel - %.2f m/s\n", pData->getSpeed(MiscElevatorData::EST_ACCELERATION));
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					sprintf(tempbuf, "Decel - %.2f m/s\n", pData->getSpeed(MiscElevatorData::EST_DECELERATION));
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					sprintf(tempbuf, "Capacity - %d", pData->getCapacity());
					_sTagValues+=tempbuf; _nNumLines++;
					if((len = strlen(tempbuf)) > _nMaxWidthValues)
						{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }
					_sTagNames+="\n\n\n\n";
				}
			}
			else {
				_sTagValues+="N/A\n";
				_nNumLines++;	
			}
		}
	}
}

int CProcessorTagWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();

	CFont fText; 
    fText.CreateFontIndirect(&lfText);
	CFont* pOldFont = pDC->SelectObject(&fText);
	
	CSize sz = BuildProcTagStrings(pDC, m_sTagNames, m_sTagValues);

	BuildTitleString(m_pDoc, m_nProcIdx, m_sTitle);

	pDC->SelectObject(pOldFont);

	CRect r;
	this->GetWindowRect(&r);
	r.right = r.left + sz.cx;
	r.bottom = r.top + sz.cy;
	this->MoveWindow(&r);
	
	return 0;
}

/*
int CProcessorTagWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	int nMaxLenNames;// = BuildProcTagNamesString(m_pDoc, m_sTagNames);
	int nMaxLenValues;// = BuildProcTagValuesString(m_pDoc, m_nProcIdx, m_sTagValues);
	int nNumLines;

	CDC* pDC = GetDC();

	CFont fText; 
    fText.CreateFontIndirect(&lfText);
	CFont* pOldFont = pDC->SelectObject(&fText);
	

	//CSize sz = BuildProcTagStrings(pDC, m_sTagNames, m_sTagValues);
	BuildProcTagStrings(m_sTagNames, m_sTagValues, nMaxLenNames, nMaxLenValues, nNumLines);

	BuildTitleString(m_pDoc, m_nProcIdx, m_sTitle);

	//CRect rcTest(0,0,0,0);
	//pDC->DrawText(m_sWidestLineValue, &rcTest, DT_LEFT|DT_NOCLIP|DT_CALCRECT);
	//// TRACE("Widest line %d pix\n", rcTest.Width());


	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);

	//m_szTagNames.cx = nMaxLenNames*tm.tmAveCharWidth;
	m_szTagValues.cx = nMaxLenValues*tm.tmAveCharWidth;
	m_szTagValues.cy = (nNumLines * 14);
	//m_szTagNames.cy = m_szTagValues.cy = (nNumLines * 14);

	CRect r;
	this->GetWindowRect(&r);
	r.right = r.left + 125 + m_szTagValues.cx + 15;
	r.bottom = r.top + m_szTagValues.cy + 20;
	this->MoveWindow(&r);
	
	return 0;
}
*/

void CProcessorTagWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}
