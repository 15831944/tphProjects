// FloarEdit.cpp : implementation file
//

#include "stdafx.h"
#include "FloarEdit.h"


// CFloarEdit

IMPLEMENT_DYNAMIC(CFloarEdit, CEdit)
CFloarEdit::CFloarEdit()
{
}

CFloarEdit::~CFloarEdit()
{
}


BEGIN_MESSAGE_MAP(CFloarEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CFloarEdit message handlers

void CFloarEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString strEdit; 
    int dotCount=0;
	GetWindowText(strEdit); 

	int length=strEdit.GetLength(); 

	int pos=strEdit.Find('.'); 
	dotCount=length-pos;
    int carePos=0;
	int textStart=0;
	this->GetSel(textStart,carePos);
    
	if((nChar>=48 && nChar<=57) || nChar==46 || nChar==8) 
	{ 

		if(nChar==8) 
		{ 
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
			return; 
		} 

		/*if(((length-pos)<=2 && pos!=-1 && nChar!=46) || pos==-1 ) 
		{ 
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
		} */
		if(((carePos-pos)<=0 && pos!=-1 && nChar!=46) || pos==-1||dotCount<3||textStart!=carePos ) 
		{ 
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
		} 
		
	}
}