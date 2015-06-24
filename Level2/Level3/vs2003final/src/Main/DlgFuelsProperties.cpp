// DlgFuelsProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgFuelsProperties.h"

#include "../InputEnviroment/FuelProperties.h"
#include ".\dlgfuelsproperties.h"



// CDlgFuelsProperties dialog

IMPLEMENT_DYNAMIC(CDlgFuelsProperties, CXTResizeDialog)
CDlgFuelsProperties::CDlgFuelsProperties(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgFuelsProperties::IDD, pParent)
{
	m_pFuelProperties = new FuelProperties;
	m_pAirPollutantList=new CAirPollutantList;
	m_pFuelProperties->ReadData(-1);
	m_pAirPollutantList->ReadData(-1);
	m_listFuelsProperties.SetAirPollutantList(m_pAirPollutantList);
}

CDlgFuelsProperties::~CDlgFuelsProperties()
{
	if(m_pFuelProperties)
		delete m_pFuelProperties;
	if (m_pAirPollutantList)
		delete m_pAirPollutantList;
}

void CDlgFuelsProperties::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_listFuelsProperties);
}


BEGIN_MESSAGE_MAP(CDlgFuelsProperties, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DATA, OnNMClickListData)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickListData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DATA, OnLvnColumnclickListData)
    ON_COMMAND(ID_ENV_FUELPROPERTIES_ADD,OnNewFuelsProperty)
	ON_COMMAND(ID_ENV_FUELPROPERTIES_DEL,OnDelFuelsProperty)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListData)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgFuelsProperties message handlers
int CDlgFuelsProperties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_toolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_toolBar.LoadToolBar(IDR_TOOLBAR_ENV_FUELPROPERTIES))
	{
		AfxMessageBox("Can't create toolbar!");
		return FALSE;
	}
	return 0;
}

BOOL CDlgFuelsProperties::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	InitToolbar();
	InitList();
// 	SetResize(IDR_TOOLBAR_ENV_FUELPROPERTIES,SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FRAME,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_RESTORE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	// TODO:  Add extra initialization here
	
	//FuelProperty* pFuelProp = new FuelProperty;
	//m_pFuelProperties->AddItem(pFuelProp);
	//m_pFuelProperties->SaveData(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFuelsProperties::InitToolbar()
{

	CRect rcToolbar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rcToolbar);
	//ClientToScreen(&rcToolbar);
	ScreenToClient(&rcToolbar);
	m_toolBar.MoveWindow(rcToolbar);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_ENV_FUELPROPERTIES_ADD,TRUE);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_ENV_FUELPROPERTIES_DEL,FALSE);
}

void CDlgFuelsProperties::InitList()
{
	DWORD dwStyle=m_listFuelsProperties.GetExtendedStyle();
	dwStyle|=LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listFuelsProperties.SetExtendedStyle(dwStyle);
	
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask=LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText=_T("Name");
	lvc.cx=120;
	lvc.fmt=LVCFMT_RIGHT;//LVCFMT_CENTER;
	lvc.csList=&strList;
	m_listFuelsProperties.InsertColumn(0,&lvc);

	lvc.pszText=_T("HV BTU/cu ft");
	lvc.cx=100;
	lvc.fmt=LVCFMT_CENTER|LVCFMT_NUMBER;
	
	m_listFuelsProperties.InsertColumn(1,&lvc);

	
	lvc.cx=55;
	lvc.fmt=LVCFMT_CHECK;


	char strSymbol[128];
	int nPollutantCount=(int)m_pAirPollutantList->GetItemCount();
	for (int i=0;i<nPollutantCount;i++)
	{
		CAirpollutant *AirPollutant=(CAirpollutant *)m_pAirPollutantList->GetItem(i);
		strcpy(strSymbol,AirPollutant->getSymbol().GetString());
		lvc.pszText=strSymbol;		
		m_listFuelsProperties.InsertColumn(i+2,&lvc);
	}
	ReloadList();
}

void CDlgFuelsProperties::ReloadList()
{
	//if the AirPollutant corresponding to the column already exist in the item of m_pFuelProperties
	int nPollutantCount=(int)m_pAirPollutantList->GetItemCount();
	bool *vColumnFlag=new bool[nPollutantCount+2];
	for (int i=0;i<nPollutantCount+2;i++)
	{
		vColumnFlag[i]=false;
	}

	m_listFuelsProperties.DeleteAllItems();
	for (int i=0;i<(int)m_pFuelProperties->GetItemCount();i++)
	{		
		FuelProperty *curProperty=m_pFuelProperties->GetItem(i);

		CString strHvbtu;
		strHvbtu.Format("%.2f",curProperty->getHvbtu());
		m_listFuelsProperties.InsertItem(i,_T(""));
		m_listFuelsProperties.SetItemText(i,0,curProperty->getName());
		m_listFuelsProperties.SetItemText(i,1,strHvbtu);
		m_listFuelsProperties.SetItemData(i,(DWORD)curProperty);

		FuelPollutantList &curPollutantList=curProperty->getFuelPollutantList();
		FuelPollutantList noExistTypeList;
		FuelPollutantList::iterator iter;
		for (iter=curPollutantList.begin();iter!=curPollutantList.end();iter++)
		{
			PropItem *curPollutant=(PropItem *)(*iter);
			int nType=curPollutant->getType();
			int nIndex=m_pAirPollutantList->getPollutantIndexByID(nType);
			if (nIndex==-1)
			{
				noExistTypeList.push_back(*iter);
				continue;
			}
			vColumnFlag[nIndex+2]=true;
			CString strValue;
			strValue.Format("%d",curPollutant->getValue());
			m_listFuelsProperties.SetItemText(i,nIndex+2,strValue);
		}

		//delete the type not exist in m_pAirPollutantList
		for(int k=0;k<(int)noExistTypeList.size();k++)
		{
			iter=std::find(curPollutantList.begin(),curPollutantList.end(),noExistTypeList.at(k));
			if (iter!=curPollutantList.end())
			{
				curPollutantList.erase(iter);
			}
		}

		//insert new type into curProperty
		for (int k=2;k<nPollutantCount+2;k++)
		{
			if (vColumnFlag[k]==false)
			{
				int nType=m_pAirPollutantList->GetItem(k-2)->GetID();
				curProperty->addItem(new PropItem(nType,0));
			}
		}
	}
}


void CDlgFuelsProperties::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	m_pFuelProperties->SaveData(-1);
}

void CDlgFuelsProperties::OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgFuelsProperties::OnHdnItemclickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgFuelsProperties::OnLvnColumnclickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CDlgFuelsProperties::OnNewFuelsProperty()
{	
	FuelProperty *newFuelsProperty=new FuelProperty();
	m_pFuelProperties->AddItem(newFuelsProperty);
	int newItemIndex=m_listFuelsProperties.GetItemCount();
	m_listFuelsProperties.InsertItem(newItemIndex,_T(""));
	CString strHvbtu;
	strHvbtu.Format("%.2f",newFuelsProperty->getHvbtu());
	m_listFuelsProperties.SetItemText(newItemIndex,0,newFuelsProperty->getName());
	m_listFuelsProperties.SetItemText(newItemIndex,1,strHvbtu);
	m_listFuelsProperties.SetItemData(newItemIndex,(DWORD)newFuelsProperty);
	m_listFuelsProperties.SetCurSel(newItemIndex);
	for (int i=0;i<m_pAirPollutantList->GetItemCount();i++)
	{
		int nType=m_pAirPollutantList->GetItem(i)->GetID();
		newFuelsProperty->addItem(new PropItem(nType,0));
	}
	m_toolBar.GetToolBarCtrl().EnableButton(ID_ENV_FUELPROPERTIES_DEL,TRUE);
}
void CDlgFuelsProperties::OnDelFuelsProperty()
{
	int nIndex=m_listFuelsProperties.GetCurSel();
	if (nIndex<0||nIndex>=m_listFuelsProperties.GetItemCount())
	{
		return;
	}
	m_pFuelProperties->DeleteItem(nIndex);
	m_listFuelsProperties.DeleteItemEx(nIndex);
	if (m_listFuelsProperties.GetItemCount()==0)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_ENV_FUELPROPERTIES_DEL,FALSE);
	}
}

void CDlgFuelsProperties::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nCurSel=m_listFuelsProperties.GetCurSel();
	if(nCurSel>=0 && nCurSel<m_listFuelsProperties.GetItemCount())
	{
        m_toolBar.GetToolBarCtrl().EnableButton(ID_ENV_FUELPROPERTIES_DEL,TRUE);
	}else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_ENV_FUELPROPERTIES_DEL,FALSE);
	}
	*pResult = 0;
}

void CDlgFuelsProperties::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_pFuelProperties->SaveData(-1);
	OnOK();
}

void CDlgFuelsProperties::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
