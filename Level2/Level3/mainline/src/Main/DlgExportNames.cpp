(((// DlgExportNames.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgExportNames.h"
#include ".\dlgexportnames.h"
#include "..\Engine\proclist.h"
#include "ARCPort.h"
#include "..\InputAirside\ALTAirport.h"


// CDlgExportNames dialog

IMPLEMENT_DYNAMIC(CDlgExportNames, CDialog)
CDlgExportNames::CDlgExportNames(CARCPort *pARCPort,int nProjectID, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportNames::IDD, pParent)
{
	m_pARCPort		= pARCPort;
	m_nProjectID	= nProjectID;

	m_hTerminalRoot = NULL;
	m_hAirsideRoot	= NULL;
	m_hLandsideRoot = NULL;
	m_hOnboardRoot	= NULL;
}

CDlgExportNames::~CDlgExportNames()
{
	std::vector<ItemData *>::iterator iter =  m_vNodeData.begin();
	for( ; iter != m_vNodeData.end(); ++ iter)
	{
		delete *iter;
	}
	m_vNodeData.clear();
}

void CDlgExportNames::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NAMES, m_wndTreeName);
}


BEGIN_MESSAGE_MAP(CDlgExportNames, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnBnClickedButtonExport)
	ON_WM_SIZE()
	ON_MESSAGE(UM_CEW_STATUS_CHANGE,OnStatusChanged)
END_MESSAGE_MAP()


// CDlgExportNames message handlers
BOOL CDlgExportNames::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class


	return CDialog::Create(lpszTemplateName, pParentWnd);
}

BOOL CDlgExportNames::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Export Names"));
	GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
	
	// TODO:  Add extra initialization here
	m_hTerminalRoot = AddItem(_T("Terminal"), TVI_ROOT);
	m_wndTreeName.SetItemBold(m_hTerminalRoot, TRUE);
	LoadTerminal();
	
	m_hAirsideRoot = AddItem(_T("Airside"),TVI_ROOT);
	m_wndTreeName.SetItemBold(m_hAirsideRoot, TRUE);
	LoadAirside();

	m_hLandsideRoot = AddItem(_T("Landside"), TVI_ROOT);
	m_wndTreeName.SetItemBold(m_hLandsideRoot, TRUE);
	LoadLandside();

	m_hOnboardRoot = AddItem(_T("Onboard"), TVI_ROOT);
	m_wndTreeName.SetItemBold(m_hOnboardRoot, TRUE);
	LoadOnboard();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgExportNames::LoadTerminal()
{
	ProcessorList* procList = m_pARCPort->getTerminal().GetProcessorList();
	if(procList == NULL)
		return;

	StringList strListL1;
	procList->getAllGroupNames (strListL1, -1);
	procList->getAllGroupNames (strListL1, BarrierProc);

	//level 1
	for(int i = 0; i < strListL1.getCount(); i++) 
	{
		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
			continue;


		CString sID = strTemp1;
		ProcessorID procID;
		procID.SetStrDict(m_pARCPort->getTerminal().inStrDict);
		procID.setID((LPCSTR) sID);
		StringList strListL2;
		procList->getMemberNames( procID, strListL2, -1 );
		procList->getMemberNames( procID, strListL2, BarrierProc );

		HTREEITEM hItem1 = NULL; 
		if(strListL2.IsEmpty()) 
		{
			AddItem(strTemp1, m_hTerminalRoot, IT_OBJECT, procID.GetIDString());
			continue;
		}
		else
		{
			hItem1 = AddItem(strTemp1, m_hTerminalRoot);
		}



		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ )
		{
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
				continue;

			HTREEITEM hItem2 = NULL;

			sID = strTemp1+"-"+strTemp2;
			procID.setID((LPCSTR) sID);
			StringList strListL3;
			procList->getMemberNames( procID, strListL3, -1 );
			procList->getMemberNames( procID, strListL3, BarrierProc );
			if(strListL3.IsEmpty())
			{
				AddItem(strTemp2, hItem1, IT_OBJECT, procID.GetIDString());
				continue;
			}
			else
			{
				hItem2 = AddItem(strTemp2, hItem1);
			}
			
			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ )
			{
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
					continue;

				HTREEITEM hItem3 = NULL;

				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				procID.setID((LPCSTR) sID);
				StringList strListL4;
				procList->getMemberNames( procID, strListL4, -1 );
				procList->getMemberNames( procID, strListL4, BarrierProc );
				if(strListL4.IsEmpty())
				{
					AddItem(strTemp3, hItem2,IT_OBJECT, procID.GetIDString());
					continue;
				}
				else
				{
					hItem3 = AddItem(strTemp3, hItem2);
				}

				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ )
				{
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
						continue;
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					procID.setID((LPCSTR) sID);
				
					HTREEITEM hItem4 = AddItem(strTemp4, hItem3, IT_OBJECT,procID.GetIDString());
				}
			}
		}
	}
}

void CDlgExportNames::LoadObjects( HTREEITEM hRoot,ALTOBJECT_TYPE enumType, CString strTypeName, int nAirportID  )
{

	HTREEITEM hTypeRoot = AddItem(strTypeName, hRoot);
	m_wndTreeName.SetItemBold(hTypeRoot, TRUE);

	ALTObjectIDList nameList;
	ALTObject::GetObjectNameList(enumType, nAirportID,nameList);

	int nCount = static_cast<int>(nameList.size());
	for( int nItem = 0; nItem < nCount; ++ nItem)
	{
		HTREEITEM hParentItem = hTypeRoot;
		const ALTObjectID& objName = nameList.at(nItem);

		for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
		{
			if (nLevel != OBJECT_STRING_LEVEL -1 )
			{
				HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
				
				if (hItem != NULL)
				{
					hParentItem = hItem;
					continue;
				}

				if (objName.m_val[nLevel+1] != _T(""))
				{			
					HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);	

					hParentItem = hTreeItem;

					continue;
				}
				else //objNode
				{
					HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem, IT_OBJECT, objName.GetIDString());
					break;
				}

			}
			else
			{
				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem, IT_OBJECT, objName.GetIDString());
				break;
			}
		}
	}
}

void CDlgExportNames::LoadAirside()
{
	//airport
	//get airport list
	std::vector<ALTAirport> vAirport;
	try
	{
		ALTAirport::GetAirportList(m_nProjectID,vAirport);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the airport information."));
		return;
	}

	for (int nAirport = 0; nAirport < static_cast<int>(vAirport.size()); ++ nAirport)
	{	
		const ALTAirport& altAirport = vAirport.at(nAirport);

		HTREEITEM hAirport = AddItem( altAirport.getName(), m_hAirsideRoot);
		m_wndTreeName.SetItemBold(hAirport, TRUE);

		//runway
		std::vector<std::pair<ALTOBJECT_TYPE, CString> > vAirportType;
		vAirportType.push_back(std::make_pair(ALT_RUNWAY, _T("Runway")));
		vAirportType.push_back(std::make_pair(ALT_HELIPORT, _T("Heliports")));
		vAirportType.push_back(std::make_pair(ALT_TAXIWAY, _T("Taxiway")));
		vAirportType.push_back(std::make_pair(ALT_STAND, _T("Stand")));
		vAirportType.push_back(std::make_pair(ALT_DEICEBAY, _T("Deice Pad")));

		vAirportType.push_back(std::make_pair(ALT_STRETCH, _T("Stretch")));
		vAirportType.push_back(std::make_pair(ALT_INTERSECTIONS, _T("Intersection")));
		vAirportType.push_back(std::make_pair(ALT_ROUNDABOUT, _T("Roundabout")));
		vAirportType.push_back(std::make_pair(ALT_VEHICLEPOOLPARKING, _T("Pool Parking")));

		vAirportType.push_back(std::make_pair(ALT_TRAFFICLIGHT, _T("Trafic Light")));
		vAirportType.push_back(std::make_pair(ALT_TOLLGATE, _T("Toll Gate")));
		vAirportType.push_back(std::make_pair(ALT_STOPSIGN, _T("Stop Sign")));
		vAirportType.push_back(std::make_pair(ALT_YIELDSIGN, _T("Yeild Sign")));

		int nTypeCount = static_cast<int>(vAirportType.size());
		for (int nType = 0; nType < nTypeCount; ++ nType )
		{
			LoadObjects(hAirport,vAirportType[nType].first, vAirportType[nType].second ,altAirport.getID());

		}
	}

	//airspace
	{
		HTREEITEM hAirspace= AddItem( _T("Airspace"), m_hAirsideRoot);
		m_wndTreeName.SetItemBold(hAirspace, TRUE);

		std::vector<std::pair<ALTOBJECT_TYPE, CString> > vAirspaceType;
		vAirspaceType.push_back(std::make_pair(ALT_WAYPOINT, _T("Waypoint")));
		vAirspaceType.push_back(std::make_pair(ALT_HOLD, _T("Hold")));
		vAirspaceType.push_back(std::make_pair(ALT_SECTOR, _T("Sector")));


		int nTypeCount = static_cast<int>(vAirspaceType.size());
		for (int nType = 0; nType < nTypeCount; ++ nType )
		{
			LoadObjects(hAirspace,vAirspaceType[nType].first, vAirspaceType[nType].second ,m_nProjectID);
		}
	}

	//topography
	{
		HTREEITEM hTopography = AddItem( _T("Topography"), m_hAirsideRoot);
		m_wndTreeName.SetItemBold(hTopography, TRUE);

		std::vector<std::pair<ALTOBJECT_TYPE, CString> > vTopographyType;
		vTopographyType.push_back(std::make_pair(ALT_SURFACE, _T("Surface")));
		vTopographyType.push_back(std::make_pair(ALT_STRUCTURE, _T("Structure")));
		vTopographyType.push_back(std::make_pair(ALT_WALLSHAPE, _T("Wall Shape")));


		int nTypeCount = static_cast<int>(vTopographyType.size());
		for (int nType = 0; nType < nTypeCount; ++ nType )
		{
			LoadObjects(hTopography,vTopographyType[nType].first, vTopographyType[nType].second ,m_nProjectID);
		}
	}



}
HTREEITEM CDlgExportNames::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_wndTreeName.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_wndTreeName.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_wndTreeName.GetNextSiblingItem(hChildItem);
	}

	return hRetItem;
}
void CDlgExportNames::LoadLandside()
{

}

void CDlgExportNames::LoadOnboard()
{

}
void CDlgExportNames::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_wndTreeName.GetSafeHwnd() == NULL)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);
	m_wndTreeName.MoveWindow(7,7,rcClient.Width() - 16, rcClient.Height() - 50);
	
	CRect rcExport;
	GetDlgItem(IDC_BUTTON_EXPORT)->GetClientRect(&rcExport);
	GetDlgItem(IDC_BUTTON_EXPORT)->MoveWindow(rcClient.Width() - 180, rcClient.Height() - 35, rcExport.Width(), rcExport.Height() );

	CRect rcOK;
	GetDlgItem(IDOK)->GetClientRect(&rcOK);
	GetDlgItem(IDOK)->MoveWindow(rcClient.Width() - 90, rcClient.Height() - 35, rcOK.Width(), rcOK.Height() );


	// TODO: Add your message handler code here
}

void CDlgExportNames::OnBnClickedButtonExport()
{

	CFileDialog fileDlg(FALSE, _T("*.csv"), _T("*.csv"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"), NULL, 0, FALSE );

	if(fileDlg.DoModal()!=IDOK)
	{
		return;
	}

	CString csFileName=fileDlg.GetPathName();

	ArctermFile exportFile;
	exportFile.openFile(csFileName,WRITE);
	
	//terminal object
	std::vector<CString> vExportNames;
	GetExportNames(m_hTerminalRoot,vExportNames);
	GetExportNames(m_hAirsideRoot,vExportNames);





	int nCount = static_cast<int>(vExportNames.size());
	for(int nItem = 0; nItem < nCount;  ++ nItem)
	{
		exportFile.writeField(vExportNames.at(nItem));
		//exportFile.writeField(_T(""));
		exportFile.writeLine();
	}
	exportFile.endFile();

	MessageBox(_T("Export successfully."), _T("ARCPort"), MB_OK);

}

void CDlgExportNames::GetExportNames( HTREEITEM hItem, std::vector<CString>& vExportName )
{
	if(m_wndTreeName.IsCheckItem(hItem) == 1)//check
	{
		ItemData *pData = (ItemData *)m_wndTreeName.GetItemData(hItem);
		if(pData && pData->enumType == IT_OBJECT)
		{
			vExportName.push_back(pData->strName);
		}

	}
	else
		return;


	HTREEITEM hChildItem = m_wndTreeName.GetChildItem(hItem);
	while (hChildItem)
	{
		GetExportNames(hChildItem, vExportName);

		hChildItem = m_wndTreeName.GetNextSiblingItem(hChildItem);
	}

}
void CDlgExportNames::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

HTREEITEM CDlgExportNames::AddItem( const CString& strItemText, HTREEITEM hParentItem , ItemType enumType, CString strName)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);
	cni.nt = NT_CHECKBOX;

	HTREEITEM hNewItem =  m_wndTreeName.InsertItem(strItemText, cni, FALSE, FALSE, hParentItem);
	ItemData *pData = new ItemData;
	pData->enumType = enumType;
	pData->strName = strName;
	m_wndTreeName.SetItemData(hNewItem, (DWORD_PTR)pData);

	return hNewItem;
}

LRESULT CDlgExportNames::OnStatusChanged( WPARAM wParam, LPARAM lParam )
{
	HTREEITEM hItem = (HTREEITEM)wParam;
	if(hItem == NULL)
		return 0;

	if(m_wndTreeName.IsCheckItem(hItem))//check
	{
		HTREEITEM hChildItem = m_wndTreeName.GetChildItem(hItem);

		while (hChildItem)
		{
			CheckChildItem(hChildItem);
			m_wndTreeName.SetCheckStatus(hChildItem,TRUE);

			hChildItem = m_wndTreeName.GetNextSiblingItem(hChildItem);
		}

	}
	else//uncheck
	{

	}

	
	return 0;
}
//check all the child items
void CDlgExportNames::CheckChildItem( HTREEITEM hItem )
{
	m_wndTreeName.SetCheckStatus(hItem,TRUE);

	HTREEITEM hChildItem = m_wndTreeName.GetChildItem(hItem);

	while (hChildItem)
	{
		CheckChildItem(hChildItem);
		m_wndTreeName.SetCheckStatus(hChildItem,TRUE);

		hChildItem = m_wndTreeName.GetNextSiblingItem(hChildItem);
	}

}


