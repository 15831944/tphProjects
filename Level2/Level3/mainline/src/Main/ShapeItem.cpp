// ShapeItem.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ShapeItem.h"
#include "afxdialogex.h"
#include "Common\ProjectManager.h"

// CShapeItem dialog

IMPLEMENT_DYNAMIC(CShapeItem, CDialog)

CShapeItem::CShapeItem(CString folderpath,CWnd* pParent /*=NULL*/)
	: CDialog(CShapeItem::IDD, pParent),
    m_oldClientWidth(0),
    m_oldClientHeight(0)
{
	m_shapePicture = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\New.bmp";
	m_shapeModel = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\FLATSQUARE24cm.dxf";
	m_style = NEW;
	m_folderLocation = folderpath;
}

CShapeItem::CShapeItem(CString folderpath,CString name, CString picture, CString model, Item_Style style /* = NEW */,CWnd* pParent /* = NULL */)
	: CDialog(CShapeItem::IDD, pParent),
    m_oldClientWidth(0),
    m_oldClientHeight(0)
{
	m_shapeName = name;
	m_shapePicture = picture;
	m_shapeModel = model;
	m_style = style;
	m_folderLocation = folderpath;
}


CShapeItem::~CShapeItem()
{
}

void CShapeItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHAPENAME, m_editShapeName);
	DDX_Control(pDX, IDC_EDIT_MODELLOCATION, m_editShapeModel);
	DDX_Control(pDX, IDC_PIC_STATIC, m_staticPic);
}


BEGIN_MESSAGE_MAP(CShapeItem, CDialog)
	ON_BN_CLICKED(IDC_BTN_LOADPICTURE, OnLoadShapePicture)
	ON_BN_CLICKED(IDC_BTN_LOADMODEL, OnLoadShapeModel)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDOK, OnOk)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CShapeItem message handlers
void CShapeItem::OnLoadShapePicture()
{
	CFileDialog  dlgFile(TRUE,NULL,m_folderLocation,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.bmp)|*.bmp|");
	if (dlgFile.DoModal() == IDOK)
	{
		m_shapePicture = dlgFile.GetPathName();
		HBITMAP pBMP = (HBITMAP)LoadImage(NULL,m_shapePicture,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(pBMP)
		{
			m_staticPic.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);   
			m_staticPic.SetBitmap(pBMP);
		}
	}
}


void CShapeItem::OnLoadShapeModel()
{
	CFileDialog  dlgFile(TRUE,NULL,m_folderLocation,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.dxf)|*.dxf|");
	if (dlgFile.DoModal() == IDOK)
	{
		m_shapeModel = dlgFile.GetPathName();
	}
	m_editShapeModel.SetWindowText(m_shapeModel);
}


BOOL CShapeItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch(m_style)
	{
	case NEW:
		break;
	case NAME:
        SetWindowText(_T("Edit Shape Name"));
		((CWnd*)GetDlgItem(IDC_BTN_LOADMODEL))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_LOADPICTURE))->EnableWindow(FALSE);
		break;
	case PICTURE:
        SetWindowText(_T("Change Shape Picture"));
		m_editShapeName.EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_LOADMODEL))->EnableWindow(FALSE);
		break;
	case MODEL:
        SetWindowText(_T("Change Shape Model"));
		m_editShapeName.EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_LOADPICTURE))->EnableWindow(FALSE);
		break;
	}

	m_editShapeName.SetWindowText(m_shapeName);
	m_editShapeModel.SetWindowText(m_shapeModel);

	HBITMAP pBMP = (HBITMAP)LoadImage(NULL,m_shapePicture,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(pBMP)
	{
		m_staticPic.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
		m_staticPic.SetBitmap(pBMP);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CShapeItem::OnCancel()
{
	CDialog::OnCancel();
}


void CShapeItem::OnOk()
{
	m_editShapeName.GetWindowText(m_shapeName);
	CDialog::OnOK();
}


void CShapeItem::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(m_editShapeName.m_hWnd == NULL)
    {
        if (nType != SIZE_MINIMIZED)
        {
            m_oldClientWidth = cx;
            m_oldClientHeight = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDC_STATIC_SHAPENAME), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_SHAPENAME), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_MODELLOCATION), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_MODELLOCATION), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_LOADMODEL), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_SHAPEICON), TopLeft, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_PIC_STATIC), TopLeft, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_LOADPICTURE), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDCANCEL), BottomRight, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDOK), BottomRight, BottomRight, cx, cy);
    if (nType != SIZE_MINIMIZED)
    {
        m_oldClientWidth = cx;
        m_oldClientHeight = cy;
    }
    InvalidateRect(NULL);
}

// refTopLeft: the control's top left references to the new window's 'refTopLeft'.
// refBottomRight: the control's bottom right references to the new window's 'refBottomRight'.
// cx: new window's width
// cy: new window's height
void CShapeItem::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldClientWidth;
    int deltaY = cy - m_oldClientHeight;
    if(refTopLeft == TopLeft && refBottomRight == TopLeft)
    {
        pCtrl->MoveWindow(&rcS);
    }
    else if(refTopLeft == TopLeft && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopRight && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == TopRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), cy+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, cx+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else
    {
        return;
    }
}

void CShapeItem::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize=CPoint(369,300);
    CDialog::OnGetMinMaxInfo(lpMMI);
}
