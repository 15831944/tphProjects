#include "StdAfx.h"
#include ".\dlgsectorflightmixspecification.h"
#include "DlgACTypeSelection.h"
#include "../InputAirside/SectorFlightMixSpecification.h"
#include "../InputAirside/AircraftClassifications.h"
//#include "../MFCExControl/CoolTreeEditSpin.h"

static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

const int SECTORLIMIT =1000;
const int ACTYPELIMIT =100;

IMPLEMENT_DYNAMIC(CDlgSectorFlightMixSpecification, CXTResizeDialog)
CDlgSectorFlightMixSpecification::CDlgSectorFlightMixSpecification(int nProjID, CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgSectorFlightMixSpecification::IDD, pParent)
,m_nProjID(nProjID)
{
	m_pSpecification = NULL;
	m_vSectors.clear();
	ReadData();
}

CDlgSectorFlightMixSpecification::~CDlgSectorFlightMixSpecification(void)
{
	if (m_pSpecification != NULL)
	{
		delete m_pSpecification;
		m_pSpecification = NULL;
	}
}

void CDlgSectorFlightMixSpecification::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SECTORLIMITATION, m_wndTreeLimit);
}


BEGIN_MESSAGE_MAP(CDlgSectorFlightMixSpecification, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ITEM, OnMsgNew)
	ON_COMMAND(ID_DEL_ITEM, OnMsgDel)
	ON_COMMAND(ID_EDIT_ITEM, OnMsgEdit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SECTORLIMITATION, OnTvnSelchangedTreeSectorLimit)
END_MESSAGE_MAP()


// message handlers

void CDlgSectorFlightMixSpecification::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pSpecification->SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgSectorFlightMixSpecification::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	OnOK();
}

void CDlgSectorFlightMixSpecification::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

int CDlgSectorFlightMixSpecification::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& sequenceToolbar = m_wndToolbar.GetToolBarCtrl();
	sequenceToolbar.SetCmdID(0, ID_NEW_ITEM);
	sequenceToolbar.SetCmdID(1, ID_DEL_ITEM);
	sequenceToolbar.SetCmdID(2, ID_EDIT_ITEM);

	return 0;
}


BOOL CDlgSectorFlightMixSpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();


	// set the position of the sequence toolbar
	CRect rectToolbar;
	m_wndTreeLimit.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 26;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_wndToolbar.MoveWindow(&rectToolbar);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,FALSE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM,FALSE);

	SetResize(m_wndToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_SECTORLIMITATION, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);


	InitLimitationTreeCtrl();
	UpdateState();

	return TRUE;
}

void CDlgSectorFlightMixSpecification::ReadData()
{
	if(m_pSpecification){
		delete m_pSpecification;
		m_pSpecification = NULL;
	}

	m_pSpecification = new SectorFlightMixSpecification;
	if(!m_pSpecification)return;
	m_pSpecification->ReadData(m_nProjID);

	ALTObject::GetObjectList(ALT_SECTOR,m_nProjID,m_vSectors);
}

void CDlgSectorFlightMixSpecification::InitLimitationTreeCtrl()
{
	m_wndTreeLimit.DeleteAllItems();
	m_wndTreeLimit.SetRedraw(FALSE);
	int nCount = -1;
	COOLTREE_NODE_INFO cni;
	cni.nt = NT_NORMAL;
	CCoolTree::InitNodeInfo(this,cni);
	nCount = m_pSpecification->GetSectorLimitCount();
	if (nCount <=0)
	{
		m_wndTreeLimit.SetRedraw(TRUE);
		return;
	}
	
	for (int i = 0; i< nCount; i++)
	{	
		CCoolTree::InitNodeInfo(this,cni);
		SectorLimitation* pSectorData = m_pSpecification->GetSectorLimitByIdx(i) ;
		if(pSectorData == NULL)
			continue;

		CString strText = _T("Sector: ");
		int nSectorId = pSectorData->GetSectorID();

		int nNum = (int)m_vSectors.size();
		for (int j = 0; j < nNum; j++)
		{
			ALTObject sector = m_vSectors.at(j);
			if ( sector.getID() == nSectorId )
			{
				CString strName = sector.GetObjNameString();
				strText += strName;
				break;
			}
		}

		cni.net = NET_COMBOBOX;
		HTREEITEM hSectorItem = m_wndTreeLimit.InsertItem(strText,cni,FALSE);
		m_wndTreeLimit.SetItemData(hSectorItem,(DWORD_PTR)pSectorData);

		int nAltCount = pSectorData->GetAltitudeLimitCount();

		//altitude limit
		for (int nIdx = 0; nIdx < nAltCount; ++nIdx) 
		{
			CString strAltText = _T("Altitude: ");

			SectorAltitudeLimitation* pAltData = pSectorData->GetAltitudeLimitByIdx(nIdx);
			if(pAltData == NULL)
				continue;

			cni.net = NET_COMBOBOX;
			SectorAltitudeLimitation::AltitudeType altType = pAltData->GetAltitudeType();
			if (altType == SectorAltitudeLimitation::AnyAltitude)
				strAltText += _T("AnyAltitude");
			else
				strAltText += _T("SameAltitude");

			HTREEITEM hAltItem = m_wndTreeLimit.InsertItem(strAltText,cni,FALSE,FALSE,hSectorItem);
			m_wndTreeLimit.SetItemData(hAltItem,(DWORD_PTR)pAltData);

			//max limit value
			int nMaxLimit = pAltData->GetMaxLimit();
			CString strMaxValue;
			strMaxValue.Format("Max aircraft number: %d",nMaxLimit);

			cni.net = NET_EDITSPIN_WITH_VALUE;
			cni.bAutoSetItemText = false;

			HTREEITEM hLimitItem = m_wndTreeLimit.InsertItem(strMaxValue,cni,FALSE,FALSE,hAltItem);
			m_wndTreeLimit.SetItemValueRange(hLimitItem, 0, 100000);
			//ac type limit
			int nACTypeCount = pAltData->GetACTypeLimitCount();
			for (int nACIdx = 0; nACIdx < nACTypeCount; nACIdx++)	
			{	
				ACTypeLimitation* pACData = pAltData->GetACTypeLimitByIdx(nACIdx);
				if (pACData == NULL)
					continue;
				AircraftClassifications acCategory(m_nProjID,pACData->GetACCategory());
				acCategory.ReadData();
				int nACId = pACData->GetACTypeID();
				CString strACName = acCategory.GetItemByID(nACId)->getName();
				int nNum = pACData->GetMaxLimit();
				CString str;
				str.Format(": %d",nNum);
				strACName += str;

				cni.net = NET_EDITSPIN_WITH_VALUE;
				cni.bAutoSetItemText = false;

				HTREEITEM hACItem = m_wndTreeLimit.InsertItem(strACName,cni,FALSE,FALSE, hLimitItem);
				m_wndTreeLimit.SetItemData(hACItem,(DWORD_PTR)pACData);
				m_wndTreeLimit.SetItemValueRange(hACItem, 0, 10000);
			}
			m_wndTreeLimit.Expand(hLimitItem, TVE_EXPAND);
			m_wndTreeLimit.Expand(hAltItem, TVE_EXPAND);
		}	
		m_wndTreeLimit.Expand(hSectorItem, TVE_EXPAND);
	}	

	m_wndTreeLimit.SetRedraw(TRUE);
}

void CDlgSectorFlightMixSpecification::UpdateState()
{
	HTREEITEM hSelItem = m_wndTreeLimit.GetSelectedItem();
	TreeNodeType selType = GetCurACTypePos(hSelItem); 

	if (selType == TREENODE_NONE) 
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM,FALSE);
	}
	if (selType == TREENODE_SECTOR) 
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM,FALSE);
	}
	if (selType == TREENODE_ALTITUDE) 
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM,FALSE);
	}
	if (selType == TREENODE_MAXLIMIT) 
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM,FALSE);
	}
	if (selType == TREENODE_ACTYPE) 
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM,TRUE);
	}
}

CDlgSectorFlightMixSpecification::TreeNodeType CDlgSectorFlightMixSpecification::GetCurACTypePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return TREENODE_NONE;
	}

	HTREEITEM hParentItem = m_wndTreeLimit.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return TREENODE_SECTOR;
	}

	hParentItem = m_wndTreeLimit.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return TREENODE_ALTITUDE;
	}

	hParentItem = m_wndTreeLimit.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return TREENODE_MAXLIMIT;
	}

	hParentItem = m_wndTreeLimit.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return TREENODE_ACTYPE;
	}

	return TREENODE_NONE;
}

void CDlgSectorFlightMixSpecification::OnMsgNew()
{
	HTREEITEM hSelItem = m_wndTreeLimit.GetSelectedItem();
	TreeNodeType selType = GetCurACTypePos(hSelItem); 

	if (selType == TREENODE_NONE) 
	{
		NewSectorLimit();
	}
	if (selType == TREENODE_SECTOR) 
	{
		NewAltitudeLimit();
	}
	if (selType == TREENODE_MAXLIMIT) 
	{
		NewACTypeLimit();
	}
	if (selType == TREENODE_ACTYPE ||selType == TREENODE_ALTITUDE) 
		return;

	//InitLimitationTreeCtrl();
}

void CDlgSectorFlightMixSpecification::OnMsgDel()
{
	HTREEITEM hSelItem = m_wndTreeLimit.GetSelectedItem();
	TreeNodeType selType = GetCurACTypePos(hSelItem); 

	if (selType == TREENODE_SECTOR) 
	{
		SectorLimitation* pDelData = (SectorLimitation*)m_wndTreeLimit.GetItemData(hSelItem);
		m_pSpecification->DelSectorLimit(pDelData);

	}
	if (selType == TREENODE_ALTITUDE) 
	{
		SectorAltitudeLimitation* pDelData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hSelItem);
		HTREEITEM hParentItem = m_wndTreeLimit.GetParentItem(hSelItem);
		SectorLimitation* pData = (SectorLimitation*)m_wndTreeLimit.GetItemData(hParentItem);
		pData->DelAltitudeLimit(pDelData);

	}
	if (selType == TREENODE_ACTYPE) 
	{
		ACTypeLimitation* pDelData = (ACTypeLimitation*)m_wndTreeLimit.GetItemData(hSelItem);
		HTREEITEM hLimtItem = m_wndTreeLimit.GetParentItem(hSelItem);
		HTREEITEM hAltItem =  m_wndTreeLimit.GetParentItem(hLimtItem);
		SectorAltitudeLimitation* pData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hAltItem);
		pData->DelACTypeLimit(pDelData);		
	}
	if (selType ==  TREENODE_NONE ||selType == TREENODE_MAXLIMIT) 
		return;

	m_wndTreeLimit.DeleteItem(hSelItem);
	InitLimitationTreeCtrl();
}

void CDlgSectorFlightMixSpecification::OnMsgEdit()
{
	HTREEITEM hSelItem = m_wndTreeLimit.GetSelectedItem();
	TreeNodeType selType = GetCurACTypePos(hSelItem); 

	if (selType == TREENODE_ACTYPE) 
	{
		ACTypeLimitation* pDelData = (ACTypeLimitation*)m_wndTreeLimit.GetItemData(hSelItem);
		HTREEITEM hLimtItem = m_wndTreeLimit.GetParentItem(hSelItem);
		HTREEITEM hAltItem =  m_wndTreeLimit.GetParentItem(hLimtItem);
		SectorAltitudeLimitation* pData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hAltItem);
		DlgACTypeSelection dlgSelACType(m_nProjID,this);

		if (dlgSelACType.DoModal()== IDOK)
		{
			pData->DelACTypeLimit(pDelData);
			ACTypeLimitation* pNewData = new ACTypeLimitation;
			pNewData->SetACCategory(dlgSelACType.GetACCategory());
			pNewData->SetACTypeID(dlgSelACType.GetACTypeID());
			pNewData->SetMaxLimit(pDelData->GetMaxLimit());
			pData->AddACTypeLimit(pNewData);
			InitLimitationTreeCtrl();
		}
	
	}
}

void CDlgSectorFlightMixSpecification::NewSectorLimit()
{
	if (m_vSectors.empty())
	{
		AfxMessageBox("No Sector Defined!");
		return;
	}
	std::vector<ALTObject> vSector;
	vSector.assign(m_vSectors.begin(),m_vSectors.end());
	int nSector = m_pSpecification->GetSectorLimitCount();
	for (int i = 0; i < nSector; i++)
	{
		for (int j = 0; j < (int)vSector.size(); j++)
		{
			if (vSector.at(j).getID() == m_pSpecification->GetSectorLimitByIdx(i)->GetSectorID())
			{
				vSector.erase(vSector.begin() + j);
			}
		}
	}
	if (vSector.empty())
	{
		AfxMessageBox("All Sectors' Limitation has been defined!");
		return;
	}
	SectorLimitation* pSectorData = new SectorLimitation;
	if (pSectorData == NULL)
		return;

	int nSectorID = vSector.at(0).getID();
	pSectorData->SetSectorID(nSectorID);
	SectorAltitudeLimitation* pAltData = new SectorAltitudeLimitation;
	pAltData->SetAltitudeType(SectorAltitudeLimitation::AnyAltitude);
	pAltData->SetMaxLimit(SECTORLIMIT);
	pSectorData->AddAltitudeLimit(pAltData);
	m_pSpecification->AddSectorLimit(pSectorData);
	InitLimitationTreeCtrl();

}

void CDlgSectorFlightMixSpecification::NewAltitudeLimit()
{
	HTREEITEM hSelItem = m_wndTreeLimit.GetSelectedItem();
	SectorLimitation* pSectorData = (SectorLimitation*)m_wndTreeLimit.GetItemData(hSelItem);
	if (pSectorData == NULL)
		return;
	int nAlt = pSectorData->GetAltitudeLimitCount();
	if (nAlt == 2)
	{
		AfxMessageBox("The Sector's Altitude Limitation has been defined!");
		return;
	}
	SectorAltitudeLimitation::AltitudeType type = pSectorData->GetAltitudeLimitByIdx(0)->GetAltitudeType();

	SectorAltitudeLimitation* pAltData = new SectorAltitudeLimitation;
	if (type == SectorAltitudeLimitation::AnyAltitude)
		pAltData->SetAltitudeType(SectorAltitudeLimitation::SameAltitude);
	else
		pAltData->SetAltitudeType(SectorAltitudeLimitation::AnyAltitude);
	pAltData->SetMaxLimit(SECTORLIMIT);
	pSectorData->AddAltitudeLimit(pAltData);
	InitLimitationTreeCtrl();
}

void CDlgSectorFlightMixSpecification::NewACTypeLimit()
{
	HTREEITEM hSelItem = m_wndTreeLimit.GetSelectedItem();
	HTREEITEM hAltItem =  m_wndTreeLimit.GetParentItem(hSelItem);
	SectorAltitudeLimitation* pData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hAltItem);
	if (pData == NULL)
		return;

	DlgACTypeSelection dlgSelACType(m_nProjID,this);

	if (dlgSelACType.DoModal()== IDOK)
	{
		int nACCount = pData->GetACTypeLimitCount();
		for (int i =0; i < nACCount; i++)
		{
			if (pData->GetACTypeLimitByIdx(i)->GetACTypeID() == dlgSelACType.GetACTypeID()
				&& pData->GetACTypeLimitByIdx(i)->GetACCategory() == dlgSelACType.GetACCategory())
			{
				AfxMessageBox("The AC type has already been defined!");
				return;
			}
		}
		ACTypeLimitation* pNewData = new ACTypeLimitation;
		pNewData->SetACCategory(dlgSelACType.GetACCategory());
		pNewData->SetACTypeID(dlgSelACType.GetACTypeID());
		pNewData->SetMaxLimit(ACTYPELIMIT);
		pData->AddACTypeLimit(pNewData);
	}
	InitLimitationTreeCtrl();
}

LRESULT CDlgSectorFlightMixSpecification::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
		case UM_CEW_EDITSPIN_BEGIN:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				CWnd* pWnd = m_wndTreeLimit.GetEditWnd(hItem);
				pWnd->SetWindowText( _T("0"));
				break;
			}
		case UM_CEW_EDITSPIN_END:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				int ntemp = (int)atoi( strValue.GetBuffer());
				if(GetCurACTypePos(hItem) == TREENODE_MAXLIMIT)
				{
					HTREEITEM hAltItem = m_wndTreeLimit.GetParentItem(hItem);
					SectorAltitudeLimitation* pAltData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hAltItem);
					if (pAltData != NULL)
						pAltData->SetMaxLimit(ntemp);
				}
				if(GetCurACTypePos(hItem) == TREENODE_ACTYPE)
				{
					ACTypeLimitation* pACData = (ACTypeLimitation*)m_wndTreeLimit.GetItemData(hItem);
					if (pACData != NULL)
					{
						HTREEITEM hLimitItem = m_wndTreeLimit.GetParentItem(hItem);
						HTREEITEM hAltItem = m_wndTreeLimit.GetParentItem(hLimitItem);
						SectorAltitudeLimitation* pAltData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hAltItem);
						int nACCount = pAltData->GetACTypeLimitCount();
						int nSum = 0;
						for (int n =0; n < nACCount; n++)
						{
							nSum += pAltData->GetACTypeLimitByIdx(n)->GetMaxLimit();
						}
						if (nSum + ntemp > pAltData->GetMaxLimit())
						{
							AfxMessageBox("The Sum of Defined Aircrafts' Limitation Exceed The Altitude Max Limitation!");
							break;
						}
						pACData->SetMaxLimit(ntemp);
					}
				}

				InitLimitationTreeCtrl();
			}
			break;
		case UM_CEW_COMBOBOX_BEGIN:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;

				CWnd* pWnd = m_wndTreeLimit.GetEditWnd(hItem);
				CRect rectWnd;
				m_wndTreeLimit.GetItemRect(hItem,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;
				pCB->ResetContent();

				if(GetCurACTypePos(hItem) == TREENODE_SECTOR)
				{
					std::vector<ALTObject>::iterator iter = m_vSectors.begin();
					for (; iter != m_vSectors.end(); ++iter)
					{
						CString strObjName = (*iter).GetObjNameString();
						int nIndex = pCB->AddString(strObjName);
						pCB->SetItemData(nIndex,(*iter).getID());
					}
				//	pCB->SetCurSel(0);
				}

				if(GetCurACTypePos(hItem) == TREENODE_ALTITUDE)
				{

					HTREEITEM hSectorItem = m_wndTreeLimit.GetParentItem(hItem);
					SectorLimitation* pSectorData = (SectorLimitation*)m_wndTreeLimit.GetItemData(hSectorItem);
					if(pSectorData->GetAltitudeLimitCount() == 2)
					{
						pCB->ResetContent();
						AfxMessageBox("You have defined all types of altitude limitation!");
						break;
					}

					pCB->AddString("Any Altitude");
					pCB->AddString("Same Altitude");
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				CWnd* pWnd = m_wndTreeLimit.GetEditWnd((HTREEITEM)wParam);
				CComboBox* pCB = (CComboBox*)pWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strText = *(CString*)lParam;
				if(GetCurACTypePos(hItem) == TREENODE_SECTOR)
				{
					int nSel = pCB->GetCurSel();
					int nSectorID = pCB->GetItemData(nSel);
					SectorLimitation* pSectorData = (SectorLimitation*)m_wndTreeLimit.GetItemData(hItem);
					if (pSectorData)
						pSectorData->SetSectorID(nSectorID);
				}
				if(GetCurACTypePos(hItem) == TREENODE_ALTITUDE)
				{
					SectorAltitudeLimitation* pAltData = (SectorAltitudeLimitation*)m_wndTreeLimit.GetItemData(hItem);
					int nSel = pCB->GetCurSel();
					if(nSel == 0)
						pAltData->SetAltitudeType(SectorAltitudeLimitation::AnyAltitude);
					else
						pAltData->SetAltitudeType(SectorAltitudeLimitation::SameAltitude);
				}
				InitLimitationTreeCtrl();
			}
			break;

		default:
			break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgSectorFlightMixSpecification::OnTvnSelchangedTreeSectorLimit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateState();
	*pResult = 0;
}
