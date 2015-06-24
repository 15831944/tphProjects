// DlgAirPollutantsList.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAirPollutantsList.h"
#include ".\dlgairpollutantslist.h"



// CDlgAirPollutantsList dialog

IMPLEMENT_DYNAMIC(CDlgAirPollutantsList, CXTResizeDialog)
CDlgAirPollutantsList::CDlgAirPollutantsList(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgAirPollutantsList::IDD, pParent)
{
}

CDlgAirPollutantsList::~CDlgAirPollutantsList()
{
}

BEGIN_MESSAGE_MAP(CDlgAirPollutantsList, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDDEFAULT, OnBnClickedDefault)
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnNewAirPollutant)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnDelAirPollutant)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_AIRPOLLUTANTS, OnLvnItemchangedListAirpollutants)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_AIRPOLLUTANTS, OnLvnEndlabeleditListAirpollutants)
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)
ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

int CDlgAirPollutantsList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBarAirPollutant.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarAirPollutant.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		return -1;
	}
	return 0;
}
void CDlgAirPollutantsList::OnNewAirPollutant()
{
	CAirpollutant *newAirPollutant=new CAirpollutant();
	m_vAirPollutantList.AddItem(newAirPollutant);
	int newItemIndex=m_listAirPollutant.InsertItem(m_listAirPollutant.GetItemCount(),_T(""));
	m_listAirPollutant.SetItemText(newItemIndex,0,newAirPollutant->getName());
	m_listAirPollutant.SetItemText(newItemIndex,1,newAirPollutant->getSymbol());
	m_listAirPollutant.SetItemData(newItemIndex,(DWORD)newAirPollutant);
}
void CDlgAirPollutantsList::OnDelAirPollutant()
{
	int nCurIndex=m_listAirPollutant.GetCurSel();
	if (nCurIndex>=0&&nCurIndex<m_listAirPollutant.GetItemCount())
	{
		CAirpollutant *curAirPollutant=(CAirpollutant *)m_listAirPollutant.GetItemData(nCurIndex);
		m_vAirPollutantList.DeleteItem(nCurIndex);
		m_listAirPollutant.DeleteItemEx(nCurIndex);
	}
	if (m_listAirPollutant.GetItemCount()==0)
	{
		m_toolBarAirPollutant.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}
}
void CDlgAirPollutantsList::ResetAirPollutantList()
{
	m_listAirPollutant.DeleteAllItems();
	
	for (int i=0;i<(int)m_vAirPollutantList.GetItemCount();i++)
	{
		CAirpollutant *curAirPollutant=m_vAirPollutantList.GetItem(i);
		m_listAirPollutant.InsertItem(i,_T(""));
		m_listAirPollutant.SetItemText(i,0,curAirPollutant->getName());
		m_listAirPollutant.SetItemText(i,1,curAirPollutant->getSymbol());
		m_listAirPollutant.SetItemData(i,(DWORD)curAirPollutant);
	}
}
void CDlgAirPollutantsList::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_AIRPOLLUTANTS,m_listAirPollutant);
}
BOOL CDlgAirPollutantsList::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	m_vAirPollutantList.ReadData(-1);	
	InitAirPollutantList();
	ResetAirPollutantList();
	InitToolBar();	
	return true;
}
void CDlgAirPollutantsList::InitToolBar()
{
	CRect rect;
	m_listAirPollutant.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	rect.right=rect.right-20;
	m_toolBarAirPollutant.MoveWindow(rect);
	m_toolBarAirPollutant.ShowWindow(SW_SHOW);
	m_toolBarAirPollutant.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);

}
void CDlgAirPollutantsList::InitAirPollutantList()
{
	DWORD dwStyle = m_listAirPollutant.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listAirPollutant.SetExtendedStyle(dwStyle);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Name");
	lvc.cx = 300;
 	lvc.fmt = LVCFMT_CENTER;
	lvc.csList = &strList;
	m_listAirPollutant.InsertColumn(0, &lvc);

	lvc.pszText = _T("Symbol");
	// 	lvc.fmt = LVCFMT_NUMBER;
	lvc.cx = 200;
	m_listAirPollutant.InsertColumn(1, &lvc);

}

// CDlgAirPollutantsList message handlers

void CDlgAirPollutantsList::OnBnClickedDefault()
{
	// TODO: Add your control notification handler code here
	CAirPollutantsDefine defaultAirPollutant;

	m_vAirPollutantList.DeleteData();
	AirPollutantList &pollutantList=defaultAirPollutant.getPolluantList();
	AirPollutantList::iterator iter=pollutantList.begin();
	for (;iter!=pollutantList.end();iter++)
	{
		AirPollutantNameSymbol nameSymbol=(AirPollutantNameSymbol)(*iter);
		CAirpollutant *newAirPollutant=new CAirpollutant();
		newAirPollutant->setName(nameSymbol.m_sName);
		newAirPollutant->setSymbol(nameSymbol.m_sSymbol);
		m_vAirPollutantList.AddItem(newAirPollutant);
	}		
	ResetAirPollutantList();
}

void CDlgAirPollutantsList::OnLvnItemchangedListAirpollutants(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (m_listAirPollutant.GetCurSel()>=0)
	{
		m_toolBarAirPollutant.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
	}else
	{
		m_toolBarAirPollutant.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}
	*pResult = 0;
}

void CDlgAirPollutantsList::OnLvnEndlabeleditListAirpollutants(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	
	CAirpollutant *curAirPollutant=(CAirpollutant *)m_listAirPollutant.GetItemData(pDispInfo->item.iItem);
	if (pDispInfo->item.iSubItem==0)
	{
		curAirPollutant->setName(pDispInfo->item.pszText);
	}else if (pDispInfo->item.iSubItem==1)
	{
		curAirPollutant->setSymbol(pDispInfo->item.pszText);
	}

	*pResult = 1;
}

void CDlgAirPollutantsList::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	m_vAirPollutantList.SaveData(-1);
}

void CDlgAirPollutantsList::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CXTResizeDialog::OnCancel();
}

void CDlgAirPollutantsList::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CXTResizeDialog::OnOK();
	OnBnClickedSave();
}
