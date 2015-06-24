// UsedProcInfoBox.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "UsedProcInfoBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUsedProcInfoBox dialog


CUsedProcInfoBox::CUsedProcInfoBox(CWnd* pParent /*=NULL*/)
	: CDialog(CUsedProcInfoBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUsedProcInfoBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUsedProcInfoBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUsedProcInfoBox)
	DDX_Control(pDX, IDC_ListBox, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUsedProcInfoBox, CDialog)
	//{{AFX_MSG_MAP(CUsedProcInfoBox)
//	ON_BN_CLICKED(IDC_YES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUsedProcInfoBox message handlers

//void CUsedProcInfoBox::OnYes() 
//{
	// TODO: Add your control notification handler code here
	
//}

void CUsedProcInfoBox::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CUsedProcInfoBox::OnInitDialog()
{
	CDialog::OnInitDialog();
	std::vector<CString> _vec;
	std::vector<CString>::iterator iter = _vec.begin();
	int iProcCount = m_vInfoList.size();
	std::vector<CUsedProcInfo>::iterator it= m_vInfoList.begin();
	CString sLine;
	for(;it!=m_vInfoList.end();it++)
	{
		sLine =(*it).getInfoID().GetIDString();
		m_ListBox.AddString(sLine);
		m_ListBox.AddString("");
		for(int i=0;i<UsedProcCheckCount;i++)
		{
			if((*it).getInfo(i,_vec))
			{
				char Title[100];
				(*it).getTile(i,Title);
				sLine = "    " ;
				sLine +=Title;
				m_ListBox.AddString(sLine);
				iter = _vec.begin();
				
				for(;iter!=_vec.end();iter++)
				{
					sLine = "        " + (*iter);
					m_ListBox.AddString(sLine);
				}
				m_ListBox.AddString("");
			}
			_vec.clear();
		}
	}
    return true;
}
bool CUsedProcInfoBox::IsNoInfo()
{
	bool testTag = false;
	std::vector<CString> _vec;
	std::vector<CString>::iterator iter = _vec.begin();
	int iProcCount = m_vInfoList.size();
	std::vector<CUsedProcInfo>::iterator it= m_vInfoList.begin();
	CString sLine;
	for(;it!=m_vInfoList.end();it++)
	{
		sLine =(*it).getInfoID().GetIDString();
		if(m_ListBox.m_hWnd != NULL)
		{
			m_ListBox.AddString(sLine);
			m_ListBox.AddString("");
		}
		for(int i=0;i<UsedProcCheckCount;i++)
		{
			if((*it).getInfo(i,_vec))
			{
				testTag = true;
			}
			_vec.clear();
		}
	}
    return testTag;
}