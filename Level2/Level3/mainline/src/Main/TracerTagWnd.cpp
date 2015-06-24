// AirsideTracerTagWnd implementation file
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TracerTagWnd.h"

#include "TermPlanDoc.h"

#include "results\AirsideFlightLogEntry.h"
#include "results\FLTLOG.h"


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

static const char* AIRSIDETRACERTAGNAMES[] = {
	"Departure Information\n",
	"    Flight ID\n",
	"    Destination\n",
	"    Aircraft Type\n",
	"Arrival Information\n",
	"    Flight ID\n",
	"    Origin\n",
	"    Aircraft Type\n",
};


static const char* TERMINALTRACERTAGNAMES[] = {
	"ID\n",
	"Element Type\n",
	"Arr ID\n",
	"Dep ID\n",
};


/////////////////////////////////////////////////////////////////////////////
// CTracerTagWnd dialog


CTracerTagWnd::CTracerTagWnd(TracerType type, UINT idx, CTermPlanDoc* pDoc)
{
	m_eTracerType = type;
	m_nTracerIdx = idx;
	m_pDoc = pDoc;
}

BEGIN_MESSAGE_MAP(CTracerTagWnd, CWnd)
	//{{AFX_MSG_MAP(CTracerTagWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTracerTagWnd message handlers

void CTracerTagWnd::OnPaint() 
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

	dc.DrawText(m_sTagValues, &rcTextArea, DT_LEFT|DT_NOCLIP);

	dc.SelectObject(pOldFont);

	dc.Draw3dRect(&rcClient, EDGE_ETCHED, BF_BOTTOMRIGHT);

	// Do not call CDialog::OnPaint() for painting messages
}

void CTracerTagWnd::DrawTitle(const CRect& r)
{

}

int CTracerTagWnd::BuildTitleString(CString& s)
{
	static char buf[256];

	if(Terminal == m_eTracerType) {
		MobLogEntry mle = m_pDoc->GetMobLogEntry(m_nTracerIdx);
		s.Empty();
		s =  m_pDoc->GetPaxTypeID(mle.GetMobDesc());
		return s.GetLength();
	}
	else if(Airside == m_eTracerType) {
		AirsideFlightLogEntry afle = m_pDoc->GetAirsideFlightLogEntry(m_nTracerIdx);
		s.Empty();
		s = afle.GetAirsideDesc()._airline.GetValue();
		s += afle.GetAirsideDesc().depID;
		s += " | ";
		s += afle.GetAirsideDesc()._airline.GetValue();
		s += afle.GetAirsideDesc().arrID;
		return s.GetLength();
	}
	else if(Landside == m_eTracerType) {
		s.Empty();
		return s.GetLength();
	}
	else {
		ASSERT(FALSE);
		return 0;
	}
}




CSize CTracerTagWnd::BuildTagStrings(CDC* pDC, CString& _sTagNames, CString& _sTagValues)
{
	static const char* _YES = "Yes\n";
	static const char* _NO = "No\n";

	static const char* _id_names[] = {
		"Pax ID\n",
		"Visitor ID\n",
		"Checked Baggage ID\n",
		"Hand Bag ID\n",
		"Carte ID\n",
		"Wheelchair ID\n",
		"Animal ID\n",
		"Car ID\n",
		"ID\n",
	};
	
	static const int _id_names_count = (sizeof(_id_names)/sizeof(char*))-1;

	static char tempbuf[1024];
	int len;

	CSize szRet(0,0); //extents of the text

	if(Terminal == m_eTracerType) {
		MobLogEntry mle = m_pDoc->GetMobLogEntry(m_nTracerIdx);
		const MobDescStruct& mds = mle.GetMobDesc();

		_sTagValues.Empty();
		_sTagNames.Empty();
		m_sWidestLineValue = "";

		int _nMaxWidthValues = pDC->GetTextExtent(m_sWidestLineValue).cx;
		int _nNumLines = 0;

		sprintf(tempbuf, "%d\n", mds.id);
		_sTagValues += tempbuf; _nNumLines++;
		if(mds.mobileType > _id_names_count)
			_sTagNames += _id_names[_id_names_count];
		else
			_sTagNames += _id_names[mds.mobileType];
		
		if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

		//------------------------------------------------------------
		// discarded by Benny, to use new method
// 		CMobileElemConstraint pc(&(m_pDoc->GetTerminal()));
// 		//pc.SetInputTerminal();
// 		pc.setIntrinsicType(mds.intrinsic);
// 		for(int l=0; l<MAX_PAX_TYPES; l++)
// 			pc.setUserType(l,mds.paxType[l]);
// 		pc.SetTypeIndex( mds.mobileType );
// 		pc.screenPrint(tempbuf,0,1024);
		int nDispPropIndex = m_pDoc->m_paxDispProps.FindBestMatch(mds, &m_pDoc->GetTerminal());
		if (-1 != nDispPropIndex)
		{
			strcpy(tempbuf, m_pDoc->m_paxDispProps.GetDispPropItem(nDispPropIndex)->GetPaxType()->GetName());
		}
		else
		{
			ASSERT(FALSE);
			strcpy(tempbuf, "N/A");
		}
		//------------------------------------------------------------

		strcat(tempbuf, "\n");
		_sTagValues += tempbuf; _nNumLines++;
		_sTagNames += TERMINALTRACERTAGNAMES[1];
		if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

		if(mds.arrFlight != -1)
			m_pDoc->GetTerminal().flightLog->getFlightID( tempbuf, mds.arrFlight, 'A' );
		else
			strcpy(tempbuf, "N/A");
		strcat(tempbuf, "\n");
		_sTagValues += tempbuf; _nNumLines++;
		_sTagNames += TERMINALTRACERTAGNAMES[2];
		if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

		if(mds.depFlight != -1)
			m_pDoc->GetTerminal().flightLog->getFlightID( tempbuf, mds.depFlight, 'D' );
		else
			strcpy(tempbuf, "N/A");
		strcat(tempbuf, "\n");
		_sTagValues += tempbuf; _nNumLines++;
		_sTagNames += TERMINALTRACERTAGNAMES[3];
		if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

		szRet.cx = 125 + _nMaxWidthValues + 15;
		szRet.cy = _nNumLines*14+20;
		return szRet;
	}
	else if(Airside == m_eTracerType) {

		AirsideFlightLogEntry afle = m_pDoc->GetAirsideFlightLogEntry(m_nTracerIdx);
		const AirsideFlightDescStruct& afds = afle.GetAirsideDesc();

		_sTagValues.Empty();
		_sTagNames.Empty();
		m_sWidestLineValue = "";

		int _nMaxWidthValues = pDC->GetTextExtent(m_sWidestLineValue).cx;
		int _nNumLines = 0;

		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[0];

		sprintf(tempbuf, "%s%s\n", afds._airline.GetValue(), afds.depID);
		_sTagValues += tempbuf; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[1];
		if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

		_sTagValues += afds.dest;
		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[2];
		if((len = pDC->GetTextExtent(afds.dest).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = afds.dest; }

		_sTagValues += afds.acType;
		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[3];
		if((len = pDC->GetTextExtent(afds.acType).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = afds.acType; }

		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[4];

		sprintf(tempbuf, "%s%s\n", afds._airline.GetValue(), afds.arrID);
		_sTagValues += tempbuf; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[5];
		if((len = pDC->GetTextExtent(tempbuf).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = tempbuf; }

		_sTagValues += afds.origin;
		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[6];
		if((len = pDC->GetTextExtent(afds.origin).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = afds.origin; }

		_sTagValues += afds.acType;
		_sTagValues += "\n"; _nNumLines++;
		_sTagNames += AIRSIDETRACERTAGNAMES[7];
		if((len = pDC->GetTextExtent(afds.acType).cx) > _nMaxWidthValues)
		{ _nMaxWidthValues = len; m_sWidestLineValue = afds.acType; }

		szRet.cx = 125 + _nMaxWidthValues + 15;
		szRet.cy = _nNumLines*14+20;
		return szRet;
	}
	else if(Landside == m_eTracerType) {
		_sTagValues.Empty();
		_sTagNames.Empty();
		return szRet;
	}
	else {
		ASSERT(FALSE);
		return szRet;
	}
}


int CTracerTagWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();

	CFont fText; 
	fText.CreateFontIndirect(&lfText);
	CFont* pOldFont = pDC->SelectObject(&fText);

	CSize sz = BuildTagStrings(pDC, m_sTagNames, m_sTagValues);

	BuildTitleString(m_sTitle);

	pDC->SelectObject(pOldFont);

	CRect r;
	this->GetWindowRect(&r);
	r.right = r.left + sz.cx;
	r.bottom = r.top + sz.cy;
	this->MoveWindow(&r);

	return 0;
}