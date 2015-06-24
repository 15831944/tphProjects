// DlgNewStd2GateMapping.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgNewStd2GateMapping.h"
#include ".\dlgnewstd2gatemapping.h"
#include "../InputAirside/ALTAirport.h"
#include "boost/array.hpp"

// CDlgNewStd2GateMapping dialog

IMPLEMENT_DYNAMIC(CDlgNewStd2GateMapping, CDialog)
CDlgNewStd2GateMapping::CDlgNewStd2GateMapping(InputTerminal* _pInTerm,int nAirportID,int nFlag,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewStd2GateMapping::IDD, pParent)
	,m_nFlag(nFlag)
	,m_nAirportID(nAirportID)
	,m_pInTerm(_pInTerm)
	,m_bRandomMapping(FALSE)
	,m_iMappingFlag(0x1)
	,m_bSel4OfLeft(false)
	,m_bSel4OfRight(false)
{
	ASSERT(_pInTerm);
}

CDlgNewStd2GateMapping::~CDlgNewStd2GateMapping()
{
}

void CDlgNewStd2GateMapping::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndStand);
	DDX_Control(pDX, IDC_TREE2, m_wndGate);
	DDX_Radio(pDX, IDC_RADIO1, m_bRandomMapping);
}


BEGIN_MESSAGE_MAP(CDlgNewStd2GateMapping, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE2, OnTvnSelchangedTree2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNewStd2GateMapping message handlers

BOOL CDlgNewStd2GateMapping::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_nFlag == 0x1) {//arr
		GetDlgItem(IDC_STATIC_GATE_DETAIL)->SetWindowText(_T("Arrival Gate: "));
	}else if (m_nFlag == 0x2) {//dep
		GetDlgItem(IDC_STATIC_GATE_DETAIL)->SetWindowText(_T("Departure Gate: "));
	}
	InitializeStand();
	InitializeGate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewStd2GateMapping::InitializeStand(void)
{
	//std::vector<ALTObject> vObject;
	ALTObjectList vObject;

	ALTObjectIDList objIDList;

	//ALTObject::GetObjectList(ALT_STAND,m_nAirportID,vObject);
	ALTAirport::GetStandList(m_nAirportID,vObject);

	boost::array<std::pair<CString,HTREEITEM>,OBJECT_STRING_LEVEL> LastObjectItem;

	int nObjectCount = vObject.size();
	for (int i =0; i < nObjectCount; ++ i)
	{
		ALTObject* pAltObject = vObject.at(i).get();
		ALTObjectID objName;
		pAltObject->getObjName(objName);

		//Level1
		int nLevel = 0;
		CString levelstr = objName.m_val[nLevel].c_str();

		if (levelstr != LastObjectItem[nLevel].first)
		{
			for (int j=nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}

			HTREEITEM hTreeItem = m_wndStand.InsertItem(objName.m_val[nLevel].c_str());
			LastObjectItem[nLevel].first = objName.m_val[nLevel].c_str();
			LastObjectItem[nLevel].second = hTreeItem;


			//CNodeData* pNodeData= NULL;
			if(objName.m_val[nLevel+1] == _T("")) //object node
			{

				continue;
			}
			else
			{

			}

		}

		//level2
		nLevel = 1;
		levelstr = objName.m_val[nLevel].c_str();
		if (levelstr != LastObjectItem[nLevel].first)
		{		
			for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}

			HTREEITEM hTreeItem = m_wndStand.InsertItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
			m_wndStand.Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);

			LastObjectItem[nLevel].first = objName.m_val[nLevel].c_str();
			LastObjectItem[nLevel].second = hTreeItem;

			if(objName.m_val[nLevel+1] == _T("")) //object node
			{

				continue;	
			}
			else
			{

			}

		}

		//level3
		nLevel = 2;
		levelstr = objName.m_val[nLevel].c_str();
		if (levelstr != LastObjectItem[nLevel].first)
		{	

			for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}

			HTREEITEM hTreeItem = m_wndStand.InsertItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
//			m_wndStand.Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);

			LastObjectItem[nLevel].first = levelstr;
			LastObjectItem[nLevel].second = hTreeItem;
			//CNodeData* pNodeData= NULL;
			if(objName.m_val[nLevel+1] == _T("")) //object node
			{

				continue;	
			}
			else
			{

			}
		}

		nLevel = 3;
		levelstr = objName.m_val[nLevel].c_str();
		if (levelstr != LastObjectItem[nLevel].first)
		{
			HTREEITEM hTreeItem = m_wndStand.InsertItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);
//			m_wndStand.Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);
			LastObjectItem[nLevel].first = levelstr;
			LastObjectItem[nLevel].second = hTreeItem;			
			continue;
		}

	}
}

void CDlgNewStd2GateMapping::InitializeGate(void)
{
	if(!m_pInTerm)return ;

	ProcessorList *procList = m_pInTerm->GetProcessorList();
	if (!procList)
		return;

	// Set data to processor tree
	ProcessorID id, *pid = NULL;
	id.SetStrDict( m_pInTerm->inStrDict );
	int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;

	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;

	StringList strDict;
	//modify by 
	procList->getAllGroupNames (strDict, GateProc);

	for (j = 0; j < strDict.getCount(); j++)   // Level 1
	{
		strTemp = strDict.getString (j);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;

		ti.mask       = TCIF_TEXT;
		ti.cChildren  = 1;
		ti.pszText    = strTemp.GetBuffer(0);
		ti.cchTextMax = 256;

		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;

		hItem = m_wndGate.InsertItem(&tis);
		if (hItem == NULL)
			continue;

		str = strTemp;
		id.setID((LPCSTR) str);

		StringList strDict1;
		procList->getMemberNames (id, strDict1, GateProc);
		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;

			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;

			hItem1 = m_wndGate.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;

			str = strTemp;
			str += "-";
			str += strTemp1;

			id.setID((LPCSTR) str);

			StringList strDict2;
			procList->getMemberNames (id, strDict2, GateProc);
			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;

				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;

				hItem2 = m_wndGate.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;

				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;

				id.setID((LPCSTR) str);

				StringList strDict3;
				procList->getMemberNames (id, strDict3, GateProc);
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;

					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;

					hItem3 = m_wndGate.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;

					id.setID((LPCSTR) str);
				}
	//			m_wndGate.Expand(hItem2,TVE_EXPAND);
			}
	//		m_wndGate.Expand(hItem1,TVE_EXPAND);
		}
		m_wndGate.Expand(hItem,TVE_EXPAND);
	}
}

void CDlgNewStd2GateMapping::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = NULL;
	hItem = pNMTreeView->itemNew.hItem;
	if (hItem == NULL) return;

	CString reslt = _T("");
	reslt = m_wndStand.GetItemText(hItem);
	HTREEITEM parentItem = hItem;
	m_bSel4OfLeft = false;
	int i = 0;
	while( parentItem = m_wndStand.GetParentItem(parentItem) ){
		CString newreslt = m_wndStand.GetItemText(parentItem);
		newreslt = newreslt + '-' + reslt;
		reslt = newreslt;	
		i++;
		if(i == 3)
			m_bSel4OfLeft = true;
	}  
	if(m_bSel4OfRight){
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(BST_UNCHECKED);
// 		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
 		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	}else{
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
	}
	m_StandID = ALTObjectID(reslt);
	*pResult = 0;
}

void CDlgNewStd2GateMapping::OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = NULL;
	hItem = pNMTreeView->itemNew.hItem;
	if (hItem == NULL) return;

	CString reslt = _T("");
	reslt = m_wndGate.GetItemText(hItem);
	HTREEITEM parentItem = hItem;
	m_bSel4OfRight = false;
	int i = 0;
	while( parentItem = m_wndGate.GetParentItem(parentItem) ){
		CString newreslt = m_wndGate.GetItemText(parentItem);
		newreslt = newreslt + '-' + reslt;
		reslt = newreslt;
		i++;
		if(i == 3)
			m_bSel4OfRight = true;
	}   
	if(m_bSel4OfRight){
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(BST_UNCHECKED);
// 		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
 		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	}else{
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
	}

	m_GateID.SetStrDict(m_pInTerm->inStrDict);
	m_GateID.setID((LPCSTR) reslt);
	*pResult = 0;
}

void CDlgNewStd2GateMapping::OnBnClickedOk()
{
	UpdateData();
	if (!m_wndStand.GetSelectedItem() || !m_wndGate.GetSelectedItem()) {
		if (m_nFlag == 0x1)//arr
			MessageBox(_T("No selected Stand(group) and Arrival Gate(group)."));
		else if (m_nFlag == 0x2)
			MessageBox(_T("No selected Stand(group) and Departure Gate(group)."));
		return;
	}

	CString strField0 = _T(""),strField1 = _T(""),strField2 = _T(""),strField3 = _T("");
	char szField[1024] = {0};
	m_GateID.getNameAtLevel(szField,0);
	strField0 = szField;

	memset(szField,0,sizeof(szField));
	m_GateID.getNameAtLevel(szField,1);
	strField1 = szField;

	memset(szField,0,sizeof(szField));
	m_GateID.getNameAtLevel(szField,2);
	strField2 = szField;

	memset(szField,0,sizeof(szField));
	m_GateID.getNameAtLevel(szField,3);
	strField3 = szField; 

// 	BOOL bOne2One = TRUE;//temp
// 	if (!m_StandID.at(0).empty() && !strField0.IsEmpty() && \
// 		!m_StandID.at(1).empty() && !strField1.IsEmpty() && \
// 		!m_StandID.at(2).empty() && !strField2.IsEmpty() && \
// 		!m_StandID.at(3).empty() && !strField3.IsEmpty() ) {
// 		bOne2One = FALSE;
// 	}else if (!m_StandID.at(0).empty() && !strField0.IsEmpty() && \
// 			!m_StandID.at(1).empty() && !strField1.IsEmpty() && \
// 			!m_StandID.at(2).empty() && !strField2.IsEmpty() ) {
// 		bOne2One = TRUE;
// 	}else if (!m_StandID.at(0).empty() && !strField0.IsEmpty() && \
// 				!m_StandID.at(1).empty() && !strField1.IsEmpty()) {
// 		bOne2One = TRUE;
// 	}else if (!m_StandID.at(0).empty() && !strField0.IsEmpty() ) {
// 		bOne2One = TRUE;
// 	}else{
// 		MessageBox(_T("Selected must be match for Stand(group) and Departure Gate(group)."));
// 		return;
// 	}
//	if(bOne2One){
		if(!m_bRandomMapping)
			m_iMappingFlag = 0x1;
		else
			m_iMappingFlag = 0x2;
	//}else
	//	m_iMappingFlag = 0x0;
	OnOK();
}
