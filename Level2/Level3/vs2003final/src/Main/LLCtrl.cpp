#include "StdAfx.h"
#include "llctrl.h"
#include ".\llctrl.h"

const int WS_EDIT = WS_CHILD | WS_VISIBLE | ES_CENTER | ES_MULTILINE;

CLLCtrl::CLLCtrl(void)
{
	nFlag = 0;
}

CLLCtrl::~CLLCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CLLCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnEnSetfocus)
	ON_WM_PAINT()
	ON_CBN_KILLFOCUS(IDC_ADDR5, OnKillFocusCombo)
	ON_EN_SETFOCUS(IDC_ADDR1,OnSetFocusEdit1)
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

int CLLCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	for (int ii = 0; ii < 4; ii++)
	{
		if (ii==0)
		{
			m_combo.Create(WS_CHILD|WS_VISIBLE | ES_CENTER|ES_MULTILINE| CBS_DROPDOWN, CRect(0,0,0,0), this, IDC_ADDR5);
			m_combo.SetFont(GetParent()->GetFont());
			//			edit.Create(WS_CHILD | ES_CENTER|ES_MULTILINE, CRect(0,0,0,0), this, IDC_ADDR1+5);

			m_Addr[ii].Create(WS_CHILD | WS_VISIBLE | ES_CENTER | ES_MULTILINE | ES_WANTRETURN, CRect(0,0,0,0), this, IDC_ADDR1);
			m_Addr[ii].LimitText(3);
			m_Addr[ii].SetParent(this);
			m_Addr[ii].SetFont(GetParent()->GetFont());

		}else if (ii==1)
		{
			m_Addr[ii].Create(WS_EDIT, CRect(0,0,0,0), this, IDC_ADDR1 + ii);
			m_Addr[ii].LimitText(3);
			m_Addr[ii].SetParent(this);
			m_Addr[ii].SetFont(GetParent()->GetFont());
		}
		else 
		{
			m_Addr[ii].Create(WS_EDIT, CRect(0,0,0,0), this, IDC_ADDR1 + ii);
			m_Addr[ii].LimitText(2);
			m_Addr[ii].SetParent(this);		
			m_Addr[ii].SetFont(GetParent()->GetFont());
		}

	}

	return 0;
}

void CLLCtrl::OnSize(UINT nType, int cx, int cy)
{
/*	CWnd::OnSize(nType, cx, cy);
	
	CDC*	pDC = GetDC();
	CSize	szDot = pDC->GetTextExtent(_T("."), 1);
	int nDotWidth = szDot.cx;
	ReleaseDC(pDC);	

	CRect	rcClient;
	GetClientRect(&rcClient);
	int nEditWidth = (rcClient.Width() - (3 * nDotWidth)) / 4;
	int nEditHeight = rcClient.Height();

	CRect rect=CRect(0,0,nEditWidth,nEditHeight);
	m_rcEdit[0]=rect;
	rect.OffsetRect(nEditWidth + nDotWidth, 0);
	m_rcEdit[1]=rect;
	rect.OffsetRect(nEditWidth + nDotWidth, 0);
	m_rcEdit[2]=rect;
	rect.OffsetRect(nEditWidth + nDotWidth, 0);
	m_rcEdit[3]=rect;*/


	CWnd::OnSize(nType, cx, cy);

	CDC*	pDC = GetDC();
	CSize	szDot = pDC->GetTextExtent(_T("."), 1);
	int nDotWidth = szDot.cx;
	ReleaseDC(pDC);	

	CRect	rcClient;
	GetClientRect(&rcClient);
	int nEditWidth = (rcClient.Width() - (3 * nDotWidth)-4) / 4;
	int nEditHeight = rcClient.Height();
	int cyEdge = ::GetSystemMetrics(SM_CYEDGE);


	CRect rect = CRect(1, cyEdge, nEditWidth+nDotWidth, nEditHeight);
	CRect rectCombo=CRect(0,0,nEditWidth+nDotWidth,nEditHeight);
	CRect rectEdit1=CRect(1, cyEdge, nEditWidth+4, nEditHeight);
	for (int ii = 0; ii < 4; ii++)
	{
		if (ii==0)
		{
			m_rcAddr[ii]=rect;
			m_combo.MoveWindow(rectCombo);
			m_combo.ShowWindow(SW_HIDE);
	
			m_Addr[ii].MoveWindow(rectEdit1);
			rect.OffsetRect(nEditWidth + nDotWidth, 0);
		}
		else
		{
			m_rcAddr[ii] = rect;
			m_Addr[ii].MoveWindow(rect);
			rect.OffsetRect(nEditWidth + nDotWidth, 0);
			
		}
	}

	rect = CRect(nEditWidth, 0, nEditWidth + nDotWidth+4, nEditHeight-3);
	for (ii = 0; ii < 3; ii++)
	{
		if (ii==0)
		{
			rect.OffsetRect(nEditWidth + nDotWidth, 0);
		}
		else
		{
			m_rcDot[ii] = rect;
			rect.OffsetRect(nEditWidth + nDotWidth, 0);
		}
	}
}

void CLLCtrl::OnChildChar(UINT nChar, UINT nRepCnt, UINT nFlags, CLLEdit& child)
{
	int iNext = 0; 
	if (VK_LEFT == nChar)
	{
		for(int i = 1;i < 4;++i)
		{
			if(child.m_hWnd == m_Addr[i].m_hWnd)
			{
				iNext = i - 1;
				if(iNext <= 0)
					iNext = 3;
				m_Addr[iNext].SetSel(0,-1);
				m_Addr[iNext].SetFocus();
			}
		}
	}
	else if(VK_RIGHT == nChar || VK_TAB == nChar ||	VK_DECIMAL  == nChar ||	VK_SPACE == nChar ||	VK_RETURN == nChar)
	{
		for(int i = 1;i < 4;++i)
		{
			if(child.m_hWnd == m_Addr[i].m_hWnd)
			{
				iNext = i + 1;
				if(iNext >= 4)
				{			
					nFlag = 1;
					this->SetFocus();
					break;
				}
			//		iNext = 1;
				m_Addr[iNext].SetSel(0,-1);
				m_Addr[iNext].SetFocus();
			}
		}
	}
}

void CLLCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rectClient;
	GetClientRect(&rectClient);
	dc.FillSolidRect(&rectClient, RGB(255, 255, 255));
//	dc.TextOut(0, 0, _T("asdf"));

//	int nOldMode = dc.SetBkMode(TRANSPARENT);

	//COLOR_GRAYTEXTCOLOR_WINDOWTEXT
	COLORREF crText;

	crText = ::GetSysColor(COLOR_WINDOWTEXT);

	COLORREF crOldText = dc.SetTextColor(crText);
	SetFont(GetParent()->GetFont());
	

	//which separate the four edit controls
	for (int ii = 0; ii < 3; ii++)
	{
		if (ii==0)
		{
		}
		if (ii==1)
		{
			dc.DrawText(_T(":"), 1, m_rcDot[1], DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
		}
		if(ii==2)
			dc.DrawText(_T(":"), 1, m_rcDot[2], DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
	}
//	dc.SetBkMode(nOldMode);
	dc.SetTextColor(crOldText);

}

void CLLCtrl::OnLButtonDblClk(UINT nHitTest, CPoint point)
{
/*	CDC*	pDC = GetDC();
	CSize	szDot = pDC->GetTextExtent(_T("."), 1);
	int nDotWidth = szDot.cx;
	ReleaseDC(pDC);	

	CRect	rcClient;
	GetClientRect(&rcClient);
	int nEditWidth = (rcClient.Width() - (3 * nDotWidth)) / 4;
	int nEditHeight = rcClient.Height();

	CRect rect=CRect(0,0,nEditWidth + 10, nEditHeight);
	m_rcEdit[0]=rect;
	rect.OffsetRect(nEditWidth + nDotWidth, 0);
	m_rcEdit[1]=rect;
	rect.OffsetRect(nEditWidth + nDotWidth, 0);
	m_rcEdit[2]=rect;
	rect.OffsetRect(nEditWidth + nDotWidth, 0);
	m_rcEdit[3]=rect;

	

//	ClientToScreen(m_rcEdit[0]);
	if(m_rcEdit[0].PtInRect(point))
	{
		if (!::IsWindow(m_combo.m_hWnd))
			m_combo.Create(WS_CHILD|WS_VISIBLE|ES_CENTER|ES_MULTILINE|CBS_DROPDOWN, m_rcEdit[0], this, IDC_ADDR5);
		
		m_combo.ResetContent();
		m_combo.SetFont(GetParent()->GetParent()->GetFont());
		m_combo.ShowWindow(SW_SHOW);
		m_combo.AddString("111");
		m_combo.AddString("22");

	}
*/
	CWnd::OnLButtonDblClk(nHitTest, point);
}

void CLLCtrl::OnKillFocusCombo()
{
	int n=m_combo.GetCurSel();
	CString strValue;
	m_combo.GetLBText(n,strValue);
	m_Addr[0].SetWindowText(strValue);

	m_combo.ShowWindow(SW_HIDE);
	m_Addr[0].ShowWindow(SW_SHOW);
}

void CLLCtrl::OnEnSetfocus()
{
//	MessageBox("aaaa");
}

void CLLCtrl::OnSetFocusEdit1()
{

	CString strValue;
	m_Addr[0].GetWindowText(strValue);
	
	int n=m_combo.FindString(-1,strValue);
	if(n==LB_ERR)
	{
		m_combo.SetCurSel(0);
	}
	else
	{
		m_combo.SetCurSel(n);
	}
	
	m_Addr[0].ShowWindow(SW_HIDE);
	m_combo.ShowWindow(SW_SHOW);
	m_combo.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CIPAddrEdit

CLLEdit::CLLEdit()
{
	m_nMax=59;
}
CLLEdit::~CLLEdit()
{
}
void CLLEdit::SetMax(int nMax)
{
	m_nMax=nMax;
}
BEGIN_MESSAGE_MAP(CLLEdit, CEdit)
	//{{AFX_MSG_MAP(CIPAddrEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPAddrEdit message handlers

void CLLEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	if (VK_TAB == nChar ||
		'.' == nChar ||
		' ' == nChar ||
		VK_RETURN == nChar)
		m_pParent->OnChildChar(nChar, nRepCnt, nFlags, *this);
	else if (('0' <= nChar && '9'>= nChar) || iscntrl(nChar))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);

		CString strValue;
		GetWindowText(strValue);
		if (  _ttoi(strValue) > m_nMax)
		{
			SetFocus();
			strValue = strValue.Left(strValue.GetLength() - 1);			
			SetWindowText(strValue);
			SetSel(strValue.GetLength(),strValue.GetLength());
			return;
		}

		if ((int)GetLimitText() == GetWindowTextLength() || _ttoi(strValue) == m_nMax)
		{
			/*			CString strValue;
			GetWindowText(strValue);
			if (_ttoi(strValue)>m_nMax)
			{
			CString strError;
			strError.Format("Input Error ,the value must at the range of 0 to %d",m_nMax);
			GetParent()->MessageBox(strError);
			SetFocus();
			SetSel(0,strlen(strValue));
			return;
			}
			*/		
			m_pParent->OnChildChar(VK_TAB, 0, 0, *this);
		}		
		//		if (3 == GetWindowTextLength())

	}
	else
		::MessageBeep(0xFFFFFFFF);
}

void CLLEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	if (VK_LEFT == nChar || VK_RIGHT == nChar)
	{
		CPoint ptCaret = GetCaretPos();
		int nCharPos = LOWORD(CharFromPos(ptCaret));
		if ((VK_LEFT == nChar && nCharPos == 0) ||
			(VK_RIGHT == nChar && nCharPos == GetWindowTextLength()))
			m_pParent->OnChildChar(nChar, nRepCnt, nFlags, *this);
	}
	else if (VK_ESCAPE == nChar)
	{
/*
		TCHAR	cClass1[12];
		TCHAR	cClass2[12];

		// Clear out window class name arrays
		memset(cClass1, 0, sizeof(cClass1));
		memset(cClass2, 0, sizeof(cClass2));


		CWnd* pDialog = m_pParent->GetParent();		// Get parent of the IP address edit control
		CWnd* pDialogParent = pDialog->GetParent();	// Get its parent

		// Get class names of the windows that own the IP address edit control and its parent

		if (NULL != pDialog)
			::GetClassName(pDialog->m_hWnd, cClass1, sizeof(cClass1)/sizeof(TCHAR));
		if (NULL != pDialogParent)
			::GetClassName(pDialogParent->m_hWnd, cClass2, sizeof(cClass2)/sizeof(TCHAR));


		if (0 == _tcscmp(cClass1, szDialogClass) && 0 == _tcscmp(cClass2, szDialogClass))
		{
			CWnd* pCancel = pDialogParent->GetDlgItem(IDCANCEL);	// Get the Cancel button
			pDialogParent->SendMessage(WM_COMMAND,
				MAKEWPARAM(IDCANCEL, BN_CLICKED),
				(LPARAM) (NULL != pCancel->m_hWnd ? pCancel->m_hWnd : NULL));
			return;
		}
*/	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CLLEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

}
void CLLEdit::SetParent(CLLCtrl* pParent)
{
	m_pParent = pParent;							
}

void CLLCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	if (::IsWindow(m_combo.m_hWnd))
		m_combo.ShowWindow(SW_HIDE);

	if(nFlag == 0)
	{
		m_Addr[1].SetSel(0,-1);
		m_Addr[1].SetFocus();
	}
	else if(nFlag == 1)
	{
		nFlag = 0;
	}
}

void CLLCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (::IsWindow(m_combo.m_hWnd))
		m_combo.ShowWindow(SW_HIDE);
	CWnd::OnLButtonDown(nFlags, point);
}
void CLLCtrl::SetComboBoxItem(CString strItem)
{
	m_combo.AddString(strItem);
}

void CLLCtrl::SetItemRange(int n1,int n2/* =0  */,int n3/* =0 */)
{
	if (n1!=0)
	{
		m_Addr[1].SetMax(n1);
	}
	if(n2!=0)
	{
		m_Addr[2].SetMax(n2);
	}
	if(n3!=0)
	{
		m_Addr[3].SetMax(n3);
	}
}



void CLLCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

}

//format string 
// N,120:12.12 or S,90:12.22
void CLLCtrl::GetCtrlText(CString &strText)
{
	CString str1,str2,str3,str4;
	m_Addr[0].GetWindowText(str1);
	m_Addr[1].GetWindowText(str2);
	m_Addr[2].GetWindowText(str3);
	m_Addr[3].GetWindowText(str4);
	str1.TrimLeft();
	str1.TrimRight();

	str2.TrimLeft();
	str2.TrimRight();
	if (strlen(str2)==0)
		str2="0";

	str3.TrimLeft();
	str3.TrimRight();
	if (strlen(str3)==0)
		str3="0";

	str4.TrimLeft();
	str4.TrimRight();
	if (strlen(str4)==0)
		str4="0";

	strText.Format("%s %s:%s:%s",str1,str2,str3,str4);
}

long CLLCtrl::GetCtrlValue()
{
	CString str1,str2,str3,str4;
	m_Addr[0].GetWindowText(str1);
	m_Addr[1].GetWindowText(str2);
	m_Addr[2].GetWindowText(str3);
	m_Addr[3].GetWindowText(str4);
	str1.TrimLeft();
	str1.TrimRight();

	str2.TrimLeft();
	str2.TrimRight();
	if (strlen(str2)==0)
		str2="0";

	str3.TrimLeft();
	str3.TrimRight();
	if (strlen(str3)==0)
		str3="0";

	str4.TrimLeft();
	str4.TrimRight();
	if (strlen(str4)==0)
		str4="0";	

	long lValue = atol(str2)*60*60+atol(str3)*60+atol(str4);

	if (str1.CompareNoCase("W") ==0 || str1.CompareNoCase("S") ==0)
	{
		lValue = lValue *(-1);
	}

	return lValue;

}

void CLLCtrl::SetCtrlText(CString strText)
{
	strText.TrimLeft();
	strText.TrimRight();
	if (strlen(strText)==0)
	{
		CString strCombo;
		if (m_combo.GetCount()>0)
		{
			m_combo.GetLBText(0,strCombo);
			m_Addr[0].SetWindowText(strCombo);
		}
		m_Addr[1].SetWindowText("0");
		m_Addr[2].SetWindowText("0");
		m_Addr[3].SetWindowText("0");
	}
	else
	{
		CStringArray saArray;
		if(SplitString(strText,strlen(strText)," ",saArray)==2)
		{

			// N or s
			CString strValue=saArray.GetAt(0);
			m_Addr[0].SetWindowText(strValue);
/*			int n=m_combo.FindString(-1,strValue);
			if(n==LB_ERR)
			{
				m_combo.SetCurSel(0);
			}
			else
			{
				m_combo.SetCurSel(n);
			}
*/			//120:12.12

			strValue=saArray.GetAt(1);
			saArray.RemoveAll();
			if(SplitString(strValue,strlen(strValue),":",saArray)==3)
			{
				//120
				strValue=saArray.GetAt(0);
				m_Addr[1].SetWindowText(strValue);

				//strValue=saArray.GetAt(1);
				//saArray.RemoveAll();
				//if (SplitString(strValue,strlen(strValue),".",saArray)==2)
				{
					//12
					m_Addr[2].SetWindowText(saArray.GetAt(1));
					//12
					m_Addr[3].SetWindowText(saArray.GetAt(2));
				}
			}
		}
	}
}

int CLLCtrl::SplitString(LPCTSTR lpszString, int nStrLen, int chSep, CStringArray &strArray)
{
	if(strArray.GetSize()>0)
		strArray.RemoveAll();
	char ch;
	if(nStrLen>0)
	{
		char *pBuffer=new char[nStrLen+1];
		int index=0,nTemp=0;
		CString strText;
		while(index<nStrLen)
		{
			ch=lpszString[index];
			if(ch==chSep)
			{
				pBuffer[nTemp]='\0';
				nTemp=0;
				strArray.Add(pBuffer);
			}
			else
			{
				pBuffer[nTemp++]=ch;
				if(index==nStrLen-1)
				{
					pBuffer[nTemp]='\0';
					strArray.Add(pBuffer);
				}
			}
			index++;
		}
		delete pBuffer;
	}
	return strArray.GetSize();
}


int CLLCtrl::SplitString(LPCTSTR lpszString, int nStrLen, LPCTSTR lpszSep, CStringArray &strArray)
{
	if(strArray.GetSize()>0)
		strArray.RemoveAll();
	if(nStrLen>0)
	{
		if(lpszSep==NULL || strlen(lpszSep)==0)
		{
			strArray.Add(lpszString);
			return 1;
		}
		char *pBuffer=new char[nStrLen+1];
		int index=0,nTemp=0;
		char *pTemp=strstr(lpszString+index,lpszSep);
		while(pTemp)
		{
			nTemp=pTemp-lpszString;
			strncpy(pBuffer,lpszString+index,nTemp-index);
			pBuffer[nTemp-index]='\0';
			strArray.Add(pBuffer);
			index=nTemp+strlen(lpszSep);
			if(index>=nStrLen)
				break;
			pTemp=strstr(lpszString+index,lpszSep);
			if(!pTemp)
				strArray.Add(lpszString+index);
		}
		if(index==0)
		{
			strArray.Add(lpszString);
		}
		delete pBuffer;
	}
	return strArray.GetSize();
}
void CLLEdit::OnEnChange()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*CString strValue;
	GetWindowText(strValue);
	if (_ttoi(strValue)>m_nMax)
	{
		CString strError;
		strError.Format("Input Error ,the value must at the range of 0 to %d",m_nMax);
		GetParent()->MessageBox(strError);
		SetFocus();
		SetSel(0,strlen(strValue));
		return;
	}*/
}

void CLLEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);
	SetSel(0,-1);
	// TODO: Add your message handler code here
}
