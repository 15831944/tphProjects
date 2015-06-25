// DlgSelectSector.cpp : implementation file
//

#include "stdafx.h"
#include "..\InputAirside\InTrailSeperationEx.h"
#include "DlgSelectSector.h"
#include "DlgInTrailSeperationEx.h"
#include "..\InputAirside\ALTObject.h"
#include ".\dlgselectsector.h"
#include "../Database/DBElementCollection_Impl.h"


// CDlgSelectSector dialog

IMPLEMENT_DYNAMIC(CDlgSelectSector, CXTResizeDialog)
CDlgSelectSector::CDlgSelectSector(int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSelectSector::IDD, pParent)
{
	m_nProjID = nProjID;
}

CDlgSelectSector::~CDlgSelectSector()
{
}

void CDlgSelectSector::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SECTOR_LIST, m_wndListCtrlSector);
}


BEGIN_MESSAGE_MAP(CDlgSelectSector, CXTResizeDialog)
	ON_LBN_SELCHANGE(IDC_SECTOR_LIST, OnLbnSelchangeSectorList)
END_MESSAGE_MAP()


// CDlgSelectSector message handlers

BOOL CDlgSelectSector::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	InitListBox();

	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_SECTOR, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_SECTOR_LIST,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgSelectSector::InitListBox()
{
	UpdateData(FALSE);
	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(ALT_SECTOR,m_nProjID,vObject);
	int nObjectCount = (int)vObject.size();
	int k = 0;
	for (int i = 0;i < nObjectCount;i++)
	{
		CString strSectorName;
		CString strTemp;
		ALTObjectID objName;
		ALTObject altObject = vObject.at(i);
	    altObject.getObjName(objName);
		if (!objName.GetIDString().IsEmpty())
		{
			strSectorName = objName.GetIDString();
			m_wndListCtrlSector.AddString(strSectorName);
			m_wndListCtrlSector.SetItemData(k,(DWORD)altObject.getID());
			k++;
		}
	}
}

CString CDlgSelectSector::GetListBoxItemText(CListBox& hListBox)
{
	CString strTmp;
	int nIndex = hListBox.GetCurSel();
	hListBox.GetText(nIndex,strTmp);

	return strTmp;
}
void CDlgSelectSector::OnLbnSelchangeSectorList()
{
	// TODO: Add your control notification handler code here
	
	m_strSector = GetListBoxItemText(m_wndListCtrlSector);
	m_nSectorID = (int)m_wndListCtrlSector.GetItemData(m_wndListCtrlSector.GetCurSel());
}
