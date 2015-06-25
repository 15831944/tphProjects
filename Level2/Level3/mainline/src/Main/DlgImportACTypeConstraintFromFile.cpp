// DlgImportACTypeConstraintFromFile.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgImportACTypeConstraintFromFile.h"
#include ".\dlgimportactypeconstraintfromfile.h"
#include "../Common/ALTObjectID.h"
#include "../InputAirside/ACTypeStandConstraint.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTObject.h"
// DlgImportACTypeConstraintFromFile dialog

IMPLEMENT_DYNAMIC(DlgImportACTypeConstraintFromFile, CXTResizeDialog)
DlgImportACTypeConstraintFromFile::DlgImportACTypeConstraintFromFile(int nAirportID, ACTypeStandConstraintList* pConstraintList, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgImportACTypeConstraintFromFile::IDD, pParent)
	,m_nAirportID(nAirportID)
	,m_pACTypeStandCons(pConstraintList)
{
	m_ConstraintArray.RemoveAll();
	//m_tipWindow.SetMargins(CSize(1,1));
	//m_tipWindow.SetLineSpace(10);
}

DlgImportACTypeConstraintFromFile::~DlgImportACTypeConstraintFromFile()
{
}

void DlgImportACTypeConstraintFromFile::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONSTRAINT, m_wndListCtrl);
	DDX_Control(pDX, IDC_PROGRESS_IMPORT, m_wndProgress);
}


BEGIN_MESSAGE_MAP(DlgImportACTypeConstraintFromFile, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnBnClickedButtonImport)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL DlgImportACTypeConstraintFromFile::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	GetDlgItem(IDC_BUTTON_IMPORT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	SetResize(IDC_STATIC,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_EDIT_FILEPATH,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_BUTTON_OPEN,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_CONSTRAINT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_RESULT,SZ_BOTTOM_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_EDIT_IMPORTRESULT,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_PROGRESS_IMPORT,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_IMPORT,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	InitListCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void DlgImportACTypeConstraintFromFile::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	m_wndListCtrl.InsertColumn(0, "Stand Group", LVCFMT_LEFT, 120);
	m_wndListCtrl.InsertColumn(1, "AC Types", LVCFMT_LEFT, 220);
	m_wndListCtrl.InsertColumn(2, "Import Status", LVCFMT_CENTER, 80);
	m_wndListCtrl.InsertColumn(3, "Fail Info", LVCFMT_LEFT, 100);
}

void DlgImportACTypeConstraintFromFile::SetListCtrl()
{
	int nCount = m_ConstraintArray.GetCount();
	for (int i =0, j =0; i < nCount; i++, j++)
	{
		CString strContent = m_ConstraintArray.GetAt(i);
		int nIdx = strContent.Find(",");
		CString strName = strContent.Mid(0,nIdx);
		CString strACTypes = strContent.Mid(nIdx+1, strContent.GetLength()-nIdx-1);

		if (i ==0)
		{
			ALTObjectID standID(strName.GetString());
			if (!IsValidateStandGroup(standID))
			{
				j--;
				continue;
			}
		}

		m_wndListCtrl.InsertItem(j,strName);
		m_wndListCtrl.SetItemText(j,1,strACTypes);
		m_wndListCtrl.SetItemData(j, 0);
	}
}

bool DlgImportACTypeConstraintFromFile::IsValidateStandGroup(const ALTObjectID& objID )
{
	if(ALTObject::CheckObjNameIsValid(objID, m_nAirportID))
		return false;

	return true;
}

// DlgImportACTypeConstraintFromFile message handlers

void DlgImportACTypeConstraintFromFile::OnBnClickedButtonOpen()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,"csv", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"AC Type Constraint File (*.csv)|*.csv;*.CSV", NULL, 0, FALSE );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	if( ReadFile( csFileName ) )
	{
		// clear
		m_wndListCtrl.DeleteAllItems();						// clear constraint list

		SetListCtrl();						// add flight schedule to edit

		GetDlgItem(IDC_EDIT_FILEPATH)->SetWindowText( csFileName );
		GetDlgItem(IDC_BUTTON_IMPORT)->EnableWindow(TRUE);
	}
}

int DlgImportACTypeConstraintFromFile::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_tipWindow.Create(this);
	//m_tipWindow.ShowWindow(SW_HIDE);

	return 0;
}

void DlgImportACTypeConstraintFromFile::OnBnClickedButtonImport()
{
	// TODO: Add your control notification handler code here
	int nCount = m_wndListCtrl.GetItemCount();
	GetDlgItem(IDC_STATIC_RESULT)->SetWindowText("Import progress:");
	GetDlgItem(IDC_EDIT_IMPORTRESULT)->ShowWindow(SW_HIDE);
	m_wndProgress.ShowWindow(SW_SHOW);

	m_pACTypeStandCons->RemoveAll();

	m_wndProgress.SetStep(0);
	int nSuccess = 0;
	for (int i =0; i < nCount ; i++)
	{
		CString strStandID = m_wndListCtrl.GetItemText(i, 0);
		CString strACTypes = m_wndListCtrl.GetItemText(i, 1);
		CString strFailInfo = "";

		ALTObjectID GroupName(strStandID.GetString());
		if (IsValidateStandGroup(GroupName))
		{
			ACTypeStandConstraint* pData = new ACTypeStandConstraint;
			pData->m_StandGroup.setName(GroupName);
			strACTypes += ";";		//Add comma at the end of string
			pData->InitACTypes(strACTypes);
			m_pACTypeStandCons->m_vDataList.push_back(pData);
			m_wndListCtrl.SetItemData(i, 2);
			m_wndListCtrl.SetItemImage(i,2,IDI_ICON_SUCCESS);
			nSuccess++;
		}
		else
		{	
			m_wndListCtrl.SetItemData(i, 1);
			strFailInfo = "Invalid Stand Group!";
			m_wndListCtrl.SetItemImage(i,2,IDI_ICON_FAIL);			
		}

		m_wndListCtrl.SetItemText(i,3, strFailInfo);
		m_wndProgress.SetStep((int)((100*i/nCount)));
	}
	m_wndProgress.SetStep(100);
	m_wndProgress.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_RESULT)->SetWindowText("Import Result:");
	GetDlgItem(IDC_EDIT_IMPORTRESULT)->ShowWindow(SW_SHOW);

	CString strResult;
	strResult.Format("Total Lines:%d    Succeed:%d    Failed:%d",nCount,nSuccess, nCount-nSuccess);
	GetDlgItem(IDC_EDIT_IMPORTRESULT)->SetWindowText(strResult);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_IMPORT)->EnableWindow(FALSE);
}

void DlgImportACTypeConstraintFromFile::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void DlgImportACTypeConstraintFromFile::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_pACTypeStandCons->RemoveAll();
	OnCancel();
}

bool DlgImportACTypeConstraintFromFile::ReadFile( const CString& strFileName)
{
	// clear string array 
	m_ConstraintArray.RemoveAll();

	CStdioFile file;
	if(!file.Open( strFileName, CFile::modeRead ) )
	{
		CString strMsg;
		strMsg.Format("Can not open file:\r\n%s", strFileName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
		return false;
	}


	CString strLineText;
	// read all line from file
	while( file.ReadString( strLineText ))
	{
		m_ConstraintArray.Add( strLineText );
	}
	file.Close();
	return true;
}

BOOL DlgImportACTypeConstraintFromFile::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_MOUSEMOVE && pMsg->wParam == 0)
	{
		int id = ::GetDlgCtrlID(pMsg->hwnd);
		CPoint point;
		point.x = LOWORD(pMsg->lParam);
		point.y = HIWORD(pMsg->lParam);
		if(id == IDC_LIST_CONSTRAINT)
			MouseOverListCtrl(point);
	}

	return CXTResizeDialog::PreTranslateMessage(pMsg);
}

void DlgImportACTypeConstraintFromFile::MouseOverListCtrl(CPoint point)
{
	int nItem, nCol;

	// Airside
	if ((nItem = m_wndListCtrl.HitTestEx(point, &nCol)) != -1)
	{
		int nOriginalCol = m_wndListCtrl.GetHeaderCtrl()->OrderToIndex(nCol);
		CString sItemText = m_wndListCtrl.GetItemText(nItem, nOriginalCol);
		int iTextWidth = m_wndListCtrl.GetStringWidth(sItemText) + 5; //5 pixed extra size

		CRect rect;
		m_wndListCtrl.GetSubItemRect(nItem, nOriginalCol, LVIR_LABEL, rect);
		if(iTextWidth > rect.Width())
		{
			rect.top--;

			//if (nOriginalCol == 0)	//Stand family
			//{
			//	CXTStringHelper sTemp(sItemText);
			//	sTemp.Replace(_T("\\n"), _T("\n"));
			//	m_tipWindow.SetTipText(_T("Stand Family:"), sTemp);
			//}
			//else if(nOriginalCol == 1)		//ACTypes
			//{
			//	CXTStringHelper sTemp(sItemText);
			//	sTemp.Replace(_T("\\n"), _T("\n"));
			//	m_tipWindow.SetTipText(_T("AC Type:"), sTemp);
			//}
			//else if (nOriginalCol == 3)
			//{
			//	CXTStringHelper sTemp(sItemText);
			//	sTemp.Replace(_T("\\n"), _T("\n"));
			//	m_tipWindow.SetTipText(_T("Fail Info:"), sTemp);
			//}
			//else
			//{
			//	sItemText.Replace(_T("\r\n"), _T("\n"));
			//	m_tipWindow.SetMargins(CSize(1,1));
			//	m_tipWindow.SetLineSpace(0);
			//	m_tipWindow.SetTipText(_T(""), sItemText);
			//}

			//Calculate the client coordinates of the dialog window
			m_wndListCtrl.ClientToScreen(&rect);
			m_wndListCtrl.ClientToScreen(&point);
			ScreenToClient(&rect);
			ScreenToClient(&point);

			//Show the tip window
			UpdateData();
			//m_tipWindow.ShowTipWindow(rect, point, TWS_XT_ALPHASHADOW | TWS_XT_DROPSHADOW, 0, 0, false, FALSE);
		}

	}
}