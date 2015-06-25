// ShapeItem.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ShapeItem.h"
#include "afxdialogex.h"
#include "Common\ProjectManager.h"

// CShapeItem dialog

IMPLEMENT_DYNAMIC(CShapeItem, CDialog)

int CShapeItem::item_id = 0;
CShapeItem::CShapeItem(CString folderpath,CWnd* pParent /*=NULL*/)
	: CDialog(CShapeItem::IDD, pParent)
{
	if (item_id != 0)
		itemName.Format("New Item(%d)",item_id);
	else
		itemName = "New Item";
	itemPicture = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\New.bmp";
	itemModel = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\FLATSQUARE24cm.dxf";
	m_style = NEW;
	folderPath = folderpath;
}

CShapeItem::CShapeItem(CString folderpath,CString name, CString picture, CString model, Item_Style style /* = NEW */,CWnd* pParent /* = NULL */)
	: CDialog(CShapeItem::IDD, pParent)
{
	itemName = name;
	itemPicture = picture;
	itemModel = model;
	m_style = style;
	folderPath = folderpath;
}


CShapeItem::~CShapeItem()
{
}

void CShapeItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_itemName);
	DDX_Control(pDX, IDC_EDIT2, m_itemModel);
	DDX_Control(pDX, IDC_PIC_STATIC, m_itemPicture);
}


BEGIN_MESSAGE_MAP(CShapeItem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON8, &CShapeItem::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON1, &CShapeItem::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CShapeItem::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CShapeItem::OnBnClickedOk)
END_MESSAGE_MAP()


// CShapeItem message handlers


void CShapeItem::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here

	CFileDialog  dlgFile(TRUE,NULL,folderPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.bmp)|*.bmp");
	if (dlgFile.DoModal() == IDOK)
	{
		itemPicture = dlgFile.GetPathName();
		HBITMAP pBMP = (HBITMAP)LoadImage(NULL,itemPicture,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(pBMP)
		{
			m_itemPicture.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);   
			m_itemPicture.SetBitmap(pBMP);
		}
	}
}


void CShapeItem::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	CFileDialog  dlgFile(TRUE,NULL,folderPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.dxf)|*.dxf");
	if (dlgFile.DoModal() == IDOK)
	{
		itemModel = dlgFile.GetPathName();
	}
	m_itemModel.SetWindowText(itemModel);
}


BOOL CShapeItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	switch(m_style)
	{
	case NEW:
		break;
	case NAME:
		((CWnd*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE);
		break;
	case PICTURE:
		m_itemName.EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
		break;
	case MODEL:
		m_itemName.EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE);
		break;
	}

	m_itemName.SetWindowText(itemName);
	m_itemModel.SetWindowText(itemModel);

	HBITMAP pBMP = (HBITMAP)LoadImage(NULL,itemPicture,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(pBMP)
	{
		m_itemPicture.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);   
		m_itemPicture.SetBitmap(pBMP);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CShapeItem::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CShapeItem::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_itemName.GetWindowText(itemName);
	CDialog::OnOK();
}
