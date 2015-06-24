// DlgNewStd2GateMapping.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgShowStd2GateMappingDetail.h"
#include ".\DlgShowStd2GateMappingDetail.h"
#include "../InputAirside/ALTAirport.h"

// CDlgNewStd2GateMapping dialog

IMPLEMENT_DYNAMIC(CDlgShowStd2GateMappingDetail, CDialog)
CDlgShowStd2GateMappingDetail::CDlgShowStd2GateMappingDetail(InputTerminal* _pInTerm,int nAirportID,int nFlag,CWnd* pParent /*=NULL*/)
: CDialog(CDlgShowStd2GateMappingDetail::IDD, pParent)
,m_nFlag(nFlag)
,m_nAirportID(nAirportID)
,m_pInTerm(_pInTerm)
,m_iMappingFlag(0x0)
{
	ASSERT(_pInTerm);
}

CDlgShowStd2GateMappingDetail::~CDlgShowStd2GateMappingDetail()
{
}

void CDlgShowStd2GateMappingDetail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndStand);
	DDX_Control(pDX, IDC_TREE2, m_wndGate);
}


BEGIN_MESSAGE_MAP(CDlgShowStd2GateMappingDetail, CDialog)
END_MESSAGE_MAP()


// CDlgNewStd2GateMapping message handlers

BOOL CDlgShowStd2GateMappingDetail::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_nFlag == 0x1) {//arr
		GetDlgItem(IDC_STATIC_GATE_DETAIL)->SetWindowText(_T("Arrival Gate: "));
	}else if (m_nFlag == 0x2) {//dep
		GetDlgItem(IDC_STATIC_GATE_DETAIL)->SetWindowText(_T("Departure Gate: "));
	}
	InitializeStand();
	InitializeGate();

	if(m_iMappingFlag == 0x0){
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	}else{
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		if(m_iMappingFlag == 0x1)
			((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
		else
			((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(BST_CHECKED);
	}

	SetWindowText(_T("Mapping detail"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgShowStd2GateMappingDetail::InitializeStand(void)
{
	//std::vector<ALTObject> vObject;

	ALTObjectIDList objIDList;

	//ALTObject::GetObjectList(ALT_STAND,m_nAirportID,vObject);
	ALTObjectList vObject;
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
		CString strMatchStand = m_StandID.m_val[nLevel].c_str();
		if(!strMatchStand.IsEmpty() && levelstr != strMatchStand)continue;

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
		strMatchStand = m_StandID.m_val[nLevel].c_str();
		if(!strMatchStand.IsEmpty() && levelstr != strMatchStand)continue;
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
		strMatchStand = m_StandID.m_val[nLevel].c_str();
		if(!strMatchStand.IsEmpty() && levelstr != strMatchStand)continue;
		if (levelstr != LastObjectItem[nLevel].first)
		{	

			for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
			{
				LastObjectItem[j].first = _T("");
				LastObjectItem[j].second = NULL;
			}

			HTREEITEM hTreeItem = m_wndStand.InsertItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
			m_wndStand.Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);

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
		strMatchStand = m_StandID.m_val[nLevel].c_str();
		if(!strMatchStand.IsEmpty() && levelstr != strMatchStand)continue;
		if (levelstr != LastObjectItem[nLevel].first)
		{
			HTREEITEM hTreeItem = m_wndStand.InsertItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);
			m_wndStand.Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);
			LastObjectItem[nLevel].first = levelstr;
			LastObjectItem[nLevel].second = hTreeItem;			
			continue;
		}

	}
}

void CDlgShowStd2GateMappingDetail::InitializeGate(void)
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

	char szField[1024] = {0};
	CString strGateField  = _T("");

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

		memset(szField,0,sizeof(szField));
		m_GateID.getNameAtLevel(szField,0);
		strGateField = szField;
		if (!strGateField.IsEmpty() && strGateField != strTemp)continue;

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

			memset(szField,0,sizeof(szField));
			m_GateID.getNameAtLevel(szField,1);
			strGateField = szField;
			if (!strGateField.IsEmpty() && strGateField != strTemp1)continue;

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

				memset(szField,0,sizeof(szField));
				m_GateID.getNameAtLevel(szField,2);
				strGateField = szField;
				if (!strGateField.IsEmpty() && strGateField != strTemp2)continue;

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

					memset(szField,0,sizeof(szField));
					m_GateID.getNameAtLevel(szField,3);
					strGateField = szField;
					if (!strGateField.IsEmpty() && strGateField != strTemp3)continue;

					hItem3 = m_wndGate.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;

					id.setID((LPCSTR) str);
				}
				m_wndGate.Expand(hItem2,TVE_EXPAND);
			}
			m_wndGate.Expand(hItem1,TVE_EXPAND);
		}
		m_wndGate.Expand(hItem,TVE_EXPAND);
	}
}

