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
    m_oldWindowWidth(0),
    m_oldWindowHeight(0)
{
	m_shapePicture = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\SQUARE24.bmp";
	m_shapeModel = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\FLATSQUARE24cm.dxf";
	m_shapeUnit = "centimeters";
	m_folderLocation = folderpath;
}

CShapeItem::CShapeItem(CString folderpath,CString name, CString picture, CString model, CString unit, CWnd* pParent /* = NULL */)
	: CDialog(CShapeItem::IDD, pParent),
    m_oldWindowWidth(0),
    m_oldWindowHeight(0)
{
	m_shapeName = name;
	m_shapePicture = picture;
	m_shapeModel = model;
	m_shapeUnit = unit;
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
	DDX_Control(pDX, IDC_COMBO2, m_Units);
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
	CFileDialog  dlgFile(TRUE,NULL,m_folderLocation + "\\",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.bmp)|*.bmp|");
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
	CFileDialog  dlgFile(TRUE,NULL,m_folderLocation + "\\",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.dxf)|*.dxf|");
	if (dlgFile.DoModal() == IDOK)
	{
		m_shapeModel = dlgFile.GetPathName();
	}
	m_editShapeModel.SetWindowText(m_shapeModel);
}


BOOL CShapeItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i=0; i<SOURCEUNITS_COUNT; i++) {
		m_Units.AddString(UNIT_NAMES[i]);
	}
	m_Units.SetCurSel(m_Units.FindString(0,m_shapeUnit));
    SetWindowText(m_strTitle);
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

	int nSel = -1;
	if(-1 != (nSel = m_Units.GetCurSel()))
	{
		m_Units.GetLBText(nSel,m_shapeUnit);
	}
	else
	{
		m_shapeUnit = "centimeters";
	}

	CDialog::OnOK();
}


void CShapeItem::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(m_editShapeName.m_hWnd == NULL)
    {
        if (nType != SIZE_MINIMIZED)
        {
            m_oldWindowWidth = cx;
            m_oldWindowHeight = cy;
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
        m_oldWindowWidth = cx;
        m_oldWindowHeight = cy;
    }
    InvalidateRect(NULL);
}

// refTopLeft: pCtrl's top left references to the new window's 'refTopLeft'.
// refBottomRight: pCtrl's bottom right references to the new window's 'refBottomRight'.
// cx: new window's width
// cy: new window's height
void CShapeItem::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldWindowWidth;
    int deltaY = cy - m_oldWindowHeight;
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
