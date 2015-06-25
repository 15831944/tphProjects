#include "stdafx.h"
#include "Resource.h"
#include "DlgDirectionalityConstraints.h"
#include ".\dlgdirectionalityconstraints.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/taxiway.h"
#include "../Common/WinMsg.h"
#include "../Common/ARCVector.h"
#include "../InputAirside/IntersectionNodesInAirport.h"
#include "../Database/DBElementCollection_Impl.h"
//--------------------------------------------------------------------------------------------------
//

static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

CDirConsListCtrlEx::CDirConsListCtrlEx()
:m_nAirportID(-1)
,m_nProjID(-1)
{
	m_strArrayType.Add(_T("Bearing"));
	m_strArrayType.Add(_T("Reciprocal"));
}

CDirConsListCtrlEx::~CDirConsListCtrlEx()
{

}


BEGIN_MESSAGE_MAP(CDirConsListCtrlEx, CListCtrlEx)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CDirConsListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index, colnum;
	if((index = CListCtrlEx::HitTestEx(point, &colnum)) != -1)
	{
		DirectionalityConstraintItem* pItem =
			(DirectionalityConstraintItem*)GetItemData(index);

		ResetDropDownListContext(pItem);

	}

	CListCtrlEx::OnLButtonDblClk(nFlags, point);
	
}


void CDirConsListCtrlEx::LoadTaxiwayInfo()
{
	ASSERT(m_nAirportID != -1);

	m_taxiwayStrList.RemoveAll();

	m_airport.ReadAirport(m_nAirportID);
	ALTAirport::GetTaxiwaysIDs(m_nAirportID, m_vectTaxiwayID);

	std::vector<int>::iterator iterTaxiwayID = m_vectTaxiwayID.begin();
	for (; iterTaxiwayID != m_vectTaxiwayID.end(); ++iterTaxiwayID)
	{
		Taxiway taxiway;
		taxiway.ReadObject(*iterTaxiwayID);

		CString strTmpTaxiway;
		strTmpTaxiway.Format("%s", taxiway.GetObjNameString());
		m_taxiwayStrList.AddTail(strTmpTaxiway);

		m_mapTaxiway.insert( std::make_pair( strTmpTaxiway, taxiway.getID()));
	}
}

void CDirConsListCtrlEx::ResetDropDownListContext(DirectionalityConstraintItem* pItem)
{
	//Taxiway
	LVCOLDROPLIST* pDroplist =  GetColumnStyle(0);
	pDroplist->List.RemoveAll();

	POSITION pos;
	for( pos = m_taxiwayStrList.GetHeadPosition(); pos;)
	{
		pDroplist->List.AddTail(m_taxiwayStrList.GetAt(pos));
		m_taxiwayStrList.GetNext( pos );
	}	

	GetNodeListByTaxiwayID(pItem->GetTaxiwayID() );

	for (int i=1; i<3; i++)
	{
		pDroplist =  GetColumnStyle(i);
		pDroplist->List.RemoveAll();

		//POSITION pos;
		for( pos = m_nodeStrList.GetHeadPosition(); pos;)
		{
			pDroplist->List.AddTail(m_nodeStrList.GetAt(pos));
			m_nodeStrList.GetNext( pos );
		}
	}

	//Bearing
	CalculateBearing(pItem);
	pDroplist =  GetColumnStyle(3);
	pDroplist->List.RemoveAll();
	for( pos = m_bearingStrList.GetHeadPosition(); pos;)
	{
		pDroplist->List.AddTail(m_bearingStrList.GetAt(pos));
		m_bearingStrList.GetNext( pos );
	}	

	//Direction
	pDroplist =  GetColumnStyle(4);
	pDroplist->List.RemoveAll();

	for (int i=0; i< m_strArrayType.GetCount(); i++)
		pDroplist->List.AddTail(m_strArrayType.GetAt(i));

   	ASSERT(pItem != NULL);

}

void CDirConsListCtrlEx::ResetTaxiwayRelativeColnum(int nItem, DirectionalityConstraintItem* pItem)
{
	std::vector<int>::iterator iterTaxiwayID = m_vectTaxiwayID.begin();
	for (; iterTaxiwayID != m_vectTaxiwayID.end(); ++iterTaxiwayID)
	{
		Taxiway taxiway;
		taxiway.ReadObject(*iterTaxiwayID);
		if(pItem->GetTaxiwayID() != taxiway.getID())
			continue;
        
		IntersectionNodeList nodeList = taxiway.GetIntersectNodes();
		if(nodeList.size() == 0)
			continue;
	
		CString strTaxiwayName = taxiway.GetObjNameString();
		
		CPoint2008 ptFrom;
		std::vector<IntersectionNode>::iterator iter = nodeList.begin();
		int nRet = 1;
		for (; iter != nodeList.end(); ++iter)
		{
			/*CString strObjName;
			ALTObject * pObj = (*iter).GetOtherObject(taxiway.getID());
			if(pObj)
				strObjName = pObj->GetObjNameString();*/
			CString strNode = iter->GetName();
			//strNode.Format("(%s) & (%s)-%d", strTaxiwayName, strObjName, ((*iter).GetIndex())+1);
			SetItemText(nItem, nRet, strNode);
			if(nRet == 1)
			{
				pItem->SetSegFromID(iter->GetID());
			}
			if(nRet == 2)
			{
				pItem->SetSegToID(iter->GetID());
				break;
			}

			nRet++;

		}
		CalculateBearing(pItem);

		CString strValue = m_bearingStrList.GetAt( m_bearingStrList.GetHeadPosition() );

		pItem->SetBearingDegree(atof(strValue));
		SetItemText(nItem, 3, strValue);

		return;
	}
}

void CDirConsListCtrlEx::ResetSegIDRelativeColnum(int nItem, DirectionalityConstraintItem* pItem)
{
	CalculateBearing(pItem);

	CString strValue = m_bearingStrList.GetAt( m_bearingStrList.GetHeadPosition() );

	SetItemText(nItem, 3, strValue);
	pItem->SetBearingDegree( atof(strValue) );

}

void CDirConsListCtrlEx::GetNodeListByTaxiwayID(int nTaxiwayID)
{
	m_mapNodeList.clear();

	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);

	CString strTaxiway;
	strTaxiway.Format("%s", taxiway.GetObjNameString());

	// get the list of intersect node
	m_intersecNodeList.clear();
	m_intersecNodeList = taxiway.GetIntersectNodes();
	if(m_intersecNodeList.size() == 0)
		return;


	m_nodeStrList.RemoveAll();
	std::vector<IntersectionNode>::iterator iter = m_intersecNodeList.begin();

	for (; iter != m_intersecNodeList.end(); ++iter)
	{
		/*CString strObjName;
		ALTObject * pObj = (*iter).GetOtherObject(nTaxiwayID);
		if(pObj)
			strObjName = pObj->GetObjNameString();
		*/
		CString strNode  = iter->GetName();
		//strNode.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);*/
		m_nodeStrList.AddTail(strNode);
		m_mapNodeList.insert( std::make_pair( strNode, iter->GetID()));
	}
}

void CDirConsListCtrlEx::ResetItemSegContext(int nItem, DirectionalityConstraintItem* pItem)
{
	Taxiway taxiway;
	taxiway.ReadObject(pItem->GetTaxiwayID());
	CString strTaxiway;
	strTaxiway.Format("%s", taxiway.GetObjNameString());

	IntersectionNodeList nodeList = taxiway.GetIntersectNodes();
	std::vector<IntersectionNode>::iterator iter = nodeList.begin();

	CString strFrom = _T(""), strTo = _T("");
	for (; iter != nodeList.end(); ++iter)
	{
		//CString strObjName;
		//ALTObject * pObj = (*iter).GetOtherObject(taxiway.getID());
		//if(pObj)
		//	strObjName = pObj->GetObjNameString();

		if(iter->GetID() == pItem->GetSegFromID())
		{
			strFrom = iter->GetName();//.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
		}
		if(iter->GetID() == pItem->GetSegToID())
		{
			strTo = iter->GetName();//.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
		}

	}

	SetItemText(nItem, 1, strFrom);
	SetItemText(nItem, 2, strTo);
}

BOOL CDirConsListCtrlEx::IfExist(int nTaxiwayID)
{
	int nCount = GetItemCount();
	for(int i=0; i<nCount; i++)
	{
		DirectionalityConstraintItem* pItem = 
			(DirectionalityConstraintItem*)GetItemData(i);
		if(nTaxiwayID == pItem->GetTaxiwayID())
			return TRUE;
	}
	return FALSE;
}

void CDirConsListCtrlEx::CalculateBearing(DirectionalityConstraintItem* pItem)
{
	m_bearingStrList.RemoveAll();

	Taxiway taxiway;
	taxiway.ReadObject(pItem->GetTaxiwayID());

	IntersectionNodeList nodeList = taxiway.GetIntersectNodes();	
	CPoint2008 ptFrom, ptTo;
	std::vector<IntersectionNode>::iterator iter = nodeList.begin();
	for (; iter != nodeList.end(); ++iter)
	{
		if (iter->GetID() == pItem->GetSegFromID() )
		{
			DistanceUnit distinTaxiway = (*iter).GetDistance(taxiway.getID());
			//float relatDist = taxiway.GetPath().GetRelateDist(distinTaxiway);
			ptFrom = CPoint2008(taxiway.GetPath().GetDistDir(distinTaxiway));
		}
		
		if (iter->GetID() == pItem->GetSegToID())
		{
			DistanceUnit distinTaxiway = (*iter).GetDistance(taxiway.getID());
			//float relatDist = taxiway.GetPath().GetRelateDist(distinTaxiway);
			ptTo = CPoint2008(0,0,0) -taxiway.GetPath().GetDistDir(distinTaxiway);
		}
	}

	double dAngleFrom, dAngleTo;
	for (int i=0; i<2; i++)
	{
		ARCVector3 vrunway3;
		if(i==0)
		{
			vrunway3 = ptFrom;
		}
		else
			vrunway3 = ptTo;

		ARCVector2 vnorth (0,1);
		
		ARCVector2 vrunway2(vrunway3[VX],vrunway3[VY]);
		double dangle =  (vrunway2.GetAngleOfTwoVector(vnorth));
		dangle -= m_airport.GetMagnectVariation().GetRealVariation();	
		if(dangle <= 0 )dangle += 360;
		if(i==0)
			dAngleFrom = dangle;
		else
			dAngleTo = dangle;

	}

	CString strTemp;
	strTemp.Format("%.f", dAngleFrom);
	m_bearingStrList.AddTail(strTemp);
	strTemp.Format("%.f", dAngleTo);
	m_bearingStrList.AddTail(strTemp);
}

LRESULT CDirConsListCtrlEx::OnComboBoxChange(WPARAM wParam, LPARAM lParam)
{
	int nSelIndex = (int)wParam;
	if(nSelIndex == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nItem = dispinfo->item.iItem;
	int nSubItem = dispinfo->item.iSubItem;	

	DirectionalityConstraintItem* pItem = 
		(DirectionalityConstraintItem*)GetItemData(nItem);

	switch(nSubItem)
	{
	case 0://Taxiway
		{
			CString strSelTaxiway = _T("");
			int i=0;
			POSITION pos;
			for( pos = m_taxiwayStrList.GetHeadPosition(); pos;)
			{
				if(i== nSelIndex)
				{
					strSelTaxiway = m_taxiwayStrList.GetAt(pos);
					break;
				}
				i++;
				m_taxiwayStrList.GetNext( pos );
			}

            std::map<CString, int>::iterator iter = m_mapTaxiway.find(strSelTaxiway);
			ASSERT(iter != m_mapTaxiway.end());
			
			int nOldTaxiwayID = pItem->GetTaxiwayID();
			int nNewTaxiwayID = iter->second;
			pItem->SetTaxiwayID(nNewTaxiwayID);
			
			if(nOldTaxiwayID != nNewTaxiwayID)
				ResetTaxiwayRelativeColnum(nItem, pItem);

		}
		break;
	case 1://From SegNode
	case 2:
		{
			CString strSelNode = _T("");;
			int i = 0;
			POSITION pos;
			for( pos = m_nodeStrList.GetHeadPosition(); pos;)
			{
				if(i== nSelIndex)
				{
					strSelNode = m_nodeStrList.GetAt(pos);
					break;
				}
				i++;
				m_nodeStrList.GetNext( pos );
			}

			std::map<CString, int>::iterator iter = m_mapNodeList.find(strSelNode);
			ASSERT(iter != m_mapNodeList.end());

			int nOldID = -1,nNewID = -1;
			if(nSubItem == 1)
			{
				nOldID = pItem->GetSegFromID();
				nNewID = iter->second;

				if(nNewID == pItem->GetSegToID())
				{
					AfxMessageBox(_T("cannot be same node"));
					ResetItemSegContext(nItem, pItem);
					return 0;
				}
				pItem->SetSegFromID(nNewID);
			}
			else
			{
				nOldID = pItem->GetSegToID();
				nNewID = iter->second;
				if(nNewID == pItem->GetSegFromID())
				{
					AfxMessageBox(_T("cannot be same node"));
					ResetItemSegContext(nItem, pItem);
					return 0;
				}
				pItem->SetSegToID(iter->second);
			}

			if(nOldID != nNewID)
				ResetSegIDRelativeColnum(nItem, pItem);
		}
		break;
	case 3://Bearing
		{
			CString strValue = _T("");
			POSITION pos;
			int i = 0;
			for( pos = m_bearingStrList.GetHeadPosition(); pos;)
			{
				if(i== nSelIndex)
				{
					strValue = m_nodeStrList.GetAt(pos);
					break;
				}
				i++;
				m_nodeStrList.GetNext( pos );
			}
			pItem->SetBearingDegree(atof(strValue));
		}
		break;
	case 4://Direction
		{

			pItem->SetDirConsType( DirectionalityConstraintType(nSelIndex) );
		}
		break;
	default:
		break;
	}

	return 0;
}
//--------------------------------------------------------------------------------------------------
//
IMPLEMENT_DYNAMIC(CDlgDirectionalityConstraints, CXTResizeDialog)
CDlgDirectionalityConstraints::CDlgDirectionalityConstraints(int nProjID, int nAirportID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgDirectionalityConstraints::IDD, pParent)
	, m_nProjID(nProjID)
	,m_nAirportID(nAirportID)
{

	m_airport.ReadAirport(m_nAirportID);
	ALTAirport::GetTaxiwaysIDs(m_nAirportID, m_vectTaxiwayID);


	m_pDirectionalityConstraints = new DirectionalityConstraints;
	m_strArrayType.Add(_T("Bearing"));
	m_strArrayType.Add(_T("Reciprocal"));
}

CDlgDirectionalityConstraints::~CDlgDirectionalityConstraints()
{
	delete m_pDirectionalityConstraints;
}

void CDlgDirectionalityConstraints::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgDirectionalityConstraints, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnLvnEndLabelEditList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnLvnItemChangedList)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnComboBoxChange)
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDelItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)

END_MESSAGE_MAP()

// CDlgDirectionalityConstraints message handlers

int CDlgDirectionalityConstraints::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}
	return 0;
}

void CDlgDirectionalityConstraints::InitToolbar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolBar);
	ScreenToClient(rectToolBar);
	rectToolBar.top += 2;
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	toolbar.EnableButton(ID_NEW_ITEM, TRUE);
	toolbar.EnableButton(ID_DEL_ITEM, TRUE);
	toolbar.EnableButton(ID_EDIT_ITEM,TRUE);
}

BOOL CDlgDirectionalityConstraints::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog(); 

	InitToolbar();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LISTBOX, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);	
	SetResize(IDSAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitListControl();

	m_pDirectionalityConstraints->ReadData(m_nAirportID);

	ResetListContent();
	GetDlgItem(IDSAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE; 
}

void CDlgDirectionalityConstraints::UpdateUIState()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DIRCONS_ADD, TRUE);

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	BOOL bEnable = (pos == NULL ? FALSE : TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DIRCONS_DEL, bEnable);

}

void CDlgDirectionalityConstraints::InitListControl()
{
	m_wndListCtrl.SetProjectID(m_nProjID);
	m_wndListCtrl.SetAirportID(m_nAirportID);
	m_wndListCtrl.LoadTaxiwayInfo();


	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;

	lvc.pszText = "Taxiway";
	lvc.cx = 130;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = "From";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = "To";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = "Bearing";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(3, &lvc);

	strList.RemoveAll();
	lvc.pszText = "Direction";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(4, &lvc);

}

void CDlgDirectionalityConstraints::ResetListContent()
{
	size_t nItemCount = m_pDirectionalityConstraints->GetElementCount();
	for (size_t i = 0; i < nItemCount; i++)
	{
		DirectionalityConstraintItem* pItem = m_pDirectionalityConstraints->GetItem(i);

		Taxiway taxiway;
		taxiway.ReadObject( pItem->GetTaxiwayID() );
		
		CString strTemp;
		strTemp.Format("%s", taxiway.GetObjNameString());
		m_wndListCtrl.InsertItem((int)i, strTemp);

		strTemp = GetNodeNameString(taxiway, pItem->GetSegFromID());
		m_wndListCtrl.SetItemText((int)i, 1, strTemp);

		strTemp = GetNodeNameString(taxiway, pItem->GetSegToID());
		m_wndListCtrl.SetItemText((int)i, 2, strTemp);
		
		strTemp.Format("%.f", pItem->GetBearingDegree());
		m_wndListCtrl.SetItemText((int)i, 3, strTemp);
		m_wndListCtrl.SetItemText((int)i, 4, m_strArrayType.GetAt((int)pItem->GetDirConsType()) );
		
		m_wndListCtrl.SetItemData(i, (DWORD_PTR)pItem);
	}
}

CString CDlgDirectionalityConstraints::GetNodeNameString(Taxiway& taxiway, int nNodeID)
{

	CString strTaxiway;
	strTaxiway.Format("%s", taxiway.GetObjNameString());

	// get the list of intersect node
	IntersectionNodeList nodelist = taxiway.GetIntersectNodes();

	std::vector<IntersectionNode>::iterator iter = nodelist.begin();
	//	m_nCurFirtNodeID = (*iter).GetID();
	for (; iter != nodelist.end(); ++iter)
	{
		if(nNodeID == iter->GetID())
		{
		/*	CString strObjName;
			ALTObject * pObj = (*iter).GetOtherObject(taxiway.getID());
			if(pObj)
				strObjName = pObj->GetObjNameString();*/

			CString strNode = iter->GetName();
			//strNode.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
			return strNode;
		}
	}

	return CString(_T(""));
}

void CDlgDirectionalityConstraints::OnLvnEndLabelEditList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;
	int nItem = pnmv->iSubItem;

	DirectionalityConstraintItem* pItem =
		(DirectionalityConstraintItem*)m_wndListCtrl.GetItemData(nItem);
	
	*pResult = 0;

	GetDlgItem(IDSAVE)->EnableWindow(TRUE);

}

void CDlgDirectionalityConstraints::OnLvnItemChangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateUIState();
	*pResult = 0;
}

LRESULT CDlgDirectionalityConstraints::OnComboBoxChange(WPARAM wParam, LPARAM lParam)
{
	ASSERT(::IsWindow(m_wndListCtrl.GetSafeHwnd()));

	return m_wndListCtrl.OnComboBoxChange(wParam, lParam);

}

void CDlgDirectionalityConstraints::OnNewItem()
{

	if(m_vectTaxiwayID.size() == 0)
	{
		AfxMessageBox(_T("No taxiway define"));
		return;
	}

	std::vector<int>::iterator iterTaxiwayID = m_vectTaxiwayID.begin();
	for (; iterTaxiwayID != m_vectTaxiwayID.end(); ++iterTaxiwayID)
	{
		Taxiway taxiway;
		taxiway.ReadObject(*iterTaxiwayID);
		
		IntersectionNodeList nodeList = taxiway.GetIntersectNodes();
		if(nodeList.size() == 0)
			continue;

		

		int nItemCount = m_wndListCtrl.GetItemCount();

		DirectionalityConstraintItem* pNewItem = new DirectionalityConstraintItem;
		pNewItem->SetAirportID(m_nAirportID);
		pNewItem->SetTaxiwayID(taxiway.getID());

		CString strTaxiwayName = taxiway.GetObjNameString();
		m_wndListCtrl.InsertItem(nItemCount,  strTaxiwayName);

		CPoint2008 ptFrom;
		std::vector<IntersectionNode>::iterator iter = nodeList.begin();
		int nRet = 1;
		for (; iter != nodeList.end(); ++iter)
		{
		/*	CString strObjName;
			ALTObject * pObj = (*iter).GetOtherObject(taxiway.getID());
			if(pObj)
				strObjName = pObj->GetObjNameString();*/
			

			CString strNode = iter->GetName();
			//strNode.Format("(%s) & (%s)-%d", strTaxiwayName, strObjName, ((*iter).GetIndex())+1);
			m_wndListCtrl.SetItemText((int)nItemCount, nRet, strNode);
			if(nRet == 1)
			{
				DistanceUnit distinTaxiway = (*iter).GetDistance(taxiway.getID());
				ptFrom = CPoint2008(taxiway.GetPath().GetIndexDir((float)distinTaxiway));
				pNewItem->SetSegFromID(iter->GetID());
			}
			if(nRet == 2)
			{
				pNewItem->SetSegToID(iter->GetID());
				break;
			}

			nRet++;
			
		}

		ARCVector2 vnorth (0,1);
		ARCVector3 vrunway3 = ptFrom;
		ARCVector2 vrunway2(vrunway3[VX],vrunway3[VY]);
		double dangle =  (vrunway2.GetAngleOfTwoVector(vnorth));
		dangle -= m_airport.GetMagnectVariation().GetRealVariation();	
		if(dangle <= 0 )dangle += 360;

		pNewItem->SetBearingDegree(dangle);
		CString strTemp;
		strTemp.Format("%.f", pNewItem->GetBearingDegree());
		m_wndListCtrl.SetItemText((int)nItemCount, 3, strTemp);

		m_wndListCtrl.SetItemText((int)nItemCount, 4, 
			m_strArrayType.GetAt( (int)pNewItem->GetDirConsType() ) );

		m_pDirectionalityConstraints->AddNewItem(pNewItem);
		m_wndListCtrl.SetItemData(nItemCount, (DWORD_PTR)pNewItem);


		return;
	}
	
	AfxMessageBox("No taxiway has intersections");
}

void CDlgDirectionalityConstraints::OnDelItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	int nItem = -1;
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		nItem = m_wndListCtrl.GetNextSelectedItem(pos);
	


	DirectionalityConstraintItem* pItem =
		(DirectionalityConstraintItem*)m_wndListCtrl.GetItemData(nItem);
	m_pDirectionalityConstraints->DeleteItem(pItem);

	m_wndListCtrl.DeleteItemEx(nItem);
	
}


void CDlgDirectionalityConstraints::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pDirectionalityConstraints->SaveData(m_nAirportID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CXTResizeDialog::OnOK();
}

void CDlgDirectionalityConstraints::OnBnClickedSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pDirectionalityConstraints->SaveData(m_nAirportID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	GetDlgItem(IDSAVE)->EnableWindow(FALSE);
}
